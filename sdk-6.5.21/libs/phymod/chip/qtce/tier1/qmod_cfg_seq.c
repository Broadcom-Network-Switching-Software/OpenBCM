/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */ 

#ifndef _DV_TB_
 #define _SDK_QMOD_ 1 
#endif

#ifdef _DV_TB_
#include <stdint.h>
#include "qmod_main.h"
#include "qmod_defines.h"
#include "qePCSRegEnums.h"
/*#include "tePMDRegEnums.h"*/ 
#include "phy_tsc_iblk.h"
#include "bcmi_qtc_xgxs_defs.h"
#endif /* _DV_TB_ */

#ifdef _SDK_QMOD_
#include "phy_tsc_iblk.h"
#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_qtce_xgxs_defs.h>
#include "qmod_enum_defines.h"
#include "qmod.h"
#include "qmod_device.h"
#include "qmod_sc_lkup_table.h"
#include "qePCSRegEnums.h"
#endif /* _SDK_QMOD_ */

#ifndef PHYMOD_ST  
#ifdef _SDK_QMOD_
  #define PHYMOD_ST  const phymod_access_t
#else
  #define PHYMOD_ST  qmod_st
#endif /* _SDK_QMOD_ */
#endif /* PHYMOD_ST */

#ifdef _SDK_QMOD_
  #define QMOD_DBG_IN_FUNC_INFO(pc) \
    PHYMOD_VDBG(QMOD_DBG_FUNC,pc,("-22%s: Adr:%08x Ln:%02d\n", __func__, pc->addr, pc->lane_mask))
  #define QMOD_DBG_OUT_FUNC_INFO(pc) \
    PHYMOD_VDBG(QMOD_DBG_FUNC,pc,("-22%s: Adr:%08x Ln:%02d\n", __func__, pc->addr, pc->lane_mask))
#endif

#ifdef _DV_TB_
  #define QMOD_DBG_IN_FUNC_INFO(pc) \
    PHYMOD_VDBG(QMOD_DBG_FUNC, pc, \
      ("QMOD IN Function : %s Port Add : %d Lane No: %d\n", \
      __func__, pc->prt_ad, pc->this_lane))
  #define QMOD_DBG_OUT_FUNC_INFO(pc) \
    PHYMOD_VDBG(QMOD_DBG_FUNC, pc, \
      ("QMOD OUT of Function : %s Port Add : %d Lane No: %d\n", \
      __func__, pc->prt_ad, pc->this_lane))
int phymod_debug_check(uint32_t flags, PHYMOD_ST *pc);
#endif

static qmod_sc_pmd_entry_st qmod_sc_pmd_entry[] = {
/*SPD_10M              0*/ { 1, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  1},
/*SPD_10M              1*/ { 1, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  1},
/*SPD_100M             2*/ { 1, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  1},
/*SPD_1000M            3*/ { 1, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  1},
/*SPD_1G_CX1           4*/ { 1, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  1},
/*SPD_1G_KX1           5*/ { 1, QMOD_PMA_OS_MODE_8_25, QMOD_PLL_MODE_DIV_66,  0, 0,  1},
/*SPD_2p5G             6*/ { 1, QMOD_PMA_OS_MODE_2,  QMOD_PLL_MODE_DIV_40,  0, 0,  1},
/*SPD_5G_X1            7*/ { 1, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_40,  1, 0,  0},
/*SPD_10G_CX4          8*/ { 4, QMOD_PMA_OS_MODE_2,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_10G_KX4          9*/ { 4, QMOD_PMA_OS_MODE_2,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_10G_X4          10*/ { 4, QMOD_PMA_OS_MODE_2,  QMOD_PLL_MODE_DIV_40,  0, 0,  1},
/*SPD_13G_X4          11*/ { 4, QMOD_PMA_OS_MODE_2,  QMOD_PLL_MODE_DIV_52,  0, 0,  1},
/*SPD_15G_X4          12*/ { 4, QMOD_PMA_OS_MODE_2,  QMOD_PLL_MODE_DIV_60,  0, 0,  1},
/*SPD_16G_X4          13*/ { 4, QMOD_PMA_OS_MODE_2,  QMOD_PLL_MODE_DIV_64,  0, 0,  1},
/*SPD_20G_CX4         14*/ { 4, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_40,  0, 0,  1},
/*SPD_10G_CX2         15*/ { 2, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_40,  0, 0,  1},
/*SPD_10G_X2          16*/ { 2, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_40,  1, 0,  1},
/*SPD_20G_X4          17*/ { 4, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_40,  1, 0,  1},
/*SPD_10p5G_X2        18*/ { 2, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_42,  1, 0,  0},
/*SPD_21G_X4          19*/ { 4, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_42,  1, 0,  0},
/*SPD_12p7G_X2        20*/ { 2, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_42,  1, 0,  0},
/*SPD_25p45G_X4       21*/ { 4, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_42,  1, 0,  0},
/*SPD_15p75G_X2       22*/ { 2, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_52,  1, 0,  0},
/*SPD_31p5G_X4        23*/ { 4, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_52,  1, 0,  0},
/*SPD_31p5G_KR4       24*/ { 4, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_52,  1, 0,  0},
/*SPD_20G_CX2         25*/ { 2, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_66,  1, 0,  0},
/*SPD_20G_X2          26*/ { 2, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_66,  1, 0,  0},
/*SPD_40G_X4          27*/ { 4, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_66,  1, 0,  0},
/*SPD_10G_KR1         28*/ { 0, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_66,  1, 0,  0},
/*SPD_10p6_X1         29*/ { 0, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_70,  1, 0,  0},
/*SPD_20G_KR2         30*/ { 2, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_66,  1, 0,  0},
/*SPD_20G_CR2         31*/ { 2, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_66,  1, 0,  0},
/*SPD_21G_X2          32*/ { 2, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_70,  1, 0,  0},
/*SPD_40G_KR4         33*/ { 4, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_66,  1, 0,  0},
/*SPD_40G_CR4         34*/ { 4, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_66,  1, 0,  0},
/*SPD_42G_X4          35*/ { 4, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_70,  1, 0,  0},
/*SPD_100G_CR10       36*/ {10, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_66,  1, 0,  0},
/*SPD_107G_CR10       37*/ {10, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_70,  1, 0,  0},
/*SPD_120G_X12        38*/ {12, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_66,  1, 0,  0},
/*SPD_127G_X12        39*/ {12, QMOD_PMA_OS_MODE_1,  QMOD_PLL_MODE_DIV_70,  1, 0,  0},
/*SPD_ILLEGAL         40*/ { 0, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_ILLEGAL         41*/ { 0, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_ILLEGAL         42*/ { 0, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_ILLEGAL         43*/ { 0, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_ILLEGAL         44*/ { 0, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_ILLEGAL         45*/ { 0, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_ILLEGAL         46*/ { 0, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_ILLEGAL         47*/ { 0, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_ILLEGAL         48*/ { 0, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_5G_KR1          49*/ { 0, QMOD_PMA_OS_MODE_2,  QMOD_PLL_MODE_DIV_66,  0, 0,  0},
/*SPD_10p5G_X4        50*/ { 4, QMOD_PMA_OS_MODE_2,  QMOD_PLL_MODE_DIV_42,  0, 0,  0},
/*SPD_ILLEGAL         51*/ { 0, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_ILLEGAL         52*/ { 0, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  0},
/*SPD_10M_10p3125     53*/ { 1, QMOD_PMA_OS_MODE_8_25, QMOD_PLL_MODE_DIV_66,  0, 0,  0},
/*SPD_100M_10p3125    54*/ { 1, QMOD_PMA_OS_MODE_8_25, QMOD_PLL_MODE_DIV_66,  0, 0,  0},
/*SPD_1000M_10p3125   55*/ { 1, QMOD_PMA_OS_MODE_8_25, QMOD_PLL_MODE_DIV_66,  0, 0,  0},
/*SPD_2p5G_X1_10p3125 56*/ { 1, QMOD_PMA_OS_MODE_3_3,QMOD_PLL_MODE_DIV_66,  0, 0,  0},
/*SPEED_10M_X1_10 :   57*/ { 1, QMOD_PMA_OS_MODE_8_25, QMOD_PLL_MODE_DIV_66,  0, 0,  0},
/*SPEED_100M_X1_10 :  58*/ { 1, QMOD_PMA_OS_MODE_8_25, QMOD_PLL_MODE_DIV_66,  0, 0,  0},
/*SPEED_1000M_X1_10 : 59*/ { 1, QMOD_PMA_OS_MODE_8_25, QMOD_PLL_MODE_DIV_66,  0, 0,  0},
/*SPEED_4G_X1_10 :    60*/ { 1, QMOD_PMA_OS_MODE_2, QMOD_PLL_MODE_DIV_64,  0, 0,  0},
/*SPD_ILLEGAL         61*/ { 0, QMOD_PMA_OS_MODE_5,  QMOD_PLL_MODE_DIV_40,  0, 0,  0}
};

int _qmod_set_port_mode_select(PHYMOD_ST *pc);
int _qmod_configure_sc_speed_configuration(PHYMOD_ST* pc);
int _qmod_init_pcs_fs(PHYMOD_ST* pc);
int _qmod_init_pcs_an(PHYMOD_ST* pc);
int _configure_st_entry(int st_entry_num, int st_entry_speed, PHYMOD_ST* pc);

#ifdef _DV_TB_
int qmod_set_an_port_mode(PHYMOD_ST* pc);
int qmod_autoneg_set(PHYMOD_ST* pc);
int qmod_autoneg_control(PHYMOD_ST* pc);
int qmod_an_abilities_enables(PHYMOD_ST* pc,int sub_port);
int qmod_an_cl37_bam_abilities(PHYMOD_ST* pc,int sub_port);
int qmod_autoneg_abilities_controls(PHYMOD_ST* pc,int sub_port);
int qmod_autoneg_cl37_base_abilities(PHYMOD_ST* pc,int sub_port);
int qmod_autoneg_cl37_bam_abilities(PHYMOD_ST* pc,int sub_port);
int qmod_autoneg_cl37_bam_over1g_abilities(PHYMOD_ST* pc,int sub_port);
int qmod_prbs_check(PHYMOD_ST* pc, int real_check, int *prbs_status);
int qmod_prbs_mode(PHYMOD_ST* pc, int port, int prbs_inv, int pat, int check_mode);
int qmod_prbs_control(PHYMOD_ST* pc, int prbs_enable);
int qmod_prbs_rx_enable_set(PHYMOD_ST* pc, int enable);
int qmod_prbs_tx_enable_set(PHYMOD_ST* pc, int enable);
int qmod_prbs_rx_invert_data_set(PHYMOD_ST* pc, int invert_data);
int qmod_prbs_rx_check_mode_set(PHYMOD_ST* pc, int check_mode);
int qmod_prbs_tx_invert_data_set(PHYMOD_ST* pc, int invert_data);
int qmod_prbs_tx_polynomial_set(PHYMOD_ST* pc, prbs_polynomial_type_t prbs_polynomial);
int qmod_prbs_rx_polynomial_set(PHYMOD_ST* pc, prbs_polynomial_type_t prbs_polynomial);
int qmod_pmd_lane_swap(PHYMOD_ST *pc);
int qmod_check_sc_stats(PHYMOD_ST* pc);
int qmod_disable_set(PHYMOD_ST* pc);
int qmod_credit_control(PHYMOD_ST* pc, int enable);
int qmod_tx_lane_control_set(PHYMOD_ST* pc, tx_lane_disable_type_t tx_dis_type);
int qmod_pll_lock_get(PHYMOD_ST* pc, int* lockStatus);
#endif /* _DV_TB_ */

/**
@brief   update the port mode
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   pll_restart Indicates if required to restart PLL.
@returns The value PHYMOD_E_NONE upon successful completion
*/
int qmod_update_port_mode( PHYMOD_ST *pc, int *pll_restart)
{
    MAIN_SETUPr_t mode_reg;
    READ_MAIN_SETUPr(pc, &mode_reg);

    MAIN_SETUPr_SINGLE_PORT_MODEf_SET(mode_reg, 0);
    MODIFY_MAIN_SETUPr(pc, mode_reg);
    return PHYMOD_E_NONE ;
}

/*!
@brief   This function reads TX-PLL PLL_LOCK bit.
@param   pc  handle to current TSCE context (#PHYMOD_ST)
@param   lockStatus reference which is updated with lock status.
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Read PLL lock status. Returns  1 or 0 (locked/not)
*/

int qmod_pll_lock_get(PHYMOD_ST* pc, int* lockStatus)
{
  PMD_X1_STSr_t  reg_pmd_x1_sts;
  QMOD_DBG_IN_FUNC_INFO(pc);
  READ_PMD_X1_STSr(pc, &reg_pmd_x1_sts);

  *lockStatus =  PMD_X1_STSr_PLL_LOCK_STSf_GET(reg_pmd_x1_sts);
  return PHYMOD_E_NONE;
}

/**
@brief   This function reads TX-PMD PMD_LOCK bit.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   lockStatus reference which is updated with pmd_lock status
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Read PMD lock status Returns 1 or 0 (locked/not)
*/

int qmod_pmd_lock_get(PHYMOD_ST* pc, uint32_t * lockStatus)
{
  PMD_X1_STSr_t  reg_pmd_x1_sts;
  QMOD_DBG_IN_FUNC_INFO(pc);
  READ_PMD_X1_STSr(pc, &reg_pmd_x1_sts);

  *lockStatus =  PMD_X1_STSr_TX_CLK_VLD_STSf_GET(reg_pmd_x1_sts);
  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/*
@brief   This function waits for TX-PLL PLL_LOCK bit. The register is one copy
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Wait for PLL to lock. Cannot be used in SDK.
*/

int qmod_pll_lock_wait(PHYMOD_ST* pc, int timeOutValue)
{
   int rv;
   QMOD_DBG_IN_FUNC_INFO(pc);

   rv = qmod_regbit_set_wait_check(pc,
                 0x9012,
                 1, 1, timeOutValue); 
                 /* TBD PMD_X1_STATUS_PLL_LOCK_STS_MASK, 1, timeOutValue); */
   if (rv == SOC_E_TIMEOUT) {
     PHYMOD_DEBUG_ERROR(("%-22s ERROR: p=%0d Timeout PLL lock:\n", __func__, pc->port)); 
     return (SOC_E_TIMEOUT);
   }
  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/*
@brief   This function waits for the the RX-PMD to lock.
@param  pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details  This function has a time out limit (TE_PLL_WAIT*50) micro-second in driver.
or TE_PLL_WAIT/20 pulling count in simualtion, if per_lane_control is set.  Otherwise, 
if per_lane_control==0, it reads back PMD_LIVE_STATUS and put it on accData.          
*/
int qmod_wait_pmd_lock(PHYMOD_ST* pc, int timeOutValue, int* lockStatus)
{
   int rv; 
   PMD_X4_STSr_t reg_pmd_x4_stats;
   QMOD_DBG_IN_FUNC_INFO(pc);

   PMD_X4_STSr_CLR(reg_pmd_x4_stats);

   if(pc->per_lane_control) {
      rv = qmod_regbit_set_wait_check(pc, 0xc012, 1,1,timeOutValue);
             /* TBD PMD_X4_STATUS_RX_LOCK_STS_MASK,1,timeOutValue); */

      if (rv == SOC_E_TIMEOUT) {
        PHYMOD_DEBUG_ERROR(("%-22s ERROR: P=%0d Timeout RX PMD lock:\n", __func__, pc->port));
         pc->accData = 0;
         *lockStatus = 0;
         return rv;
      } else {
         pc->accData = 1;
        *lockStatus = 1;
      }
   } else {
      READ_PMD_X4_STSr(pc, &reg_pmd_x4_stats);
      if(PMD_X4_STSr_RX_LOCK_STSf_GET(reg_pmd_x4_stats)) {
         pc->accData = 1;
        *lockStatus = 1;
      } else {
         pc->accData = 0;
         *lockStatus = 0;
      }
   }
   return (PHYMOD_E_NONE);
}
#endif

/**
@brief   controls PCS Bypass Control function.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   cntl Control value (bypass, un-bypass)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Set cntl <B>0 to disable</B> PCS and <B>1 to enable</B> PCS
When bypassed, PCS relinquishes control of PMD resets and PCS logic programming.
TEMod can take complete control of PMD programming. Bypass is also used in ILKN
mode where the PCS is not used.

*/
int qmod_pcs_bypass_ctl (PHYMOD_ST* pc, int cntl)
{
  /* remove this function. */
  return PHYMOD_E_FAIL;
}

#ifdef _DV_TB_
/**
@brief   Controls PMD datapath resets.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   cntl Controls if PMD should be reset or unset
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Set cntl arg <B>0</B>:disable <B>1</B>: enable
 The PMD datapath is reset even if PCS is actively controlling PMD resets.
 There are two types of control. One per lane and one per Core.
*/
int qmod_pmd_reset_bypass (PHYMOD_ST* pc, int cntl)     /* PMD_RESET_BYPASS */
{
  int lnCntl, coreCntl;
  lnCntl   = cntl & 0x10 ; 
  coreCntl = cntl & 0x1 ; 
  PMD_X4_OVRRr_t reg_pmd_x4_or;
  PMD_X4_CTLr_t  reg_pmd_x4_ctrl;
  PMD_X1_OVRRr_t reg_pmd_x1_or;
  PMD_X1_CTLr_t  reg_pmd_x1_ctrl;
  SC_X4_CTLr_t reg_sc_ctrl;

  QMOD_DBG_IN_FUNC_INFO(pc);
  PMD_X4_OVRRr_CLR(reg_pmd_x4_or);
  PMD_X4_CTLr_CLR(reg_pmd_x4_ctrl);
  PMD_X1_OVRRr_CLR(reg_pmd_x1_or);
  PMD_X1_CTLr_CLR(reg_pmd_x1_ctrl);
  SC_X4_CTLr_CLR(reg_sc_ctrl);

  if (lnCntl) {
    PMD_X4_OVRRr_LN_DP_H_RSTB_OENf_SET(reg_pmd_x4_or, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_OVRRr(pc, reg_pmd_x4_or));

    PMD_X4_CTLr_LN_DP_H_RSTBf_SET(reg_pmd_x4_ctrl, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, reg_pmd_x4_ctrl));

    /* toggle added */
    PMD_X4_OVRRr_CLR(reg_pmd_x4_or);
    PMD_X4_CTLr_CLR(reg_pmd_x4_ctrl);

    PMD_X4_CTLr_LN_DP_H_RSTBf_SET(reg_pmd_x4_ctrl, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, reg_pmd_x4_ctrl));

    PMD_X4_OVRRr_LN_DP_H_RSTB_OENf_SET(reg_pmd_x4_or, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_OVRRr(pc, reg_pmd_x4_or));

  } else {
    PMD_X4_OVRRr_LN_DP_H_RSTB_OENf_SET(reg_pmd_x4_or, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_OVRRr(pc, reg_pmd_x4_or));

    PMD_X4_CTLr_LN_DP_H_RSTBf_SET(reg_pmd_x4_ctrl, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, reg_pmd_x4_ctrl));
  }

  if (coreCntl) {
    PMD_X1_OVRRr_CORE_DP_H_RSTB_OENf_SET(reg_pmd_x1_or, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_OVRRr(pc, reg_pmd_x1_or));

    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(reg_pmd_x1_ctrl, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_CTLr(pc, reg_pmd_x1_ctrl));

    /* toggle added */
    PMD_X1_OVRRr_CLR(reg_pmd_x1_or);
    PMD_X1_CTLr_CLR(reg_pmd_x1_ctrl);
    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(reg_pmd_x1_ctrl, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_CTLr(pc, reg_pmd_x1_ctrl));

    PMD_X1_OVRRr_CORE_DP_H_RSTB_OENf_SET(reg_pmd_x1_or, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_OVRRr(pc, reg_pmd_x1_or));

  } else {
    PMD_X1_OVRRr_CORE_DP_H_RSTB_OENf_SET(reg_pmd_x1_or, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_OVRRr(pc, reg_pmd_x1_or));

    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(reg_pmd_x1_ctrl, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_CTLr(pc, reg_pmd_x1_ctrl));
  }

  /* sw_speed_change toggled */
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 0);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, reg_sc_ctrl));

  SC_X4_CTLr_CLR(reg_sc_ctrl);
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, reg_sc_ctrl));

  return PHYMOD_E_NONE;
} /* PMD_RESET_BYPASS */
#endif

#ifdef _SDK_QMOD_
/*!
\brief Controls the init setting/resetting of autoneg  registers.
\param  pc handle to current TSCE context (#PHYMOD_ST)
\param  qmod_an_init_t structure with all the onr time autoneg init cfg.
\returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
\details Get aneg features via #qmod_an_init_t.
  This does not start the autoneg. That is done in qmod_autoneg_control

*/
int qmod_autoneg_set_init(PHYMOD_ST* pc, qmod_an_init_t *an_init_st) /* AUTONEG_SET */
{
  return PHYMOD_E_NONE;
} 
#endif

#ifdef _SDK_QMOD_
/*!
\brief Controls the setting/resetting of autoneg timers.
\param  pc handle to current TSCE context (#PHYMOD_ST)
\returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
\details
  Get aneg features via #PHYMOD_ST->an_tech_ability and write the pages
  This does not start the autoneg. That is done in qmod_autoneg_control

*/
int qmod_autoneg_timer_init(PHYMOD_ST* pc)               /* AUTONEG timer set*/
{
  /*
  PHYMOD_ST  pc_10G_X1;
  PHYMOD_ST*  pc_10G_X1_tmp;
  PHYMOD_ST  pc_40G_X4;
  PHYMOD_ST*  pc_40G_X4_tmp;
  PHYMOD_ST  pc_100G_X4;
  PHYMOD_ST*  pc_100G_X4_tmp;
  */
  /* MAIN_SETUPr_t reg_main_setup; */ 

  CL37_RESTARTr_t reg_cl37_restart_timers;
  CL37_ACKr_t     reg_cl37_ack_timers;
 /* CL37_ERRr_t     reg_cl37_err_timers; */
 /*  CL37_SYNC_STS_FILTER_TMRr_t reg_an_sync_stat_fil_timer; */
  LNK_UP_TYPEr_t  reg_link_up_timer;
  SGMII_CL37_TMR_TYPEr_t reg_an_cl37_sgmii_tmr;
 /* MAIN_TICK_CTL1r_t reg_tick_ctrl; */
 /*  MAIN_TICK_CTL0r_t reg_tick_ctrl0; */
  IGNORE_LNK_TMR_TYPEr_t  reg_ignore_link_timers;
  LNK_FAIL_INHBT_TMR_NOT_CL72_TYPEr_t  reg_inhbt_not_cl72_timers;

#if 0
  BAM_SPD_PRI_5_0r_t reg_bam_pri5;
  OUI_LWRr_t reg_an_oui_lower;
  OUI_UPRr_t reg_an_oui_upper;
  SC_X1_TX_RST_CNTr_t reg_sc_rst_cnt;
#endif

/*0x9250 AN_X1_TIMERS_cl37_restart */
  CL37_RESTARTr_CLR(reg_cl37_restart_timers);
  /* coverity[operator_confusion] */
  CL37_RESTARTr_CL37_RESTART_TIMER_PERIODf_SET(reg_cl37_restart_timers, 0x29a);
  PHYMOD_IF_ERR_RETURN(WRITE_CL37_RESTARTr(pc, reg_cl37_restart_timers));

  /*0x9251 AN_X1_TIMERS_cl37_ack */
  CL37_ACKr_CLR(reg_cl37_ack_timers);
  /* coverity[operator_confusion] */
  CL37_ACKr_CL37_ACK_TIMER_PERIODf_SET(reg_cl37_ack_timers, 0x29a);
  PHYMOD_IF_ERR_RETURN(WRITE_CL37_ACKr(pc, reg_cl37_ack_timers));

#if 0
/*0x9252 AN_X1_TIMERS_cl37_error */
  CL37_ERRr_CLR(reg_cl37_err_timers);
  data = pc->cl37_error_timer_period;
  CL37_ERRr_CL37_ERROR_TIMER_PERIODf_SET(reg_cl37_err_timers, data);
  PHYMOD_IF_ERR_RETURN(WRITE_CL37_ERRr(pc, reg_cl37_err_timers));

  /*0x9253 AN_X1_TIMERS_cl37_sync_status_filter_timer*/ 
  data = pc->cl37_sync_status_filter;
  CL37_SYNC_STS_FILTER_TMRr_CLR(reg_an_sync_stat_fil_timer);
  CL37_SYNC_STS_FILTER_TMRr_CL37_SYNC_STATUS_FILTER_TIMER_PERIODf_SET(reg_an_sync_stat_fil_timer, data);
  PHYMOD_IF_ERR_RETURN(WRITE_CL37_SYNC_STS_FILTER_TMRr(pc, reg_an_sync_stat_fil_timer));
#endif

/*TBD::0x9254 AN_X1_TIMERS_sgmii_cl73_timer_type*/
  SGMII_CL37_TMR_TYPEr_CLR(reg_an_cl37_sgmii_tmr);
  /* coverity[operator_confusion] */
  SGMII_CL37_TMR_TYPEr_SGMII_TIMERf_SET(reg_an_cl37_sgmii_tmr, 0x6b);
  PHYMOD_IF_ERR_RETURN(WRITE_SGMII_CL37_TMR_TYPEr(pc, reg_an_cl37_sgmii_tmr));

/*TBD::0x9255 AN_X1_TIMERS_link_up_typ*/ 
  LNK_UP_TYPEr_CLR(reg_link_up_timer);
  LNK_UP_TYPEr_SET(reg_link_up_timer, 0x29a);
  PHYMOD_IF_ERR_RETURN(WRITE_LNK_UP_TYPEr(pc, reg_link_up_timer));

/*0x9256 AN_X1_TIMERS_ignore_link */
  IGNORE_LNK_TMR_TYPEr_CLR(reg_ignore_link_timers);
  IGNORE_LNK_TMR_TYPEr_SET(reg_ignore_link_timers, 0x29a);
  PHYMOD_IF_ERR_RETURN(WRITE_IGNORE_LNK_TMR_TYPEr(pc, reg_ignore_link_timers));

/*0x9256 AN_X1_TIMERS_link_fail_inhbt_tmr_not_cl72 */
  LNK_FAIL_INHBT_TMR_NOT_CL72_TYPEr_CLR(reg_inhbt_not_cl72_timers);
  LNK_FAIL_INHBT_TMR_NOT_CL72_TYPEr_SET(reg_inhbt_not_cl72_timers, 0x8382);
  PHYMOD_IF_ERR_RETURN(WRITE_LNK_FAIL_INHBT_TMR_NOT_CL72_TYPEr(pc, reg_inhbt_not_cl72_timers));

#if 0
/*TBD::0x9263 sc_x1_control_tx_reset_count*/ 
  data = pc->tx_reset_count;
  SC_X1_TX_RST_CNTr_CLR(reg_sc_rst_cnt);
  SC_X1_TX_RST_CNTr_TX_RESET_COUNTf_SET(reg_sc_rst_cnt, data);
  PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_TX_RST_CNTr(pc, reg_sc_rst_cnt));

/*0x9240 AN_X1_CONTROL_oui_upper  */
  data =  (pc->oui_upper_data & 0xff);
  OUI_UPRr_CLR(reg_an_oui_upper);
  OUI_UPRr_OUI_UPPER_DATAf_SET(reg_an_oui_upper, data);
  PHYMOD_IF_ERR_RETURN(WRITE_OUI_UPRr(pc, reg_an_oui_upper));

/*0x9241 AN_X1_CONTROL_oui_lower  */
  data =  ((pc->oui_lower_data));
  OUI_LWRr_CLR(reg_an_oui_lower);
  OUI_LWRr_OUI_LOWER_DATAf_SET(reg_an_oui_lower, data);
  PHYMOD_IF_ERR_RETURN(WRITE_OUI_LWRr(pc, reg_an_oui_lower));

/*0x9242 AN_X1_CONTROL_BAM_SPEED_PRI_5_0  */
  data =  ((pc->bam_spd_pri_5_0));
  BAM_SPD_PRI_5_0r_CLR(reg_bam_pri5);
  BAM_SPD_PRI_5_0r_SET(reg_bam_pri5, data);
  PHYMOD_IF_ERR_RETURN(WRITE_BAM_SPD_PRI_5_0r(pc, reg_bam_pri5));
#endif


  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/**
@brief   Controls the setting/resetting of autoneg advertisement registers.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details
  Get aneg features via #PHYMOD_ST->an_tech_ability and write the pages
  This does not start the autoneg. That is done in qmod_autoneg_control

*/
int qmod_autoneg_abilities_controls(PHYMOD_ST* pc, int sub_port)
{
 AN_X4_ABI_SP0_CTLSr_t                     reg_an_ctrl_sp0;
 AN_X4_ABI_SP1_CTLSr_t                     reg_an_ctrl_sp1;
 AN_X4_ABI_SP2_CTLSr_t                     reg_an_ctrl_sp2;
 AN_X4_ABI_SP3_CTLSr_t                     reg_an_ctrl_sp3;

 if(sub_port == 0){
   AN_X4_ABI_SP0_CTLSr_CLR(reg_an_ctrl_sp0);
   AN_X4_ABI_SP0_CTLSr_AN_GOOD_TRAPf_SET(reg_an_ctrl_sp0,  pc->an_good_trap);
   AN_X4_ABI_SP0_CTLSr_AN_GOOD_CHECK_TRAPf_SET(reg_an_ctrl_sp0, pc->an_good_check_trap);
   AN_X4_ABI_SP0_CTLSr_LINKFAILTIMER_DISf_SET(reg_an_ctrl_sp0, pc->linkfailtimer_dis);
   AN_X4_ABI_SP0_CTLSr_LINKFAILTIMERQUAL_ENf_SET(reg_an_ctrl_sp0, pc->linkfailtimerqua_en);
   AN_X4_ABI_SP0_CTLSr_AN_FAIL_COUNT_LIMITf_SET(reg_an_ctrl_sp0, pc->an_fail_cnt);
   AN_X4_ABI_SP0_CTLSr_OUI_CONTROLf_SET(reg_an_ctrl_sp0, pc->an_oui_ctrl);
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP0_CTLSr(pc, reg_an_ctrl_sp0));
 }else if(sub_port == 1){
   AN_X4_ABI_SP1_CTLSr_CLR(reg_an_ctrl_sp1);
   AN_X4_ABI_SP1_CTLSr_AN_GOOD_TRAPf_SET(reg_an_ctrl_sp1,  pc->an_good_trap);
   AN_X4_ABI_SP1_CTLSr_AN_GOOD_CHECK_TRAPf_SET(reg_an_ctrl_sp1, pc->an_good_check_trap);
   AN_X4_ABI_SP1_CTLSr_LINKFAILTIMER_DISf_SET(reg_an_ctrl_sp1, pc->linkfailtimer_dis);
   AN_X4_ABI_SP1_CTLSr_LINKFAILTIMERQUAL_ENf_SET(reg_an_ctrl_sp1, pc->linkfailtimerqua_en);
   AN_X4_ABI_SP1_CTLSr_AN_FAIL_COUNT_LIMITf_SET(reg_an_ctrl_sp1, pc->an_fail_cnt);
   AN_X4_ABI_SP1_CTLSr_OUI_CONTROLf_SET(reg_an_ctrl_sp1, pc->an_oui_ctrl);
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP1_CTLSr(pc, reg_an_ctrl_sp1));
 }else if(sub_port == 2){
   AN_X4_ABI_SP2_CTLSr_CLR(reg_an_ctrl_sp2);
   AN_X4_ABI_SP2_CTLSr_AN_GOOD_TRAPf_SET(reg_an_ctrl_sp2,  pc->an_good_trap);
   AN_X4_ABI_SP2_CTLSr_AN_GOOD_CHECK_TRAPf_SET(reg_an_ctrl_sp2, pc->an_good_check_trap);
   AN_X4_ABI_SP2_CTLSr_LINKFAILTIMER_DISf_SET(reg_an_ctrl_sp2, pc->linkfailtimer_dis);
   AN_X4_ABI_SP2_CTLSr_LINKFAILTIMERQUAL_ENf_SET(reg_an_ctrl_sp2, pc->linkfailtimerqua_en);
   AN_X4_ABI_SP2_CTLSr_AN_FAIL_COUNT_LIMITf_SET(reg_an_ctrl_sp2, pc->an_fail_cnt);
   AN_X4_ABI_SP2_CTLSr_OUI_CONTROLf_SET(reg_an_ctrl_sp2, pc->an_oui_ctrl);
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP2_CTLSr(pc, reg_an_ctrl_sp2));
 }else if(sub_port == 3){
   AN_X4_ABI_SP3_CTLSr_CLR(reg_an_ctrl_sp3);
   AN_X4_ABI_SP3_CTLSr_AN_GOOD_TRAPf_SET(reg_an_ctrl_sp3,  pc->an_good_trap);
   AN_X4_ABI_SP3_CTLSr_AN_GOOD_CHECK_TRAPf_SET(reg_an_ctrl_sp3, pc->an_good_check_trap);
   AN_X4_ABI_SP3_CTLSr_LINKFAILTIMER_DISf_SET(reg_an_ctrl_sp3, pc->linkfailtimer_dis);
   AN_X4_ABI_SP3_CTLSr_LINKFAILTIMERQUAL_ENf_SET(reg_an_ctrl_sp3, pc->linkfailtimerqua_en);
   AN_X4_ABI_SP3_CTLSr_AN_FAIL_COUNT_LIMITf_SET(reg_an_ctrl_sp3, pc->an_fail_cnt);
   AN_X4_ABI_SP3_CTLSr_OUI_CONTROLf_SET(reg_an_ctrl_sp3, pc->an_oui_ctrl);
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP3_CTLSr(pc, reg_an_ctrl_sp3));
 }

  return PHYMOD_E_NONE;
}
int qmod_autoneg_cl37_base_abilities(PHYMOD_ST* pc,int sub_port)
{
 AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp0;
 AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp1;
 AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp2;
 AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp3;

 if(sub_port == 0){
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp0);
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_sgmii_speed & 3));
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_sgmii_duplex & 1));
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_fd & 1));
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_HALF_DUPLEXf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_hd & 1));
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_pause & 3));
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_np & 1));
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_sgmii_master_mode & 1));

   if(pc->cl37_bam_en){
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp0, 0);
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp0, 0);     
   }else{
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp0, 1);
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp0, (pc->cl37_an_restart | pc->cl37_sw_restart_reset_disable));     
   }
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_AN_DISABLE_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_disable_reset_disable & 1));
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities_sp0));
 }else if(sub_port == 1){
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp1);
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_SET(reg_cl37_base_abilities_sp1, (pc->cl37_sgmii_speed & 3));
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp1, (pc->cl37_sgmii_duplex & 1));
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp1, (pc->cl37_an_fd & 1));
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_HALF_DUPLEXf_SET(reg_cl37_base_abilities_sp1, (pc->cl37_an_hd & 1));
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_SET(reg_cl37_base_abilities_sp1, (pc->cl37_an_pause & 3));
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_cl37_base_abilities_sp1, (pc->cl37_an_np & 1));
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_cl37_base_abilities_sp1, (pc->cl37_sgmii_master_mode & 1));

   if(pc->cl37_bam_en){
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp1, 0);
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp1, 0);     
   }else {
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp1, 1);
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp1, (pc->cl37_an_restart | pc->cl37_sw_restart_reset_disable));     
   }
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_AN_DISABLE_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp1, (pc->cl37_an_disable_reset_disable & 1));
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities_sp1));
 }else if(sub_port == 2){
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp2);
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_SET(reg_cl37_base_abilities_sp2, (pc->cl37_sgmii_speed & 3));
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp2, (pc->cl37_sgmii_duplex & 1));
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp2, (pc->cl37_an_fd & 1));
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_HALF_DUPLEXf_SET(reg_cl37_base_abilities_sp2, (pc->cl37_an_hd & 1));
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_SET(reg_cl37_base_abilities_sp2, (pc->cl37_an_pause & 3));
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_cl37_base_abilities_sp2, (pc->cl37_an_np & 1));
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_cl37_base_abilities_sp2, (pc->cl37_sgmii_master_mode & 1));

   if(pc->cl37_bam_en){
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp2, 0);
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp2, 0);     
   }else { 
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp2, 1);
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp2, (pc->cl37_an_restart | pc->cl37_sw_restart_reset_disable));     
   } 
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_AN_DISABLE_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp2, (pc->cl37_an_disable_reset_disable & 1));
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities_sp2));
 }else if(sub_port == 3){
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp3);
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_SET(reg_cl37_base_abilities_sp3, (pc->cl37_sgmii_speed & 3));
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp3, (pc->cl37_sgmii_duplex & 1));
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp3, (pc->cl37_an_fd & 1));
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_HALF_DUPLEXf_SET(reg_cl37_base_abilities_sp3, (pc->cl37_an_hd & 1));
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_SET(reg_cl37_base_abilities_sp3, (pc->cl37_an_pause & 3));
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_cl37_base_abilities_sp3, (pc->cl37_an_np & 1));
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_cl37_base_abilities_sp3, (pc->cl37_sgmii_master_mode & 1));

   if(pc->cl37_bam_en){
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp3, 0);
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp3, 0);     
   }else {
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp3, 1);
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp3, (pc->cl37_an_restart | pc->cl37_sw_restart_reset_disable));     
   }
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_AN_DISABLE_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp3, (pc->cl37_an_disable_reset_disable & 1));
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities_sp3));
 }

  return PHYMOD_E_NONE;
}
int qmod_autoneg_cl37_bam_abilities(PHYMOD_ST* pc,int sub_port)
{
 AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_t    reg_cl37_bam_abilities_sp0;
 AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_t    reg_cl37_bam_abilities_sp1;
 AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_t    reg_cl37_bam_abilities_sp2;
 AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_t    reg_cl37_bam_abilities_sp3;

 if(sub_port==0){
   AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_CLR(reg_cl37_bam_abilities_sp0);
   AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_cl37_bam_abilities_sp0, (pc->cl37_bam_ovr1g_pgcnt & 3));
   AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_cl37_bam_abilities_sp0, (pc->cl37_bam_ovr1g_en & 1));
   AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_cl37_bam_abilities_sp0, (pc->cl37_bam_code & 0x1ff));
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr(pc, reg_cl37_bam_abilities_sp0));
 }else if(sub_port==1){
   AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_CLR(reg_cl37_bam_abilities_sp1);
   AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_cl37_bam_abilities_sp1, (pc->cl37_bam_ovr1g_pgcnt & 3));
   AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_cl37_bam_abilities_sp1, (pc->cl37_bam_ovr1g_en & 1));
   AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_cl37_bam_abilities_sp1, (pc->cl37_bam_code & 0x1ff));
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr(pc, reg_cl37_bam_abilities_sp1));
 }else if(sub_port==2){
   AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_CLR(reg_cl37_bam_abilities_sp2);
   AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_cl37_bam_abilities_sp2, (pc->cl37_bam_ovr1g_pgcnt & 3));
   AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_cl37_bam_abilities_sp2, (pc->cl37_bam_ovr1g_en & 1));
   AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_cl37_bam_abilities_sp2, (pc->cl37_bam_code & 0x1ff));
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr(pc, reg_cl37_bam_abilities_sp2));
 }else if(sub_port==3){
   AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_CLR(reg_cl37_bam_abilities_sp3);
   AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_cl37_bam_abilities_sp3, (pc->cl37_bam_ovr1g_pgcnt & 3));
   AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_cl37_bam_abilities_sp3, (pc->cl37_bam_ovr1g_en & 1));
   AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_cl37_bam_abilities_sp3, (pc->cl37_bam_code & 0x1ff));
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr(pc, reg_cl37_bam_abilities_sp3));
 }

  return PHYMOD_E_NONE;
}
int qmod_autoneg_cl37_bam_over1g_abilities(PHYMOD_ST* pc,int sub_port)
{
 AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r_t     reg_over1g_abilities_sp0;
 AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_t     reg_over1g_abilities_sp1;
 AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r_t     reg_over1g_abilities_sp2;
 AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r_t     reg_over1g_abilities_sp3;
 if(sub_port == 0){
   AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r_CLR(reg_over1g_abilities_sp0); 
   AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r_SET(reg_over1g_abilities_sp0, (pc->cl37_bam_speed & 0x7ff));    
   PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r(pc, reg_over1g_abilities_sp0));
 }else if(sub_port == 1){
   AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_CLR(reg_over1g_abilities_sp1); 
   AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_SET(reg_over1g_abilities_sp1, (pc->cl37_bam_speed & 0x7ff));    
   PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r(pc, reg_over1g_abilities_sp1));
 }else if(sub_port == 2){
   AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r_CLR(reg_over1g_abilities_sp2); 
   AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r_SET(reg_over1g_abilities_sp2, (pc->cl37_bam_speed & 0x7ff));    
   PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r(pc, reg_over1g_abilities_sp2));
 }else if(sub_port == 3){
   AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r_CLR(reg_over1g_abilities_sp3); 
   AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r_SET(reg_over1g_abilities_sp3, (pc->cl37_bam_speed & 0x7ff));    
   PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r(pc, reg_over1g_abilities_sp3));
 }
  return PHYMOD_E_NONE;
}
int qmod_autoneg_set(PHYMOD_ST* pc)               /* AUTONEG_SET */
{
 /*AN_X4_ABI_SP1_CTLSr_t                     reg_an_ctrl;*/
 /*DIG_CTL1000X2r_t                  reg_ctrl1000x2;*/
 /*AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities;*/
/* AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_t    reg_cl37_bam_abilities;*/
 /*AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_t     reg_over1g_abilities;*/

 QMOD_DBG_IN_FUNC_INFO(pc);
/*
 AN_X4_ABI_SP1_CTLSr_CLR(reg_an_ctrl);
 AN_X4_ABI_SP1_CTLSr_AN_GOOD_TRAPf_SET(reg_an_ctrl,  pc->an_good_trap);
 AN_X4_ABI_SP1_CTLSr_AN_GOOD_CHECK_TRAPf_SET(reg_an_ctrl, pc->an_good_check_trap);
 AN_X4_ABI_SP1_CTLSr_LINKFAILTIMER_DISf_SET(reg_an_ctrl, pc->linkfailtimer_dis);
 AN_X4_ABI_SP1_CTLSr_LINKFAILTIMERQUAL_ENf_SET(reg_an_ctrl, pc->linkfailtimerqua_en);
 AN_X4_ABI_SP1_CTLSr_AN_FAIL_COUNT_LIMITf_SET(reg_an_ctrl, pc->an_fail_cnt);
 AN_X4_ABI_SP1_CTLSr_OUI_CONTROLf_SET(reg_an_ctrl, pc->an_oui_ctrl);
 PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP1_CTLSr(pc, reg_an_ctrl));
 */
 qmod_autoneg_abilities_controls(pc,pc->sub_port);

/*
 DIG_CTL1000X2r_CLR(reg_ctrl1000x2);
 DIG_CTL1000X2r_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_ctrl1000x2, pc->disable_rf_report);
 PHYMOD_IF_ERR_RETURN(MODIFY_DIG_CTL1000X2r(pc, reg_ctrl1000x2));
*/
  if(pc->sc_mode == QMOD_SC_MODE_AN_CL37){
      /* an37 */
      /*Local device cl37 base abilities setting*/
      qmod_autoneg_cl37_base_abilities(pc,pc->sub_port);
      /*
      AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities);
      AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_SET(reg_cl37_base_abilities, (pc->cl37_sgmii_speed & 3));
      AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities, (pc->cl37_sgmii_duplex & 1));
      AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(reg_cl37_base_abilities, (pc->cl37_an_fd & 1));
      AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_HALF_DUPLEXf_SET(reg_cl37_base_abilities, (pc->cl37_an_hd & 1));
      AN_ABI_SP1_X4_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_SET(reg_cl37_base_abilities, (pc->cl37_an_pause & 3));
      AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_cl37_base_abilities, (pc->cl37_an_np & 1));
      AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_cl37_base_abilities, (pc->cl37_sgmii_master_mode & 1));

      if(!(pc->cl37_bam_en)){
       AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities);
       AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities, 1);
       AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities, 1);     
      } 
      
          
       PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities));
      */
      /*Local device cl37 bam abilities setting*/

      qmod_autoneg_cl37_bam_abilities(pc,pc->sub_port);
      /* CL37 BAM ABILITIES */
      /*AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_CLR(reg_cl37_bam_abilities);
      AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_cl37_bam_abilities, (pc->cl37_bam_ovr1g_pgcnt & 3));
      AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_cl37_bam_abilities, (pc->cl37_bam_ovr1g_en & 1));
      AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_cl37_bam_abilities, (pc->cl37_bam_code & 0x1ff));

      PHYMOD_IF_ERR_RETURN 
             (MODIFY_AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr(pc, reg_cl37_bam_abilities));

      */
      /***** Setting AN_X4_ABILITIES_local_device_over1g_abilities_0  0xC184 *******/
      if(pc->cl37_bam_speed){
        qmod_autoneg_cl37_bam_over1g_abilities(pc,pc->sub_port);
        /*AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_CLR(reg_over1g_abilities); 
        AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_SET(reg_over1g_abilities, (pc->cl37_bam_speed & 0x7ff));    
        PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r(pc, reg_over1g_abilities));
        */
      }  


  }/*Completion of CL37 abilities*/


  return PHYMOD_E_NONE;
}
#endif

/*
@brief   To get autoneg control registers.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   an_control details
@param   an_complete  reference which is updated with an_complete info
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Upper layer software calls this function to get autoneg info. 

<B>How to call:</B><br>
<p>Call directly </p>
*/

int qmod_autoneg_control_get(PHYMOD_ST* pc, int sub_port, qmod_an_control_t *an_control, int *an_complete) 
{
  /* code not complete.*/
  AN_X4_SW_SP0_AN_MISC_STSr_t an_misc_reg_sub0;
  AN_X4_SW_SP1_AN_MISC_STSr_t an_misc_reg_sub1;
  AN_X4_SW_SP2_AN_MISC_STSr_t an_misc_reg_sub2;
  AN_X4_SW_SP3_AN_MISC_STSr_t an_misc_reg_sub3;
  AN_X4_ABI_SP0_ENSr_t  reg_an_enb0;
  AN_X4_ABI_SP1_ENSr_t  reg_an_enb1;
  AN_X4_ABI_SP2_ENSr_t  reg_an_enb2;
  AN_X4_ABI_SP3_ENSr_t  reg_an_enb3;

  an_control->num_lane_adv = 1;
  an_control->enable = 0;
  *an_complete = 0;
  switch (sub_port) {
  case 0:
      READ_AN_X4_ABI_SP0_ENSr(pc, &reg_an_enb0);
      if (AN_X4_ABI_SP0_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb0) ||
          AN_X4_ABI_SP0_ENSr_CL37_ENABLEf_GET(reg_an_enb0)) {
          an_control->enable = 1;

          READ_AN_X4_SW_SP0_AN_MISC_STSr(pc, &an_misc_reg_sub0);
          *an_complete = AN_X4_SW_SP0_AN_MISC_STSr_AN_COMPLETEf_GET(an_misc_reg_sub0);
      }
      break;
  case 1:
      READ_AN_X4_ABI_SP1_ENSr(pc, &reg_an_enb1);
      if (AN_X4_ABI_SP1_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb1) ||
          AN_X4_ABI_SP1_ENSr_CL37_ENABLEf_GET(reg_an_enb1)) {
          an_control->enable = 1;

          READ_AN_X4_SW_SP1_AN_MISC_STSr(pc, &an_misc_reg_sub1);
          *an_complete = AN_X4_SW_SP1_AN_MISC_STSr_AN_COMPLETEf_GET(an_misc_reg_sub1);
      }
      break;
  case 2:
      READ_AN_X4_ABI_SP2_ENSr(pc, &reg_an_enb2);
      if (AN_X4_ABI_SP2_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb2) ||
          AN_X4_ABI_SP2_ENSr_CL37_ENABLEf_GET(reg_an_enb2)) {
          an_control->enable = 1;

          READ_AN_X4_SW_SP2_AN_MISC_STSr(pc, &an_misc_reg_sub2);
          *an_complete = AN_X4_SW_SP2_AN_MISC_STSr_AN_COMPLETEf_GET(an_misc_reg_sub2);
      }
      break;
  default:
      READ_AN_X4_ABI_SP3_ENSr(pc, &reg_an_enb3);
      if (AN_X4_ABI_SP3_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb3) ||
          AN_X4_ABI_SP3_ENSr_CL37_ENABLEf_GET(reg_an_enb3)) {
          an_control->enable = 1;

          READ_AN_X4_SW_SP3_AN_MISC_STSr(pc, &an_misc_reg_sub3);
          *an_complete = AN_X4_SW_SP3_AN_MISC_STSr_AN_COMPLETEf_GET(an_misc_reg_sub3);
      }
      break;
  }
  return PHYMOD_E_NONE;
/*
  This code is from TEMod. It may not match 1:1 with QTC.
  AN_X4_LOC_DEV_CL37_BASE_ABILr_t reg_an_cl37_abl;
  AN_X4_ENSr_t reg_an_enb;
  AN_X4_CTLSr_t an_x4_abl_ctrl;
  AN_X4_AN_MISC_STSr_t an_misc_sts;

  QMOD_DBG_IN_FUNC_INFO(pc);
  READ_AN_X4_LOC_DEV_CL37_BASE_ABILr(pc, &reg_an_cl37_abl);
  if(AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(reg_an_cl37_abl) == 1){
    an_control->an_property_type = TEMOD_AN_PROPERTY_ENABLE_SGMII_MASTER_MODE;
  }

  READ_AN_X4_ENSr(pc, &reg_an_enb);
  if(AN_X4_ENSr_CL37_BAM_ENABLEf_GET(reg_an_enb) == 1){
     an_control->an_type = TEMOD_AN_MODE_CL37BAM;
     an_control->enable = 1;
  } else if ( AN_X4_ENSr_CL37_ENABLEf_GET(reg_an_enb) == 1){
     an_control->an_type = TEMOD_AN_MODE_CL37;
     an_control->enable = 1;
  } else {
     an_control->an_type = TEMOD_AN_MODE_NONE;
     an_control->enable = 0;
  }

  if(AN_X4_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_GET(reg_an_enb) == 1) {
    an_control->an_property_type = TEMOD_AN_PROPERTY_ENABLE_SGMII_TO_CL37_AUTO;
  } else if(AN_X4_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_GET(reg_an_enb) == 1) {
    an_control->an_property_type = TEMOD_AN_PROPERTY_ENABLE_CL37_BAM_to_SGMII_AUTO;
  }

  an_control->num_lane_adv = 1;

  AN_X4_AN_MISC_STSr_CLR(an_misc_sts);
  READ_AN_X4_AN_MISC_STSr(pc, &an_misc_sts);
  *an_complete = AN_X4_AN_MISC_STSr_AN_COMPLETEf_GET(an_misc_sts);

  return PHYMOD_E_NONE;
*/
}

/**
@brief   To get autoneg advertisement registers.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   an_ability_st, with the local abilities
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Upper layer software calls this function to get local autoneg
         info.

<B>How to call:</B><br>
<p>Call directly </p>
*/
   
int qmod_autoneg_local_ability_get(PHYMOD_ST* pc, qmod_an_ability_t *an_ability_st, int sub_port)
{
  AN_X4_ABI_SP0_ENSr_t  reg_an_enb0;
  AN_X4_ABI_SP1_ENSr_t  reg_an_enb1;
  AN_X4_ABI_SP2_ENSr_t  reg_an_enb2;
  AN_X4_ABI_SP3_ENSr_t  reg_an_enb3;  
  AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp0;
  AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp1;
  AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp2;  
  AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp3; 
  AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r_t reg_an_over1g_ability_sp0;
  AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_t reg_an_over1g_ability_sp1;
  AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r_t reg_an_over1g_ability_sp2;
  AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r_t reg_an_over1g_ability_sp3;
  AN_X4_LP_SP0_LP_BASE_PAGE1r_t    reg_lp_sp0 ;
  AN_X4_LP_SP1_LP_BASE_PAGE1r_t    reg_lp_sp1 ;
  AN_X4_LP_SP2_LP_BASE_PAGE1r_t    reg_lp_sp2 ;
  AN_X4_LP_SP3_LP_BASE_PAGE1r_t    reg_lp_sp3 ;
  uint16_t                           value=0 ; 
  
  an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_NONE ;
  an_ability_st->cl37_adv.cl37_sgmii_speed = 0 ;
  an_ability_st->cl37_adv.an_pause         = 0 ;
  an_ability_st->cl37_adv.an_bam_speed     = 0 ;
  an_ability_st->cl37_adv.an_duplex        = 0 ;
  
  AN_X4_ABI_SP0_ENSr_CLR(reg_an_enb0);
  AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp0);
  AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r_CLR(reg_an_over1g_ability_sp0);
  AN_X4_ABI_SP1_ENSr_CLR(reg_an_enb1);
  AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp1);
  AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_CLR(reg_an_over1g_ability_sp1);
  AN_X4_ABI_SP2_ENSr_CLR(reg_an_enb2);
  AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp2);
  AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r_CLR(reg_an_over1g_ability_sp2);
  AN_X4_ABI_SP3_ENSr_CLR(reg_an_enb3);
  AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp3);
  AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r_CLR(reg_an_over1g_ability_sp3);
  AN_X4_LP_SP0_LP_BASE_PAGE1r_CLR(reg_lp_sp0) ;
  AN_X4_LP_SP1_LP_BASE_PAGE1r_CLR(reg_lp_sp1) ;
  AN_X4_LP_SP2_LP_BASE_PAGE1r_CLR(reg_lp_sp2) ;
  AN_X4_LP_SP3_LP_BASE_PAGE1r_CLR(reg_lp_sp3) ;

  if(sub_port == 0){
      PHYMOD_IF_ERR_RETURN (READ_AN_X4_ABI_SP0_ENSr(pc, &reg_an_enb0));
      if(AN_X4_ABI_SP0_ENSr_CL37_ENABLEf_GET(reg_an_enb0)) {
          an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37 ;
          
          PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr(pc, &reg_cl37_base_abilities_sp0));
          an_ability_st->cl37_adv.an_pause         = AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_GET(reg_cl37_base_abilities_sp0);
          an_ability_st->cl37_adv.an_duplex        = QMOD_AN_FULL_DUPLEX_SET ;

          /* no combined modes yet; cl37 default full duplex */
          if(AN_X4_ABI_SP0_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb0)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_SGMII ;
              an_ability_st->cl37_adv.an_pause         = QMOD_SYMM_PAUSE ;

              if(AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(reg_cl37_base_abilities_sp0)) { /* master mode dicates a role */
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(reg_cl37_base_abilities_sp0) ;
                  an_ability_st->cl37_adv.an_duplex        = (AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(reg_cl37_base_abilities_sp0))?
                                                              QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              } else {
                  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_SP0_LP_BASE_PAGE1r(pc, &reg_lp_sp0));
                  value = AN_X4_LP_SP0_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(reg_lp_sp0) ;
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  0x3 & (value>>10) ; 
                  an_ability_st->cl37_adv.an_duplex        = (0x1 & (value>>12))? QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              }
          }
          if(AN_X4_ABI_SP0_ENSr_CL37_BAM_ENABLEf_GET(reg_an_enb0)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37BAM ;

              PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r(pc, &reg_an_over1g_ability_sp0));
              if(AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r_BAM_2P5GBASE_Xf_GET(reg_an_over1g_ability_sp0)) {
                  an_ability_st->cl37_adv.an_bam_speed |= (1 << QMOD_CL37_BAM_2p5GBASE_X) ;
              }
          }
      }
  } else if(sub_port == 1){
      PHYMOD_IF_ERR_RETURN (READ_AN_X4_ABI_SP1_ENSr(pc, &reg_an_enb1));
      if(AN_X4_ABI_SP1_ENSr_CL37_ENABLEf_GET(reg_an_enb1)) {
          an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37 ;

          PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr(pc, &reg_cl37_base_abilities_sp1));
          an_ability_st->cl37_adv.an_pause         =  AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_GET(reg_cl37_base_abilities_sp1);
          an_ability_st->cl37_adv.an_duplex        =  QMOD_AN_FULL_DUPLEX_SET ;

          /* no combined modes yet */
          if(AN_X4_ABI_SP1_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb1)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_SGMII ;
              an_ability_st->cl37_adv.an_pause         = QMOD_SYMM_PAUSE ;

              if(AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(reg_cl37_base_abilities_sp1)) {
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(reg_cl37_base_abilities_sp1) ;
                  an_ability_st->cl37_adv.an_duplex        = (AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(reg_cl37_base_abilities_sp1))?
                                                              QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              } else {
                  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_SP1_LP_BASE_PAGE1r(pc, &reg_lp_sp1));
                  value = AN_X4_LP_SP1_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(reg_lp_sp1) ;
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  0x3 & (value>>10) ; 
                  an_ability_st->cl37_adv.an_duplex        = (0x1 & (value>>12))? QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              }
          }
          if(AN_X4_ABI_SP1_ENSr_CL37_BAM_ENABLEf_GET(reg_an_enb1)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37BAM ;

              PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r(pc, &reg_an_over1g_ability_sp1));
              if(AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_BAM_2P5GBASE_Xf_GET(reg_an_over1g_ability_sp1)) {
                  an_ability_st->cl37_adv.an_bam_speed |= (1 << QMOD_CL37_BAM_2p5GBASE_X) ;
              }
          }
      }
  } else if(sub_port == 2){
      PHYMOD_IF_ERR_RETURN (READ_AN_X4_ABI_SP2_ENSr(pc, &reg_an_enb2));
      if(AN_X4_ABI_SP2_ENSr_CL37_ENABLEf_GET(reg_an_enb2)) {
          an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37 ;

          PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr(pc, &reg_cl37_base_abilities_sp2));
          an_ability_st->cl37_adv.an_pause         = AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_GET(reg_cl37_base_abilities_sp2);
          an_ability_st->cl37_adv.an_duplex        = QMOD_AN_FULL_DUPLEX_SET  ;

          /* no combined modes yet */
          if(AN_X4_ABI_SP2_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb2)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_SGMII ;
              an_ability_st->cl37_adv.an_pause         = QMOD_SYMM_PAUSE ;

              if(AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(reg_cl37_base_abilities_sp2)) {
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(reg_cl37_base_abilities_sp2) ;
                  an_ability_st->cl37_adv.an_duplex        = (AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(reg_cl37_base_abilities_sp2))?
                                                              QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              } else {
                  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_SP2_LP_BASE_PAGE1r(pc, &reg_lp_sp2));
                  value = AN_X4_LP_SP2_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(reg_lp_sp2) ;
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  0x3 & (value>>10) ; 
                  an_ability_st->cl37_adv.an_duplex        = (0x1 & (value>>12))?  QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              }
          }
          if(AN_X4_ABI_SP2_ENSr_CL37_BAM_ENABLEf_GET(reg_an_enb2)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37BAM ;

              PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r(pc, &reg_an_over1g_ability_sp2));
              if(AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r_BAM_2P5GBASE_Xf_GET(reg_an_over1g_ability_sp2)) {
                  an_ability_st->cl37_adv.an_bam_speed |= (1 << QMOD_CL37_BAM_2p5GBASE_X) ;
              }
          }
      }
  } else if(sub_port == 3){
      PHYMOD_IF_ERR_RETURN (READ_AN_X4_ABI_SP3_ENSr(pc, &reg_an_enb3));
      if(AN_X4_ABI_SP3_ENSr_CL37_ENABLEf_GET(reg_an_enb3)) {
          an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37 ;

          PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr(pc, &reg_cl37_base_abilities_sp3));
          an_ability_st->cl37_adv.an_pause         =  AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_GET(reg_cl37_base_abilities_sp3);
          an_ability_st->cl37_adv.an_duplex        =  QMOD_AN_FULL_DUPLEX_SET;

          /* no combined modes yet */
          if(AN_X4_ABI_SP3_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb3)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_SGMII ;
              an_ability_st->cl37_adv.an_pause         = QMOD_SYMM_PAUSE ;

              if(AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(reg_cl37_base_abilities_sp3)) {
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(reg_cl37_base_abilities_sp3) ;
                  an_ability_st->cl37_adv.an_duplex        = (AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(reg_cl37_base_abilities_sp3))?
                                                                QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;       
              } else {
                  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_SP3_LP_BASE_PAGE1r(pc, &reg_lp_sp3));
                  value = AN_X4_LP_SP3_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(reg_lp_sp3) ;
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  0x3 & (value>>10) ; 
                  an_ability_st->cl37_adv.an_duplex        = (0x1 & (value>>12))? QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              }
          }
          if(AN_X4_ABI_SP3_ENSr_CL37_BAM_ENABLEf_GET(reg_an_enb3)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37BAM ;

              PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r(pc, &reg_an_over1g_ability_sp3));
              if(AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r_BAM_2P5GBASE_Xf_GET(reg_an_over1g_ability_sp3)) {
                  an_ability_st->cl37_adv.an_bam_speed |= (1 << QMOD_CL37_BAM_2p5GBASE_X) ;
              }
          }
      }
  } else { return PHYMOD_E_PARAM ; }
  return PHYMOD_E_NONE;
}

/**
@brief   To get autoneg advertisement registers.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   an_ability_st, with the remote abilities
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Upper layer software calls this function to get autoneg
         info. 
*/
   
int qmod_autoneg_remote_ability_get(PHYMOD_ST* pc, qmod_an_ability_t *an_ability_st, int sub_port)
{
  AN_X4_ABI_SP0_ENSr_t  reg_an_enb0;   
  AN_X4_ABI_SP1_ENSr_t  reg_an_enb1;
  AN_X4_ABI_SP2_ENSr_t  reg_an_enb2;
  AN_X4_ABI_SP3_ENSr_t  reg_an_enb3;
  AN_X4_LP_SP0_LP_BASE_PAGE1r_t    reg_lp_sp0 ;  /* 0xc248 */
  AN_X4_LP_SP1_LP_BASE_PAGE1r_t    reg_lp_sp1 ;
  AN_X4_LP_SP2_LP_BASE_PAGE1r_t    reg_lp_sp2 ;
  AN_X4_LP_SP3_LP_BASE_PAGE1r_t    reg_lp_sp3 ; 
  AN_X4_LP_SP0_LP_MP1024_UP1r_t    reg_bam37_sp0 ;  /* 0xc254 */
  AN_X4_LP_SP1_LP_MP1024_UP1r_t    reg_bam37_sp1 ;  /* 0xc254 */
  AN_X4_LP_SP2_LP_MP1024_UP1r_t    reg_bam37_sp2 ;  /* 0xc254 */
  AN_X4_LP_SP3_LP_MP1024_UP1r_t    reg_bam37_sp3 ;  /* 0xc254 */
  AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp0;
  AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp1;
  AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp2;  
  AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp3; 
  uint16_t                                    value = 0;
 
  an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_NONE ;
  an_ability_st->cl37_adv.cl37_sgmii_speed = 0 ;
  an_ability_st->cl37_adv.an_pause         = 0 ;
  an_ability_st->cl37_adv.an_bam_speed     = 0 ;
  an_ability_st->cl37_adv.an_duplex        = 0 ;

  AN_X4_ABI_SP0_ENSr_CLR(reg_an_enb0);
  AN_X4_ABI_SP1_ENSr_CLR(reg_an_enb1);
  AN_X4_ABI_SP2_ENSr_CLR(reg_an_enb2);
  AN_X4_ABI_SP3_ENSr_CLR(reg_an_enb3);
  AN_X4_LP_SP0_LP_BASE_PAGE1r_CLR(reg_lp_sp0) ;
  AN_X4_LP_SP1_LP_BASE_PAGE1r_CLR(reg_lp_sp1) ;
  AN_X4_LP_SP2_LP_BASE_PAGE1r_CLR(reg_lp_sp2) ;
  AN_X4_LP_SP3_LP_BASE_PAGE1r_CLR(reg_lp_sp3) ;
  AN_X4_LP_SP0_LP_MP1024_UP1r_CLR(reg_bam37_sp0) ;
  AN_X4_LP_SP1_LP_MP1024_UP1r_CLR(reg_bam37_sp1) ;
  AN_X4_LP_SP2_LP_MP1024_UP1r_CLR(reg_bam37_sp2) ;
  AN_X4_LP_SP3_LP_MP1024_UP1r_CLR(reg_bam37_sp3) ;
  AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp0);
  AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp1);
  AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp2);
  AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp3);

  if(sub_port == 0){
      PHYMOD_IF_ERR_RETURN (READ_AN_X4_ABI_SP0_ENSr(pc, &reg_an_enb0));
      
      if(AN_X4_ABI_SP0_ENSr_CL37_ENABLEf_GET(reg_an_enb0)) {
          /* no combined modes yet */
          PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_SP0_LP_BASE_PAGE1r(pc, &reg_lp_sp0));
          if(AN_X4_ABI_SP0_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb0)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_SGMII ;

              PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr(pc, &reg_cl37_base_abilities_sp0));

              if(AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(reg_cl37_base_abilities_sp0)) { /* master mode dicates a role */
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(reg_cl37_base_abilities_sp0) ;
                  an_ability_st->cl37_adv.an_duplex        = (AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(reg_cl37_base_abilities_sp0))?
                                                              QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              } else {
                  value = AN_X4_LP_SP0_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(reg_lp_sp0) ;
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  0x3 & (value>>10) ; 
                  an_ability_st->cl37_adv.an_duplex        = (0x1 & (value>>12))? QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              }
              an_ability_st->cl37_adv.an_pause         = QMOD_NO_PAUSE ;
          } else {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37 ;
              value = AN_X4_LP_SP0_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(reg_lp_sp0) ;
              an_ability_st->cl37_adv.an_duplex        = 0x3 & (value>>6) ;
              an_ability_st->cl37_adv.an_pause         = 0x3 & (value>>7) ;
          }
          if(AN_X4_ABI_SP0_ENSr_CL37_BAM_ENABLEf_GET(reg_an_enb0)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37BAM ;
              
              PHYMOD_IF_ERR_RETURN (READ_AN_X4_LP_SP0_LP_MP1024_UP1r(pc, &reg_bam37_sp0));

              if(AN_X4_LP_SP0_LP_MP1024_UP1r_LP_MP1024_UP1_PAGE_DATAf_GET(reg_bam37_sp0) & 0x1) {
                  an_ability_st->cl37_adv.an_bam_speed |= (1 << QMOD_CL37_BAM_2p5GBASE_X) ;
              }
          }
      }
  } else if(sub_port == 1){
      PHYMOD_IF_ERR_RETURN (READ_AN_X4_ABI_SP1_ENSr(pc, &reg_an_enb1));
      if(AN_X4_ABI_SP1_ENSr_CL37_ENABLEf_GET(reg_an_enb1)) {
          /* no combined modes yet */
          PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_SP1_LP_BASE_PAGE1r(pc, &reg_lp_sp1));
          if(AN_X4_ABI_SP1_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb1)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_SGMII ;

              PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr(pc, &reg_cl37_base_abilities_sp1));
              if(AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(reg_cl37_base_abilities_sp1)) {
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(reg_cl37_base_abilities_sp1) ;
                  an_ability_st->cl37_adv.an_duplex        = (AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(reg_cl37_base_abilities_sp1))?
                                                              QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              } else {
                  value = AN_X4_LP_SP1_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(reg_lp_sp1) ;
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  0x3 & (value>>10) ; 
                  an_ability_st->cl37_adv.an_duplex        = (0x1 & (value>>12))? QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              }
              an_ability_st->cl37_adv.an_pause         = QMOD_NO_PAUSE ;
          } else {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37 ;
              value = AN_X4_LP_SP1_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(reg_lp_sp1) ;
              an_ability_st->cl37_adv.an_duplex        = 0x3 & (value>>6) ;
              an_ability_st->cl37_adv.an_pause         = 0x3 & (value>>7) ;
          }
          if(AN_X4_ABI_SP1_ENSr_CL37_BAM_ENABLEf_GET(reg_an_enb1)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37BAM ;
              
              PHYMOD_IF_ERR_RETURN (READ_AN_X4_LP_SP1_LP_MP1024_UP1r(pc, &reg_bam37_sp1));

              if(AN_X4_LP_SP1_LP_MP1024_UP1r_LP_MP1024_UP1_PAGE_DATAf_GET(reg_bam37_sp1) & 0x1) {
                  an_ability_st->cl37_adv.an_bam_speed |= (1 << QMOD_CL37_BAM_2p5GBASE_X) ;
              }
          }
      }
  } else if(sub_port == 2){
      PHYMOD_IF_ERR_RETURN (READ_AN_X4_ABI_SP2_ENSr(pc, &reg_an_enb2));
      if(AN_X4_ABI_SP2_ENSr_CL37_ENABLEf_GET(reg_an_enb2)) {
          /* no combined modes yet */
          PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_SP2_LP_BASE_PAGE1r(pc, &reg_lp_sp2));
          if(AN_X4_ABI_SP2_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb2)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_SGMII ;
  
              PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr(pc, &reg_cl37_base_abilities_sp2));

              if(AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(reg_cl37_base_abilities_sp2)) {
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(reg_cl37_base_abilities_sp2) ;
                  an_ability_st->cl37_adv.an_duplex        = (AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(reg_cl37_base_abilities_sp2))?
                                                              QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              } else {
                  value = AN_X4_LP_SP2_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(reg_lp_sp2) ;
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  0x3 & (value>>10) ; 
                  an_ability_st->cl37_adv.an_duplex        = (0x1 & (value>>12))? QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              }
              an_ability_st->cl37_adv.an_pause         = QMOD_NO_PAUSE ;
          } else {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37 ;
              value = AN_X4_LP_SP2_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(reg_lp_sp2) ;  
              an_ability_st->cl37_adv.an_duplex        = 0x3 & (value>>6) ;
              an_ability_st->cl37_adv.an_pause         = 0x3 & (value>>7) ;
          }
          if(AN_X4_ABI_SP2_ENSr_CL37_BAM_ENABLEf_GET(reg_an_enb2)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37BAM ;
              
              PHYMOD_IF_ERR_RETURN (READ_AN_X4_LP_SP2_LP_MP1024_UP1r(pc, &reg_bam37_sp2));

              if(AN_X4_LP_SP2_LP_MP1024_UP1r_LP_MP1024_UP1_PAGE_DATAf_GET(reg_bam37_sp2) & 0x1) {
                  an_ability_st->cl37_adv.an_bam_speed |= (1 << QMOD_CL37_BAM_2p5GBASE_X) ;
              }
          }
      }
  } else if(sub_port == 3){
      PHYMOD_IF_ERR_RETURN (READ_AN_X4_ABI_SP3_ENSr(pc, &reg_an_enb3));
      if(AN_X4_ABI_SP3_ENSr_CL37_ENABLEf_GET(reg_an_enb3)) {
          /* no combined modes yet */
          PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_SP3_LP_BASE_PAGE1r(pc, &reg_lp_sp3));
          if(AN_X4_ABI_SP3_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb3)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_SGMII ;
  
              PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr(pc, &reg_cl37_base_abilities_sp3));
              if(AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(reg_cl37_base_abilities_sp3)) {
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(reg_cl37_base_abilities_sp3) ;
                  an_ability_st->cl37_adv.an_duplex        = (AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(reg_cl37_base_abilities_sp3))?
                                                              QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              } else {
                  value = AN_X4_LP_SP3_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(reg_lp_sp3) ;
                  an_ability_st->cl37_adv.cl37_sgmii_speed =  0x3 & (value>>10) ; 
                  an_ability_st->cl37_adv.an_duplex        = (0x1 & (value>>12))? QMOD_AN_FULL_DUPLEX_SET:QMOD_AN_HALF_DUPLEX_SET;
              }
              an_ability_st->cl37_adv.an_pause         = QMOD_NO_PAUSE ;
          } else {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37 ;
              value = AN_X4_LP_SP3_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(reg_lp_sp3) ;
              an_ability_st->cl37_adv.an_duplex        = 0x3 & (value>>6) ;
              an_ability_st->cl37_adv.an_pause         = 0x3 & (value>>7) ;
          }
          if(AN_X4_ABI_SP3_ENSr_CL37_BAM_ENABLEf_GET(reg_an_enb3)) {
              an_ability_st->cl37_adv.an_type          = QMOD_AN_MODE_CL37BAM ;
              
              PHYMOD_IF_ERR_RETURN (READ_AN_X4_LP_SP3_LP_MP1024_UP1r(pc, &reg_bam37_sp3));

              if(AN_X4_LP_SP3_LP_MP1024_UP1r_LP_MP1024_UP1_PAGE_DATAf_GET(reg_bam37_sp3) & 0x1) {
                  an_ability_st->cl37_adv.an_bam_speed |= (1 << QMOD_CL37_BAM_2p5GBASE_X) ;
              }
          }
      }
  } else { return PHYMOD_E_PARAM ; }
  
  return PHYMOD_E_NONE;
}

/*
@brief   Controls port RX squelch
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable is the control to  RX  squelch
@returns The value PHYMOD_E_NONE upon successful completion.
@details Controls port RX squelch
*/
int qmod_rx_squelch_set(PHYMOD_ST *pc, int enable)
{
  SIGDET_CTL1r_t sigdet_ctl;

  SIGDET_CTL1r_CLR(sigdet_ctl);

  if(enable){
     SIGDET_CTL1r_SIGNAL_DETECT_FRCf_SET(sigdet_ctl, 1);
     SIGDET_CTL1r_SIGNAL_DETECT_FRC_VALf_SET(sigdet_ctl, 0);
    /*  PHYMOD_IF_ERR_RETURN(qmod_rx_lane_control_set(pc, 0));*/
  } else {
     SIGDET_CTL1r_SIGNAL_DETECT_FRCf_SET(sigdet_ctl, 0);
     SIGDET_CTL1r_SIGNAL_DETECT_FRC_VALf_SET(sigdet_ctl, 0);
     /*PHYMOD_IF_ERR_RETURN(qmod_rx_lane_control_set(pc, 1)); */
  }
  PHYMOD_IF_ERR_RETURN(MODIFY_SIGDET_CTL1r(pc, sigdet_ctl));

  return PHYMOD_E_NONE;
}

/**
@brief   Controls port TX squelch
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable is the control to  TX  squelch
@returns The value PHYMOD_E_NONE upon successful completion.
@details Controls port TX squelch
*/
int qmod_tx_squelch_set(PHYMOD_ST *pc, int enable)
{
  TXFIR_MISC_CTL1r_t tx_misc_ctl;

  TXFIR_MISC_CTL1r_CLR(tx_misc_ctl);

  TXFIR_MISC_CTL1r_SDK_TX_DISABLEf_SET(tx_misc_ctl, enable);
  PHYMOD_IF_ERR_RETURN(MODIFY_TXFIR_MISC_CTL1r(pc, tx_misc_ctl));

  return PHYMOD_E_NONE;
}

/**
@brief   Controls port TX/RX squelch
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable is the control to  TX/RX  squelch. Enable=1 means enable the port,no squelch
@returns The value PHYMOD_E_NONE upon successful completion.
@details Controls port TX/RX squelch
*/
int qmod_port_enable_set(PHYMOD_ST *pc, int enable)
{
  if (enable)  {
      qmod_rx_squelch_set(pc, 0);
      qmod_tx_squelch_set(pc, 0);
  } else {
      qmod_rx_squelch_set(pc, 1);
      qmod_tx_squelch_set(pc, 1);
  }

  return PHYMOD_E_NONE;
}

/**
@brief   Get port TX squelch control settings
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   val Receiver for status of TX squelch
@returns The value PHYMOD_E_NONE upon successful completion.
@details Get port TX squelch control settings
*/
int qmod_tx_squelch_get(PHYMOD_ST *pc, int *val)
{
  TXFIR_MISC_CTL1r_t tx_misc_ctl;

  TXFIR_MISC_CTL1r_CLR(tx_misc_ctl);

  PHYMOD_IF_ERR_RETURN(READ_TXFIR_MISC_CTL1r(pc, &tx_misc_ctl));
  *val = TXFIR_MISC_CTL1r_SDK_TX_DISABLEf_GET(tx_misc_ctl);

  return PHYMOD_E_NONE;
}

/**
@brief   Gets port RX squelch settings
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   val Receiver for status of  RX  squelch
@returns The value PHYMOD_E_NONE upon successful completion.
@details Gets port RX squelch settings
*/
int qmod_rx_squelch_get(PHYMOD_ST *pc, int *val)
{
  SIGDET_CTL1r_t sigdet_ctl;

  SIGDET_CTL1r_CLR(sigdet_ctl);

  PHYMOD_IF_ERR_RETURN(READ_SIGDET_CTL1r(pc, &sigdet_ctl));
  *val = SIGDET_CTL1r_SIGNAL_DETECT_FRCf_GET(sigdet_ctl);

  return PHYMOD_E_NONE;
}

/**
@brief   Get port TX/RX squelch Settings
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   tx_enable - get the TX  squelch settings
@param   rx_enable - get the RX  squelch settings
@returns The value PHYMOD_E_NONE upon successful completion.
@details Controls port TX/RX squelch
*/
int qmod_port_enable_get(PHYMOD_ST *pc, int *tx_enable, int *rx_enable)
{

  qmod_rx_squelch_get(pc, rx_enable);
  qmod_tx_squelch_get(pc, tx_enable);

  return PHYMOD_E_NONE;
}



/**
@brief   Controls setting/resetting of autoneg ability and enabling/disabling
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   an_control structure with AN controls
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.

This function programs auto-negotiation (AN) modes for the TSCE. It can
enable/disable clause37/clause73/BAM autonegotiation capabilities. Call this
function once for combo mode and once per lane in independent lane mode.

The autonegotiation mode is controlled by setting specific bits of
#PHYMOD_ST::an_type. 

The autonegotiation mode is indicated by setting an_type with

\li QMOD_AN_NONE   (disable AN)
\li QMOD_CL73
\li QMOD_CL73_BAM
\li QMOD_HPAM
*/

#ifdef _DV_TB_
int qmod_an_abilities_enables(PHYMOD_ST* pc,int sub_port)
{
  uint16_t cl37_bam_enable  ;
  uint16_t cl37_sgmii_enable, cl37_enable,qsgmii_enable;
/*  uint16_t cl37_bam_code, over1g_ability, over1g_page_count;*/
/*  uint16_t sgmii_full_duplex ;*/
  uint16_t cl37_restart;
/*  int    no_an ;*/
  /* UC_COMMAND3r_t reg_com3; */
  AN_X4_ABI_SP0_ENSr_t  reg_an_enb0;
  AN_X4_ABI_SP1_ENSr_t  reg_an_enb1;
  AN_X4_ABI_SP2_ENSr_t  reg_an_enb2;
  AN_X4_ABI_SP3_ENSr_t  reg_an_enb3;
  SC_X4_SC_AN_SUBP_CTLr_t reg_see_sc;
  QMOD_DBG_IN_FUNC_INFO(pc);

/* TSCE_COMMANDr_t reg_com;
*/
 
/*  no_an                         = 0 ; */

  cl37_bam_enable               = 0x0;
  cl37_sgmii_enable             = 0x0;
  qsgmii_enable                 = 0x0;
  cl37_enable                   = 0x0;
  cl37_restart                  = 0x0;
/*  cl37_bam_code                 = 0x0;*/
/*  over1g_ability                = 0x0;*/
/*  over1g_page_count             = 0x0;*/


  if (pc->sc_mode == QMOD_SC_MODE_AN_CL37) {
    cl37_restart                = pc->cl37_an_restart;
    cl37_enable                 = pc->cl37_an_en;
    cl37_bam_enable             = pc->cl37_bam_en;
    cl37_sgmii_enable           = pc->cl37_sgmii_en;
    qsgmii_enable               = pc->qsgmii_mode;
/*    cl37_bam_code               = pc->cl37_bam_code & 0x1ff;*/
/*    over1g_ability              = pc->cl37_bam_ovr1g_en & 1;*/
/*    over1g_page_count           = pc->cl37_bam_ovr1g_pgcnt & 3;  */ /* PHY-863: =1: 5 pages; =0: 3 pages; WC uses 3 pages */
/*    sgmii_full_duplex           = pc->cl37_sgmii_duplex & 1;*/
  } else {
/*     no_an = 1 ; */
     return PHYMOD_E_FAIL;
  }

  if(pc->tsc_port_touched == 0){
   SC_X4_SC_AN_SUBP_CTLr_CLR(reg_see_sc);
   SC_X4_SC_AN_SUBP_CTLr_SET(reg_see_sc,pc->sub_port);
   PHYMOD_IF_ERR_RETURN (MODIFY_SC_X4_SC_AN_SUBP_CTLr(pc, reg_see_sc)); 
  }

    if(cl37_restart == 0){
      if(sub_port == 0){
        AN_X4_ABI_SP0_ENSr_CLR(reg_an_enb0);
        AN_X4_ABI_SP0_ENSr_CL37_ENABLEf_SET(reg_an_enb0,0);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP0_ENSr(pc, reg_an_enb0)); 
      }else if(sub_port == 1){
        AN_X4_ABI_SP1_ENSr_CLR(reg_an_enb1);
        AN_X4_ABI_SP1_ENSr_CL37_ENABLEf_SET(reg_an_enb1,0);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP1_ENSr(pc, reg_an_enb1)); 
      }else if(sub_port == 2){
        AN_X4_ABI_SP2_ENSr_CLR(reg_an_enb2);
        AN_X4_ABI_SP2_ENSr_CL37_ENABLEf_SET(reg_an_enb2,0);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP2_ENSr(pc, reg_an_enb2)); 
      }else if(sub_port == 3){
        AN_X4_ABI_SP3_ENSr_CLR(reg_an_enb3);
        AN_X4_ABI_SP3_ENSr_CL37_ENABLEf_SET(reg_an_enb3,0);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP3_ENSr(pc, reg_an_enb3)); 
      }
    }

    /*Setting X4 abilities*/
    if(sub_port == 0){
      AN_X4_ABI_SP0_ENSr_CLR(reg_an_enb0);
      AN_X4_ABI_SP0_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb0,cl37_restart);
      AN_X4_ABI_SP0_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb0,pc->sgmii_to_cl37_auto_enable);
      AN_X4_ABI_SP0_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb0,pc->cl37_bam_to_sgmii_auto_enable);
      AN_X4_ABI_SP0_ENSr_CL37_ENABLEf_SET(reg_an_enb0,cl37_enable);
      AN_X4_ABI_SP0_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb0,cl37_sgmii_enable);
      AN_X4_ABI_SP0_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb0,cl37_bam_enable);
      AN_X4_ABI_SP0_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb0,pc->disable_rf_report);
      AN_X4_ABI_SP0_ENSr_QSGMII_ENf_SET(reg_an_enb0,qsgmii_enable);
      PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP0_ENSr(pc, reg_an_enb0)); 
    }else if(sub_port == 1){
      AN_X4_ABI_SP1_ENSr_CLR(reg_an_enb1);
      AN_X4_ABI_SP1_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb1,cl37_restart);
      AN_X4_ABI_SP1_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb1,pc->sgmii_to_cl37_auto_enable);
      AN_X4_ABI_SP1_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb1,pc->cl37_bam_to_sgmii_auto_enable);
      AN_X4_ABI_SP1_ENSr_CL37_ENABLEf_SET(reg_an_enb1,cl37_enable);
      AN_X4_ABI_SP1_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb1,cl37_sgmii_enable);
      AN_X4_ABI_SP1_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb1,cl37_bam_enable);
      AN_X4_ABI_SP1_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb1,pc->disable_rf_report);
      AN_X4_ABI_SP1_ENSr_QSGMII_ENf_SET(reg_an_enb1,qsgmii_enable);
      PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP1_ENSr(pc, reg_an_enb1)); 
    }else if(sub_port == 2){
      AN_X4_ABI_SP2_ENSr_CLR(reg_an_enb2);
      AN_X4_ABI_SP2_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb2,cl37_restart);
      AN_X4_ABI_SP2_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb2,pc->sgmii_to_cl37_auto_enable);
      AN_X4_ABI_SP2_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb2,pc->cl37_bam_to_sgmii_auto_enable);
      AN_X4_ABI_SP2_ENSr_CL37_ENABLEf_SET(reg_an_enb2,cl37_enable);
      AN_X4_ABI_SP2_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb2,cl37_sgmii_enable);
      AN_X4_ABI_SP2_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb2,cl37_bam_enable);
      AN_X4_ABI_SP2_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb2,pc->disable_rf_report);
      AN_X4_ABI_SP2_ENSr_QSGMII_ENf_SET(reg_an_enb2,qsgmii_enable);
      PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP2_ENSr(pc, reg_an_enb2)); 
    }else if(sub_port == 3){
      AN_X4_ABI_SP3_ENSr_CLR(reg_an_enb3);
      AN_X4_ABI_SP3_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb3,cl37_restart);
      AN_X4_ABI_SP3_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb3,pc->sgmii_to_cl37_auto_enable);
      AN_X4_ABI_SP3_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb3,pc->cl37_bam_to_sgmii_auto_enable);
      AN_X4_ABI_SP3_ENSr_CL37_ENABLEf_SET(reg_an_enb3,cl37_enable);
      AN_X4_ABI_SP3_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb3,cl37_sgmii_enable);
      AN_X4_ABI_SP3_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb3,cl37_bam_enable);
      AN_X4_ABI_SP3_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb3,pc->disable_rf_report);
      AN_X4_ABI_SP3_ENSr_QSGMII_ENf_SET(reg_an_enb3,qsgmii_enable);
      PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP3_ENSr(pc, reg_an_enb3)); 
    }

    if(cl37_restart){
      if(sub_port == 0){
        AN_X4_ABI_SP0_ENSr_CLR(reg_an_enb0);
        AN_X4_ABI_SP0_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb0,0);
        AN_X4_ABI_SP0_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb0,pc->sgmii_to_cl37_auto_enable);
        AN_X4_ABI_SP0_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb0,pc->cl37_bam_to_sgmii_auto_enable);
        AN_X4_ABI_SP0_ENSr_CL37_ENABLEf_SET(reg_an_enb0,cl37_enable);
        AN_X4_ABI_SP0_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb0,cl37_sgmii_enable);
        AN_X4_ABI_SP0_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb0,cl37_bam_enable);
        AN_X4_ABI_SP0_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb0,pc->disable_rf_report);
        AN_X4_ABI_SP0_ENSr_QSGMII_ENf_SET(reg_an_enb0,qsgmii_enable);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP0_ENSr(pc, reg_an_enb0)); 
      }else if(sub_port == 1){
        AN_X4_ABI_SP1_ENSr_CLR(reg_an_enb1);
        AN_X4_ABI_SP1_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb1,0);
        AN_X4_ABI_SP1_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb1,pc->sgmii_to_cl37_auto_enable);
        AN_X4_ABI_SP1_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb1,pc->cl37_bam_to_sgmii_auto_enable);
        AN_X4_ABI_SP1_ENSr_CL37_ENABLEf_SET(reg_an_enb1,cl37_enable);
        AN_X4_ABI_SP1_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb1,cl37_sgmii_enable);
        AN_X4_ABI_SP1_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb1,cl37_bam_enable);
        AN_X4_ABI_SP1_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb1,pc->disable_rf_report);
        AN_X4_ABI_SP1_ENSr_QSGMII_ENf_SET(reg_an_enb1,qsgmii_enable);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP1_ENSr(pc, reg_an_enb1)); 
      }else if(sub_port == 2){
        AN_X4_ABI_SP2_ENSr_CLR(reg_an_enb2);
        AN_X4_ABI_SP2_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb2,0);
        AN_X4_ABI_SP2_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb2,pc->sgmii_to_cl37_auto_enable);
        AN_X4_ABI_SP2_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb2,pc->cl37_bam_to_sgmii_auto_enable);
        AN_X4_ABI_SP2_ENSr_CL37_ENABLEf_SET(reg_an_enb2,cl37_enable);
        AN_X4_ABI_SP2_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb2,cl37_sgmii_enable);
        AN_X4_ABI_SP2_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb2,cl37_bam_enable);
        AN_X4_ABI_SP2_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb2,pc->disable_rf_report);
        AN_X4_ABI_SP2_ENSr_QSGMII_ENf_SET(reg_an_enb2,qsgmii_enable);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP2_ENSr(pc, reg_an_enb2)); 
      }else if(sub_port == 3){
        AN_X4_ABI_SP3_ENSr_CLR(reg_an_enb3);
        AN_X4_ABI_SP3_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb3,0);
        AN_X4_ABI_SP3_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb3,pc->sgmii_to_cl37_auto_enable);
        AN_X4_ABI_SP3_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb3,pc->cl37_bam_to_sgmii_auto_enable);
        AN_X4_ABI_SP3_ENSr_CL37_ENABLEf_SET(reg_an_enb3,cl37_enable);
        AN_X4_ABI_SP3_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb3,cl37_sgmii_enable);
        AN_X4_ABI_SP3_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb3,cl37_bam_enable);
        AN_X4_ABI_SP3_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb3,pc->disable_rf_report);
        AN_X4_ABI_SP3_ENSr_QSGMII_ENf_SET(reg_an_enb3,qsgmii_enable);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP3_ENSr(pc, reg_an_enb3)); 
      }
    }
  return PHYMOD_E_NONE;
}
int qmod_an_cl37_bam_abilities(PHYMOD_ST* pc,int sub_port)
{
  uint16_t cl37_bam_code, over1g_ability, over1g_page_count;
  AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_t reg_an_cl37_bam_abilities_sp0;
  AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_t reg_an_cl37_bam_abilities_sp1;
  AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_t reg_an_cl37_bam_abilities_sp2;
  AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_t reg_an_cl37_bam_abilities_sp3;
  QMOD_DBG_IN_FUNC_INFO(pc);

/* TSCE_COMMANDr_t reg_com;
*/
 
  cl37_bam_code                 = 0x0;
  over1g_ability                = 0x0;
  over1g_page_count             = 0x0;


  if (pc->sc_mode == QMOD_SC_MODE_AN_CL37) {
    cl37_bam_code               = pc->cl37_bam_code & 0x1ff;
    over1g_ability              = pc->cl37_bam_ovr1g_en & 1;
    over1g_page_count           = pc->cl37_bam_ovr1g_pgcnt & 3;   /* PHY-863: =1: 5 pages; =0: 3 pages; WC uses 3 pages */
  } else {
     return PHYMOD_E_FAIL;
  }

 
  /* cl37 bam abilities 0xc182 */
    if(sub_port == 0){
      AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_CLR(reg_an_cl37_bam_abilities_sp0);
      AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_an_cl37_bam_abilities_sp0, over1g_page_count, 1);
      AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_an_cl37_bam_abilities_sp0, over1g_ability, 1);
      AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_an_cl37_bam_abilities_sp0, cl37_bam_code, 1);
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr(pc, reg_an_cl37_bam_abilities_sp0));
    }else if(sub_port == 1){
      AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_CLR(reg_an_cl37_bam_abilities_sp1);
      AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_an_cl37_bam_abilities_sp1, over1g_page_count);
      AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_an_cl37_bam_abilities_sp1, over1g_ability);
      AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_an_cl37_bam_abilities_sp1, cl37_bam_code);
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr(pc, reg_an_cl37_bam_abilities_sp1));
    }else if(sub_port == 2){
      AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_CLR(reg_an_cl37_bam_abilities_sp2);
      AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_an_cl37_bam_abilities_sp2, over1g_page_count);
      AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_an_cl37_bam_abilities_sp2, over1g_ability);
      AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_an_cl37_bam_abilities_sp2, cl37_bam_code);
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr(pc, reg_an_cl37_bam_abilities_sp2));
    }else if(sub_port == 3){
      AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_CLR(reg_an_cl37_bam_abilities_sp3);
      AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_an_cl37_bam_abilities_sp3, over1g_page_count);
      AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_an_cl37_bam_abilities_sp3, over1g_ability);
      AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_an_cl37_bam_abilities_sp3, cl37_bam_code);
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr(pc, reg_an_cl37_bam_abilities_sp3));
    }

  return PHYMOD_E_NONE;
}

int qmod_autoneg_control(PHYMOD_ST* pc)
{

/*
  uint16_t data ;
  uint16_t an_setup_enable, num_advertised_lanes, cl37_bam_enable  ;
  uint16_t cl37_sgmii_enable, cl37_enable;
  uint16_t next_page  ;
  uint16_t cl37_next_page,  cl37_full_duplex; 
  uint16_t sgmii_full_duplex ;
  uint16_t cl37_bam_code, over1g_ability, over1g_page_count;
  uint16_t cl37_restart;
  int    no_an ;
*/
  /*AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_t reg_an_cl37_bam_abilities;*/
  /* UC_COMMAND3r_t reg_com3; */
  /*AN_X4_ABI_SP1_ENSr_t  reg_an_enb;*/
  QMOD_DBG_IN_FUNC_INFO(pc);

/* TSCE_COMMANDr_t reg_com;
*/
 
/*
  no_an                         = 0 ; 

  an_setup_enable               = 0x1;
  num_advertised_lanes          = 0x2;   
  cl37_bam_enable               = 0x0;
  cl37_sgmii_enable             = 0x0;
  cl37_enable                   = 0x0;
  next_page                     = 0x0;
  cl37_next_page                = 0x0;
  cl37_full_duplex              = 0x1; 
  sgmii_full_duplex             = 0x1;
  cl37_bam_code                 = 0x0;
  over1g_ability                = 0x0;
  over1g_page_count             = 0x0;
  cl37_restart                  = 0x0;

  num_advertised_lanes = pc->no_of_lanes;
*/

  if (pc->sc_mode == QMOD_SC_MODE_AN_CL37) {
/*
    cl37_restart                = pc->cl37_an_restart;
    cl37_enable                 = pc->cl37_an_en;
    cl37_bam_enable             = pc->cl37_bam_en;
    cl37_sgmii_enable           = pc->cl37_sgmii_en;
    cl37_bam_code               = pc->cl37_bam_code & 0x1ff;
    over1g_ability              = pc->cl37_bam_ovr1g_en & 1;
    over1g_page_count           = pc->cl37_bam_ovr1g_pgcnt & 3;   // PHY-863: =1: 5 pages; =0: 3 pages; WC uses 3 pages 
    sgmii_full_duplex           = pc->cl37_sgmii_duplex & 1;
    cl37_next_page              = pc->cl37_an_np & 1;
*/
  } else {
/*     no_an = 1 ; */
     return PHYMOD_E_FAIL;
  }

 
  /* cl37 bam abilities 0xc182 */
  /*
  AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_CLR(reg_an_cl37_bam_abilities);
  AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_an_cl37_bam_abilities, over1g_page_count);
  AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_an_cl37_bam_abilities, over1g_ability);
  AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_an_cl37_bam_abilities, cl37_bam_code);

  PHYMOD_IF_ERR_RETURN 
         (MODIFY_AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr(pc, reg_an_cl37_bam_abilities));
  */
  qmod_an_cl37_bam_abilities(pc,pc->sub_port);
  /* disable ecc of uC 0xffcc */
  /* 
      TSCE_COMMAND3r_SET(reg_com3, 0x0c04);
      PHYMOD_IF_ERR_RETURN (TSCE_WRITE_COMMAND3r(pc, reg_com3)); 
  */

  /* enable micro_controller 0xffc2 */
  /*
    TSCE_READ_COMMANDr(pc, &reg_com);
    TSCE_COMMANDr_MICRO_MDIO_DW8051_RESET_Nf_SET(reg_com,1);
                UC_COMMAND_MDIO_UC_RESET_N_MASK));
  */ 
   /* set cl73 nonce set 0xc340 */
  /* 0x924a for PHY_892 TBD*/ 
  /*
  if((no_an==0)&&(pc->port_type==TSCMOD_SINGLE_PORT)) {
     data = 0x1a0a ; // 100ms TX_RESET_TIMER_PERIOD 
     data = 0xd05 ;  // 50ms 
  } else {
     data = 0x5 ;
  }
    PHYMOD_IF_ERR_RETURN 
    TSC_REG_WRITE(pc->unit, pc, 0x00, 0x0000924a, data)) ;
  */

    /*Disabling AN if already enabled required for dynamic speed change tests*/

    /*Setting X4 abilities*/
    qmod_an_abilities_enables(pc,pc->sub_port);
    /*
    AN_X4_ABI_SP1_ENSr_CLR(reg_an_enb);

    AN_X4_ABI_SP1_ENSr_CL37_ENABLEf_SET(reg_an_enb,0);
    PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP1_ENSr(pc, reg_an_enb)); 

    AN_X4_ABI_SP1_ENSr_CLR(reg_an_enb);
    AN_X4_ABI_SP1_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb,cl37_restart);
    AN_X4_ABI_SP1_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb,pc->sgmii_to_cl37_auto_enable);
    AN_X4_ABI_SP1_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb,pc->cl37_bam_to_sgmii_auto_enable);
    AN_X4_ABI_SP1_ENSr_CL37_ENABLEf_SET(reg_an_enb,cl37_enable);
    AN_X4_ABI_SP1_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb,cl37_sgmii_enable);
    AN_X4_ABI_SP1_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb,cl37_bam_enable);
    AN_X4_ABI_SP1_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb,pc->disable_rf_report);

    PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP1_ENSr(pc, reg_an_enb)); 
    */

  return PHYMOD_E_NONE;
} /* qmod_autoneg_control */

#endif

#ifdef _SDK_QMOD_

int qmod_an_cl37_bam_abilities(PHYMOD_ST* pc, qmod_an_ability_t *an_ability, int sub_port)
{

  uint16_t cl37_bam_code, over1g_ability, over1g_page_count;
  AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_t reg_an_cl37_bam_abilities_sp0;
  AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_t reg_an_cl37_bam_abilities_sp1;
  AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_t reg_an_cl37_bam_abilities_sp2;
  AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_t reg_an_cl37_bam_abilities_sp3;
  AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_t  reg_an_cl37_base_sp0;
  AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_t  reg_an_cl37_base_sp1;
  AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_t  reg_an_cl37_base_sp2;
  AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_t  reg_an_cl37_base_sp3;
  AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r_t reg_an_over1g_ability_sp0;
  AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_t reg_an_over1g_ability_sp1;
  AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r_t reg_an_over1g_ability_sp2;
  AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r_t reg_an_over1g_ability_sp3;

  QMOD_DBG_IN_FUNC_INFO(pc);

/* TSCE_COMMANDr_t reg_com;
*/
 
  cl37_bam_code                 = 0x1;
  over1g_ability                = 0x1;
  over1g_page_count             = 0x0;


 
  /* cl37 bam abilities 0xc182 */
    if(sub_port == 0){
      AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r_CLR(reg_an_over1g_ability_sp0);
      AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r_BAM_2P5GBASE_Xf_SET(reg_an_over1g_ability_sp0, 1);
      MODIFY_AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r(pc, reg_an_over1g_ability_sp0); 
      AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CLR(reg_an_cl37_base_sp0);
      AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_an_cl37_base_sp0, 1);
      AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(reg_an_cl37_base_sp0, 1);
      AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_CLR(reg_an_cl37_bam_abilities_sp0);
      AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_an_cl37_bam_abilities_sp0, over1g_page_count);
      AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_an_cl37_bam_abilities_sp0, over1g_ability);
      AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_an_cl37_bam_abilities_sp0, cl37_bam_code);
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr(pc, reg_an_cl37_bam_abilities_sp0));
      MODIFY_AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr(pc, reg_an_cl37_base_sp0); 
    }else if(sub_port == 1){
      AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_CLR(reg_an_over1g_ability_sp1);
      AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_BAM_2P5GBASE_Xf_SET(reg_an_over1g_ability_sp1, 1);
      MODIFY_AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r(pc, reg_an_over1g_ability_sp1); 
      AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CLR(reg_an_cl37_base_sp1);
      AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_an_cl37_base_sp1, 1);
      AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(reg_an_cl37_base_sp1, 1);
      AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_CLR(reg_an_cl37_bam_abilities_sp1);
      AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_an_cl37_bam_abilities_sp1, over1g_page_count);
      AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_an_cl37_bam_abilities_sp1, over1g_ability);
      AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_an_cl37_bam_abilities_sp1, cl37_bam_code);
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr(pc, reg_an_cl37_bam_abilities_sp1));
      MODIFY_AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr(pc, reg_an_cl37_base_sp1); 
    }else if(sub_port == 2){
      AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r_CLR(reg_an_over1g_ability_sp2);
      AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r_BAM_2P5GBASE_Xf_SET(reg_an_over1g_ability_sp2, 1);
      MODIFY_AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r(pc, reg_an_over1g_ability_sp2); 
      AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CLR(reg_an_cl37_base_sp2);
      AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_an_cl37_base_sp2, 1);
      AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(reg_an_cl37_base_sp2, 1);
      AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_CLR(reg_an_cl37_bam_abilities_sp2);
      AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_an_cl37_bam_abilities_sp2, over1g_page_count);
      AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_an_cl37_bam_abilities_sp2, over1g_ability);
      AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_an_cl37_bam_abilities_sp2, cl37_bam_code);
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr(pc, reg_an_cl37_bam_abilities_sp2));
      MODIFY_AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr(pc, reg_an_cl37_base_sp2); 
    }else if(sub_port == 3){
      AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r_CLR(reg_an_over1g_ability_sp3);
      AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r_BAM_2P5GBASE_Xf_SET(reg_an_over1g_ability_sp3, 1);
      MODIFY_AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r(pc, reg_an_over1g_ability_sp3); 
      AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CLR(reg_an_cl37_base_sp3);
      AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_an_cl37_base_sp3, 1);
      AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(reg_an_cl37_base_sp3, 1);
      AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_CLR(reg_an_cl37_bam_abilities_sp3);
      AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_an_cl37_bam_abilities_sp3, over1g_page_count);
      AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_an_cl37_bam_abilities_sp3, over1g_ability);
      AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_an_cl37_bam_abilities_sp3, cl37_bam_code);
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr(pc, reg_an_cl37_bam_abilities_sp3));
      MODIFY_AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr(pc, reg_an_cl37_base_sp3); 
    }
  return PHYMOD_E_NONE;
}

int qmod_autoneg_cl37_base_abilities(PHYMOD_ST* pc, qmod_an_ability_t *an_ability, int sub_port)
{
 AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp0;
 AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp1;
 AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp2;
 AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities_sp3;

 if(sub_port == 0){
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp0);
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_SET(reg_cl37_base_abilities_sp0, (an_ability->cl37_adv.cl37_sgmii_speed & 3));
   if(an_ability->cl37_adv.an_duplex & QMOD_AN_HALF_DUPLEX_SET) {
       AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp0, 0);
   } else {
       AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp0, 1);
   }
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_SET(reg_cl37_base_abilities_sp0, (an_ability->cl37_adv.an_pause & 3));
  /* AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_np & 1)); */
  /* AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_sgmii_master_mode & 1)); */

   if (an_ability->cl37_adv.an_bam_speed) {
        AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp0, 0);
        AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp0, 0);     
   } else {
        AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp0, 1);
        /*
        AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp0, (pc->cl37_an_restart | pc->cl37_sw_restart_reset_disable));     
        */
        AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp0, 1);     
   }
   /* AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_AN_DISABLE_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_disable_reset_disable & 1)); */
   AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_AN_DISABLE_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp0, 1);
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities_sp0));
 } else if(sub_port == 1){
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp1);
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_SET(reg_cl37_base_abilities_sp1, (an_ability->cl37_adv.cl37_sgmii_speed & 3));
   if(an_ability->cl37_adv.an_duplex & QMOD_AN_HALF_DUPLEX_SET) {
       AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp1, 0);
   } else {
       AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp1, 1);
   }
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_SET(reg_cl37_base_abilities_sp1, (an_ability->cl37_adv.an_pause & 3));
  /* AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_np & 1)); */
  /* AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_sgmii_master_mode & 1)); */

   if (an_ability->cl37_adv.an_bam_speed) {
        AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp1, 0);
        AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp1, 0);     
   } else {
        AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp1, 1);
        /*
        AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp0, (pc->cl37_an_restart | pc->cl37_sw_restart_reset_disable));     
        */
        AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp1, 1);     
   }
   /* AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_AN_DISABLE_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_disable_reset_disable & 1)); */
   AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_AN_DISABLE_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp1, 1);
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities_sp1));
 } else if (sub_port == 2) {
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp2);
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_SET(reg_cl37_base_abilities_sp2, (an_ability->cl37_adv.cl37_sgmii_speed & 3));
   if(an_ability->cl37_adv.an_duplex & QMOD_AN_HALF_DUPLEX_SET) {
       AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp2, 0);
   } else {
       AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp2, 1);
   }
       
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_SET(reg_cl37_base_abilities_sp2, (an_ability->cl37_adv.an_pause & 3));
  /* AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_np & 1)); */
  /* AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_sgmii_master_mode & 1)); */

   if (an_ability->cl37_adv.an_bam_speed) {
        AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp2, 0);
        AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp2, 0);     
   } else {
        AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp2, 1);
        /*
        AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp0, (pc->cl37_an_restart | pc->cl37_sw_restart_reset_disable));     
        */
        AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp2, 1);     
   }
   /* AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_AN_DISABLE_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_disable_reset_disable & 1)); */
   AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_AN_DISABLE_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp2, 1);
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities_sp2));
 } else if (sub_port == 3) {
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities_sp3);
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_SET(reg_cl37_base_abilities_sp3, (an_ability->cl37_adv.cl37_sgmii_speed & 3));
   if(an_ability->cl37_adv.an_duplex & QMOD_AN_HALF_DUPLEX_SET) {
       AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp3, 0);
   } else {
       AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities_sp3, 1);
   }
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_SET(reg_cl37_base_abilities_sp3, (an_ability->cl37_adv.an_pause & 3));
  /* AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_np & 1)); */
  /* AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_sgmii_master_mode & 1)); */

   if (an_ability->cl37_adv.an_bam_speed) {
        AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp3, 0);
        AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp3, 0);     
   } else {
        AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp3, 1);
        /*
        AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp0, (pc->cl37_an_restart | pc->cl37_sw_restart_reset_disable));     
        */
        AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities_sp3, 1);     
   }
   /* AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_AN_DISABLE_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp0, (pc->cl37_an_disable_reset_disable & 1)); */
   AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_AN_DISABLE_RESET_DISABLEf_SET(reg_cl37_base_abilities_sp3, 1);
   PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities_sp3));
 }

  return PHYMOD_E_NONE;
}

int qmod_autoneg_set(PHYMOD_ST* pc,
    qmod_an_ability_t *an_ability_st, int sub_port) /* AUTONEG_SET */
{
   qmod_autoneg_cl37_base_abilities(pc, an_ability_st, sub_port);
   qmod_an_cl37_bam_abilities(pc, an_ability_st, sub_port);
   return PHYMOD_E_NONE;
}


int qmod_autoneg_control(PHYMOD_ST* pc, qmod_an_control_t *an_control, int sub_port)
{
  uint16_t cl37_bam_enable  ;
  uint16_t cl37_sgmii_enable, cl37_enable,qsgmii_enable;
/*  uint16_t cl37_bam_code, over1g_ability, over1g_page_count;*/
/*  uint16_t sgmii_full_duplex ;*/
  uint16_t cl37_restart;
/*  int    no_an ;*/
  /* UC_COMMAND3r_t reg_com3; */
  AN_X4_ABI_SP0_ENSr_t  reg_an_enb0;
  AN_X4_ABI_SP1_ENSr_t  reg_an_enb1;
  AN_X4_ABI_SP2_ENSr_t  reg_an_enb2;
  AN_X4_ABI_SP3_ENSr_t  reg_an_enb3;
  AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_t  cl37_base_ability;
  /* SC_X4_SC_AN_SUBP_CTLr_t reg_see_sc; */
  QMOD_DBG_IN_FUNC_INFO(pc);
  AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CLR(cl37_base_ability);

/* TSCE_COMMANDr_t reg_com;
*/
 
/*  no_an                         = 0 ; */

  cl37_bam_enable               = 0x0;
  cl37_sgmii_enable             = 0x0;
  qsgmii_enable                 = 0x0;
  cl37_enable                   = 0x0;
  cl37_restart                  = 0x0;
/*  cl37_bam_code                 = 0x0;*/
/*  over1g_ability                = 0x0;*/
/*  over1g_page_count             = 0x0;*/


  if (an_control->an_type == QMOD_AN_MODE_CL37) {
    cl37_restart                = an_control->enable;
    cl37_enable                 = an_control->enable;
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(cl37_base_ability, 1);
    MODIFY_AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr(pc, cl37_base_ability); 
  } else if (an_control->an_type == QMOD_AN_MODE_SGMII) { 
    cl37_sgmii_enable           = an_control->enable;
    cl37_restart                = an_control->enable;
    cl37_enable                 = an_control->enable;
  } else {
    cl37_bam_enable             = 1;
    cl37_restart                = an_control->enable;
    cl37_enable                 = an_control->enable;
  }

  /* next check if we need to enable qsgmii mode */
  if (PHYMOD_ACC_F_QMODE_GET(pc)) {
    qsgmii_enable = 1;
    cl37_sgmii_enable           = an_control->enable;
  }

  /* if cl37 is enabled, then we need to enable 

  if(pc->tsc_port_touched == 0){
   SC_X4_SC_AN_SUBP_CTLr_CLR(reg_see_sc);
   SC_X4_SC_AN_SUBP_CTLr_SET(reg_see_sc,pc->sub_port);
   PHYMOD_IF_ERR_RETURN (MODIFY_SC_X4_SC_AN_SUBP_CTLr(pc, reg_see_sc)); 
  }

    if(cl37_restart == 0){
      if(sub_port == 0){
        AN_X4_ABI_SP0_ENSr_CLR(reg_an_enb0);
        AN_X4_ABI_SP0_ENSr_CL37_ENABLEf_SET(reg_an_enb0,0);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP0_ENSr(pc, reg_an_enb0)); 
      }else if(sub_port == 1){
        AN_X4_ABI_SP1_ENSr_CLR(reg_an_enb1);
        AN_X4_ABI_SP1_ENSr_CL37_ENABLEf_SET(reg_an_enb1,0);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP1_ENSr(pc, reg_an_enb1)); 
      }else if(sub_port == 2){
        AN_X4_ABI_SP2_ENSr_CLR(reg_an_enb2);
        AN_X4_ABI_SP2_ENSr_CL37_ENABLEf_SET(reg_an_enb2,0);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP2_ENSr(pc, reg_an_enb2)); 
      }else if(sub_port == 3){
        AN_X4_ABI_SP3_ENSr_CLR(reg_an_enb3);
        AN_X4_ABI_SP3_ENSr_CL37_ENABLEf_SET(reg_an_enb3,0);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP3_ENSr(pc, reg_an_enb3)); 
      }
    }
    */ 

    /*Setting X4 abilities*/
    if(sub_port == 0){
      AN_X4_ABI_SP0_ENSr_CLR(reg_an_enb0);
      AN_X4_ABI_SP0_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb0,cl37_restart);
      /*
      AN_X4_ABI_SP0_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb0,pc->sgmii_to_cl37_auto_enable);
      AN_X4_ABI_SP0_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb0,pc->cl37_bam_to_sgmii_auto_enable);
      */
      AN_X4_ABI_SP0_ENSr_CL37_ENABLEf_SET(reg_an_enb0,cl37_enable);
      AN_X4_ABI_SP0_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb0,cl37_sgmii_enable);
      AN_X4_ABI_SP0_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb0,cl37_bam_enable);
      /*
      AN_X4_ABI_SP0_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb0,pc->disable_rf_report);
      */
      AN_X4_ABI_SP0_ENSr_QSGMII_ENf_SET(reg_an_enb0,qsgmii_enable);
      PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP0_ENSr(pc, reg_an_enb0)); 
    }else if(sub_port == 1){
      AN_X4_ABI_SP1_ENSr_CLR(reg_an_enb1);
      AN_X4_ABI_SP1_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb1,cl37_restart);
      /*
      AN_X4_ABI_SP1_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb1,pc->sgmii_to_cl37_auto_enable);
      AN_X4_ABI_SP1_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb1,pc->cl37_bam_to_sgmii_auto_enable);
      */
      AN_X4_ABI_SP1_ENSr_CL37_ENABLEf_SET(reg_an_enb1,cl37_enable);
      AN_X4_ABI_SP1_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb1,cl37_sgmii_enable);
      AN_X4_ABI_SP1_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb1,cl37_bam_enable);
      /*
      AN_X4_ABI_SP1_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb1,pc->disable_rf_report);
      */
      AN_X4_ABI_SP1_ENSr_QSGMII_ENf_SET(reg_an_enb1,qsgmii_enable);
      PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP1_ENSr(pc, reg_an_enb1)); 
    }else if(sub_port == 2){
      AN_X4_ABI_SP2_ENSr_CLR(reg_an_enb2);
      AN_X4_ABI_SP2_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb2,cl37_restart);
      /*
      AN_X4_ABI_SP2_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb2,pc->sgmii_to_cl37_auto_enable);
      AN_X4_ABI_SP2_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb2,pc->cl37_bam_to_sgmii_auto_enable);
      */
      AN_X4_ABI_SP2_ENSr_CL37_ENABLEf_SET(reg_an_enb2,cl37_enable);
      AN_X4_ABI_SP2_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb2,cl37_sgmii_enable);
      AN_X4_ABI_SP2_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb2,cl37_bam_enable);
      /*
      AN_X4_ABI_SP2_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb2,pc->disable_rf_report);
      */
      AN_X4_ABI_SP2_ENSr_QSGMII_ENf_SET(reg_an_enb2,qsgmii_enable);
      PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP2_ENSr(pc, reg_an_enb2)); 
    }else if(sub_port == 3){
      AN_X4_ABI_SP3_ENSr_CLR(reg_an_enb3);
      AN_X4_ABI_SP3_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb3,cl37_restart);
      /*
      AN_X4_ABI_SP3_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb3,pc->sgmii_to_cl37_auto_enable);
      AN_X4_ABI_SP3_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb3,pc->cl37_bam_to_sgmii_auto_enable);
      */
      AN_X4_ABI_SP3_ENSr_CL37_ENABLEf_SET(reg_an_enb3,cl37_enable);
      AN_X4_ABI_SP3_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb3,cl37_sgmii_enable);
      AN_X4_ABI_SP3_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb3,cl37_bam_enable);
      /*
      AN_X4_ABI_SP3_ENSr_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_an_enb3,pc->disable_rf_report);
      */
      AN_X4_ABI_SP3_ENSr_QSGMII_ENf_SET(reg_an_enb3,qsgmii_enable);
      PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP3_ENSr(pc, reg_an_enb3)); 
    }
  
    if(cl37_restart){
      if(sub_port == 0){
        AN_X4_ABI_SP0_ENSr_CLR(reg_an_enb0);
        AN_X4_ABI_SP0_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb0,0);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP0_ENSr(pc, reg_an_enb0)); 
      }else if(sub_port == 1){
        AN_X4_ABI_SP1_ENSr_CLR(reg_an_enb1);
        AN_X4_ABI_SP1_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb1,0);
        AN_X4_ABI_SP1_ENSr_QSGMII_ENf_SET(reg_an_enb1,qsgmii_enable);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP1_ENSr(pc, reg_an_enb1)); 
      }else if(sub_port == 2){
        AN_X4_ABI_SP2_ENSr_CLR(reg_an_enb2);
        AN_X4_ABI_SP2_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb2,0);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP2_ENSr(pc, reg_an_enb2)); 
      }else if(sub_port == 3){
        AN_X4_ABI_SP3_ENSr_CLR(reg_an_enb3);
        AN_X4_ABI_SP3_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb3,0);
        AN_X4_ABI_SP3_ENSr_QSGMII_ENf_SET(reg_an_enb3,qsgmii_enable);
        PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ABI_SP3_ENSr(pc, reg_an_enb3)); 
      }
    }

  return PHYMOD_E_NONE;
} /* qmod_autoneg_control */
#endif /* _SDK_QMOD_*/

#ifdef _DV_TB_
/*
@brief   Speeds up DSC and VGA functions for simulation purpose only
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details

This function speeds up PLL, DSC and VGA functions for any speed mode. Please don't
call this function in normal operation.
Set #PHYMOD_ST::per_lane_control to 1 for speed up.

This function is controlled by the following register bit:

\li acq2_timeout, acq1_timeout, acqcdr_timeout (0xc210) -> 0x0421
\li acqvga_timeout (0xc211) -> 0x0800

*/
int qmod_afe_speed_up_dsc_vga(PHYMOD_ST* pc, int pmd_touched)   /* AFE_SPEED_UP_DSC_VGA */
{
PLL_CAL_CTL0r_t    reg_ctrl0;
PLL_CAL_CTL1r_t    reg_ctrl1;
PLL_CAL_CTL2r_t    reg_ctrl2;
PLL_CAL_CTL5r_t    reg_ctrl5;
PLL_CAL_CTL6r_t    reg_ctrl6;
DSC_SM_CTL4r_t    reg_ctrl4;

QMOD_DBG_IN_FUNC_INFO(pc);
PLL_CAL_CTL5r_CLR(reg_ctrl5);
PLL_CAL_CTL6r_CLR(reg_ctrl6);
PLL_CAL_CTL0r_CLR(reg_ctrl0);
PLL_CAL_CTL1r_CLR(reg_ctrl1);
PLL_CAL_CTL2r_CLR(reg_ctrl2);
DSC_SM_CTL4r_CLR(reg_ctrl4);

  if (pmd_touched == 0) {
/* PLL speed_up begin */
  /* refclk_divcnt = 5 (default 16'h186a ) */
  PLL_CAL_CTL5r_REFCLK_DIVCNTf_SET(reg_ctrl5, 0x5);
  PHYMOD_IF_ERR_RETURN (MODIFY_PLL_CAL_CTL5r(pc, reg_ctrl5));

  /* refclk_divcnt_sel = 7 */
  /* data = 0x7 << PLL_CAL_COM_CTL_6_REFCLK_DIVCNT_SEL_SHIFT ; */
  PLL_CAL_CTL6r_REFCLK_DIVCNT_SELf_SET(reg_ctrl6, 0x7);
  PHYMOD_IF_ERR_RETURN (MODIFY_PLL_CAL_CTL6r(pc, reg_ctrl6));

  /* vco_start_time = 5 */
  /* data = 0x5 << PLL_CAL_COM_CTL_0_VCO_START_TIME_SHIFT ; */
  PLL_CAL_CTL0r_VCO_START_TIMEf_SET(reg_ctrl0, 0x5);
  PHYMOD_IF_ERR_RETURN (MODIFY_PLL_CAL_CTL0r(pc, reg_ctrl0));

  /* pre_freq_det_time = 8'd100 , retry_time = 8'd100 */
  PLL_CAL_CTL1r_PRE_FREQ_DET_TIMEf_SET(reg_ctrl1, 0xc8);
  PLL_CAL_CTL1r_RETRY_TIMEf_SET(reg_ctrl1, 0xff);
  PHYMOD_IF_ERR_RETURN (MODIFY_PLL_CAL_CTL1r(pc, reg_ctrl1));

  /* res_cal_cntr = 0 win_cal_cntr=1 */
  PLL_CAL_CTL2r_RES_CAL_CNTRf_SET(reg_ctrl2, 0x2);
  PLL_CAL_CTL2r_WIN_CAL_CNTRf_SET(reg_ctrl2, 0x1);

  PHYMOD_IF_ERR_RETURN (MODIFY_PLL_CAL_CTL2r(pc, reg_ctrl2));

  }
  /* PLL speed_up end */
  /* DSC speed_up begin */

  DSC_SM_CTL4r_HW_TUNE_TIMEOUTf_SET(reg_ctrl4, 0x1);
  DSC_SM_CTL4r_CDR_SETTLE_TIMEOUTf_SET(reg_ctrl4, 0x1);
  DSC_SM_CTL4r_ACQ_CDR_TIMEOUTf_SET(reg_ctrl4, 0x4);

  PHYMOD_IF_ERR_RETURN (MODIFY_DSC_SM_CTL4r(pc, reg_ctrl4));

  /* DSC speed_up end */

  return PHYMOD_E_NONE;
}
#endif /* _DV_TB_ */

static int _qmod_getRevDetails(PHYMOD_ST* pc)
{
  MAIN_SERDESIDr_t reg_serdesid;

  MAIN_SERDESIDr_CLR(reg_serdesid);
  PHYMOD_IF_ERR_RETURN(READ_MAIN_SERDESIDr(pc,&reg_serdesid));
  return (MAIN_SERDESIDr_GET(reg_serdesid));
}

/**
@brief   Init routine sets various operating modes of TSCE.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   pmd_touched  
@param   spd_intf  #qmod_spd_intfc_type
@param   pll_mode # to override the pll div 
@returns PHYMOD_E_NONE if successful. PHYMOD_E_FAIL else.
@details

This function is called once per TSCE. It cannot be called multiple times
and should be called immediately after reset. Elements of #PHYMOD_ST should be
initialized to required values prior to calling this function. The following
sub-configurations are performed here.

\li Set pll divider for VCO setting in PMD. pll divider is calculated from max_speed. 
*/

int qmod_set_pll_mode(PHYMOD_ST* pc, int pmd_touched, qmod_spd_intfc_type spd_intf, int pll_mode)           /* SET_PLL_MODE */
{
  PLL_CAL_CTL7r_t    reg_ctl7;
  int speed;

  QMOD_DBG_IN_FUNC_INFO(pc);
  PLL_CAL_CTL7r_CLR(reg_ctl7);
  if (pmd_touched == 0) {
    qmod_get_mapped_speed(spd_intf, &speed);
    /*Support Override PLL DIV */
    if(pll_mode & 0x80000000) {
      PLL_CAL_CTL7r_PLL_MODEf_SET(reg_ctl7, (pll_mode) & 0x0000ffff);
    } else {
       PLL_CAL_CTL7r_PLL_MODEf_SET(reg_ctl7, (qmod_sc_pmd_entry[speed].pll_mode)); 
    }

#ifdef _DV_TB_
    if( pc->verbosity) {
      printf("%-22s: plldiv:%d data:%x\n", __func__, pc->plldiv, PLL_CAL_CTL7r_PLL_MODEf_GET(reg_ctl7));
      printf("%-22s: main0_setup=%x\n", __func__, PLL_CAL_CTL7r_PLL_MODEf_GET(reg_ctl7));
    }
#endif
    PHYMOD_IF_ERR_RETURN (MODIFY_PLL_CAL_CTL7r(pc, reg_ctl7));
  }
  return PHYMOD_E_NONE;

} /* qmod_set_pll_mode(PHYMOD_ST* pc) */

#ifdef _DV_TB_
/**
@brief   set the AN tick overrides
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   tick_override
@param   numerator
@param   denominator
@returns PHYMOD_E_NONE if successful.
@details
 */
int qmod_tick_override_set(PHYMOD_ST* pc, int tick_override, int numerator, int denominator)
{
  return PHYMOD_E_NONE;

}
#endif /* _DV_TB_ */

/**
@brief   Set the AN Portmode and Single Port for AN of TSCE.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   number_of_lanes
@param   starting lane #port no.
@param   single_port
@returns PHYMOD_E_NONE if successful. PHYMOD_E_FAIL else.
@details

This function can be called multiple times.Elements of #PHYMOD_ST should be
initialized to required values prior to calling this function. The following
sub-configurations are performed here.

\li Set pll divider for VCO setting (0x9000[12, 11:8]). pll divider is calculated from max_speed. 

*/
#ifdef _DV_TB_
int qmod_set_an_port_mode(PHYMOD_ST* pc)    /* SET_AN_PORT_MODE */
{
  uint16_t data; /*, mask;*/
  /*
  PHYMOD_ST  pc_10G_X1;
  PHYMOD_ST*  pc_10G_X1_tmp;
  PHYMOD_ST  pc_40G_X4;
  PHYMOD_ST*  pc_40G_X4_tmp;
  PHYMOD_ST  pc_100G_X4;
  PHYMOD_ST*  pc_100G_X4_tmp;
  */
  MAIN_SETUPr_t reg_main_setup; 

  CL37_RESTARTr_t reg_cl37_restart_timers;
  CL37_ACKr_t     reg_cl37_ack_timers;
  CL37_ERRr_t     reg_cl37_err_timers;
  CL37_SYNC_STS_FILTER_TMRr_t reg_an_sync_stat_fil_timer;
  LNK_UP_TYPEr_t  reg_link_up_timer;
  SGMII_CL37_TMR_TYPEr_t reg_an_cl37_sgmii_tmr;
  MAIN_TICK_CTL1r_t reg_tick_ctrl;
  MAIN_TICK_CTL0r_t reg_tick_ctrl0;
  IGNORE_LNK_TMR_TYPEr_t  reg_ignore_link_timers;
  LNK_FAIL_INHBT_TMR_NOT_CL72_TYPEr_t  reg_inhbt_not_cl72_timers;

  BAM_SPD_PRI_5_0r_t reg_bam_pri5;
  OUI_LWRr_t reg_an_oui_lower;
  OUI_UPRr_t reg_an_oui_upper;
  SC_X1_TX_RST_CNTr_t reg_sc_rst_cnt;

  QMOD_DBG_IN_FUNC_INFO(pc);
  MAIN_SETUPr_CLR(reg_main_setup);

  data=0;
/*  mask=0;*/
  /*Ref clock selection*/
  data = main0_refClkSelect_clk_156p25MHz;
  /*Selectng div 40 for CL37*/
  /*if(pc->sc_mode == QMOD_SC_MODE_AN_CL37)
    data = data | (MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div40 <<
    MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_SHIFT); */
  /*Selectng div 66 for CL77*/
  /*if(pc->sc_mode == QMOD_SC_MODE_AN_CL73)
    data = data | (MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div66 <<
    MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_SHIFT);*/
    
  /*For AN port mode should be always QUAD*/

  PHYMOD_IF_ERR_RETURN (READ_MAIN_SETUPr(pc, &reg_main_setup));
  
  MAIN_SETUPr_REFCLK_SELf_SET(reg_main_setup, data);
  data = 0;
  
  PHYMOD_IF_ERR_RETURN(MODIFY_MAIN_SETUPr(pc,reg_main_setup));

  /*Tick over_write enable*/
  MAIN_TICK_CTL1r_CLR(reg_tick_ctrl);
  MAIN_TICK_CTL0r_CLR(reg_tick_ctrl0);

  MAIN_TICK_CTL1r_TICK_OVERRIDEf_SET(reg_tick_ctrl, pc->an_tick_override);
  MAIN_TICK_CTL1r_TICK_NUMERATOR_UPPERf_SET(reg_tick_ctrl, (pc->an_tick_numerator & 0x7fff0) >> 4);
  PHYMOD_IF_ERR_RETURN(WRITE_MAIN_TICK_CTL1r(pc, reg_tick_ctrl));

  data=0;
  /*Setting numerator to 0x10 and denominator to 0x1 for CL37*/
  MAIN_TICK_CTL0r_TICK_DENOMINATORf_SET(reg_tick_ctrl0,(pc->an_tick_denominator & 0x3ff));
  MAIN_TICK_CTL0r_TICK_NUMERATOR_LOWERf_SET(reg_tick_ctrl0,(pc->an_tick_numerator & 0xf));
  PHYMOD_IF_ERR_RETURN(WRITE_MAIN_TICK_CTL0r(pc, reg_tick_ctrl0));


/*AN from ST 
#if defined (_DV_TB_)
  if(pc->per_lane_control & 0x4) {
     pc_10G_X1 = *pc;
     pc_10G_X1.speed = digital_operationSpeeds_SPEED_10G_KR1; 
     pc_10G_X1_tmp = &pc_10G_X1;
     get_ht_entries(pc_10G_X1_tmp);
     if(pc->cl72_en & 0x80000000)
       pc_10G_X1_tmp->cl72_en = 0x0;
     else  
       pc_10G_X1_tmp->cl72_en = 0x1;
     pc_10G_X1_tmp->cl72_en = pc_10G_X1_tmp->cl72_en | 0x40000000;
     _configure_st_entry(1, digital_operationSpeeds_SPEED_40G_KR4, pc_10G_X1_tmp); 
  }
  else if(pc->per_lane_control & 0x8) {
     pc_40G_X4 = *pc;
     pc_40G_X4.speed = digital_operationSpeeds_SPEED_40G_KR4; 
     pc_40G_X4_tmp = &pc_40G_X4;
     get_ht_entries(pc_40G_X4_tmp);
     if(pc->cl72_en & 0x80000000)
       pc_40G_X4_tmp->cl72_en = 0x0;
     else  
       pc_40G_X4_tmp->cl72_en = 0x1;
     pc_40G_X4_tmp->cl72_en = pc_40G_X4_tmp->cl72_en | 0x40000000;
     _configure_st_entry(1, digital_operationSpeeds_SPEED_10G_KR1, pc_40G_X4_tmp); 
  }
  else if(pc->per_lane_control & 0x20) {
     pc_100G_X4 = *pc;
     pc_100G_X4.speed = digital_operationSpeeds_SPEED_100M; 
     pc_100G_X4_tmp = &pc_100G_X4;
     get_ht_entries(pc_100G_X4_tmp);
     pc_100G_X4_tmp->cl72_en = pc_100G_X4_tmp->cl72_en | 0x40000000;
     pc_100G_X4_tmp->this_lane = 0;
     pc_100G_X4_tmp->prt_ad = 1;
     _configure_st_entry(1, digital_operationSpeeds_SPEED_100G_CR10, pc_100G_X4_tmp); 
     pc_100G_X4_tmp->this_lane = 4;
     pc_100G_X4_tmp->prt_ad = 5;
     _configure_st_entry(1, digital_operationSpeeds_SPEED_100G_CR10, pc_100G_X4_tmp); 
     pc_100G_X4_tmp->this_lane = 8;
     pc_100G_X4_tmp->prt_ad = 9;
     _configure_st_entry(1, digital_operationSpeeds_SPEED_100G_CR10, pc_100G_X4_tmp); 
  }
#endif
*/

/*0x9250 AN_X1_TIMERS_cl37_restart */
  CL37_RESTARTr_CLR(reg_cl37_restart_timers);
  data = pc->cl37_restart_timer_period;
  if(data == 0)
    data = 0x1f;
  CL37_RESTARTr_CL37_RESTART_TIMER_PERIODf_SET(reg_cl37_restart_timers, data);
  PHYMOD_IF_ERR_RETURN(WRITE_CL37_RESTARTr(pc, reg_cl37_restart_timers));
/*0x9251 AN_X1_TIMERS_cl37_ack */
  CL37_ACKr_CLR(reg_cl37_ack_timers);
  data = pc->cl37_ack_timer_period;
  if(data == 0)
    data = 0x1f;
  CL37_ACKr_CL37_ACK_TIMER_PERIODf_SET(reg_cl37_ack_timers, data);
  PHYMOD_IF_ERR_RETURN(WRITE_CL37_ACKr(pc, reg_cl37_ack_timers));
/*0x9252 AN_X1_TIMERS_cl37_error */
  CL37_ERRr_CLR(reg_cl37_err_timers);
  data = pc->cl37_error_timer_period;
  CL37_ERRr_CL37_ERROR_TIMER_PERIODf_SET(reg_cl37_err_timers, data);
  PHYMOD_IF_ERR_RETURN(WRITE_CL37_ERRr(pc, reg_cl37_err_timers));
  /*0x9253 AN_X1_TIMERS_cl37_sync_status_filter_timer*/ 
  data = pc->cl37_sync_status_filter;
  CL37_SYNC_STS_FILTER_TMRr_CLR(reg_an_sync_stat_fil_timer);
  CL37_SYNC_STS_FILTER_TMRr_CL37_SYNC_STATUS_FILTER_TIMER_PERIODf_SET(reg_an_sync_stat_fil_timer, data);
  PHYMOD_IF_ERR_RETURN(WRITE_CL37_SYNC_STS_FILTER_TMRr(pc, reg_an_sync_stat_fil_timer));
/*TBD::0x9254 AN_X1_TIMERS_link_up_typ*/ 
  data = pc->cl37_sgmii_timer_period;
  SGMII_CL37_TMR_TYPEr_CLR(reg_an_cl37_sgmii_tmr);
  SGMII_CL37_TMR_TYPEr_SGMII_TIMERf_SET(reg_an_cl37_sgmii_tmr, data);
  PHYMOD_IF_ERR_RETURN(WRITE_SGMII_CL37_TMR_TYPEr(pc, reg_an_cl37_sgmii_tmr));
/*TBD::0x9255 AN_X1_TIMERS_sgmii_cl73_timer_type*/ 
  if(pc->an_tick_override)
    data = 100;
  else 
    data = 2;
  LNK_UP_TYPEr_CLR(reg_link_up_timer);
  LNK_UP_TYPEr_SET(reg_link_up_timer, data);
  PHYMOD_IF_ERR_RETURN(WRITE_LNK_UP_TYPEr(pc, reg_link_up_timer));
/*0x9256 AN_X1_TIMERS_ignore_link */
  IGNORE_LNK_TMR_TYPEr_CLR(reg_ignore_link_timers);
  data = pc->an_ignore_link_timer;
  IGNORE_LNK_TMR_TYPEr_SET(reg_ignore_link_timers, data);
  PHYMOD_IF_ERR_RETURN(WRITE_IGNORE_LNK_TMR_TYPEr(pc, reg_ignore_link_timers));
/*0x9256 AN_X1_TIMERS_link_fail_inhbt_tmr_not_cl72 */
  LNK_FAIL_INHBT_TMR_NOT_CL72_TYPEr_CLR(reg_inhbt_not_cl72_timers);
  data = pc->link_fail_inhibit_timer_not_cl72_period;
  LNK_FAIL_INHBT_TMR_NOT_CL72_TYPEr_SET(reg_inhbt_not_cl72_timers, data);
  PHYMOD_IF_ERR_RETURN(WRITE_LNK_FAIL_INHBT_TMR_NOT_CL72_TYPEr(pc, reg_inhbt_not_cl72_timers));
/*TBD::0x9263 sc_x1_control_tx_reset_count*/ 
  data = pc->tx_reset_count;
  SC_X1_TX_RST_CNTr_CLR(reg_sc_rst_cnt);
  SC_X1_TX_RST_CNTr_TX_RESET_COUNTf_SET(reg_sc_rst_cnt, data);
  PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_TX_RST_CNTr(pc, reg_sc_rst_cnt));
/*0x9240 AN_X1_CONTROL_oui_upper  */
  data =  (pc->oui_upper_data & 0xff);
  OUI_UPRr_CLR(reg_an_oui_upper);
  OUI_UPRr_OUI_UPPER_DATAf_SET(reg_an_oui_upper, data);
  PHYMOD_IF_ERR_RETURN(WRITE_OUI_UPRr(pc, reg_an_oui_upper));
/*0x9241 AN_X1_CONTROL_oui_lower  */
  data =  ((pc->oui_lower_data));
  OUI_LWRr_CLR(reg_an_oui_lower);
  OUI_LWRr_OUI_LOWER_DATAf_SET(reg_an_oui_lower, data);
  PHYMOD_IF_ERR_RETURN(WRITE_OUI_LWRr(pc, reg_an_oui_lower));
/*0x9242 AN_X1_CONTROL_BAM_SPEED_PRI_5_0  */
  data =  ((pc->bam_spd_pri_5_0));
  BAM_SPD_PRI_5_0r_CLR(reg_bam_pri5);
  BAM_SPD_PRI_5_0r_SET(reg_bam_pri5, data);
  PHYMOD_IF_ERR_RETURN(WRITE_BAM_SPD_PRI_5_0r(pc, reg_bam_pri5));
  return PHYMOD_E_NONE;
} /* qmod_set_an_port_mode(PHYMOD_ST* pc) */
#endif

#ifdef _SDK_QMOD_
int qmod_set_an_port_mode(PHYMOD_ST* pc, int num_of_lanes, int starting_lane, int single_port)    /* SET_AN_PORT_MODE */
{
  return PHYMOD_E_NONE;
} /* qmod_set_an_port_mode(PHYMOD_ST* pc, int num_of_lanes, int starting_lane, int single_port) */
#endif /* _SDK_QMOD_ */

int qmod_refclk_set(PHYMOD_ST* pc, qmod_ref_clk_t refclk)
{
    MAIN_SETUPr_t  reg_setup;

    MAIN_SETUPr_CLR(reg_setup);
    PHYMOD_IF_ERR_RETURN (READ_MAIN_SETUPr(pc, &reg_setup));

    if(refclk!=QMODREFCLK125MHZ) {
      MAIN_SETUPr_REFCLK_SELf_SET(reg_setup, main0_refClkSelect_clk_156p25MHz);
    } else {
      /* not supported */
      return PHYMOD_E_FAIL;
    }

    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN_SETUPr(pc,reg_setup));
    return PHYMOD_E_NONE;
}


/**
@brief   Sets loopback mode at PCS/PMD parallel interface. (PCS gloop).
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable loopbacks.
@returns The value PHYMOD_E_NONE upon successful completion

This function sets the TX-to-RX digital loopback mode at the PCS/PMD parallel interface, which is set
based on port_type.

\li 0:1 : Enable  TX->RX loopback
\li 0:0 : Disable TX->RX loopback

Note that this function can program <B>multiple lanes simultaneously</B>.

*/
int qmod_tx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int num_lanes)           /* TX_LOOPBACK_CONTROL  */
{
  BCMI_QTC_XGXS_MAIN_LPBK_CTLr_t          reg_lb_ctrl;
  BCMI_QTC_XGXS_PMD_X4_OVRRr_t            reg_pmd_ovr;
  BCMI_QTC_XGXS_PMD_X4_CTLr_t            reg_pmd_ctrl;
  uint16_t                   lane_mask, data, tmp_data, i;

  QMOD_DBG_IN_FUNC_INFO(pc);
  lane_mask = 0;
  READ_MAIN_LPBK_CTLr(pc, &reg_lb_ctrl);
  tmp_data = MAIN_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_GET(reg_lb_ctrl);
  PMD_X4_OVRRr_CLR(reg_pmd_ovr);
  PMD_X4_CTLr_CLR(reg_pmd_ctrl);


  data = 0;
  for (i = 0; i < num_lanes; i++) {
    lane_mask |= 1 << (starting_lane + i);
    data |= enable << (starting_lane + i);
  }
  
  tmp_data &= ~lane_mask;
  tmp_data |= data;
  MAIN_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_SET(reg_lb_ctrl, tmp_data);   

#if 0

   switch(port_type) {
     case QMOD_MULTI_PORT:   lane_mask = (1 << (starting_lane%4)); break;
     case QMOD_TRI1_PORT:    lane_mask = ((starting_lane%4)==2)?0xc : (1 << (starting_lane%4)); break;
     case QMOD_TRI2_PORT:    lane_mask = ((starting_lane%4)==0)?0x3 : (1 << (starting_lane%4)); break;
     case QMOD_DXGXS:        lane_mask = ((starting_lane%4)==0)?0x3 : 0xc; break;
     case QMOD_SINGLE_PORT:  lane_mask = 0xf; break;
     default: break ;
   }
   data = enable ? lane_mask : 0; 

   PHYMOD_IF_ERR_RETURN
      (READ_MAIN_LPBK_CTLr(pc, &reg_lb_ctrl));

   data_temp = MAIN_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_GET(reg_lb_ctrl); 
   data_temp = data_temp & ~lane_mask;
   data = data | data_temp;

   MAIN_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_SET(reg_lb_ctrl, data);
#endif

   PHYMOD_IF_ERR_RETURN
      (MODIFY_MAIN_LPBK_CTLr(pc, reg_lb_ctrl));

/* signal_detect and rx_lock OVRDf are set in core_init, no need to set them again in here */
  if (enable) {
       PMD_X4_OVRRr_TX_DISABLE_OENf_SET(reg_pmd_ovr, 1);
  } else {
       PMD_X4_OVRRr_TX_DISABLE_OENf_SET(reg_pmd_ovr, 0);
  }

   PHYMOD_IF_ERR_RETURN
      (MODIFY_PMD_X4_OVRRr(pc, reg_pmd_ovr));

/* set tx_disable */
  if (enable) {
    PMD_X4_CTLr_TX_DISABLEf_SET(reg_pmd_ctrl, 1);
  } else {
    PMD_X4_CTLr_TX_DISABLEf_SET(reg_pmd_ctrl, 0);
  }
   /* mask = PMD_X4_CONTROL_TX_DISABLE_MASK ; */
   PHYMOD_IF_ERR_RETURN
      (MODIFY_PMD_X4_CTLr(pc, reg_pmd_ctrl));
 /* When the phy is put in gloop, the rx clk changes, so need to reset the rxp to clear the rx pipe */
  qmod_rx_lane_control_set(pc, 1);

  return PHYMOD_E_NONE;

}


int qmod_tx_loopback_get(PHYMOD_ST* pc, uint32_t *enable)  /* TX_LOOPBACK_get  */
{
  MAIN_LPBK_CTLr_t loopReg;
  READ_MAIN_LPBK_CTLr(pc, &loopReg);
  *enable = MAIN_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_GET(loopReg);

  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/*!
@brief   Sets loopback mode at PMD serial interface. (gloop(PCS) + PMD)
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   cntl select tx/rx loopback
@returns The value PHYMOD_E_NONE upon successful completion

This function sets the PMD TX-to-RX digital loopback mode.

\li 0:1 : Enable  TX->RX loopback
\li 0:0 : Disable TX->RX loopback

Note that this function can program <B>multiple lanes simultaneously</B>.
As an example, to enable gloop on all lanes,
set #PHYMOD_ST::per_lane_control to 0x01010101

*/
int qmod_tx_pmd_loopback_control(PHYMOD_ST* pc, int cntl)           /* TX_PMD_LOOPBACK_CONTROL  */
{
  BCMI_QTC_XGXS_TLB_RX_DIG_LPBK_CFGr_t    reg_lpbk_cfg;

  QMOD_DBG_IN_FUNC_INFO(pc);
  TLB_RX_DIG_LPBK_CFGr_CLR(reg_lpbk_cfg);
  READ_TLB_RX_DIG_LPBK_CFGr(pc, &reg_lpbk_cfg);

   /*cntl = (pc-> pmd_gloop);*/
   if (cntl) {
      TLB_RX_DIG_LPBK_CFGr_DIG_LPBK_ENf_SET(reg_lpbk_cfg, 1);
   } else {
      TLB_RX_DIG_LPBK_CFGr_DIG_LPBK_ENf_SET(reg_lpbk_cfg, 0);
   }
   PHYMOD_IF_ERR_RETURN
      (WRITE_TLB_RX_DIG_LPBK_CFGr(pc, reg_lpbk_cfg));
  return PHYMOD_E_NONE;

}
#endif 

/**
@brief   Sets loopback mode at PMD serial interface. (pcs -> rloop)
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion

This function sets the RX-to-TX(PCS)  Remote loopback mode, based on port_type.

\li 0:1 : Enable  RX->TX loopback
\li 0:0 : Disable RX->TX loopback

Note that this function can program <B>multiple lanes simultaneously</B>.
As an example, to enable gloop on all lanes,
set #PHYMOD_ST::per_lane_control to 0x01010101

*/
int qmod_rx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type)           /* RX_LOOPBACK_CONTROL  */
{
   BCMI_QTC_XGXS_MAIN_LPBK_CTLr_t      reg_lb_ctrl;
   BCMI_QTC_XGXS_TLB_TX_TX_PI_LOOP_TIMING_CFGr_t   reg_lb_timing;
   BCMI_QTC_XGXS_TX_PI_CTL0r_t                     reg_com_ctrl;
   uint16_t lane_mask, data, data_temp;

   QMOD_DBG_IN_FUNC_INFO(pc);
   MAIN_LPBK_CTLr_CLR(reg_lb_ctrl);
   TLB_TX_TX_PI_LOOP_TIMING_CFGr_CLR(reg_lb_timing);
   TX_PI_CTL0r_CLR(reg_com_ctrl);

   lane_mask = 0;
   switch(port_type) {
       case QMOD_MULTI_PORT:   lane_mask = (1 << (starting_lane%4)); break;
       case QMOD_TRI1_PORT:    lane_mask = ((starting_lane%4)==2)?0xc : (1 << (starting_lane%4)); break;
       case QMOD_TRI2_PORT:    lane_mask = ((starting_lane%4)==0)?0x3 : (1 << (starting_lane%4)); break;
       case QMOD_DXGXS:        lane_mask = ((starting_lane%4)==0)?0x3 : 0xc; break;
       case QMOD_SINGLE_PORT:  lane_mask = 0xf; break;
       default: break ;
   }
   data = enable ? lane_mask : 0;

   PHYMOD_IF_ERR_RETURN
      (READ_MAIN_LPBK_CTLr(pc, &reg_lb_ctrl));

   data_temp = MAIN_LPBK_CTLr_REMOTE_PCS_LOOPBACK_ENABLEf_GET(reg_lb_ctrl);
   data_temp = data_temp & ~lane_mask;
   data = data | data_temp;


   MAIN_LPBK_CTLr_REMOTE_PCS_LOOPBACK_ENABLEf_SET(reg_lb_ctrl, data );
   PHYMOD_IF_ERR_RETURN
        (MODIFY_MAIN_LPBK_CTLr(pc, reg_lb_ctrl));

   /* set Tx_PI */
   TLB_TX_TX_PI_LOOP_TIMING_CFGr_TX_PI_LOOP_TIMING_SRC_SELf_SET(reg_lb_timing, 1);

   PHYMOD_IF_ERR_RETURN
        (MODIFY_TLB_TX_TX_PI_LOOP_TIMING_CFGr(pc, reg_lb_timing));

   TX_PI_CTL0r_TX_PI_ENf_SET(reg_com_ctrl, 1);
   PHYMOD_IF_ERR_RETURN
        (MODIFY_TX_PI_CTL0r(pc, reg_com_ctrl));
   return PHYMOD_E_NONE;
}

/**
@brief   set tx encoder of any particular lane
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.

This function is used to select encoder of any transmit lane.

\li select encodeMode

*/
#ifdef _DV_TB_
int qmod_encode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf)         /* ENCODE_SET */
{
  return PHYMOD_E_NONE;
}
#endif /* _DV_TB_ */

#ifdef _SDK_QMOD_

int qmod_encode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf)         /* ENCODE_SET */
{
  return PHYMOD_E_NONE;

}

#endif /* _SDK_QMOD_ */

/**
@brief   set rx decoder of any particular lane
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details This function is used to select decoder of any receive lane.

\li select decodeMode

*/

#ifdef _DV_TB_
int qmod_decode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf)         /* DECODE_SET */
{

  return PHYMOD_E_NONE;
}  /* DECODE_SET */
#endif

#ifdef _SDK_QMOD_
int qmod_decode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf)         /* DECODE_SET */
{
  return PHYMOD_E_NONE;
}  /* DECODE_SET */

#endif /* SDK_QMOD_ */

/*!
@brief   get  port speed configured
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   get the resolved speed cfg in the spd_intf
@returns The value PHYMOD_E_NONE upon successful completion.
@details get  port speed configured
*/
int qmod_spd_intf_get(PHYMOD_ST* pc, int *spd_intf)         /* SET_SPD_INTF */
{
  return PHYMOD_E_NONE;
}

/*!
@brief   tx lane reset and enable of any particular lane
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable Enable/disable traffic or reset/unreset lane
@param   tx_dis_type enum type #tx_lane_disable_type_t
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This function is used to reset tx lane and enable/disable tx lane of any
transmit lane.  Set bit 0 of #PHYMOD_ST::per_lane_control to enable the TX_LANE
(1) or disable the TX lane (0). When diable TX lane, two types of operations
are inovked independently.  
If #PHYMOD_ST::per_lane_control bit [7:4] = 1, only traffic is disabled. 
      (QMOD_TX_LANE_TRAFFIC =0x10)
If bit [7:4] = 2, only reset function is invoked.
      (QMOD_TX_LANE_RESET = 0x20)

*/
int qmod_tx_lane_control_set(PHYMOD_ST* pc, tx_lane_disable_type_t tx_dis_type)         /* TX_LANE_CONTROL */
{
  MISCr_t    reg_misc_ctrl;

  QMOD_DBG_IN_FUNC_INFO(pc);
  MISCr_CLR(reg_misc_ctrl);

  switch(tx_dis_type) {
    case QMOD_TX_LANE_RESET:
     MISCr_RSTB_TX_LANEf_SET(reg_misc_ctrl, 0);
     PHYMOD_IF_ERR_RETURN (MODIFY_MISCr(pc, reg_misc_ctrl));
     MISCr_CLR(reg_misc_ctrl);
     MISCr_RSTB_TX_LANEf_SET(reg_misc_ctrl, 1);
     PHYMOD_IF_ERR_RETURN (MODIFY_MISCr(pc, reg_misc_ctrl));
    break;
    case QMOD_TX_LANE_TRAFFIC_ENABLE:
     MISCr_ENABLE_TX_LANEf_SET(reg_misc_ctrl, 1);
     PHYMOD_IF_ERR_RETURN (MODIFY_MISCr(pc, reg_misc_ctrl));
    break;
    case QMOD_TX_LANE_TRAFFIC_DISABLE:
     MISCr_ENABLE_TX_LANEf_SET(reg_misc_ctrl, 0);
     PHYMOD_IF_ERR_RETURN (MODIFY_MISCr(pc, reg_misc_ctrl));
    break;
    case QMOD_TX_LANE_RESET_TRAFFIC_ENABLE:
     MISCr_RSTB_TX_LANEf_SET(reg_misc_ctrl, 1);
     PHYMOD_IF_ERR_RETURN (MODIFY_MISCr(pc, reg_misc_ctrl));
     MISCr_CLR(reg_misc_ctrl);
     MISCr_ENABLE_TX_LANEf_SET(reg_misc_ctrl, 1);
     PHYMOD_IF_ERR_RETURN (MODIFY_MISCr(pc, reg_misc_ctrl));
    break;
    case QMOD_TX_LANE_RESET_TRAFFIC_DISABLE:
     MISCr_RSTB_TX_LANEf_SET(reg_misc_ctrl, 0);
     PHYMOD_IF_ERR_RETURN (MODIFY_MISCr(pc, reg_misc_ctrl));
     MISCr_CLR(reg_misc_ctrl);
     MISCr_ENABLE_TX_LANEf_SET(reg_misc_ctrl, 0);
     PHYMOD_IF_ERR_RETURN (MODIFY_MISCr(pc, reg_misc_ctrl));
    break;
   default:
    break;
  }
  return PHYMOD_E_NONE;
}

int qmod_reset(PHYMOD_ST* pc)         /* pcs reset CONTROL */
{
  SC_X4_CTLr_t    reg; 
  
  QMOD_DBG_IN_FUNC_INFO(pc);

  /* write 0 to the speed change */
  SC_X4_CTLr_CLR(reg);
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc, &reg));
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg, 0);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, reg));

  return PHYMOD_E_NONE;
}

/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   *revid int ref. to return revision id to calling function.
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This  fucntion reads the revid register that contains core number, revision
number and stores the 16-bit value in the revid

*/
int qmod_revid_read(PHYMOD_ST* pc, uint32_t *revid)              /* REVID_READ */
{
  *revid=_qmod_getRevDetails(pc);
  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init

*/
int qmod_init_pcs(PHYMOD_ST* pc)              /* INIT_PCS */
{
  QMOD_DBG_IN_FUNC_INFO(pc);
  _qmod_set_port_mode_select(pc);
  qmod_rx_lane_control_set(pc, 1);
  qmod_tx_lane_control_set(pc, QMOD_TX_LANE_RESET_TRAFFIC_ENABLE); /* TX_LANE_CONTROL */

  if((pc->sc_mode == QMOD_SC_MODE_AN_CL37))
    _qmod_init_pcs_an(pc); 
  else /* Forced Speed */
    _qmod_init_pcs_fs(pc);

  return PHYMOD_E_NONE;
}
#endif

/**
@brief   Init the PMD
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   pmd_touched If the PMD is already initialized
@returns The value PHYMOD_E_NONE upon successful completion
@details Per core PMD resets (both datapath and entire core)
We only intend to use this function if the PMD has never been initialized.
*/
int qmod_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched) /* PMD_RESET_SEQ */
{
  PMD_X1_CTLr_t reg_pmd_x1_ctrl;
#ifdef KEEP_THIS
  SC_X4_BYPASSr_t reg_bypass;
  SC_X4_BYPASSr_CLR(reg_bypass);
#endif
  QMOD_DBG_IN_FUNC_INFO(pc);
  PMD_X1_CTLr_CLR(reg_pmd_x1_ctrl);
  
  if (pmd_touched == 0) {
    PMD_X1_CTLr_POR_H_RSTBf_SET(reg_pmd_x1_ctrl,1);
    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(reg_pmd_x1_ctrl,1);
    PHYMOD_IF_ERR_RETURN(BCMI_QTC_XGXS_WRITE_PMD_X1_CTLr(pc,reg_pmd_x1_ctrl));
  }

#ifdef KEEP_THIS
    SC_X4_BYPASSr_SC_BYPASSf_SET(reg_bypass, 1);
    PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_BYPASSr(pc,reg_bypass));
#endif

  return PHYMOD_E_NONE;
}

/**
@brief   Wait for SC done bit set.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   data handle to return SC_X4_CONTROL_STATUS reg.
@returns The value PHYMOD_E_NONE upon successful completion
@details Wait for SC done bit set. This is a very short wait Hence we allow a
loop here. We never allow looping in tier1 as a rule. All looping should be done
in tier2 and above.

*/
int qmod_wait_sc_done(PHYMOD_ST* pc, uint16_t *data)
{
  static int timeOut = 5000;
  int localTimeOut = timeOut;
  SC_X4_STSr_t reg_sc_ctrl_sts;

  QMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_STSr_CLR(reg_sc_ctrl_sts);

  while(1) {
    localTimeOut--;
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_STSr(pc, &reg_sc_ctrl_sts));
    if((SC_X4_STSr_SW_SPEED_CHANGE_DONEf_GET(reg_sc_ctrl_sts) == 1) || localTimeOut < 1)
      break;
  }
  return PHYMOD_E_NONE;

}

#ifdef _DV_TB_
/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init
*/
int _qmod_wait_sc_stats_set(PHYMOD_ST* pc)
{
  uint16_t data;
  SC_X4_STSr_t reg_sc_ctrl_sts;

  SC_X4_STSr_CLR(reg_sc_ctrl_sts);
   
  while(1){
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_STSr(pc, &reg_sc_ctrl_sts));
   data = SC_X4_STSr_SW_SPEED_CONFIG_VLDf_GET(reg_sc_ctrl_sts);
   if(data == 1)
     break;
  }
  return PHYMOD_E_NONE;
}
#endif

/**
@brief   Read PCS Link status
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   *link Reference for Status of PCS Link
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init

*/
/* _SDK_QMOD_ */
int qmod_get_pcs_link_status(PHYMOD_ST* pc, uint32_t sub_port, uint32_t *link)
{
  BCMI_QTC_XGXS_PCS_LIVE_STSr_t  reg_pcs_live_sts;
  BCMI_QTC_XGXS_PCS_LATCH_STSr_t reg_pcs_ll_sts; 
  uint16_t                         loc_link ;   

  QMOD_DBG_IN_FUNC_INFO(pc);

  BCMI_QTC_XGXS_PCS_LIVE_STSr_CLR(reg_pcs_live_sts);
  BCMI_QTC_XGXS_PCS_LATCH_STSr_CLR(reg_pcs_ll_sts) ;

  PHYMOD_IF_ERR_RETURN (BCMI_QTC_XGXS_READ_PCS_LIVE_STSr(pc, &reg_pcs_live_sts));
  PHYMOD_IF_ERR_RETURN (BCMI_QTC_XGXS_READ_PCS_LATCH_STSr(pc, &reg_pcs_ll_sts)) ;

 /* next check if we are in qsgmii mode */
 if (PHYMOD_ACC_F_QMODE_GET(pc)) {
    switch (sub_port) {
        case 0:
            loc_link = BCMI_QTC_XGXS_PCS_LIVE_STSr_LINK_STATUS_SP0f_GET(reg_pcs_live_sts);
            if(loc_link) {
                loc_link = (BCMI_QTC_XGXS_PCS_LATCH_STSr_LINK_STATUS_LL_SP0f_GET(reg_pcs_ll_sts))?0:1 ; 
            }
            break;
        case 1:
            loc_link = BCMI_QTC_XGXS_PCS_LIVE_STSr_LINK_STATUS_SP1f_GET(reg_pcs_live_sts);
            if(loc_link) {
                loc_link = (BCMI_QTC_XGXS_PCS_LATCH_STSr_LINK_STATUS_LL_SP1f_GET(reg_pcs_ll_sts))?0:1 ; 
            }
            break;
        case 2:
            loc_link = BCMI_QTC_XGXS_PCS_LIVE_STSr_LINK_STATUS_SP2f_GET(reg_pcs_live_sts);
            if(loc_link) {
                loc_link = (BCMI_QTC_XGXS_PCS_LATCH_STSr_LINK_STATUS_LL_SP2f_GET(reg_pcs_ll_sts))?0:1 ; 
            }
            break;
        default:
            loc_link = BCMI_QTC_XGXS_PCS_LIVE_STSr_LINK_STATUS_SP3f_GET(reg_pcs_live_sts);
            if(loc_link) {
                loc_link = (BCMI_QTC_XGXS_PCS_LATCH_STSr_LINK_STATUS_LL_SP3f_GET(reg_pcs_ll_sts))?0:1 ; 
            }
            break;
    }
  /* *link = BCMI_QTC_XGXS_PCS_LIVE_STSr_GET(reg_pcs_live_sts); */
  } else {
     loc_link = BCMI_QTC_XGXS_PCS_LIVE_STSr_LINK_STATUS_SP0f_GET(reg_pcs_live_sts);
     if(loc_link) {
         loc_link = (BCMI_QTC_XGXS_PCS_LATCH_STSr_LINK_STATUS_LL_SP0f_GET(reg_pcs_ll_sts))?0:1 ; 
     }
  }
  *link = loc_link ;
  return PHYMOD_E_NONE;
}   /* _SDK_QMOD_ */


#ifdef _DV_TB_
/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init

*/
int qmod_mld_am_timers_set(PHYMOD_ST* pc,int rx_am_timer_init_val,int tx_am_timer_init_val) /*MLD_AM_TIMERS_SET */
{
  return PHYMOD_E_NONE;
}
#endif

/**
@brief   PMD per lane reset
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per lane PMD ln_rst and ln_dp_rst by writing to PMD_X4_CONTROL in pcs space

*/
int qmod_pmd_x4_reset(PHYMOD_ST* pc)              /* PMD_X4_RESET */
{
  PMD_X4_CTLr_t reg_pmd_x4_ctrl;

  QMOD_DBG_IN_FUNC_INFO(pc);
  PMD_X4_CTLr_CLR(reg_pmd_x4_ctrl);
  PMD_X4_CTLr_LN_H_RSTBf_SET(reg_pmd_x4_ctrl,0);
  PMD_X4_CTLr_LN_DP_H_RSTBf_SET(reg_pmd_x4_ctrl,0);
  PHYMOD_IF_ERR_RETURN (MODIFY_PMD_X4_CTLr(pc, reg_pmd_x4_ctrl));
  
  PMD_X4_CTLr_CLR(reg_pmd_x4_ctrl);
  PMD_X4_CTLr_LN_H_RSTBf_SET(reg_pmd_x4_ctrl,1);
  PMD_X4_CTLr_LN_DP_H_RSTBf_SET(reg_pmd_x4_ctrl,1);
  PHYMOD_IF_ERR_RETURN (MODIFY_PMD_X4_CTLr(pc, reg_pmd_x4_ctrl));
  
  return PHYMOD_E_NONE;
}

/**
@brief   PMD per lane reset get
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per lane PMD ln_rst and ln_dp_rst by writing to PMD_X4_CONTROL in pcs space
*/
int qmod_pmd_x4_reset_get(PHYMOD_ST* pc, int *is_in_reset)              /* PMD_X4_RESET */
{
  PMD_X4_CTLr_t reg_pmd_x4_ctrl;

  QMOD_DBG_IN_FUNC_INFO(pc);
  PMD_X4_CTLr_CLR(reg_pmd_x4_ctrl);
  PHYMOD_IF_ERR_RETURN (READ_PMD_X4_CTLr(pc, &reg_pmd_x4_ctrl));
  
  if(PMD_X4_CTLr_LN_H_RSTBf_GET(reg_pmd_x4_ctrl) &&
     PMD_X4_CTLr_LN_DP_H_RSTBf_GET(reg_pmd_x4_ctrl)) {
      *is_in_reset = 0 ;  
  } else {
      *is_in_reset = 1 ;
  }
  return PHYMOD_E_NONE;
}

/**
@brief   Initialize the PMD in independent mode. (No PCS involved)
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   pmd_touched 
@param   uc_active 
@param   spd_intf #qmod_spd_intfc_type 
@param   t_pma_os_mode per lane OS Mode in PMD
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PMD Init done by software

*/
int qmod_init_pmd_sw(PHYMOD_ST* pc, int pmd_touched, int uc_active,
                      qmod_spd_intfc_type spd_intf,  int t_pma_os_mode)  /* INIT_PMD_SW */
{
  QMOD_DBG_IN_FUNC_INFO(pc);
  qmod_init_pmd(pc, pmd_touched,  uc_active);
  qmod_pmd_osmode_set(pc, spd_intf,  t_pma_os_mode);
  return PHYMOD_E_NONE;
}

/**
@brief   Initialize PMD. for both PCS and independent modes.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   pmd_touched Indicates this PMD was previously initialized for another
port in the same core.
@param   uc_active Actually means PMD config. will come from PCS and not TMod
@returns PHYMOD_E_NONE upon success, PHYMOD_E_FAIL else.
@details Per Port PMD Init

*/
int qmod_init_pmd(PHYMOD_ST* pc, int pmd_touched, int uc_active)   /* INIT_PMD */
{
  CKRST_LN_CLK_RST_N_PWRDWN_CTLr_t    reg_pwrdwn_ctrl;
  DIG_TOP_USER_CTL0r_t                    reg_usr_ctrl;

  QMOD_DBG_IN_FUNC_INFO(pc);
  CKRST_LN_CLK_RST_N_PWRDWN_CTLr_CLR(reg_pwrdwn_ctrl);
  DIG_TOP_USER_CTL0r_CLR(reg_usr_ctrl);

  CKRST_LN_CLK_RST_N_PWRDWN_CTLr_LN_DP_S_RSTBf_SET(reg_pwrdwn_ctrl, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_CKRST_LN_CLK_RST_N_PWRDWN_CTLr(pc, reg_pwrdwn_ctrl));

  if (pmd_touched == 0) {
    if(uc_active == 1){
      DIG_TOP_USER_CTL0r_UC_ACTIVEf_SET(reg_usr_ctrl, 1);
      DIG_TOP_USER_CTL0r_CORE_DP_S_RSTBf_SET(reg_usr_ctrl, 1);
      /* release reset to pll data path. TBD need for all lanes  and uc_active set */
      PHYMOD_IF_ERR_RETURN (MODIFY_DIG_TOP_USER_CTL0r(pc, reg_usr_ctrl));
    } else{
      DIG_TOP_USER_CTL0r_CORE_DP_S_RSTBf_SET(reg_usr_ctrl, 1);
      PHYMOD_IF_ERR_RETURN (MODIFY_DIG_TOP_USER_CTL0r(pc, reg_usr_ctrl));
    }
  }

  return PHYMOD_E_NONE;
}

/**
@brief   Set Per lane OS mode set in PMD
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   spd_intf speed type #qmod_spd_intfc_type
@param   os_mode over sample rate.
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PMD Init

*/
int qmod_pmd_osmode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf, int os_mode)   /* INIT_PMD */
{
  CKRST_OSR_MODE_CTLr_t    reg_osr_mode;
  int speed;

  QMOD_DBG_IN_FUNC_INFO(pc);
  CKRST_OSR_MODE_CTLr_CLR(reg_osr_mode);
  qmod_get_mapped_speed(spd_intf, &speed);

  /*os_mode         = 0x0; */ /* 0= OS MODE 1;  1= OS MODE 2; 2=OS MODE 3; 
                             3=OS MODE 3.3; 4=OS MODE 4; 5=OS MODE 5; 
                             6=OS MODE 8;   7=OS MODE 8.25; 8: OS MODE 10*/
  if(os_mode & 0x80000000) {
    os_mode =  (os_mode) & 0x0000ffff;
  }
 else {
    os_mode =  qmod_sc_pmd_entry[speed].t_pma_os_mode; 
  }

  CKRST_OSR_MODE_CTLr_OSR_MODE_FRCf_SET(reg_osr_mode, 1);
  CKRST_OSR_MODE_CTLr_OSR_MODE_FRC_VALf_SET(reg_osr_mode, os_mode);

  PHYMOD_IF_ERR_RETURN
     (MODIFY_CKRST_OSR_MODE_CTLr(pc, reg_osr_mode));

    return PHYMOD_E_NONE;
}

/**
@brief   Get Per lane OS mode set in PMD
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   os_mode over sample rate.
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PMD Init

*/
int qmod_pmd_osmode_get(PHYMOD_ST* pc, int *os_mode)   /* INIT_PMD */
{
  CKRST_OSR_MODE_CTLr_t    reg_osr_mode;

  QMOD_DBG_IN_FUNC_INFO(pc);
  CKRST_OSR_MODE_CTLr_CLR(reg_osr_mode);

  PHYMOD_IF_ERR_RETURN
     (READ_CKRST_OSR_MODE_CTLr(pc, &reg_osr_mode));

 if(CKRST_OSR_MODE_CTLr_OSR_MODE_FRCf_GET(reg_osr_mode) == 1)
   *os_mode = CKRST_OSR_MODE_CTLr_OSR_MODE_FRC_VALf_GET(reg_osr_mode);
 else
   *os_mode = 0xff;

  return PHYMOD_E_NONE;
}

/**
@brief   Generic Override mechanism. Any PCS parameter can be overriden.
@param   pc handle to current TSCE context (#PHYMOD_ST)
 param   cntl bit mask of parameters to override.
@returns The value PHYMOD_E_NONE upon successful completion
@details The cntl encoding is as follows.

          NUM_LANES_OEN |
          OS_MODE_OEN |
          DESKEWMODE_OEN |
          DESC2_MODE_OEN |
          CL36BYTEDELETEMODE_OEN |
          BRCM64B66_DESCRAMBLER_ENABLE_OEN |
          CHK_END_EN_OEN |
          BLOCK_SYNC_MODE_OEN |
          DECODERMODE_OEN |
          ENCODEMODE_OEN |
          DESCRAMBLERMODE_OEN |
          SCR_MODE_OEN

*/
int qmod_override_set(PHYMOD_ST* pc, override_type_t or_type, uint16_t or_val)   /* OVERRIDE_SET */
{
 int or_value;
  SC_X4_FLD_OVRR_EN0_TYPEr_t reg_or_en0;
  PMA_CTLr_t reg_os_mode_or_val;
  PCS_CTLr_t reg_pcs_or_val;
  CL36TX_CTLr_t reg_cl36_or_val;
  TX_X4_SP0_CRED0r_t reg_sp0_credit_or_val;

  QMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_FLD_OVRR_EN0_TYPEr_CLR(reg_or_en0);

  or_value = or_val & 0x0000ffff;
  
  switch(or_type) {
  case QMOD_OVERRIDE_RESET :
   /* reset credits */
   WRITE_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);

   MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case QMOD_OVERRIDE_ALL :
    /* get info. from table and apply it to all credit regs. */
  break;
  case QMOD_OVERRIDE_QSGMII_EN:
       CL36TX_CTLr_CLR(reg_cl36_or_val);
       CL36TX_CTLr_QSGMII_ENf_SET(reg_cl36_or_val,or_value);
       MODIFY_CL36TX_CTLr(pc,reg_cl36_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_QSGMII_EN_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case QMOD_OVERRIDE_CL36BYTEDEL_MODE:
       PCS_CTLr_CLR(reg_pcs_or_val);
       PCS_CTLr_CL36BYTEDELETEMODEf_SET(reg_pcs_or_val,or_value);
       MODIFY_PCS_CTLr(pc,reg_pcs_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_CL36BYTEDELETEMODE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case QMOD_OVERRIDE_OS_MODE:
       PMA_CTLr_CLR(reg_os_mode_or_val);
       PMA_CTLr_OS_MODEf_SET(reg_os_mode_or_val,or_value);
       MODIFY_PMA_CTLr(pc,reg_os_mode_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_OS_MODE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case QMOD_OVERRIDE_CREDIT_EN:
       TX_X4_SP0_CRED0r_CLR(reg_sp0_credit_or_val);
       TX_X4_SP0_CRED0r_CREDITENABLEf_SET(reg_sp0_credit_or_val,or_value);
       MODIFY_TX_X4_SP0_CRED0r(pc,reg_sp0_credit_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_CREDITENABLE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
 }
  return PHYMOD_E_NONE;

}


/**
@brief   Override the credits.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   credit_type type of credit to override #credit_type_t
@param   userCredit User override value
@returns The value PHYMOD_E_NONE upon successful completion
@details Credit override mechanism is a seperate function. However it is similar
to other override mechanisms.

*/
int qmod_credit_override_set(PHYMOD_ST* pc, credit_type_t credit_type, int userCredit)   /* CREDIT_OVERRIDE_SET */
{
  SC_X4_FLD_OVRR_EN0_TYPEr_t    reg_enable_type;
  TX_X4_SP0_CRED0r_t          reg_credit0;
  TX_X4_SP0_CRED1r_t          reg_credit1;
  TX_X4_SP0_LOOPCNTr_t                        reg_loopcnt;
  TX_X4_SP0_MAC_CREDGENCNTr_t               reg_maccreditgen;
  TX_X4_SP0_PCS_CLKCNT0r_t                  reg_pcs_clk_cnt;
  TX_X4_SP0_PCS_CREDGENCNTr_t               reg_pcs_creditgen;

  QMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_FLD_OVRR_EN0_TYPEr_CLR(reg_enable_type);
  TX_X4_SP0_CRED0r_CLR(reg_credit0);
  TX_X4_SP0_CRED1r_CLR(reg_credit1);
  TX_X4_SP0_LOOPCNTr_CLR(reg_loopcnt);
  TX_X4_SP0_MAC_CREDGENCNTr_CLR(reg_maccreditgen);
  TX_X4_SP0_PCS_CLKCNT0r_CLR(reg_pcs_clk_cnt);
  TX_X4_SP0_PCS_CREDGENCNTr_CLR(reg_pcs_creditgen);

  switch ( credit_type ) {
    case QMOD_CREDIT_RESET: 
    /* reset credits */
       SC_X4_FLD_OVRR_EN0_TYPEr_CLOCKCNT0_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN0_TYPEr_CLOCKCNT1_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN0_TYPEr_LOOPCNT0_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN0_TYPEr_LOOPCNT1_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN0_TYPEr_REPLICATION_CNT_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN0_TYPEr_MAC_CREDITGENCNT_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN0_TYPEr_PCS_CREDITENABLE_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN0_TYPEr_PCS_CLOCKCNT0_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN0_TYPEr_PCS_CREDITGENCNT_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN0_TYPEr_SGMII_SPD_SWITCH_OENf_SET(reg_enable_type, 0);

       PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_FLD_OVRR_EN0_TYPEr(pc, reg_enable_type));
      break;
    case QMOD_CREDIT_TABLE: 
    /* get info. from table and apply it to all credit regs. */
    break;
    case  QMOD_CREDIT_CLOCK_COUNT_0:
       /* write to clockcnt0 */
       TX_X4_SP0_CRED0r_CLOCKCNT0f_SET(reg_credit0, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_SP0_CRED0r(pc, reg_credit0));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_CLOCKCNT0_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, reg_enable_type));
      break;
    case  QMOD_CREDIT_CLOCK_COUNT_1:  /* (cntl & 0x00020000) */
       /* write to clockcnt1 */
       TX_X4_SP0_CRED1r_CLOCKCNT1f_SET(reg_credit1, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_SP0_CRED1r(pc, reg_credit1));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_CLOCKCNT1_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, reg_enable_type));
       break;
    case  QMOD_CREDIT_LOOP_COUNT_0: 
       /* write to loopcnt0 */
       TX_X4_SP0_LOOPCNTr_LOOPCNT0f_SET(reg_loopcnt, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_SP0_LOOPCNTr(pc, reg_loopcnt));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_LOOPCNT0_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, reg_enable_type));
       break;
    case QMOD_CREDIT_LOOP_COUNT_1:   /* (cntl & 0x00080000) */
       /* write to pcs_clockcnt0 */
       TX_X4_SP0_LOOPCNTr_LOOPCNT1f_SET(reg_loopcnt, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_SP0_LOOPCNTr(pc, reg_loopcnt));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_LOOPCNT1_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, reg_enable_type));
       break;
     case QMOD_CREDIT_MAC:   /* (cntl & 0x00100000) */
       /* write to mac_creditgencnt */
       TX_X4_SP0_MAC_CREDGENCNTr_MAC_CREDITGENCNTf_SET(reg_maccreditgen, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_SP0_MAC_CREDGENCNTr(pc, reg_maccreditgen));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_MAC_CREDITGENCNT_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, reg_enable_type));
      break;
    case  QMOD_CREDIT_PCS_CLOCK_COUNT_0:   /* (cntl & 0x00200000) */
       /* write to pcs_clockcnt0 */
       TX_X4_SP0_PCS_CLKCNT0r_PCS_CLOCKCNT0f_SET(reg_pcs_clk_cnt, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_SP0_PCS_CLKCNT0r(pc, reg_pcs_clk_cnt));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_PCS_CLOCKCNT0_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, reg_enable_type));
      break;
    case QMOD_CREDIT_PCS_GEN_COUNT:   /* (cntl & 0x00400000) */
       /* write to pcs_creditgencnt */
       TX_X4_SP0_PCS_CREDGENCNTr_PCS_CREDITGENCNTf_SET(reg_pcs_creditgen, userCredit) ;
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_SP0_PCS_CREDGENCNTr(pc, reg_pcs_creditgen));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_PCS_CREDITGENCNT_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, reg_enable_type));
      break;
    case QMOD_CREDIT_EN: /* (cntl & 0x00800000) */
       /* write to pcs_crden */
       TX_X4_SP0_PCS_CLKCNT0r_PCS_CREDITENABLEf_SET(reg_pcs_clk_cnt, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_SP0_PCS_CLKCNT0r(pc, reg_pcs_clk_cnt));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_PCS_CREDITENABLE_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, reg_enable_type));
       break;
    case  QMOD_CREDIT_PCS_REPCNT:    /* (cntl & 0x01000000) */
       /* write to pcs_repcnt */
       TX_X4_SP0_PCS_CLKCNT0r_REPLICATION_CNTf_SET(reg_pcs_clk_cnt, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_SP0_PCS_CLKCNT0r(pc, reg_pcs_clk_cnt));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_REPLICATION_CNT_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, reg_enable_type));
       break;
    case  QMOD_CREDIT_SGMII_SPD: /* (cntl & 0x02000000) */
       /* write to sgmii */
       TX_X4_SP0_CRED0r_SGMII_SPD_SWITCHf_SET(reg_credit0, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_SP0_CRED0r(pc, reg_credit0));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_SGMII_SPD_SWITCH_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, reg_enable_type));
       break;
   default:
      return PHYMOD_E_FAIL;
      break;
  }
  return PHYMOD_E_NONE;
} /* CREDIT_SET */

/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details 

*/
int qmod_pmd_lane_reset(PHYMOD_ST* pc)              /* PMD_LANE_RESET */
{
  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/**
@brief   Control per lane clause 72 auto tuning function  (training patterns)
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   cl_72_type #cl72_type_t
@returns The value PHYMOD_E_NONE upon successful completion
@details
This function enables/disables clause-72 auto-tuning per lane.  It is used
in 40G_KR4, 40G_CR4, 10G_KR modes etc.
Lane is selected via #PHYMOD_ST::lane_select.
This Function is controlled by #PHYMOD_ST::per_lane_control as follows.

\li 0Xxxxxxxx3      (b11): Enable  CL72 hardware block
\li 0Xxxxxxxx2      (b01): Disable CL72 hardware block
\li 0Xxxxxxxx0      (b00): no changed.
\li 0Xxxxxxx3- (b011----): Forced  enable CL72 under AN.
\li 0Xxxxxxx7- (b111----): Forced  disable CL72 under AN.
\li 0Xxxxxxx1- (b001----): Default CL72 under AN.
\li 0Xxxxxxx0- (b000----): no changed
\li 0Xxxxxx100           : read back CL72 HW en bit and put in accData. 

*/
int qmod_clause72_control(PHYMOD_ST* pc, cl72_type_t cl_72_type)                /* CLAUSE_72_CONTROL */
{
  return PHYMOD_E_NONE;
} /* clause72_control */
#endif /* _DV_TB_ */

#ifdef _DV_TB_

int qmod_prbs_check(PHYMOD_ST* pc, int real_check, int *prbs_status)
{
   TLB_RX_PRBS_CHK_LOCK_STSr_t      reg_prbs_chk_lock;
   TLB_RX_PRBS_CHK_ERR_CNT_MSB_STSr_t    reg_prbs_err_cnt_msb;
   TLB_RX_PRBS_CHK_ERR_CNT_LSB_STSr_t     reg_prbs_err_cnt_lsb;
   int lock, lock_lost, err ;

   QMOD_DBG_IN_FUNC_INFO(pc);
   pc->accData   = 0 ;

   TLB_RX_PRBS_CHK_LOCK_STSr_CLR(reg_prbs_chk_lock);
   TLB_RX_PRBS_CHK_ERR_CNT_MSB_STSr_CLR(reg_prbs_err_cnt_msb);
   TLB_RX_PRBS_CHK_ERR_CNT_LSB_STSr_CLR(reg_prbs_err_cnt_lsb);

   PHYMOD_IF_ERR_RETURN
      ( READ_TLB_RX_PRBS_CHK_LOCK_STSr(pc, &reg_prbs_chk_lock));
   PHYMOD_IF_ERR_RETURN
      (READ_TLB_RX_PRBS_CHK_ERR_CNT_MSB_STSr(pc, &reg_prbs_err_cnt_msb));
   PHYMOD_IF_ERR_RETURN
      (READ_TLB_RX_PRBS_CHK_ERR_CNT_LSB_STSr(pc, &reg_prbs_err_cnt_lsb));

   lock      = TLB_RX_PRBS_CHK_LOCK_STSr_PRBS_CHK_LOCKf_GET(reg_prbs_chk_lock) ? 1 : 0 ;
   lock_lost = TLB_RX_PRBS_CHK_ERR_CNT_MSB_STSr_PRBS_CHK_LOCK_LOST_LHf_GET(reg_prbs_err_cnt_msb) ? 1 : 0 ;
   err       = TLB_RX_PRBS_CHK_ERR_CNT_MSB_STSr_PRBS_CHK_ERR_CNT_MSBf_GET(reg_prbs_err_cnt_msb) ;
   err       = (err << 16) | TLB_RX_PRBS_CHK_ERR_CNT_LSB_STSr_PRBS_CHK_ERR_CNT_LSBf_GET(reg_prbs_err_cnt_lsb) ; 
   
#ifdef _DV_TB_
   printf("prbs_check u=%0d p=%0d ln=%0d lck=%0d lost=%0d err=%0d(H%x L%x)\n",
             pc->unit, pc->port, pc->this_lane, lock, lock_lost, err, reg_prbs_err_cnt_msb, reg_prbs_err_cnt_lsb);
#endif

   if (TLB_RX_PRBS_CHK_LOCK_STSr_PRBS_CHK_LOCKf_GET(reg_prbs_chk_lock)) {
      if (TLB_RX_PRBS_CHK_ERR_CNT_MSB_STSr_PRBS_CHK_LOCK_LOST_LHf_GET(reg_prbs_err_cnt_msb)) {
         /* locked now but lost before */
         *prbs_status = TLB_RX_PRBS_CHK_ERR_CNT_MSB_STSr_PRBS_CHK_ERR_CNT_MSBf_GET(reg_prbs_err_cnt_msb);
         *prbs_status = (*prbs_status << 16) | TLB_RX_PRBS_CHK_ERR_CNT_LSB_STSr_PRBS_CHK_ERR_CNT_LSBf_GET(reg_prbs_err_cnt_lsb) | 1  ;  /* locked once, so one error at least */
          pc->accData   = real_check ? 1 : 0 ;
          return PHYMOD_E_FAIL;
      } else {
         *prbs_status = TLB_RX_PRBS_CHK_ERR_CNT_MSB_STSr_PRBS_CHK_ERR_CNT_MSBf_GET(reg_prbs_err_cnt_msb) ; 
         *prbs_status = (*prbs_status << 16)| TLB_RX_PRBS_CHK_ERR_CNT_LSB_STSr_PRBS_CHK_ERR_CNT_LSBf_GET(reg_prbs_err_cnt_lsb) ;  
          if (err) {
            pc->accData   = real_check ? 1 : 0 ;
         return PHYMOD_E_FAIL;
          }
      }
   } else {
      if(TLB_RX_PRBS_CHK_ERR_CNT_MSB_STSr_PRBS_CHK_LOCK_LOST_LHf_GET(reg_prbs_err_cnt_msb)) {
         *prbs_status = -2 ;  /* locked before but lost now */
          pc->accData   = real_check ? 1 : 0 ;
          return PHYMOD_E_FAIL;
      } else {
         *prbs_status = -1 ;  /* never locked */
          pc->accData   = real_check ? 1 : 0 ;
         return PHYMOD_E_FAIL;
      }
   }
  return PHYMOD_E_NONE ;
}
#endif /* _DV_TB_ */

#ifdef _DV_TB_

int qmod_prbs_mode(PHYMOD_ST* pc, int port, int prbs_inv, int pattern_type, int prbs_check_mode)    /* PRBS_MODE */
{
  TLB_TX_PRBS_GEN_CFGr_t    reg_prbs_gen_cfg;
  TLB_RX_PRBS_CHK_CFGr_t    reg_prbs_chk_cfg;
  /* SC_X4_BYPASSr_t      reg_bypass; */

  /*int prbs_check_mode ; */
  int prbs_sel_tx, prbs_inv_tx ;
  int prbs_sel_rx, prbs_inv_rx ;


  QMOD_DBG_IN_FUNC_INFO(pc);
  TLB_TX_PRBS_GEN_CFGr_CLR(reg_prbs_gen_cfg);
  TLB_RX_PRBS_CHK_CFGr_CLR(reg_prbs_chk_cfg);

  prbs_sel_tx =  pattern_type;
  prbs_sel_rx =  pattern_type;
  prbs_inv_tx =  prbs_inv;
  prbs_inv_rx =  prbs_inv;

  TLB_TX_PRBS_GEN_CFGr_PRBS_GEN_MODE_SELf_SET(reg_prbs_gen_cfg, prbs_sel_tx);
  TLB_TX_PRBS_GEN_CFGr_PRBS_GEN_INVf_SET(reg_prbs_gen_cfg, prbs_inv_tx);

  PHYMOD_IF_ERR_RETURN (MODIFY_TLB_TX_PRBS_GEN_CFGr(pc, reg_prbs_gen_cfg));

  TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_MODEf_SET(reg_prbs_chk_cfg, prbs_check_mode);
  TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_MODE_SELf_SET(reg_prbs_chk_cfg, prbs_sel_rx);
  TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_INVf_SET(reg_prbs_chk_cfg, prbs_inv_rx);

  PHYMOD_IF_ERR_RETURN (MODIFY_TLB_RX_PRBS_CHK_CFGr(pc, reg_prbs_chk_cfg));

  return PHYMOD_E_NONE;

} /* PRBS_MODE */
#endif /* _DV_TB_ */

#ifdef _DV_TB_

int qmod_prbs_control(PHYMOD_ST* pc, int prbs_enable)                /* PRBS_CONTROL */
{
  TLB_TX_PRBS_GEN_CFGr_t    reg_prbs_gen_cfg;
  TLB_RX_PRBS_CHK_CFGr_t    reg_prbs_chk_cfg;
  int prbs_rx_en, prbs_tx_en ;

  QMOD_DBG_IN_FUNC_INFO(pc);
  TLB_TX_PRBS_GEN_CFGr_CLR(reg_prbs_gen_cfg);
  TLB_RX_PRBS_CHK_CFGr_CLR(reg_prbs_chk_cfg);
  
  prbs_tx_en = prbs_enable;
  prbs_rx_en = prbs_enable;

  TLB_TX_PRBS_GEN_CFGr_PRBS_GEN_ENf_SET(reg_prbs_gen_cfg, prbs_tx_en);
  PHYMOD_IF_ERR_RETURN (MODIFY_TLB_TX_PRBS_GEN_CFGr(pc, reg_prbs_gen_cfg));

  TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_ENf_SET(reg_prbs_chk_cfg, prbs_rx_en);
  PHYMOD_IF_ERR_RETURN (MODIFY_TLB_RX_PRBS_CHK_CFGr(pc, reg_prbs_chk_cfg));

  return PHYMOD_E_NONE;

}  /* PRBS_CONTROL */
#endif /* _DV_TB_ */

/**
@brief   Sets CJPAT/CRPAT parameters for a particular port
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   port handle to current TSCE context (#PHYMOD_ST)
@param   pattern_type type of CJPat
@param   pkt_number number of packets
@param   pkt_size size of packets
@param   ipg_size IPG between packets
@returns The value PHYMOD_E_NONE upon successful completion

This function enables either a CJPAT or CRPAT for a particular port. 
The speed mode of the lane must be chosen from the enumerated type 
#qmod_spd_intfc_type and set in the
#PHYMOD_ST::spd_intf field.  To enable CJPAT, set #PHYMOD_ST::per_lane_control to
the value #USE_CJPAT; to enable CRPAT, set #PHYMOD_ST::per_lane_control to the
value #USE_CRPAT.  This function is used in tandem with qmod_cjpat_crpat_check().

*/

int qmod_cjpat_crpat_mode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf, int port, int pattern_type, int pkt_number, int pkt_size, int ipg_size)   /* CJPAT_CRPAT_MODE_SET  */
{

   int number_pkt, crc_check,intf_type, rx_port_sel, tx_port_sel;
   int pkt_size_int, ipg_size_int,payload_type; 
   uint16_t data16 ;

   BCMI_QTC_XGXS_PKTGENCTL1r_t  reg_pktgenctl1; 
   BCMI_QTC_XGXS_PKTGENCTL2r_t reg_pktgenctl2; 
   BCMI_QTC_XGXS_PAYLOADBYTESr_t payloadbytes;

  QMOD_DBG_IN_FUNC_INFO(pc);
  BCMI_QTC_XGXS_PKTGENCTL1r_CLR(reg_pktgenctl1); 
  BCMI_QTC_XGXS_PKTGENCTL2r_CLR(reg_pktgenctl2);
  BCMI_QTC_XGXS_PAYLOADBYTESr_CLR(payloadbytes);


   number_pkt = pkt_number ; 
   crc_check  = 1 ;
   rx_port_sel= port*4 ;
   tx_port_sel= port*4 ;
 

  pkt_size_int   = pkt_size ;
 /*  pkt_size_int   = 1;
   ipg_size_int   = 8 ;*/
   ipg_size_int   = ipg_size ; 

  /*  payload_type = pattern_type; */
  
  if ((pattern_type == 0x9) | (pattern_type == 0xA)) {
     payload_type = 0;
   } else {
     payload_type = pattern_type;
   }


   /* intf_type  0: XLGMII/XGMII;  1: MIII/GMII */

   intf_type = ((spd_intf == QMOD_SPD_10_X1_SGMII) |
     (spd_intf == QMOD_SPD_100_X1_SGMII) |
     (spd_intf == QMOD_SPD_1000_X1_SGMII) |
     (spd_intf == QMOD_SPD_10_SGMII) |
     (spd_intf == QMOD_SPD_100_SGMII) |
     (spd_intf == QMOD_SPD_1000_SGMII) |
     (spd_intf == QMOD_SPD_2500) |
     (spd_intf == QMOD_SPD_2500_X1)|
     (spd_intf == QMOD_SPD_10_X1_10)|
     (spd_intf == QMOD_SPD_100_X1_10)|
     (spd_intf == QMOD_SPD_1000_X1_10)|
     (spd_intf == QMOD_SPD_10_X1_12P5)|
     (spd_intf == QMOD_SPD_100_X1_12P5)|
     (spd_intf == QMOD_SPD_1000_X1_12P5)|
     (spd_intf == QMOD_SPD_2500_X1_12P5)
     ) ? 0 : 1;

    
 /* if payload type is Repeat 2 Bytes in 0x9040*/

  if (payload_type == 0) {
    data16 = 0x2323;
    BCMI_QTC_XGXS_PAYLOADBYTESr_BYTE1f_SET(payloadbytes, ((data16 >> 8)  & 0xff));
    BCMI_QTC_XGXS_PAYLOADBYTESr_BYTE0f_SET(payloadbytes, (data16 & 0xff));
    BCMI_QTC_XGXS_WRITE_PAYLOADBYTESr(pc,payloadbytes);
  }

   /* 0x9030 */

   BCMI_QTC_XGXS_PKTGENCTL1r_TX_PORT_SELf_SET(reg_pktgenctl1, tx_port_sel);
   BCMI_QTC_XGXS_PKTGENCTL1r_RX_PORT_SELf_SET(reg_pktgenctl1, rx_port_sel);
   BCMI_QTC_XGXS_PKTGENCTL1r_RX_PKT_CHECK_ENf_SET(reg_pktgenctl1, crc_check);
   BCMI_QTC_XGXS_PKTGENCTL1r_RX_BUS_TYPEf_SET(reg_pktgenctl1, intf_type);
   BCMI_QTC_XGXS_PKTGENCTL1r_NUMBER_PKTf_SET(reg_pktgenctl1, number_pkt);

   PHYMOD_IF_ERR_RETURN
      (BCMI_QTC_XGXS_WRITE_PKTGENCTL1r(pc, reg_pktgenctl1));

      /*BCMI_QTC_XGXS_WRITE_PKTGENCTL1r*/

   /* 0x9031 */
   
   BCMI_QTC_XGXS_PKTGENCTL2r_IPG_SIZEf_SET(reg_pktgenctl2, ipg_size_int);
   BCMI_QTC_XGXS_PKTGENCTL2r_PKT_SIZEf_SET(reg_pktgenctl2, pkt_size_int);
   BCMI_QTC_XGXS_PKTGENCTL2r_PAYLOAD_TYPEf_SET(reg_pktgenctl2, payload_type);
   BCMI_QTC_XGXS_PKTGENCTL2r_TX_BUS_TYPEf_SET(reg_pktgenctl2, intf_type);

   PHYMOD_IF_ERR_RETURN
      (BCMI_QTC_XGXS_WRITE_PKTGENCTL2r(pc, reg_pktgenctl2));
  


   return PHYMOD_E_NONE;
} /* int qmod_cjpat_crpat_mode_set */

/**
@brief   Checks CJPAT/CRPAT parameters for a particular port
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion

This function checks for CJPAT or CRPAT for a particular port.  The speed mode
of the lane must be chosen from the enumerated type #qmod_spd_intfc_type and
set in the #PHYMOD_ST::spd_intf field.  CJPAT/CRPAT must first enabled using
#qmod_cjpat_crpat_control().  The duration of packet transmission is controlled
externally and is the time duration between the two function calls above.

This function compares the CJPAT/CRPAT TX and RX counters and prints the
results.  Currently only packet count is supported.

This function reads the following registers:

\li PATGEN1_TXPKTCNT_U (0xC040) Upper 16b of count of Transmitted packets
\li PATGEN1_TXPKTCNT_L (0xC041) Lower 16b of count of Transmitted packets
\li PATGEN1_RXPKTCNT_U (0xC042) Upper 16b of count of Recieved    packets
\li PATGEN1_RXPKTCNT_L (0xC043) Lower 16b of count of Recieved    packets
\li PKTGEN0_CRCERRORCOUNT (0x9033) CRC error conunt 
*/

int qmod_cjpat_crpat_check(PHYMOD_ST* pc)     /* CJPAT_CRPAT_CHECK */
{
   return PHYMOD_E_FAIL;
#if 0
Disabled support for this function. Will figure out how to do this later.
   uint32_t sp0_tx_cnt, sp0_rx_cnt;
   uint32_t sp1_tx_cnt, sp1_rx_cnt;
   uint32_t sp2_tx_cnt, sp2_rx_cnt;
   uint32_t sp3_tx_cnt, sp3_rx_cnt;

   /* for SP0 */
   PKT_CTRS_X4_SP0_TXPKTCNT_Ur_t    sp0_reg_tx_u;  /* To do: change reg_tx_* names by adding SP0, SP1 .. : DONE*/
   PKT_CTRS_X4_SP0_TXPKTCNT_Lr_t    sp0_reg_tx_l;
   PKT_CTRS_X4_SP0_RXPKTCNT_Ur_t    sp0_reg_rx_u;
   PKT_CTRS_X4_SP0_RXPKTCNT_Lr_t    sp0_reg_rx_l;
   
  /*  for SP1  */
   PKT_CTRS_X4_SP1_TXPKTCNT_Ur_t    sp1_reg_tx_u;
   PKT_CTRS_X4_SP1_TXPKTCNT_Lr_t    sp1_reg_tx_l;
   PKT_CTRS_X4_SP1_RXPKTCNT_Ur_t    sp1_reg_rx_u;
   PKT_CTRS_X4_SP1_RXPKTCNT_Lr_t    sp1_reg_rx_l;
   
   /* for SP2  */
   PKT_CTRS_X4_SP2_TXPKTCNT_Ur_t    sp2_reg_tx_u;
   PKT_CTRS_X4_SP2_TXPKTCNT_Lr_t    sp2_reg_tx_l;
   PKT_CTRS_X4_SP2_RXPKTCNT_Ur_t    sp2_reg_rx_u;
   PKT_CTRS_X4_SP2_RXPKTCNT_Lr_t    sp2_reg_rx_l;
   
   /* for SP3  */
   PKT_CTRS_X4_SP3_TXPKTCNT_Ur_t    sp3_reg_tx_u;
   PKT_CTRS_X4_SP3_TXPKTCNT_Lr_t    sp3_reg_tx_l;
   PKT_CTRS_X4_SP3_RXPKTCNT_Ur_t    sp3_reg_rx_u;
   PKT_CTRS_X4_SP3_RXPKTCNT_Lr_t    sp3_reg_rx_l;

   CRCERRCNTr_t reg_crc_cnt;

   QMOD_DBG_IN_FUNC_INFO(pc);

   /* for SP0 */  /* To do: implement this functionality for all SP and add check for SGMII or QSGMII modes */

    /*Check to be performed  */
  if((pc->port== 0)||(pc->port == 4)||(pc->port == 8)||(pc->port == 12)) {
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP0_TXPKTCNT_Ur_CLR(sp0_reg_tx_u);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP0_TXPKTCNT_Ur(pc, &sp0_reg_tx_u);
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP0_TXPKTCNT_Lr_CLR(sp0_reg_tx_l);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP0_TXPKTCNT_Lr(pc, &sp0_reg_tx_l);
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP0_RXPKTCNT_Ur_CLR(sp0_reg_rx_u);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP0_RXPKTCNT_Ur(pc, &sp0_reg_rx_u);
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP0_RXPKTCNT_Lr_CLR(sp0_reg_rx_l);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP0_RXPKTCNT_Lr(pc, &sp0_reg_rx_l);

   BCMI_QTC_XGXS_CRCERRCNTr_CLR(reg_crc_cnt);
   BCMI_QTC_XGXS_READ_CRCERRCNTr(pc, &reg_crc_cnt);

   sp0_tx_cnt = BCMI_QTC_XGXS_PKT_CTRS_X4_SP0_TXPKTCNT_Ur_GET(sp0_reg_tx_u) <<16;
   sp0_tx_cnt |= BCMI_QTC_XGXS_PKT_CTRS_X4_SP0_TXPKTCNT_Lr_GET(sp0_reg_tx_l);

   sp0_rx_cnt = BCMI_QTC_XGXS_PKT_CTRS_X4_SP0_RXPKTCNT_Ur_GET(sp0_reg_rx_u) <<16;
   sp0_rx_cnt |= BCMI_QTC_XGXS_PKT_CTRS_X4_SP0_RXPKTCNT_Lr_GET(sp0_reg_rx_l);
    }


   BCMI_QTC_XGXS_PKT_CTRS_X4_SP1_TXPKTCNT_Ur_CLR(sp1_reg_tx_u);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP1_TXPKTCNT_Ur(pc, &sp1_reg_tx_u);
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP1_TXPKTCNT_Lr_CLR(sp1_reg_tx_l);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP1_TXPKTCNT_Lr(pc, &sp1_reg_tx_l);
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP1_RXPKTCNT_Ur_CLR(sp1_reg_rx_u);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP1_RXPKTCNT_Ur(pc, &sp1_reg_rx_u);
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP1_RXPKTCNT_Lr_CLR(sp1_reg_rx_l);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP1_RXPKTCNT_Lr(pc, &sp1_reg_rx_l);

   sp1_tx_cnt = BCMI_QTC_XGXS_PKT_CTRS_X4_SP1_TXPKTCNT_Ur_GET(sp1_reg_tx_u) <<16;
   sp1_tx_cnt |= BCMI_QTC_XGXS_PKT_CTRS_X4_SP1_TXPKTCNT_Lr_GET(sp1_reg_tx_l);

   sp1_rx_cnt = BCMI_QTC_XGXS_PKT_CTRS_X4_SP1_RXPKTCNT_Ur_GET(sp1_reg_rx_u) <<16;
   sp1_rx_cnt |= BCMI_QTC_XGXS_PKT_CTRS_X4_SP1_RXPKTCNT_Lr_GET(sp1_reg_rx_l);

   BCMI_QTC_XGXS_PKT_CTRS_X4_SP2_TXPKTCNT_Ur_CLR(sp2_reg_tx_u);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP2_TXPKTCNT_Ur(pc, &sp2_reg_tx_u);
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP2_TXPKTCNT_Lr_CLR(sp2_reg_tx_l);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP2_TXPKTCNT_Lr(pc, &sp2_reg_tx_l);
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP2_RXPKTCNT_Ur_CLR(sp2_reg_rx_u);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP2_RXPKTCNT_Ur(pc, &sp2_reg_rx_u);
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP2_RXPKTCNT_Lr_CLR(sp2_reg_rx_l);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP2_RXPKTCNT_Lr(pc, &sp2_reg_rx_l);

   sp2_tx_cnt = BCMI_QTC_XGXS_PKT_CTRS_X4_SP2_TXPKTCNT_Ur_GET(sp2_reg_tx_u) <<16;
   sp2_tx_cnt |= BCMI_QTC_XGXS_PKT_CTRS_X4_SP2_TXPKTCNT_Lr_GET(sp2_reg_tx_l);

   sp2_rx_cnt = BCMI_QTC_XGXS_PKT_CTRS_X4_SP2_RXPKTCNT_Ur_GET(sp2_reg_rx_u) <<16;
   sp2_rx_cnt |= BCMI_QTC_XGXS_PKT_CTRS_X4_SP2_RXPKTCNT_Lr_GET(sp2_reg_rx_l);

   BCMI_QTC_XGXS_PKT_CTRS_X4_SP3_TXPKTCNT_Ur_CLR(sp3_reg_tx_u);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP3_TXPKTCNT_Ur(pc, &sp3_reg_tx_u);
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP3_TXPKTCNT_Lr_CLR(sp3_reg_tx_l);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP3_TXPKTCNT_Lr(pc, &sp3_reg_tx_l);
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP3_RXPKTCNT_Ur_CLR(sp3_reg_rx_u);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP3_RXPKTCNT_Ur(pc, &sp3_reg_rx_u);
   BCMI_QTC_XGXS_PKT_CTRS_X4_SP3_RXPKTCNT_Lr_CLR(sp3_reg_rx_l);
   BCMI_QTC_XGXS_READ_PKT_CTRS_X4_SP3_RXPKTCNT_Lr(pc, &sp3_reg_rx_l);

   sp3_tx_cnt = BCMI_QTC_XGXS_PKT_CTRS_X4_SP3_TXPKTCNT_Ur_GET(sp3_reg_tx_u) <<16;
   sp3_tx_cnt |= BCMI_QTC_XGXS_PKT_CTRS_X4_SP3_TXPKTCNT_Lr_GET(sp3_reg_tx_l);

   sp3_rx_cnt = BCMI_QTC_XGXS_PKT_CTRS_X4_SP3_RXPKTCNT_Ur_GET(sp3_reg_rx_u) <<16;
   sp3_rx_cnt |= BCMI_QTC_XGXS_PKT_CTRS_X4_SP3_RXPKTCNT_Lr_GET(sp3_reg_rx_l);


#ifdef _DV_TB_
   pc->accData = 0 ;
#endif /* _DV_TB_ */
   if( ((sp0_tx_cnt != sp0_rx_cnt) ||(sp0_tx_cnt==0)) ) {
#ifdef _DV_TB_
  
      PHYMOD_DEBUG_ERROR(("%-22s ERROR: u=%0d p=%0d sp0_tx_cnt=%0d sp0_rx_cnt=%0d mismatch\n", __func__, pc->unit, pc->port, sp0_tx_cnt, sp0_rx_cnt));
      pc->accData = 1;
#endif /* _DV_TB_ */
      return PHYMOD_E_FAIL; 
   }
 /* Check the other cnt regs */
   if( ((sp1_tx_cnt != sp1_rx_cnt) ||(sp1_tx_cnt!=0))||((sp2_tx_cnt != sp2_rx_cnt) ||(sp2_tx_cnt!=0))||((sp3_tx_cnt != sp3_rx_cnt) ||(sp3_tx_cnt!=0)) ) {
#ifdef _DV_TB_
  
      PHYMOD_DEBUG_ERROR(("%-22s ERROR: u=%0d p=%0d sp1_tx_cnt=%0d sp1_rx_cnt=%0d sp2_tx_cnt=%0d sp2_rx_cnt=%0d sp3_tx_cnt=%0d sp3_rx_cnt=%0d mismatch\n", __func__, pc->unit, pc->port, sp1_tx_cnt, sp1_rx_cnt, sp2_tx_cnt, sp2_rx_cnt, sp3_tx_cnt, sp3_rx_cnt ));
      pc->accData = 1;
#endif /* _DV_TB_ */
      return PHYMOD_E_FAIL; 
   }


   if(reg_crc_cnt.crcerrcnt[0]) {
#ifdef _DV_TB_
  
      PHYMOD_DEBUG_ERROR(("%-22s ERROR: u=%0d p=%0d crc error=%0d\n", __func__, pc->unit, pc->port, reg_crc_cnt.crcerrcnt[0]));
      pc->accData = 1;
#endif /* _DV_TB_ */
      return PHYMOD_E_FAIL; 
      }

   return PHYMOD_E_NONE;
#endif
}


int qmod_cjpat_crpat_control(PHYMOD_ST* pc, int enable)         /* CJPAT_CRPAT_CONTROL  */
{

  PKTGENCTL2r_t    reg;

  QMOD_DBG_IN_FUNC_INFO(pc);
  PKTGENCTL2r_CLR(reg);
  /* 0x9031 */
  PHYMOD_IF_ERR_RETURN
     (READ_PKTGENCTL2r(pc, &reg));

  if(enable) {
  BCMI_QTC_XGXS_PKTGENCTL2r_PKTGEN_ENf_SET(reg,  1);

  } else {
  BCMI_QTC_XGXS_PKTGENCTL2r_PKTGEN_ENf_SET(reg,  0);
  }
   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGENCTL2r(pc, reg));


  return PHYMOD_E_NONE;
}


int qmod_pcs_lane_swap(PHYMOD_ST *pc, int lane_map)
{

/*  unsigned int pcs_map;*/
  MAIN_LN_SWPr_t reg;
  QMOD_DBG_IN_FUNC_INFO(pc) ;

  MAIN_LN_SWPr_CLR(reg);

  MAIN_LN_SWPr_LOG0_TO_PHY_LNSWAP_SELf_SET(reg, (lane_map >> 0)  & 0x3);
  MAIN_LN_SWPr_LOG1_TO_PHY_LNSWAP_SELf_SET(reg, (lane_map >> 4)  & 0x3);
  MAIN_LN_SWPr_LOG2_TO_PHY_LNSWAP_SELf_SET(reg, (lane_map >> 8)  & 0x3);
  MAIN_LN_SWPr_LOG3_TO_PHY_LNSWAP_SELf_SET(reg, (lane_map >> 12)  & 0x3 );


  PHYMOD_IF_ERR_RETURN
      (MODIFY_MAIN_LN_SWPr(pc, reg)) ;

  return PHYMOD_E_NONE ;
}

#ifdef _DV_TB_
/**
@brief   sets PMD TX lane swap values for all lanes simultaneously.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion

This function sets the TX lane swap values for all lanes simultaneously.

The TSCE has several two sets of lane swap registers per lane.  This function
uses the lane swap registers closest to the pads.  For TX, the lane swap occurs
after all other analog/digital functionality.  For RX, the lane swap occurs
prior to all other analog/digital functionality.

It is not advisable to swap lanes without going through reset.

How the swap is done is indicated in the #PHYMOD_ST::per_lane_control field;
bits 0 through 15 represent the swap settings for RX while bits 16
through 31 represent the swap settings for TX.

Each 4 bit chunk within both the RX and TX sections represent the swap settings
for a particular lane as follows:

Bits lane assignment

\li 15:12  TX lane 3
\li 11:8   TX lane 2
\li 7:4    TX lane 1
\li 3:0    TX lane 0

Each 4-bit value may be either 0, 1, 2 or 3, with the value indicating the lane
mapping.  By default, the equivalent per_lane_control settings are 0x3210_3210,
indicating that there is no re-routing of traffic.  As an example, to swap lanes
0 and 1 for both RX and TX, the per_lane_control should be set to 0x3201_3201.
To swap lanes 0 and 1, and also lanes 2 and 3 for both RX and TX, the
per_lane_control should be set to 0x2301_2301.

*/
int qmod_pmd_lane_swap(PHYMOD_ST *pc)
{
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_t reg_adr_01;
  DIG_LN_ADDR_2_3r_t reg_adr_23;
  unsigned int map;
  unsigned int lane_addr_0, lane_addr_1, lane_addr_2, lane_addr_3;

  QMOD_DBG_IN_FUNC_INFO(pc);
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_CLR(reg_adr_01);
  DIG_LN_ADDR_2_3r_CLR(reg_adr_23);

  map = (unsigned int)pc->per_lane_control;

  lane_addr_0 = (map >> 16) & 0xf;
  lane_addr_1 = (map >> 20) & 0xf;
  lane_addr_2 = (map >> 24) & 0xf;
  lane_addr_3 = (map >> 28) & 0xf;

#ifdef _DV_TB_
  pc->adjust_port_mode = 1;
#endif
#ifdef _SDK_QMOD_

#endif
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_LANE_ADDR_0f_SET(reg_adr_01, lane_addr_0);
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_LANE_ADDR_1f_SET(reg_adr_01, lane_addr_1);
  PHYMOD_IF_ERR_RETURN
     (MODIFY_DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r(pc, reg_adr_01));

  DIG_LN_ADDR_2_3r_LANE_ADDR_2f_SET(reg_adr_23, lane_addr_2);
  DIG_LN_ADDR_2_3r_LANE_ADDR_3f_SET(reg_adr_23, lane_addr_3);

  PHYMOD_IF_ERR_RETURN
     (WRITE_DIG_LN_ADDR_2_3r(pc, reg_adr_23));

#ifdef _DV_TB_
  pc->adjust_port_mode = 0;
#endif
#ifdef _SDK_QMOD_

#endif
  return PHYMOD_E_NONE ;
}
#endif /* _DV_TB_ */

/**
@brief   sets PMD TX lane swap values for all lanes simultaneously.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion

This function sets the TX lane swap values for all lanes simultaneously.

The TSCE has several two sets of lane swap registers per lane.  This function uses the
lane swap registers closest to the pads.  For TX, the lane swap occurs after all other
analog/digital functionality.  For RX, the lane swap occurs prior to all other
analog/digital functionality.

It is not advisable to swap lanes without going through reset.

How the swap is done is indicated in the #PHYMOD_ST::per_lane_control field;
bits 0 through 15 represent the swap settings for RX while bits 16
through 31 represent the swap settings for TX.

Each 4 bit chunk within both the RX and TX sections represent the swap settings
for a particular lane as follows:

Bits lane assignment

\li 15:12  TX lane 3
\li 11:8   TX lane 2
\li 7:4    TX lane 1
\li 3:0    TX lane 0

Each 4-bit value may be either 0, 1, 2 or 3, with the value indicating the lane
mapping.  By default, the equivalent per_lane_control settings are 0x3210_3210,
indicating that there is no re-routing of traffic.  As an example, to swap lanes
0 and 1 for both RX and TX, the per_lane_control should be set to 0x3201_3201.
To swap lanes 0 and 1, and also lanes 2 and 3 for both RX and TX, the
per_lane_control should be set to 0x2301_2301.

*/

int qmod_pmd_lane_swap_tx(PHYMOD_ST *pc, uint32_t tx_lane_map)
{
  uint16_t tx_lane_map_0, tx_lane_map_1, tx_lane_map_2, tx_lane_map_3;
  DIG_TX_LN_MAP_0_1_2r_t                reg;
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_t    reg1;

  QMOD_DBG_IN_FUNC_INFO(pc);
  DIG_TX_LN_MAP_0_1_2r_CLR(reg);
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_CLR(reg1);

  tx_lane_map_0 = tx_lane_map       & 0xf;
  tx_lane_map_1 = (tx_lane_map>>4)  & 0xf;
  tx_lane_map_2 = (tx_lane_map>>8) & 0xf;
  tx_lane_map_3 = (tx_lane_map>>12) & 0xf;

#ifdef _DV_TB_
  pc->adjust_port_mode = 1;
#endif
#ifdef _SDK_QMOD_

#endif

  DIG_TX_LN_MAP_0_1_2r_TX_LANE_MAP_0f_SET(reg, tx_lane_map_0);
  DIG_TX_LN_MAP_0_1_2r_TX_LANE_MAP_1f_SET(reg, tx_lane_map_1);
  DIG_TX_LN_MAP_0_1_2r_TX_LANE_MAP_2f_SET(reg, tx_lane_map_2);

  PHYMOD_IF_ERR_RETURN
     (WRITE_DIG_TX_LN_MAP_0_1_2r(pc, reg));

  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_TX_LANE_MAP_3f_SET(reg1, tx_lane_map_3);
  PHYMOD_IF_ERR_RETURN
     (MODIFY_DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r(pc, reg1));

#ifdef _DV_TB_
  pc->adjust_port_mode = 0;
#endif
#ifdef _SDK_QMOD_

#endif
  return PHYMOD_E_NONE ;
}

/**
@brief   sets PMD TX lane swap values for all lanes simultaneously.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   addr_lane_index lane swap info as described in description
@returns The value PHYMOD_E_NONE upon successful completion

This function sets the TX lane swap values for all lanes simultaneously.

The TSCE has several two sets of lane swap registers per lane.  This function
uses the lane swap registers closest to the pads.  For TX, the lane swap occurs
after all other analog/digital functionality.  For RX, the lane swap occurs
prior to all other analog/digital functionality.

It is not advisable to swap lanes without going through reset.

How the swap is done is indicated in the lane control field;
bits 0 through 15 represent the swap settings for RX while bits 16
through 31 represent the swap settings for TX.

Each 4 bit chunk within both the RX and TX sections represent the swap settings
for a particular lane as follows:

Bits lane assignment

\li 15:12  TX lane 3
\li 11:8   TX lane 2
\li 7:4    TX lane 1
\li 3:0    TX lane 0

Each 4-bit value may be either 0, 1, 2 or 3, with the value indicating the lane
mapping.  By default, the equivalent addr_lane_index settings are 0x3210_3210,
indicating that there is no re-routing of traffic.  As an example, to swap lanes
0 and 1 for both RX and TX, the addr_lane_index should be set to 0x3201_3201.
To swap lanes 0 and 1, and also lanes 2 and 3 for both RX and TX, the
addr_lane_index should be set to 0x2301_2301.

*/
int qmod_pmd_addr_lane_swap(PHYMOD_ST *pc, uint32_t addr_lane_index)
{
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_t reg_adr_01;
  DIG_LN_ADDR_2_3r_t reg_adr_23;
  unsigned int map;
  unsigned int lane_addr_0, lane_addr_1, lane_addr_2, lane_addr_3;

  QMOD_DBG_IN_FUNC_INFO(pc);
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_CLR(reg_adr_01);
  DIG_LN_ADDR_2_3r_CLR(reg_adr_23);

#ifdef _DV_TB_
  map = (unsigned int)pc->per_lane_control;

  lane_addr_0 = (map >> 16) & 0xf;
  lane_addr_1 = (map >> 20) & 0xf;
  lane_addr_2 = (map >> 24) & 0xf;
  lane_addr_3 = (map >> 28) & 0xf;

#else
  map = addr_lane_index ;

  lane_addr_0 = (map >> 0) & 0xf;
  lane_addr_1 = (map >> 4) & 0xf;
  lane_addr_2 = (map >> 8) & 0xf;
  lane_addr_3 = (map >> 12) & 0xf;
#endif

#ifdef _DV_TB_
  pc->adjust_port_mode = 1;
#endif

#ifdef _SDK_QMOD_

#endif

  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_LANE_ADDR_0f_SET(reg_adr_01, lane_addr_0);
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_LANE_ADDR_1f_SET(reg_adr_01, lane_addr_1);
  PHYMOD_IF_ERR_RETURN
     (MODIFY_DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r(pc, reg_adr_01));

  DIG_LN_ADDR_2_3r_LANE_ADDR_2f_SET(reg_adr_23, lane_addr_2);
  DIG_LN_ADDR_2_3r_LANE_ADDR_3f_SET(reg_adr_23, lane_addr_3);

  PHYMOD_IF_ERR_RETURN
     (WRITE_DIG_LN_ADDR_2_3r(pc, reg_adr_23));

#ifdef _DV_TB_
  pc->adjust_port_mode = 0;
#endif
#ifdef _SDK_QMOD_

#endif

  return PHYMOD_E_NONE ;
}


/**  
@brief   Control pram ablity
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable Enable value for PRAM set.
@returns The value PHYMOD_E_NONE upon successful completion
@details Per core PMD resets (both datapath and entire core)
We only intend to use this function if the PMD has never been initialized.
*/
/* ???? */
int qmod_pram_abl_enable_set(PHYMOD_ST* pc, int enable) /* PMD_RESET_SEQ */
{
  PMD_X1_CTLr_t reg_pmd_x1_ctrl;

  PMD_X1_CTLr_CLR(reg_pmd_x1_ctrl);

  PMD_X1_CTLr_PRAM_ABILITYf_SET(reg_pmd_x1_ctrl,enable);
  PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_CTLr(pc,reg_pmd_x1_ctrl));

  return PHYMOD_E_NONE;
}

/**
@brief   is used to power down transmitter or receiver per lane basis.
@param   pc handle to current TSCE context (a.k.a #PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details

This function is used to power on/off the TX and RX lanes in one swoop.  To
change the power down state for TX or RX, bit-wise OR one of the following

\li CHANGE_TX0_PWRED_STATE    0x10
\li CHANGE_TX1_PWRED_STATE    0x20
\li CHANGE_TX2_PWRED_STATE    0x40
\li CHANGE_TX3_PWRED_STATE    0x80

<br>

\li CHANGE_RX0_PWRED_STATE  0x1000
\li CHANGE_RX1_PWRED_STATE  0x2000
\li CHANGE_RX2_PWRED_STATE  0x4000
\li CHANGE_RX3_PWRED_STATE  0x8000

into #PHYMOD_ST::per_lane_control.  These values serve as masks for whether the
power on/off for a particular lane should be changed.

To turn off the power for the selected TX/RX lane, bit-wise OR one of these

\li PWR_DOWN_TX0   0x1
\li PWR_DOWN_TX1   0x2
\li PWR_DOWN_TX2   0x4
\li PWR_DOWN_TX3   0x8

<br>

\li PWR_DOWN_RX0  0x100 
\li PWR_DOWN_RX1  0x200
\li PWR_DOWN_RX2  0x400  
\li PWR_DOWN_RX3  0x800

into #PHYMOD_ST::per_lane_control.

As an example, to turn off the TX power on lane 0, turn on the TX power on lane1,
leave the RX power states along with the TX power on lanes 2 and 3 unaffected, set

#PHYMOD_ST::per_lane_control = CHANGE_TX0_PWRED_STATE |
                              PWR_DOWN_TX0 |
                              CHANGE_TX1_PWRED_STATE;
*/
int qmod_power_control(PHYMOD_ST* pc)
{
  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful
@details PCS Init calls this Per Port PCS Init routine
*/
int _qmod_set_port_mode_select(PHYMOD_ST* pc)
{
   MAIN_SETUPr_t reg;
 
    MAIN_LN_SWPr_t ln_swp_reg;
  QMOD_DBG_IN_FUNC_INFO(pc);

  MAIN_LN_SWPr_CLR(ln_swp_reg);
 
  MAIN_LN_SWPr_MAC_TO_PCS_LANE_MAP_SEL_SGMIIf_SET(ln_swp_reg,pc->pcs_lane_map_sel);
  
  PHYMOD_IF_ERR_RETURN
      (MODIFY_MAIN_LN_SWPr(pc, ln_swp_reg)) ;

   MAIN_SETUPr_CLR(reg);
   if(pc->tsc_touched == 1) {
        MAIN_SETUPr_CLR(reg);
   } else {
      MAIN_SETUPr_MASTER_PORT_NUMf_SET(reg, pc->master_portnum);
      MAIN_SETUPr_PLL_RESET_ENf_SET(reg, 1);
   }
  
   if(pc->sc_mode == QMOD_SC_MODE_AN_CL37) {
      MAIN_SETUPr_CL37_HIGH_VCOf_SET(reg, pc->cl37_high_vco);
      MAIN_SETUPr_SINGLE_PORT_MODEf_SET(reg, pc->single_port_mode);
      MAIN_SETUPr_REFCLK_SELf_SET(reg, pc->refclk);
      PHYMOD_IF_ERR_RETURN (MODIFY_MAIN_SETUPr(pc, reg));
   } else {
      MAIN_SETUPr_CL37_HIGH_VCOf_SET(reg, pc->cl37_high_vco);
      MAIN_SETUPr_REFCLK_SELf_SET(reg, pc->refclk);
      PHYMOD_IF_ERR_RETURN (MODIFY_MAIN_SETUPr(pc, reg));
  }
  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init

*/
/* TBD  PARAMETERIZE  CODE for SDK*/
int _configure_st_entry(int st_entry_num, int st_entry_speed, PHYMOD_ST* pc)
{

  SC_X1_SPD_OVRR0_SPDr_t    reg0_ovr_spd;
  SC_X1_SPD_OVRR0_0r_t        reg0_ovr;
  SC_X1_SPD_OVRR0_1r_t        reg0_ovr1;
  SC_X1_SPD_OVRR0_2r_t        reg0_ovr2;
  SC_X1_SPD_OVRR0_3r_t        reg0_ovr3;
  SC_X1_SPD_OVRR0_4r_t        reg0_ovr4;
  SC_X1_SPD_OVRR0_5r_t        reg0_ovr5;
  SC_X1_SPD_OVRR0_6r_t        reg0_ovr6;

  SC_X1_SPD_OVRR1_SPDr_t    reg1_ovr_spd;
  SC_X1_SPD_OVRR1_0r_t        reg1_ovr;
  SC_X1_SPD_OVRR1_1r_t        reg1_ovr1;
  SC_X1_SPD_OVRR1_2r_t        reg1_ovr2;
  SC_X1_SPD_OVRR1_3r_t        reg1_ovr3;
  SC_X1_SPD_OVRR1_4r_t        reg1_ovr4;
  SC_X1_SPD_OVRR1_5r_t        reg1_ovr5;
  SC_X1_SPD_OVRR1_6r_t        reg1_ovr6;

  SC_X1_SPD_OVRR2_SPDr_t    reg2_ovr_spd;
  SC_X1_SPD_OVRR2_0r_t        reg2_ovr;
  SC_X1_SPD_OVRR2_1r_t        reg2_ovr1;
  SC_X1_SPD_OVRR2_2r_t        reg2_ovr2;
  SC_X1_SPD_OVRR2_3r_t        reg2_ovr3;
  SC_X1_SPD_OVRR2_4r_t        reg2_ovr4;
  SC_X1_SPD_OVRR2_5r_t        reg2_ovr5;
  SC_X1_SPD_OVRR2_6r_t        reg2_ovr6;

  SC_X1_SPD_OVRR3_SPDr_t    reg3_ovr_spd;
  SC_X1_SPD_OVRR3_0r_t        reg3_ovr;
  SC_X1_SPD_OVRR3_1r_t        reg3_ovr1;
  SC_X1_SPD_OVRR3_2r_t        reg3_ovr2;
  SC_X1_SPD_OVRR3_3r_t        reg3_ovr3;
  SC_X1_SPD_OVRR3_4r_t        reg3_ovr4;
  SC_X1_SPD_OVRR3_5r_t        reg3_ovr5;
  SC_X1_SPD_OVRR3_6r_t        reg3_ovr6;

if(st_entry_num == 0) {

  SC_X1_SPD_OVRR0_SPDr_CLR(reg0_ovr_spd);
  SC_X1_SPD_OVRR0_SPDr_SPEEDf_SET(reg0_ovr_spd, st_entry_speed);

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_SPDr(pc, reg0_ovr_spd));

  SC_X1_SPD_OVRR0_0r_CLR(reg0_ovr);
  SC_X1_SPD_OVRR0_1r_CLR(reg0_ovr1);
  SC_X1_SPD_OVRR0_2r_CLR(reg0_ovr2);
  SC_X1_SPD_OVRR0_3r_CLR(reg0_ovr3);
  SC_X1_SPD_OVRR0_4r_CLR(reg0_ovr4);
  SC_X1_SPD_OVRR0_5r_CLR(reg0_ovr5);
  SC_X1_SPD_OVRR0_6r_CLR(reg0_ovr6);

  SC_X1_SPD_OVRR0_0r_OS_MODEf_SET(reg0_ovr, pc->t_pma_os_mode);
  SC_X1_SPD_OVRR0_0r_CL36BYTEDELETEMODEf_SET(reg0_ovr, pc->r_desc2_byte_deletion);
  SC_X1_SPD_OVRR0_0r_QSGMII_ENf_SET(reg0_ovr, pc->qsgmii_mode);



  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_0r(pc, reg0_ovr));

  SC_X1_SPD_OVRR0_1r_CLOCKCNT0f_SET(reg0_ovr1, pc->clkcnt0);
  SC_X1_SPD_OVRR0_1r_SGMII_SPD_SWITCHf_SET(reg0_ovr1, pc->sgmii);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_1r(pc, reg0_ovr1));

  SC_X1_SPD_OVRR0_2r_CLOCKCNT1f_SET(reg0_ovr2, pc->clkcnt1);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_2r(pc, reg0_ovr2));

  SC_X1_SPD_OVRR0_3r_LOOPCNT1f_SET(reg0_ovr3, pc->lpcnt1);
  SC_X1_SPD_OVRR0_3r_LOOPCNT0f_SET(reg0_ovr3, pc->lpcnt0);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_3r(pc, reg0_ovr3));

  SC_X1_SPD_OVRR0_4r_MAC_CREDITGENCNTf_SET(reg0_ovr4, pc->mac_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_4r(pc, reg0_ovr4));

  
  SC_X1_SPD_OVRR0_5r_PCS_CLOCKCNT0f_SET(reg0_ovr5, pc->pcs_clkcnt);
  SC_X1_SPD_OVRR0_5r_PCS_CREDITENABLEf_SET(reg0_ovr5, pc->pcs_crdten);
  SC_X1_SPD_OVRR0_5r_REPLICATION_CNTf_SET(reg0_ovr5, pc->pcs_repcnt);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_5r(pc, reg0_ovr5));

  SC_X1_SPD_OVRR0_6r_PCS_CREDITGENCNTf_SET(reg0_ovr6, pc->pcs_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_6r(pc, reg0_ovr6));
} /* (st_entry_num == 0) */

else if(st_entry_num == 1) {


 SC_X1_SPD_OVRR1_SPDr_CLR(reg1_ovr_spd);
  SC_X1_SPD_OVRR1_SPDr_SPEEDf_SET(reg1_ovr_spd, st_entry_speed);

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_SPDr(pc, reg1_ovr_spd));

  SC_X1_SPD_OVRR1_0r_CLR(reg1_ovr);
  SC_X1_SPD_OVRR1_1r_CLR(reg1_ovr1);
  SC_X1_SPD_OVRR1_2r_CLR(reg1_ovr2);
  SC_X1_SPD_OVRR1_3r_CLR(reg1_ovr3);
  SC_X1_SPD_OVRR1_4r_CLR(reg1_ovr4);
  SC_X1_SPD_OVRR1_5r_CLR(reg1_ovr5);
  SC_X1_SPD_OVRR1_6r_CLR(reg1_ovr6);

  SC_X1_SPD_OVRR1_0r_OS_MODEf_SET(reg1_ovr, pc->t_pma_os_mode);
  SC_X1_SPD_OVRR1_0r_CL36BYTEDELETEMODEf_SET(reg1_ovr, pc->r_desc2_byte_deletion);
  SC_X1_SPD_OVRR1_0r_QSGMII_ENf_SET(reg1_ovr, pc->qsgmii_mode);



  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_0r(pc, reg1_ovr));

  SC_X1_SPD_OVRR1_1r_CLOCKCNT0f_SET(reg1_ovr1, pc->clkcnt0);
  SC_X1_SPD_OVRR1_1r_SGMII_SPD_SWITCHf_SET(reg1_ovr1, pc->sgmii);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_1r(pc, reg1_ovr1));

  SC_X1_SPD_OVRR1_2r_CLOCKCNT1f_SET(reg1_ovr2, pc->clkcnt1);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_2r(pc, reg1_ovr2));

  SC_X1_SPD_OVRR1_3r_LOOPCNT1f_SET(reg1_ovr3, pc->lpcnt1);
  SC_X1_SPD_OVRR1_3r_LOOPCNT0f_SET(reg1_ovr3, pc->lpcnt0);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_3r(pc, reg1_ovr3));

  SC_X1_SPD_OVRR1_4r_MAC_CREDITGENCNTf_SET(reg1_ovr4, pc->mac_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_4r(pc, reg1_ovr4));

  
  SC_X1_SPD_OVRR1_5r_PCS_CLOCKCNT0f_SET(reg1_ovr5, pc->pcs_clkcnt);
  SC_X1_SPD_OVRR1_5r_PCS_CREDITENABLEf_SET(reg1_ovr5, pc->pcs_crdten);
  SC_X1_SPD_OVRR1_5r_REPLICATION_CNTf_SET(reg1_ovr5, pc->pcs_repcnt);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_5r(pc, reg1_ovr5));

  SC_X1_SPD_OVRR1_6r_PCS_CREDITGENCNTf_SET(reg1_ovr6, pc->pcs_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_6r(pc, reg1_ovr6));



} /* (st_entry_num == 1) */

else if(st_entry_num == 2) {

 SC_X1_SPD_OVRR2_SPDr_CLR(reg2_ovr_spd);
  SC_X1_SPD_OVRR2_SPDr_SPEEDf_SET(reg2_ovr_spd, st_entry_speed);

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_SPDr(pc, reg2_ovr_spd));

  SC_X1_SPD_OVRR2_0r_CLR(reg2_ovr);
  SC_X1_SPD_OVRR2_1r_CLR(reg2_ovr1);
  SC_X1_SPD_OVRR2_2r_CLR(reg2_ovr2);
  SC_X1_SPD_OVRR2_3r_CLR(reg2_ovr3);
  SC_X1_SPD_OVRR2_4r_CLR(reg2_ovr4);
  SC_X1_SPD_OVRR2_5r_CLR(reg2_ovr5);
  SC_X1_SPD_OVRR2_6r_CLR(reg2_ovr6);

  SC_X1_SPD_OVRR2_0r_OS_MODEf_SET(reg2_ovr, pc->t_pma_os_mode);
  SC_X1_SPD_OVRR2_0r_CL36BYTEDELETEMODEf_SET(reg2_ovr, pc->r_desc2_byte_deletion);
  SC_X1_SPD_OVRR2_0r_QSGMII_ENf_SET(reg2_ovr, pc->qsgmii_mode);



  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_0r(pc, reg2_ovr));

  SC_X1_SPD_OVRR2_1r_CLOCKCNT0f_SET(reg2_ovr1, pc->clkcnt0);
  SC_X1_SPD_OVRR2_1r_SGMII_SPD_SWITCHf_SET(reg2_ovr1, pc->sgmii);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_1r(pc, reg2_ovr1));

  SC_X1_SPD_OVRR2_2r_CLOCKCNT1f_SET(reg2_ovr2, pc->clkcnt1);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_2r(pc, reg2_ovr2));

  SC_X1_SPD_OVRR2_3r_LOOPCNT1f_SET(reg2_ovr3, pc->lpcnt1);
  SC_X1_SPD_OVRR2_3r_LOOPCNT0f_SET(reg2_ovr3, pc->lpcnt0);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_3r(pc, reg2_ovr3));

  SC_X1_SPD_OVRR2_4r_MAC_CREDITGENCNTf_SET(reg2_ovr4, pc->mac_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_4r(pc, reg2_ovr4));

  
  SC_X1_SPD_OVRR2_5r_PCS_CLOCKCNT0f_SET(reg2_ovr5, pc->pcs_clkcnt);
  SC_X1_SPD_OVRR2_5r_PCS_CREDITENABLEf_SET(reg2_ovr5, pc->pcs_crdten);
  SC_X1_SPD_OVRR2_5r_REPLICATION_CNTf_SET(reg2_ovr5, pc->pcs_repcnt);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_5r(pc, reg2_ovr5));

  SC_X1_SPD_OVRR2_6r_PCS_CREDITGENCNTf_SET(reg2_ovr6, pc->pcs_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_6r(pc, reg2_ovr6));


} /* (st_entry_num == 2) */

else  /*(st_entry_num == 3) */ {

 SC_X1_SPD_OVRR3_SPDr_CLR(reg3_ovr_spd);
  SC_X1_SPD_OVRR3_SPDr_SPEEDf_SET(reg3_ovr_spd, st_entry_speed);

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_SPDr(pc, reg3_ovr_spd));

  SC_X1_SPD_OVRR3_0r_CLR(reg3_ovr);
  SC_X1_SPD_OVRR3_1r_CLR(reg3_ovr1);
  SC_X1_SPD_OVRR3_2r_CLR(reg3_ovr2);
  SC_X1_SPD_OVRR3_3r_CLR(reg3_ovr3);
  SC_X1_SPD_OVRR3_4r_CLR(reg3_ovr4);
  SC_X1_SPD_OVRR3_5r_CLR(reg3_ovr5);
  SC_X1_SPD_OVRR3_6r_CLR(reg3_ovr6);

  SC_X1_SPD_OVRR3_0r_OS_MODEf_SET(reg3_ovr, pc->t_pma_os_mode);
  SC_X1_SPD_OVRR3_0r_CL36BYTEDELETEMODEf_SET(reg3_ovr, pc->r_desc2_byte_deletion);
  SC_X1_SPD_OVRR3_0r_QSGMII_ENf_SET(reg3_ovr, pc->qsgmii_mode);



  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_0r(pc, reg3_ovr));

  SC_X1_SPD_OVRR3_1r_CLOCKCNT0f_SET(reg3_ovr1, pc->clkcnt0);
  SC_X1_SPD_OVRR3_1r_SGMII_SPD_SWITCHf_SET(reg3_ovr1, pc->sgmii);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_1r(pc, reg3_ovr1));

  SC_X1_SPD_OVRR3_2r_CLOCKCNT1f_SET(reg3_ovr2, pc->clkcnt1);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_2r(pc, reg3_ovr2));

  SC_X1_SPD_OVRR3_3r_LOOPCNT1f_SET(reg3_ovr3, pc->lpcnt1);
  SC_X1_SPD_OVRR3_3r_LOOPCNT0f_SET(reg3_ovr3, pc->lpcnt0);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_3r(pc, reg3_ovr3));

  SC_X1_SPD_OVRR3_4r_MAC_CREDITGENCNTf_SET(reg3_ovr4, pc->mac_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_4r(pc, reg3_ovr4));

  
  SC_X1_SPD_OVRR3_5r_PCS_CLOCKCNT0f_SET(reg3_ovr5, pc->pcs_clkcnt);
  SC_X1_SPD_OVRR3_5r_PCS_CREDITENABLEf_SET(reg3_ovr5, pc->pcs_crdten);
  SC_X1_SPD_OVRR3_5r_REPLICATION_CNTf_SET(reg3_ovr5, pc->pcs_repcnt);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_5r(pc, reg3_ovr5));

  SC_X1_SPD_OVRR3_6r_PCS_CREDITGENCNTf_SET(reg3_ovr6, pc->pcs_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_6r(pc, reg3_ovr6));
} /* (st_entry_num == 3) */
  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init
*/
int _qmod_configure_sc_speed_configuration(PHYMOD_ST* pc)
{
  int data =0;
  int mask =0;
  int st_entry =0;
  int speed_entry = 0;
  PHYMOD_ST pc_st_tmp;
  PHYMOD_ST* pc_st;

  if(pc->sc_mode == QMOD_SC_MODE_HT_WITH_BASIC_OVERRIDE){
    /* Configure the ffec_eb regsiter */
    data |= (1 << 10);
    mask |= 0x0400;

  }
  else if(pc->sc_mode == QMOD_SC_MODE_ST) {
     st_entry = pc->per_lane_control & 0xff;
     speed_entry = (pc->per_lane_control & 0xff00) >> 8;

/*
    A pc_st_tmp is created so the overriden expected values passed from SV are not 
    loaded in ST, and the ST has the same HT values
*/
     init_qmod_st(&pc_st_tmp);
     pc_st_tmp.spd_intf = pc->spd_intf;
     pc_st_tmp.prt_ad = pc->prt_ad;
     pc_st_tmp.unit = pc->unit;
     pc_st_tmp.port_type = pc->port_type;
     pc_st_tmp.this_lane = pc->this_lane;
     pc_st_tmp.sc_mode = pc->sc_mode;
     pc_st = &pc_st_tmp;
     qmod_sc_lkup_table(pc_st);
     /*configure the ST regsiters */
     _configure_st_entry(st_entry, speed_entry,  pc_st);

  }
  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init
*/
int _qmod_init_pcs_an(PHYMOD_ST* pc)             
{
  UC_COMMAND4r_t reg_com4;
  UC_COMMANDr_t reg_com;

  UC_COMMAND4r_CLR(reg_com4);
  UC_COMMANDr_CLR(reg_com);

  if(pc->uc_active){
    UC_COMMAND4r_MICRO_SYSTEM_CLK_ENf_SET(reg_com4,1);
    UC_COMMAND4r_MICRO_SYSTEM_RESET_Nf_SET(reg_com4,1);
    PHYMOD_IF_ERR_RETURN(MODIFY_UC_COMMAND4r(pc, reg_com4));
      
    UC_COMMANDr_MICRO_MDIO_DW8051_RESET_Nf_SET(reg_com,1);
    PHYMOD_IF_ERR_RETURN(MODIFY_UC_COMMANDr(pc, reg_com));
  }
  qmod_set_an_port_mode(pc);
  pc->port_type=QMOD_MULTI_PORT;
  qmod_autoneg_set(pc);

  qmod_autoneg_control(pc);

   return PHYMOD_E_NONE;
}
#endif /* _DV_TB_ */
#ifdef _SDK_QMOD_TEMP_ 

int _qmod_init_pcs_an(PHYMOD_ST* pc)             
{
  return PHYMOD_E_NONE;
}
#endif /* _SDK_QMOD_TEMP_ */

/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init
*/

int qmod_trigger_speed_change(PHYMOD_ST* pc)
{
  SC_X4_CTLr_t    reg; 
  
  QMOD_DBG_IN_FUNC_INFO(pc);
#ifdef _DV_TB_
  pc->adjust_port_mode = 1; 
#endif /* _DV_TB_ */
#ifdef _SDK_QMOD_

#endif

  /* write 0 to the speed change */
  SC_X4_CTLr_CLR(reg);
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc, &reg));
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg, 0);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, reg));

  /* write 1 to the speed change. No need to read again before write*/
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, reg));

#ifdef _DV_TB_
  pc->adjust_port_mode = 0; 
#endif /* _DV_TB_ */
#ifdef _SDK_QMOD_

#endif

  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init
*/
int _read_and_compare_final_status(PHYMOD_ST* pc)             
{
   uint16_t fail;
   int  actual_speed;
   PHYMOD_ST   sc_final_stats_actual;
   PHYMOD_ST*  sc_final_stats_actual_tmp;

   SC_X4_FI_SP0_RSLVD_SPDr_t     reg_resolved_sp0_spd;
   SC_X4_FI_SP0_RSLVD0r_t        reg_resolved_sp0_0;
   SC_X4_FI_SP0_RSLVD1r_t        reg_resolved_sp0_1;
   SC_X4_FI_SP0_RSLVD2r_t        reg_resolved_sp0_2;
   SC_X4_FI_SP0_RSLVD3r_t        reg_resolved_sp0_3;
   SC_X4_FI_SP0_RSLVD4r_t        reg_resolved_sp0_4;
   SC_X4_FI_SP0_RSLVD5r_t        reg_resolved_sp0_5;
   SC_X4_FI_SP0_RSLVD6r_t        reg_resolved_sp0_6;

   SC_X4_FI_SP1_RSLVD_SPDr_t     reg_resolved_sp1_spd;
   SC_X4_FI_SP1_RSLVD0r_t        reg_resolved_sp1_0;
   SC_X4_FI_SP1_RSLVD1r_t        reg_resolved_sp1_1;
   SC_X4_FI_SP1_RSLVD2r_t        reg_resolved_sp1_2;
   SC_X4_FI_SP1_RSLVD3r_t        reg_resolved_sp1_3;
   SC_X4_FI_SP1_RSLVD4r_t        reg_resolved_sp1_4;
   SC_X4_FI_SP1_RSLVD5r_t        reg_resolved_sp1_5;
   SC_X4_FI_SP1_RSLVD6r_t        reg_resolved_sp1_6;

   SC_X4_FI_SP2_RSLVD_SPDr_t     reg_resolved_sp2_spd;
   SC_X4_FI_SP2_RSLVD0r_t        reg_resolved_sp2_0;
   SC_X4_FI_SP2_RSLVD1r_t        reg_resolved_sp2_1;
   SC_X4_FI_SP2_RSLVD2r_t        reg_resolved_sp2_2;
   SC_X4_FI_SP2_RSLVD3r_t        reg_resolved_sp2_3;
   SC_X4_FI_SP2_RSLVD4r_t        reg_resolved_sp2_4;
   SC_X4_FI_SP2_RSLVD5r_t        reg_resolved_sp2_5;
   SC_X4_FI_SP2_RSLVD6r_t        reg_resolved_sp2_6;

   SC_X4_FI_SP3_RSLVD_SPDr_t     reg_resolved_sp3_spd;
   SC_X4_FI_SP3_RSLVD0r_t        reg_resolved_sp3_0;
   SC_X4_FI_SP3_RSLVD1r_t        reg_resolved_sp3_1;
   SC_X4_FI_SP3_RSLVD2r_t        reg_resolved_sp3_2;
   SC_X4_FI_SP3_RSLVD3r_t        reg_resolved_sp3_3;
   SC_X4_FI_SP3_RSLVD4r_t        reg_resolved_sp3_4;
   SC_X4_FI_SP3_RSLVD5r_t        reg_resolved_sp3_5;
   SC_X4_FI_SP3_RSLVD6r_t        reg_resolved_sp3_6;

   if(pc->sub_port == 0){
     SC_X4_FI_SP0_RSLVD_SPDr_CLR(reg_resolved_sp0_spd);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD_SPDr(pc, &reg_resolved_sp0_spd));
     actual_speed = SC_X4_FI_SP0_RSLVD_SPDr_SPEEDf_GET(reg_resolved_sp0_spd);
     sc_final_stats_actual.speed = actual_speed;

     SC_X4_FI_SP0_RSLVD0r_CLR(reg_resolved_sp0_0);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD0r(pc, &reg_resolved_sp0_0));
     sc_final_stats_actual.qsgmii_en = SC_X4_FI_SP0_RSLVD0r_QSGMII_ENf_GET(reg_resolved_sp0_0);
     sc_final_stats_actual.r_desc2_byte_deletion = SC_X4_FI_SP0_RSLVD0r_CL36BYTEDELETEMODEf_GET(reg_resolved_sp0_0);
     sc_final_stats_actual.t_pma_os_mode = SC_X4_FI_SP0_RSLVD0r_OS_MODEf_GET(reg_resolved_sp0_0);

     SC_X4_FI_SP0_RSLVD1r_CLR(reg_resolved_sp0_1);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD1r(pc, &reg_resolved_sp0_1));
     sc_final_stats_actual.clkcnt0 = SC_X4_FI_SP0_RSLVD1r_CLOCKCNT0f_GET(reg_resolved_sp0_1);
     sc_final_stats_actual.sgmii =  SC_X4_FI_SP0_RSLVD1r_SGMII_SPD_SWITCHf_GET(reg_resolved_sp0_1);

     SC_X4_FI_SP0_RSLVD2r_CLR(reg_resolved_sp0_2);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD2r(pc, &reg_resolved_sp0_2));
     sc_final_stats_actual.clkcnt1 = SC_X4_FI_SP0_RSLVD2r_CLOCKCNT1f_GET(reg_resolved_sp0_2);

     SC_X4_FI_SP0_RSLVD3r_CLR(reg_resolved_sp0_3);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD3r(pc, &reg_resolved_sp0_3));
     sc_final_stats_actual.lpcnt1 = SC_X4_FI_SP0_RSLVD3r_LOOPCNT1f_GET(reg_resolved_sp0_3);
     sc_final_stats_actual.lpcnt0 = SC_X4_FI_SP0_RSLVD3r_LOOPCNT0f_GET(reg_resolved_sp0_3);

     SC_X4_FI_SP0_RSLVD4r_CLR(reg_resolved_sp0_4);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD4r(pc, &reg_resolved_sp0_4));
     sc_final_stats_actual.mac_cgc = SC_X4_FI_SP0_RSLVD4r_MAC_CREDITGENCNTf_GET(reg_resolved_sp0_4);

     SC_X4_FI_SP0_RSLVD5r_CLR(reg_resolved_sp0_5);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD5r(pc, &reg_resolved_sp0_5));
     sc_final_stats_actual.pcs_clkcnt = SC_X4_FI_SP0_RSLVD5r_PCS_CLOCKCNT0f_GET(reg_resolved_sp0_5);
     sc_final_stats_actual.pcs_crdten = SC_X4_FI_SP0_RSLVD5r_PCS_CREDITENABLEf_GET(reg_resolved_sp0_5);
     sc_final_stats_actual.pcs_repcnt = SC_X4_FI_SP0_RSLVD5r_REPLICATION_CNTf_GET(reg_resolved_sp0_5);

     SC_X4_FI_SP0_RSLVD6r_CLR(reg_resolved_sp0_6);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD6r(pc, &reg_resolved_sp0_6));
     sc_final_stats_actual.pcs_cgc = SC_X4_FI_SP0_RSLVD6r_PCS_CREDITGENCNTf_GET(reg_resolved_sp0_6);
   }

   if(pc->sub_port == 1){
     SC_X4_FI_SP1_RSLVD_SPDr_CLR(reg_resolved_sp1_spd);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD_SPDr(pc, &reg_resolved_sp1_spd));
     actual_speed = SC_X4_FI_SP1_RSLVD_SPDr_SPEEDf_GET(reg_resolved_sp1_spd);
     sc_final_stats_actual.speed = actual_speed;

     SC_X4_FI_SP1_RSLVD0r_CLR(reg_resolved_sp1_0);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD0r(pc, &reg_resolved_sp1_0));
     sc_final_stats_actual.qsgmii_en = SC_X4_FI_SP1_RSLVD0r_QSGMII_ENf_GET(reg_resolved_sp1_0);
     sc_final_stats_actual.r_desc2_byte_deletion = SC_X4_FI_SP1_RSLVD0r_CL36BYTEDELETEMODEf_GET(reg_resolved_sp1_0);
     sc_final_stats_actual.t_pma_os_mode = SC_X4_FI_SP1_RSLVD0r_OS_MODEf_GET(reg_resolved_sp1_0);

     SC_X4_FI_SP1_RSLVD1r_CLR(reg_resolved_sp1_1);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD1r(pc, &reg_resolved_sp1_1));
     sc_final_stats_actual.clkcnt0 = SC_X4_FI_SP1_RSLVD1r_CLOCKCNT0f_GET(reg_resolved_sp1_1);
     sc_final_stats_actual.sgmii =  SC_X4_FI_SP1_RSLVD1r_SGMII_SPD_SWITCHf_GET(reg_resolved_sp1_1);

     SC_X4_FI_SP1_RSLVD2r_CLR(reg_resolved_sp1_2);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD2r(pc, &reg_resolved_sp1_2));
     sc_final_stats_actual.clkcnt1 = SC_X4_FI_SP1_RSLVD2r_CLOCKCNT1f_GET(reg_resolved_sp1_2);

     SC_X4_FI_SP1_RSLVD3r_CLR(reg_resolved_sp1_3);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD3r(pc, &reg_resolved_sp1_3));
     sc_final_stats_actual.lpcnt1 = SC_X4_FI_SP1_RSLVD3r_LOOPCNT1f_GET(reg_resolved_sp1_3);
     sc_final_stats_actual.lpcnt0 = SC_X4_FI_SP1_RSLVD3r_LOOPCNT0f_GET(reg_resolved_sp1_3);

     SC_X4_FI_SP1_RSLVD4r_CLR(reg_resolved_sp1_4);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD4r(pc, &reg_resolved_sp1_4));
     sc_final_stats_actual.mac_cgc = SC_X4_FI_SP1_RSLVD4r_MAC_CREDITGENCNTf_GET(reg_resolved_sp1_4);

     SC_X4_FI_SP1_RSLVD5r_CLR(reg_resolved_sp1_5);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD5r(pc, &reg_resolved_sp1_5));
     sc_final_stats_actual.pcs_clkcnt = SC_X4_FI_SP1_RSLVD5r_PCS_CLOCKCNT0f_GET(reg_resolved_sp1_5);
     sc_final_stats_actual.pcs_crdten = SC_X4_FI_SP1_RSLVD5r_PCS_CREDITENABLEf_GET(reg_resolved_sp1_5);
     sc_final_stats_actual.pcs_repcnt = SC_X4_FI_SP1_RSLVD5r_REPLICATION_CNTf_GET(reg_resolved_sp1_5);

     SC_X4_FI_SP1_RSLVD6r_CLR(reg_resolved_sp1_6);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD6r(pc, &reg_resolved_sp1_6));
     sc_final_stats_actual.pcs_cgc = SC_X4_FI_SP1_RSLVD6r_PCS_CREDITGENCNTf_GET(reg_resolved_sp1_6);
   }

   if(pc->sub_port == 2){
     SC_X4_FI_SP2_RSLVD_SPDr_CLR(reg_resolved_sp2_spd);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD_SPDr(pc, &reg_resolved_sp2_spd));
     actual_speed = SC_X4_FI_SP2_RSLVD_SPDr_SPEEDf_GET(reg_resolved_sp2_spd);
     sc_final_stats_actual.speed = actual_speed;

     SC_X4_FI_SP2_RSLVD0r_CLR(reg_resolved_sp2_0);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD0r(pc, &reg_resolved_sp2_0));
     sc_final_stats_actual.qsgmii_en = SC_X4_FI_SP2_RSLVD0r_QSGMII_ENf_GET(reg_resolved_sp2_0);
     sc_final_stats_actual.r_desc2_byte_deletion = SC_X4_FI_SP2_RSLVD0r_CL36BYTEDELETEMODEf_GET(reg_resolved_sp2_0);
     sc_final_stats_actual.t_pma_os_mode = SC_X4_FI_SP2_RSLVD0r_OS_MODEf_GET(reg_resolved_sp2_0);

     SC_X4_FI_SP2_RSLVD1r_CLR(reg_resolved_sp2_1);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD1r(pc, &reg_resolved_sp2_1));
     sc_final_stats_actual.clkcnt0 = SC_X4_FI_SP2_RSLVD1r_CLOCKCNT0f_GET(reg_resolved_sp2_1);
     sc_final_stats_actual.sgmii =  SC_X4_FI_SP2_RSLVD1r_SGMII_SPD_SWITCHf_GET(reg_resolved_sp2_1);

     SC_X4_FI_SP2_RSLVD2r_CLR(reg_resolved_sp2_2);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD2r(pc, &reg_resolved_sp2_2));
     sc_final_stats_actual.clkcnt1 = SC_X4_FI_SP2_RSLVD2r_CLOCKCNT1f_GET(reg_resolved_sp2_2);

     SC_X4_FI_SP2_RSLVD3r_CLR(reg_resolved_sp2_3);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD3r(pc, &reg_resolved_sp2_3));
     sc_final_stats_actual.lpcnt1 = SC_X4_FI_SP2_RSLVD3r_LOOPCNT1f_GET(reg_resolved_sp2_3);
     sc_final_stats_actual.lpcnt0 = SC_X4_FI_SP2_RSLVD3r_LOOPCNT0f_GET(reg_resolved_sp2_3);

     SC_X4_FI_SP2_RSLVD4r_CLR(reg_resolved_sp2_4);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD4r(pc, &reg_resolved_sp2_4));
     sc_final_stats_actual.mac_cgc = SC_X4_FI_SP2_RSLVD4r_MAC_CREDITGENCNTf_GET(reg_resolved_sp2_4);

     SC_X4_FI_SP2_RSLVD5r_CLR(reg_resolved_sp2_5);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD5r(pc, &reg_resolved_sp2_5));
     sc_final_stats_actual.pcs_clkcnt = SC_X4_FI_SP2_RSLVD5r_PCS_CLOCKCNT0f_GET(reg_resolved_sp2_5);
     sc_final_stats_actual.pcs_crdten = SC_X4_FI_SP2_RSLVD5r_PCS_CREDITENABLEf_GET(reg_resolved_sp2_5);
     sc_final_stats_actual.pcs_repcnt = SC_X4_FI_SP2_RSLVD5r_REPLICATION_CNTf_GET(reg_resolved_sp2_5);

     SC_X4_FI_SP2_RSLVD6r_CLR(reg_resolved_sp2_6);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD6r(pc, &reg_resolved_sp2_6));
     sc_final_stats_actual.pcs_cgc = SC_X4_FI_SP2_RSLVD6r_PCS_CREDITGENCNTf_GET(reg_resolved_sp2_6);
   }

   if(pc->sub_port == 3){
     SC_X4_FI_SP3_RSLVD_SPDr_CLR(reg_resolved_sp3_spd);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD_SPDr(pc, &reg_resolved_sp3_spd));
     actual_speed = SC_X4_FI_SP3_RSLVD_SPDr_SPEEDf_GET(reg_resolved_sp3_spd);
     sc_final_stats_actual.speed = actual_speed;

     SC_X4_FI_SP3_RSLVD0r_CLR(reg_resolved_sp3_0);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD0r(pc, &reg_resolved_sp3_0));
     sc_final_stats_actual.qsgmii_en = SC_X4_FI_SP3_RSLVD0r_QSGMII_ENf_GET(reg_resolved_sp3_0);
     sc_final_stats_actual.r_desc2_byte_deletion = SC_X4_FI_SP3_RSLVD0r_CL36BYTEDELETEMODEf_GET(reg_resolved_sp3_0);
     sc_final_stats_actual.t_pma_os_mode = SC_X4_FI_SP3_RSLVD0r_OS_MODEf_GET(reg_resolved_sp3_0);

     SC_X4_FI_SP3_RSLVD1r_CLR(reg_resolved_sp3_1);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD1r(pc, &reg_resolved_sp3_1));
     sc_final_stats_actual.clkcnt0 = SC_X4_FI_SP3_RSLVD1r_CLOCKCNT0f_GET(reg_resolved_sp3_1);
     sc_final_stats_actual.sgmii =  SC_X4_FI_SP3_RSLVD1r_SGMII_SPD_SWITCHf_GET(reg_resolved_sp3_1);

     SC_X4_FI_SP3_RSLVD2r_CLR(reg_resolved_sp3_2);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD2r(pc, &reg_resolved_sp3_2));
     sc_final_stats_actual.clkcnt1 = SC_X4_FI_SP3_RSLVD2r_CLOCKCNT1f_GET(reg_resolved_sp3_2);

     SC_X4_FI_SP3_RSLVD3r_CLR(reg_resolved_sp3_3);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD3r(pc, &reg_resolved_sp3_3));
     sc_final_stats_actual.lpcnt1 = SC_X4_FI_SP3_RSLVD3r_LOOPCNT1f_GET(reg_resolved_sp3_3);
     sc_final_stats_actual.lpcnt0 = SC_X4_FI_SP3_RSLVD3r_LOOPCNT0f_GET(reg_resolved_sp3_3);

     SC_X4_FI_SP3_RSLVD4r_CLR(reg_resolved_sp3_4);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD4r(pc, &reg_resolved_sp3_4));
     sc_final_stats_actual.mac_cgc = SC_X4_FI_SP3_RSLVD4r_MAC_CREDITGENCNTf_GET(reg_resolved_sp3_4);

     SC_X4_FI_SP3_RSLVD5r_CLR(reg_resolved_sp3_5);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD5r(pc, &reg_resolved_sp3_5));
     sc_final_stats_actual.pcs_clkcnt = SC_X4_FI_SP3_RSLVD5r_PCS_CLOCKCNT0f_GET(reg_resolved_sp3_5);
     sc_final_stats_actual.pcs_crdten = SC_X4_FI_SP3_RSLVD5r_PCS_CREDITENABLEf_GET(reg_resolved_sp3_5);
     sc_final_stats_actual.pcs_repcnt = SC_X4_FI_SP3_RSLVD5r_REPLICATION_CNTf_GET(reg_resolved_sp3_5);

     SC_X4_FI_SP3_RSLVD6r_CLR(reg_resolved_sp3_6);
     PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD6r(pc, &reg_resolved_sp3_6));
     sc_final_stats_actual.pcs_cgc = SC_X4_FI_SP3_RSLVD6r_PCS_CREDITGENCNTf_GET(reg_resolved_sp3_6);
   }

   /* Compare the actual and expected stats */
   fail = 0;

   if(pc->spd_intf == QMOD_SPD_4000){
     if(pc->subp_speed == 2){
       pc->clkcnt0 = 1000;
       pc->mac_cgc = 999;
       pc->sgmii = 1;
       pc->pcs_repcnt = 1;
       pc->pcs_crdten = 1;
       pc->r_desc2_byte_deletion = 1;
     }
     if(pc->subp_speed == 1){
       pc->clkcnt0 = 100;
       pc->mac_cgc = 99;
       pc->sgmii = 1;
       pc->pcs_repcnt = 0;
       pc->pcs_crdten = 1;
       pc->r_desc2_byte_deletion = 0;
     }
     if(pc->subp_speed == 0){
       pc->clkcnt0 = 10;
       pc->mac_cgc = 9;
       pc->pcs_clkcnt = 0;
       pc->pcs_cgc = 0;
     }
   }

   if(pc->sc_mode == QMOD_SC_MODE_ST) {
     if(actual_speed != (pc->per_lane_control >> 8))
        fail = 1; 
   } else if((pc->per_lane_control & 0xffff) == (0x004 | (QMOD_SPD_10000_XFI << 8))) { /* DV Verification Only */
     if(actual_speed != digital_operationSpeeds_SPEED_10G_KR1)
        fail = 1; 
   } else if((pc->per_lane_control & 0xffff) == (0x004 | (QMOD_SPD_10_SGMII << 8)))  { /* DV Verification Only */
     if(actual_speed != digital_operationSpeeds_SPEED_10M)
        fail = 1; 
   } else if((pc->per_lane_control & 0xffff) == (0x004 | (QMOD_SPD_5000_XFI << 8)))  { /* DV Verification Only */
     if(actual_speed != digital_operationSpeeds_SPEED_5G_KR1)
        fail = 1; 
   } else {
     if(actual_speed != pc->speed)
        fail = 1; 
   }

   if(sc_final_stats_actual.t_pma_os_mode != pc->t_pma_os_mode)
      fail = 1;

   if(sc_final_stats_actual.qsgmii_en != pc->qsgmii_en)
      fail = 1;

   if(sc_final_stats_actual.r_desc2_byte_deletion != pc->r_desc2_byte_deletion)
      fail = 1;

   /*if(sc_final_stats_actual.cl36_en != pc->cl36_en)
      fail = 1;*/

   if(sc_final_stats_actual.sgmii != pc->sgmii)
      fail = 1;
   if(sc_final_stats_actual.clkcnt0 != pc->clkcnt0)
      fail = 1;
   if(sc_final_stats_actual.clkcnt1 != pc->clkcnt1)
      fail = 1;
   if(sc_final_stats_actual.lpcnt0 != pc->lpcnt0)
      fail = 1;
   if(sc_final_stats_actual.lpcnt1 != pc->lpcnt1)
      fail = 1;
   if(sc_final_stats_actual.mac_cgc != pc->mac_cgc)
      fail = 1;
   if(sc_final_stats_actual.pcs_repcnt != pc->pcs_repcnt)
      fail = 1;
   if(sc_final_stats_actual.pcs_crdten != pc->pcs_crdten)
      fail = 1;
   if(sc_final_stats_actual.pcs_clkcnt != pc->pcs_clkcnt)
      fail = 1;
   if(sc_final_stats_actual.pcs_cgc != pc->pcs_cgc)
      fail = 1;

   pc->accData   = fail;
   if(fail == 1) {
      sc_final_stats_actual_tmp = &sc_final_stats_actual;
      qmod_print_qmod_sc_lkup_table(sc_final_stats_actual_tmp);
      qmod_print_qmod_sc_lkup_table(pc);
      return PHYMOD_E_FAIL;
   }
   return PHYMOD_E_NONE;

}
#endif

#ifdef _DV_TB_
/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init for forced speeds

*/
int _qmod_init_pcs_fs(PHYMOD_ST* pc)             
{
  uint16_t data;
  SC_X4_CTLr_t    reg_swmode;

  _qmod_get_sc_speed_configuration(pc);
  _qmod_configure_sc_speed_configuration(pc);
   
  /* Set speed */
  if(pc->sc_mode == QMOD_SC_MODE_ST) 
    data = (pc->per_lane_control & 0xff00) >> 8;
  else if((pc->per_lane_control & 0xffff) == (0x004 | (QMOD_SPD_10_SGMII << 8))) /* DV Verification Only */
    data = digital_operationSpeeds_SPEED_10M;
  else if((pc->per_lane_control & 0xffff) == (0x004 | (QMOD_SPD_5000_XFI << 8))) /* DV Verification Only */
    data = digital_operationSpeeds_SPEED_5G_KR1;
  else
    data = pc->speed;

   SC_X4_CTLr_CLR(reg_swmode);
   SC_X4_CTLr_SW_SPEEDf_SET(reg_swmode, data);
   PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, reg_swmode));

   qmod_trigger_speed_change(pc); 
   _qmod_wait_sc_stats_set(pc);
  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/**
@brief   Checks to ensure SC has programmed the PCS as expected
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details Called to Configure tsc12 for 100G+ speeds

*/

int qmod_check_sc_stats(PHYMOD_ST* pc)   /* CHECK_SC_STATS */
{
  uint16_t data ;

  QMOD_DBG_IN_FUNC_INFO(pc);
  data = _read_and_compare_final_status(pc); 

  return data;
}
#endif /* _DV_TB_ */

#ifdef _DV_TB_
/**
@brief  
@param  
@returns The value PHYMOD_E_NONE upon successful completion.
@details 

*/

int qmod_disable_set(PHYMOD_ST* pc)   /* CHECK_SC_STATS */
{
  SC_X4_CTLr_t reg_sc_ctrl;
  
  QMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_CTLr_CLR(reg_sc_ctrl); 

  /* write 0 to the speed change */
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 0);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc,reg_sc_ctrl));

  return PHYMOD_E_NONE;
}
#endif /* _DV_TB_ */

/**
@brief   Get info on Disable status of the Port
@param  pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Disables the port by writing 0 to the speed config logic in PCS.
This makes the PCS to bring down the PCS blocks and also apply lane datapath
reset to the PMD. There is no control input to this function since it only does
one thing.

*/
int qmod_disable_set(PHYMOD_ST* pc)
{
  SC_X4_CTLr_t reg_sc_ctrl;

  QMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_CTLr_CLR(reg_sc_ctrl);

  /* write 0 to the speed change */
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 0);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc,reg_sc_ctrl));

  return PHYMOD_E_NONE;
}

/**
@brief   Get the plldiv from lookup table
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   spd_intf speed as specified by enum #qmod_spd_intfc_type
@param   plldiv Receives the pll divider value
@param   speed_vec  Receives the speed id.
@returns The value PHYMOD_E_NONE upon successful completion
@details Get the plldiv from lookup table
*/
int qmod_plldiv_lkup_get(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf, uint32_t *plldiv, uint16_t *speed_vec)
{
  int speed_id;

  qmod_get_mapped_speed(spd_intf, &speed_id);
  *plldiv = qmod_sc_pmd_entry[speed_id].pll_mode;
  *speed_vec = speed_id ;

  return PHYMOD_E_NONE;
}


/**
@brief   get the rx lane reset  staus for any lane
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   get the status as *value
@returns The value PHYMOD_E_NONE upon successful completion.

This function enables/disables rx lane (RSTB_LANE) or read back control bit for
that based on per_lane_control being 1 or 0. If per_lane_control is 0xa, only
read back RSTB_LANE.
*/

int qmod_rx_lane_control_get(PHYMOD_ST* pc, int *value)     /* RX_LANE_CONTROL */
{
  return PHYMOD_E_NONE;
}

/**
@brief   rx lane reset and enable of any particular lane
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable to reset the lane.
@returns The value PHYMOD_E_NONE upon successful completion.

This function enables/disables rx lane (RSTB_LANE) or read back control bit for
that based on per_lane_control being 1 or 0. If per_lane_control is 0xa, only
read back RSTB_LANE.

*/
int qmod_rx_lane_control_set(PHYMOD_ST* pc, int enable)         /* RX_LANE_CONTROL */
{
  PMA_CTLr_t reg_pma_ctrl;
  MAIN_LN_SWPr_t reg;
  unsigned int pcs_map;
  int i, logic_lane=0;
  int physical_lane=0;
  phymod_access_t pa_copy;

  QMOD_DBG_IN_FUNC_INFO(pc);
  PMA_CTLr_CLR(reg_pma_ctrl);
  MAIN_LN_SWPr_CLR(reg);

  PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
  PHYMOD_IF_ERR_RETURN
      (READ_MAIN_LN_SWPr(pc, &reg)) ;

  pcs_map = MAIN_LN_SWPr_GET(reg) & 0xff;
  /*If lane is swapped, re-map the lane mask*/
  if (pcs_map != 0xe4) {
      for (i = 0; i < QMOD_NOF_LANES_IN_CORE; i++) {
          if(pc->lane_mask & (1 << i)) {
              logic_lane = i;
              break;
          }
      }
      for (i = 0; i < QMOD_NOF_LANES_IN_CORE; i++) {
          physical_lane = (pcs_map >> (i * 2)) & 0x3;
          if (physical_lane == logic_lane) {
              pa_copy.lane_mask = 1 << i;
              break;
          }
      }
  }
  if (enable) {
    PMA_CTLr_RSTB_LANEf_SET(reg_pma_ctrl, 1);
    PHYMOD_IF_ERR_RETURN (MODIFY_PMA_CTLr(&pa_copy, reg_pma_ctrl));
  } else {
     /* bit set to 0 for disabling RXP */
    PMA_CTLr_RSTB_LANEf_SET( reg_pma_ctrl, 0);
    PHYMOD_IF_ERR_RETURN (MODIFY_PMA_CTLr(&pa_copy, reg_pma_ctrl));
  }
  return PHYMOD_E_NONE;
}

/*!
@brief   tx lane reset and enable of any particular lane
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   *reset returns the tx_reset configured.
@param   *enable returns the tx_enable configured.
@returns The value PHYMOD_E_NONE upon successful completion.
@details
        returns the configured tx lane reset and enable of any particular lane
*/
int qmod_tx_lane_control_get(PHYMOD_ST* pc, int *reset, int *enable) /* TX_LANE_CONTROL_GET */
{
  MISCr_t    reg_misc_ctrl;

  QMOD_DBG_IN_FUNC_INFO(pc);
  MISCr_CLR(reg_misc_ctrl);

  PHYMOD_IF_ERR_RETURN (READ_MISCr(pc, &reg_misc_ctrl));
  *reset =  MISCr_RSTB_TX_LANEf_GET(reg_misc_ctrl);
  *enable = MISCr_ENABLE_TX_LANEf_GET(reg_misc_ctrl);

  return PHYMOD_E_NONE;
}


#ifdef _DV_TB_
int qmod_prbs_rx_enable_set(PHYMOD_ST* pc, int enable)
{
  return PHYMOD_E_NONE;
}
#endif

/*!
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   cntl  input (see details)
@param   value handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PMD Override.
         Can be enhanced to also configure per core PMD Override
  Various bits are enabled as bit encoded by the cntl parameter
  \li 0x8000 ln_dp_h_rstb_oen
  \li 0x4000 tx_disable_oen
  \li 0x2000 rx_dme_en_oen
  \li 0x1000 osr_mode_oen
  \li 0x0800 lane_mode_oen
  \li 0x0400 rx_clk_vld_ovrd
  \li 0x0200 signal_detect_ovrd
  \li 0x0100 rx_lock_ovrd

*/
int qmod_pmd_override_control(PHYMOD_ST* pc, int cntl, int value)           /* PMD_OVERRIDE_CONTROL */
{
  return PHYMOD_E_NONE;
}

/**
@brief   Set subport speed in qsgmii mode 
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details Set subport speed in qsgmii mode 

*/

int qmod_set_qport_spd(PHYMOD_ST* pc, int sub_port, qmod_spd_intfc_type spd_intf)
{
  SC_X4_QSGMII_SPDr_t reg_qsgmii_spd;
  int speed_id = 0;
  
  QMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_QSGMII_SPDr_CLR(reg_qsgmii_spd);

  if ((spd_intf == QMOD_SPD_10_SGMII) || (spd_intf == QMOD_SPD_10_X1_SGMII)) speed_id = 2;
  if ((spd_intf == QMOD_SPD_100_SGMII) || (spd_intf == QMOD_SPD_100_X1_SGMII)) speed_id = 1;
  if ((spd_intf == QMOD_SPD_1000_SGMII) || (spd_intf == QMOD_SPD_1000_X1_SGMII)) speed_id = 0;

  switch(sub_port) {
     case 0: SC_X4_QSGMII_SPDr_SPEED_SP0f_SET(reg_qsgmii_spd, speed_id);  break ;
     case 1: SC_X4_QSGMII_SPDr_SPEED_SP1f_SET(reg_qsgmii_spd, speed_id);  break ;
     case 2: SC_X4_QSGMII_SPDr_SPEED_SP2f_SET(reg_qsgmii_spd, speed_id);  break ;
     case 3: SC_X4_QSGMII_SPDr_SPEED_SP3f_SET(reg_qsgmii_spd, speed_id);  break ;
     default: break ;
  }
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X4_QSGMII_SPDr( pc, reg_qsgmii_spd));


  return PHYMOD_E_NONE;
}

int qmod_get_qport_spd(PHYMOD_ST* pc, int sub_port, qmod_spd_intfc_type *spd_intf)
{
  SC_X4_QSGMII_SPDr_t reg_qsgmii_spd;
  int speed_id = 0;
  
  QMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_QSGMII_SPDr_CLR(reg_qsgmii_spd);
  PHYMOD_IF_ERR_RETURN (READ_SC_X4_QSGMII_SPDr(pc, &reg_qsgmii_spd));

  switch (sub_port) {
     case 0: 
         speed_id = SC_X4_QSGMII_SPDr_SPEED_SP0f_GET(reg_qsgmii_spd);  
         break ;
     case 1: 
         speed_id = SC_X4_QSGMII_SPDr_SPEED_SP1f_GET(reg_qsgmii_spd);
         break ;
     case 2: 
         speed_id = SC_X4_QSGMII_SPDr_SPEED_SP2f_GET(reg_qsgmii_spd);  
         break ;
     case 3: 
         speed_id = SC_X4_QSGMII_SPDr_SPEED_SP3f_GET(reg_qsgmii_spd);  
         break ;
     default: 
         break ;
  }
  
  switch (speed_id) {
  case 2: 
      *spd_intf = QMOD_SPD_10_SGMII;
      break;
  case 1: 
      *spd_intf = QMOD_SPD_100_SGMII;
      break;
  default:
      *spd_intf = QMOD_SPD_1000_SGMII;
      break;
  }
  
  return PHYMOD_E_NONE;
}

/*
@brief   use spare registers to get the state info for shared QSGMII ports.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   qmod_state_type has MOD_PORT_STATE_CONFIGED(1), 
         QMOD_PORT_STATE_TX_SQUELCH(2), QMOD_PORT_STATE_RX_SQUELCH(3).
@param   sub_port and value to choose which bit to set.
@param   all_sub_port_val returns all shared ports information.
@returns The value PHYMOD_E_NONE upon successful completion.

These set/get functions will set and get the state info for shared QSGMII ports
that might affect how each lane will react to BCM APIs.
*/
int qmod_port_state_set(PHYMOD_ST* pc, int qmod_state_type, int sub_port, int val)
{
    SPARE0r_t  reg_spare0 ;
    SPARE1r_t  reg_spare1 ;
    int        all_sub_port_val = 0;

    QMOD_DBG_IN_FUNC_INFO(pc);
    SPARE0r_CLR(reg_spare0);
    SPARE1r_CLR(reg_spare1);
    switch (qmod_state_type) {
    case QMOD_PORT_STATE_CONFIGED:
        PHYMOD_IF_ERR_RETURN(READ_SPARE0r(pc, &reg_spare0));
        all_sub_port_val = SPARE0r_SPARE0f_GET(reg_spare0) & 0xf;
        if (val) {
            all_sub_port_val |= 1<< sub_port;
        } else {
            all_sub_port_val = all_sub_port_val & ~(1 << sub_port);
        }
        SPARE0r_SPARE0f_SET(reg_spare0, all_sub_port_val);
        PHYMOD_IF_ERR_RETURN(WRITE_SPARE0r(pc, reg_spare0));
        break;
    case QMOD_PORT_STATE_TX_SQUELCH:
        PHYMOD_IF_ERR_RETURN(READ_SPARE1r(pc, &reg_spare1));
        all_sub_port_val = SPARE1r_SPARE1f_GET(reg_spare1);
        if (val) {
            all_sub_port_val |= 1<< sub_port;
        } else {
            all_sub_port_val = all_sub_port_val & ~(1 << sub_port);
        }
        SPARE1r_SPARE1f_SET(reg_spare1, all_sub_port_val);
        PHYMOD_IF_ERR_RETURN(WRITE_SPARE1r(pc, reg_spare1));
        break;
    case QMOD_PORT_STATE_RX_SQUELCH:
        PHYMOD_IF_ERR_RETURN(READ_SPARE1r(pc, &reg_spare1));
        all_sub_port_val = SPARE1r_SPARE1f_GET(reg_spare1);
        if (val) {
            all_sub_port_val |= 1<< (sub_port+4);
        } else {
            all_sub_port_val = all_sub_port_val & ~(1 << (sub_port+4));
        }
        SPARE1r_SPARE1f_SET(reg_spare1, all_sub_port_val);
        PHYMOD_IF_ERR_RETURN(WRITE_SPARE1r(pc, reg_spare1));
        break;
    default:
        return PHYMOD_E_PARAM;
        break;
    }
    return PHYMOD_E_NONE;
}

int qmod_port_state_get(PHYMOD_ST* pc, int qmod_state_type, int *all_sub_port_val)
{
    SPARE0r_t  reg_spare0 ;
    SPARE1r_t  reg_spare1 ;

    QMOD_DBG_IN_FUNC_INFO(pc);
    SPARE0r_CLR(reg_spare0);
    SPARE1r_CLR(reg_spare1);
    switch (qmod_state_type) {
    case QMOD_PORT_STATE_CONFIGED:
        PHYMOD_IF_ERR_RETURN(READ_SPARE0r(pc, &reg_spare0));
        *all_sub_port_val = SPARE0r_SPARE0f_GET(reg_spare0) & 0xf;
        break;
    case QMOD_PORT_STATE_TX_SQUELCH:
        PHYMOD_IF_ERR_RETURN(READ_SPARE1r(pc, &reg_spare1));
        *all_sub_port_val = SPARE1r_SPARE1f_GET(reg_spare1) & 0xf;
        break;
    case QMOD_PORT_STATE_RX_SQUELCH:
        PHYMOD_IF_ERR_RETURN(READ_SPARE1r(pc, &reg_spare1));
        *all_sub_port_val = (SPARE1r_SPARE1f_GET(reg_spare1) >> 4) & 0xf;
        break;
    default:
        return PHYMOD_E_PARAM;
        break;
    }
    return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/**
@brief   Set qsgmii_en for tx and rx
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details Set subport speed in qsgmii mode 

*/

int qmod_set_qsgmii_en(PHYMOD_ST* pc)
{
  CL36TX_CTLr_t reg_cl36tx_ctl;
  CL36_RXr_t reg_cl36_rx;
  
  QMOD_DBG_IN_FUNC_INFO(pc);
  CL36TX_CTLr_CLR(reg_cl36tx_ctl);
  CL36_RXr_CLR(reg_cl36_rx);

  CL36TX_CTLr_QSGMII_ENf_SET(reg_cl36tx_ctl, pc->qsgmii_mode);
  CL36_RXr_QSGMII_ENf_SET(reg_cl36_rx, pc->qsgmii_mode);

  PHYMOD_IF_ERR_RETURN (MODIFY_CL36TX_CTLr( pc, reg_cl36tx_ctl));
  PHYMOD_IF_ERR_RETURN (MODIFY_CL36_RXr( pc, reg_cl36_rx));

  return PHYMOD_E_NONE;
}
#endif

int qmod_speedchange_get (PHYMOD_ST *pc, uint32_t* enable)
{
  SC_X4_CTLr_t reg_sc_ctrl;

  QMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_CTLr_CLR(reg_sc_ctrl);

  PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc,&reg_sc_ctrl));
  *enable = SC_X4_CTLr_SW_SPEED_CHANGEf_GET(reg_sc_ctrl);

  return PHYMOD_E_NONE;
} 

int qmod_get_plldiv (PHYMOD_ST *pc,  uint32_t *pll_div)
{
  PLL_CAL_CTL7r_t    reg_ctl7;
  READ_PLL_CAL_CTL7r(pc, &reg_ctl7);
  *pll_div = PLL_CAL_CTL7r_PLL_MODEf_GET(reg_ctl7);

  return PHYMOD_E_NONE;
}
int qmod_master_port_num_set ( PHYMOD_ST *pc,  int port_num) 
{
  MAIN_SETUPr_t main_reg;

  MAIN_SETUPr_CLR(main_reg);
  MAIN_SETUPr_MASTER_PORT_NUMf_SET(main_reg, port_num);
  MODIFY_MAIN_SETUPr(pc, main_reg);  

  return PHYMOD_E_NONE;
}

int qmod_pcs_lane_swap_get (PHYMOD_ST *pc,  uint32_t *tx_rx_swap) 
{
  unsigned int pcs_map;
  MAIN_LN_SWPr_t reg;
  QMOD_DBG_IN_FUNC_INFO(pc);

  MAIN_LN_SWPr_CLR(reg);

  PHYMOD_IF_ERR_RETURN
      (READ_MAIN_LN_SWPr(pc, &reg)) ;

  pcs_map = MAIN_LN_SWPr_GET(reg);

  *tx_rx_swap = (((pcs_map >> 0)  & 0x3) << 0) |
                (((pcs_map >> 2)  & 0x3) << 4) |
                (((pcs_map >> 4)  & 0x3) << 8) |
                (((pcs_map >> 6)  & 0x3) << 12) ;

  return PHYMOD_E_NONE ;
}

int qmod_pll_reset_enable_set ( PHYMOD_ST *pc , int enable) 
{
  MAIN_SETUPr_t main_reg;

  MAIN_SETUPr_CLR(main_reg);
  MAIN_SETUPr_PLL_RESET_ENf_SET(main_reg, enable);
  MODIFY_MAIN_SETUPr(pc, main_reg);
  return PHYMOD_E_NONE ; 
}

int qmod_pmd_lane_swap_tx_get ( PHYMOD_ST *pc, uint32_t *tx_lane_map) 
{
  uint16_t tx_lane_map_0, tx_lane_map_1, tx_lane_map_2, tx_lane_map_3;
  DIG_TX_LN_MAP_0_1_2r_t              reg;
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_t    reg1;

  QMOD_DBG_IN_FUNC_INFO(pc);
  DIG_TX_LN_MAP_0_1_2r_CLR(reg);
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_CLR(reg1);

  PHYMOD_IF_ERR_RETURN
     (READ_DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r(pc, &reg1));
  PHYMOD_IF_ERR_RETURN
     (READ_DIG_TX_LN_MAP_0_1_2r(pc, &reg));


  tx_lane_map_0 = DIG_TX_LN_MAP_0_1_2r_TX_LANE_MAP_0f_GET(reg);
  tx_lane_map_1 = DIG_TX_LN_MAP_0_1_2r_TX_LANE_MAP_1f_GET(reg);
  tx_lane_map_2 = DIG_TX_LN_MAP_0_1_2r_TX_LANE_MAP_2f_GET(reg);
  tx_lane_map_3 = DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_TX_LANE_MAP_3f_GET(reg1);

  *tx_lane_map = ((tx_lane_map_0 & 0xf) << 0) 
              | ((tx_lane_map_1 & 0xf) << 4) 
              | ((tx_lane_map_2 & 0xf) << 8)
              | ((tx_lane_map_3 & 0xf) << 12);

  return PHYMOD_E_NONE ;
}

/*!
@brief   get port speed id configured
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   speed_id Receives the the resolved speed cfg as recorded in h/w
@returns The value PHYMOD_E_NONE upon successful completion.
@details get port speed configured
*/
int qmod_speed_id_get ( PHYMOD_ST *pc,  int *speed_id, int subport) 
{
  int start_lane, num_lane;
  SC_X4_FI_SP0_RSLVD_SPDr_t sc_final_sp0_resolved_speed;
  SC_X4_FI_SP0_RSLVD0r_t      sc_final_sp0_resolved_0;
  SC_X4_FI_SP1_RSLVD0r_t      sc_final_sp1_resolved_0;
  SC_X4_FI_SP2_RSLVD0r_t      sc_final_sp2_resolved_0;
  SC_X4_FI_SP3_RSLVD0r_t      sc_final_sp3_resolved_0;


  int sp_spd_id;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

  QMOD_DBG_IN_FUNC_INFO(pc);

  SC_X4_FI_SP0_RSLVD_SPDr_CLR(sc_final_sp0_resolved_speed);
 
  SC_X4_FI_SP0_RSLVD0r_CLR(sc_final_sp0_resolved_0);
  SC_X4_FI_SP1_RSLVD0r_CLR(sc_final_sp1_resolved_0);
  SC_X4_FI_SP2_RSLVD0r_CLR(sc_final_sp2_resolved_0);
  SC_X4_FI_SP3_RSLVD0r_CLR(sc_final_sp3_resolved_0);


  /* next check if we are in qsgmii mode */
 if (!PHYMOD_ACC_F_QMODE_GET(pc)) {
    /* switch (start_lane) {
      case 1: READ_SC_X4_FI_SP1_RSLVD_SPDr(pc,&sc_final_sp1_resolved_speed);
         sp_spd_id = SC_X4_FI_SP1_RSLVD_SPDr_SPEEDf_GET(sc_final_sp1_resolved_speed);
         break ;
      case 2: READ_SC_X4_FI_SP2_RSLVD_SPDr(pc,&sc_final_sp2_resolved_speed);
         sp_spd_id = SC_X4_FI_SP2_RSLVD_SPDr_SPEEDf_GET(sc_final_sp2_resolved_speed);
         break ;
      case 3: READ_SC_X4_FI_SP3_RSLVD_SPDr(pc,&sc_final_sp3_resolved_speed);
         sp_spd_id = SC_X4_FI_SP3_RSLVD_SPDr_SPEEDf_GET(sc_final_sp3_resolved_speed);
         break ;
      default: */
     READ_SC_X4_FI_SP0_RSLVD_SPDr(pc,&sc_final_sp0_resolved_speed);
     sp_spd_id = SC_X4_FI_SP0_RSLVD_SPDr_SPEEDf_GET(sc_final_sp0_resolved_speed);
         /* break ; */
  } else {
    switch (subport) {
      case 1: READ_SC_X4_FI_SP1_RSLVD0r(pc,&sc_final_sp1_resolved_0);
         sp_spd_id = SC_X4_FI_SP1_RSLVD0r_CL36BYTEDELETEMODEf_GET(sc_final_sp1_resolved_0);
         break ;
      case 2: READ_SC_X4_FI_SP2_RSLVD0r(pc,&sc_final_sp2_resolved_0);
         sp_spd_id = SC_X4_FI_SP2_RSLVD0r_CL36BYTEDELETEMODEf_GET(sc_final_sp2_resolved_0);
         break ;
      case 3: READ_SC_X4_FI_SP3_RSLVD0r(pc,&sc_final_sp3_resolved_0);
         sp_spd_id = SC_X4_FI_SP3_RSLVD0r_CL36BYTEDELETEMODEf_GET(sc_final_sp3_resolved_0);
         break ;
      default:
         READ_SC_X4_FI_SP0_RSLVD0r(pc,&sc_final_sp0_resolved_0);
         sp_spd_id = SC_X4_FI_SP0_RSLVD0r_CL36BYTEDELETEMODEf_GET(sc_final_sp0_resolved_0);
         break ;
    }
    switch (sp_spd_id) {
        case 0:
            sp_spd_id = 0x2;
            break;
        case 1:
            sp_spd_id = 0x1;
            break;
        default:
            sp_spd_id = 0x3;
            break;
    }
 
  }

  *speed_id = sp_spd_id ;

  return PHYMOD_E_NONE;
}

/**
@brief   Gets the TX And RX Polarity 
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   tx_polarity Receive Polarity for TX side
@param   rx_polarity Receive Polarity for RX side
@returns The value PHYMOD_E_NONE upon successful completion.
@details Gets the TX And RX Polarity
*/
int qmod_tx_rx_polarity_get ( PHYMOD_ST *pc, uint32_t* tx_polarity, uint32_t* rx_polarity) 
{
  TLB_TX_TLB_TX_MISC_CFGr_t tx_pol_inv;
  TLB_RX_TLB_RX_MISC_CFGr_t rx_pol_inv;

  PHYMOD_IF_ERR_RETURN(READ_TLB_TX_TLB_TX_MISC_CFGr(pc, &tx_pol_inv));
  *tx_polarity = TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_GET(tx_pol_inv);

  PHYMOD_IF_ERR_RETURN(READ_TLB_RX_TLB_RX_MISC_CFGr(pc, &rx_pol_inv));
  *rx_polarity = TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_GET(rx_pol_inv);
 
  return PHYMOD_E_NONE;
}

/**
@brief   Sets the TX And RX Polarity 
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   tx_polarity Control Polarity for TX side
@param   rx_polarity Control Polarity for RX side
@returns The value PHYMOD_E_NONE upon successful completion.
@details Sets the TX And RX Polarity
*/
int qmod_tx_rx_polarity_set ( PHYMOD_ST *pc, uint32_t tx_polarity, uint32_t rx_polarity) 
{
  TLB_TX_TLB_TX_MISC_CFGr_t tx_pol_inv;
  TLB_RX_TLB_RX_MISC_CFGr_t rx_pol_inv;
  
  TLB_TX_TLB_TX_MISC_CFGr_CLR(tx_pol_inv);
  TLB_RX_TLB_RX_MISC_CFGr_CLR(rx_pol_inv);

  TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_SET(tx_pol_inv, tx_polarity);
  PHYMOD_IF_ERR_RETURN(MODIFY_TLB_TX_TLB_TX_MISC_CFGr(pc, tx_pol_inv));

  TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_SET(rx_pol_inv, rx_polarity);
  PHYMOD_IF_ERR_RETURN(MODIFY_TLB_RX_TLB_RX_MISC_CFGr(pc, rx_pol_inv));

  return PHYMOD_E_NONE;
}

/**
@brief   Set the port speed and enable the port
@param   pc handle to current QTCE context (#PHYMOD_ST)
@param   spd_intf the speed to set the port enum #qmod_set_spd_intf
@returns The value PHYMOD_E_NONE upon successful completion
@details Sets the port to the specified speed and triggers the port, and waits 
         for the port to be configured
*/
int qmod_set_spd_intf(PHYMOD_ST *pc, qmod_spd_intfc_type spd_intf, int no_trig)
{
  SC_X4_CTLr_t xgxs_x4_ctrl;
  phymod_access_t pa_copy;
  int speed_id = 0, start_lane = 0, num_lane = 0;

  QMOD_DBG_IN_FUNC_INFO(pc);

  /* need to figure out what's the starting lane */
  PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
  PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
  pa_copy.lane_mask = 0x1 << start_lane; 

  PHYMOD_IF_ERR_RETURN (qmod_get_mapped_speed(spd_intf, &speed_id));
  /* write the speed_id into the speed_change register */
  SC_X4_CTLr_CLR(xgxs_x4_ctrl);
  SC_X4_CTLr_SW_SPEEDf_SET(xgxs_x4_ctrl, speed_id);
  MODIFY_SC_X4_CTLr(pc, xgxs_x4_ctrl);
  /*next call the speed_change routine */
  if(no_trig) {

  } else {
  PHYMOD_IF_ERR_RETURN (qmod_trigger_speed_change(&pa_copy));
  /* removed check for the speed_change_done init*/
  }
  return PHYMOD_E_NONE;
}


int qmod_pll_sequencer_control(PHYMOD_ST *pa, int enable)
{
  return PHYMOD_E_NONE;
}

/* #ifdef _SDK_QMOD_  */
int qmod_lane_info(PHYMOD_ST *pa, int *lane, int *sub_port)
{
    int i = 0 ; uint32_t mask ;
    mask = pa->lane_mask ;
    *lane = 0 ; *sub_port=0 ;
    if(mask==0) return PHYMOD_E_INTERNAL ;
    while(1) {
        if(mask & 0xf) {
            switch(mask) {
            case 0x1: *sub_port=0 ; break ;
            case 0x2: *sub_port=1 ; break ;
            case 0x4: *sub_port=2 ; break ;
            case 0x8: *sub_port=3 ; break ;
            case 0xf: *sub_port=0 ; break ;
            default: 
                return PHYMOD_E_INTERNAL ; break;
            }
        }
        mask = mask >> 4;
        if(mask) {
            i++ ;
        } else {
            break ;
        }
    }
    *lane = i ;
    if(!PHYMOD_ACC_F_QMODE_GET(pa)) {  /* no qmode */
        *lane = *sub_port; *sub_port=0;
    }
    return PHYMOD_E_NONE ;
}
/* #endif  */

/**
@brief   EEE Control Set
@param   pc handle to current QTCE context (#PHYMOD_ST)
@param   enable enable or disable EEE control
@returns The value PHYMOD_E_NONE upon successful completion
@details EEE Control
         enable =1 : Allow LPI pass through. i.e. dont convert LPI.
         enable =0 : Convert LPI to idle. So  MAC will not see it.

*/
int qmod_eee_control_set(PHYMOD_ST* pc, uint32_t enable)
{
  /* PCS_CTLr_t is for writing the RX reg; CL36TX_CTLr_t is for writing the TX reg */
  PCS_CTLr_t    reg_pcs_or_val; 
  CL36TX_CTLr_t reg_cl36_or_val;

  /* following variables are to find out the lane ID and the subport ID of the current logical port */
  phymod_access_t pa_copy;
  int lane_id=0, sub_port=0;
 
  QMOD_DBG_IN_FUNC_INFO(pc);

  /* figure out what's the starting lane */
  PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
  PHYMOD_IF_ERR_RETURN
  	  (qmod_lane_info(pc, &lane_id, &sub_port));

  pa_copy.lane_mask = 0x1 << lane_id;

  PCS_CTLr_CLR(reg_pcs_or_val);
  CL36TX_CTLr_CLR(reg_cl36_or_val);

  PCS_CTLr_LPI_ENABLEf_SET(reg_pcs_or_val, enable & 0x1);
  PHYMOD_IF_ERR_RETURN (MODIFY_PCS_CTLr(&pa_copy, reg_pcs_or_val));

  CL36TX_CTLr_CL36TX_LPI_ENf_SET(reg_cl36_or_val, enable & 0x1);
  PHYMOD_IF_ERR_RETURN (MODIFY_CL36TX_CTLr(&pa_copy,reg_cl36_or_val));

  return PHYMOD_E_NONE;
}

/**
@brief   EEE Control Get
@param   pc handle to current QTCE context (#PHYMOD_ST)
@param   enable handle to pass back EEE control
@returns The value PHYMOD_E_NONE upon successful completion
@details EEE Control
         enable =1 : Allow LPI pass through. i.e. dont convert LPI.
         enable =0 : Convert LPI to idle. So  MAC will not see it.

*/
int qmod_eee_control_get(PHYMOD_ST* pc, uint32_t *enable) 
{
  /* PCS_CTLr_t is for reading the RX reg */
  PCS_CTLr_t reg_pcs_or_val;

  /* following variables are to find out the lane ID and the subport ID of the current logical port */
  phymod_access_t pa_copy;
  int lane_id=0, sub_port=0; 

  QMOD_DBG_IN_FUNC_INFO(pc);

  /* figure out what's the starting lane */
  PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
  PHYMOD_IF_ERR_RETURN
	  (qmod_lane_info(pc, &lane_id, &sub_port));

  pa_copy.lane_mask = 0x1 << lane_id;

  PCS_CTLr_CLR(reg_pcs_or_val);
  PHYMOD_IF_ERR_RETURN (READ_PCS_CTLr(&pa_copy, &reg_pcs_or_val));
  *enable = PCS_CTLr_LPI_ENABLEf_GET(reg_pcs_or_val);

  return PHYMOD_E_NONE;
}
int qmod_pmd_lane_override(PHYMOD_ST* pc, qmod_pmd_lane_override_t ovrd_type)
{
    BCMI_QTC_XGXS_PMD_X4_OVRRr_t reg_pmd_ovr;

    PMD_X4_OVRRr_CLR(reg_pmd_ovr);

    switch (ovrd_type) {
        case QMOD_PMD_LANE_SIGNAL_DETECT_OVRD:
            PMD_X4_OVRRr_SIGNAL_DETECT_OVRDf_SET(reg_pmd_ovr, 1);
            break;
        case QMOD_PMD_LANE_RX_LOCK_OVRD:
            PMD_X4_OVRRr_RX_LOCK_OVRDf_SET(reg_pmd_ovr, 1);
            break;
        default:
            return PHYMOD_E_NONE ;
            break;
    }
    PHYMOD_IF_ERR_RETURN
      (MODIFY_PMD_X4_OVRRr(pc, reg_pmd_ovr));

    return PHYMOD_E_NONE ;
}

STATIC
int _qmod_rx_phy_lane_get(PHYMOD_ST *pc, int rx_logical_lane, int* rx_phy_lane)
{
    MAIN_LN_SWPr_t reg_lane_swap;
    phymod_access_t pa_copy;

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));

    /*always use lane 0 copy since register is single copy */
    pa_copy.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (READ_MAIN_LN_SWPr(&pa_copy, &reg_lane_swap));

    switch (rx_logical_lane) {
        case 0:
            *rx_phy_lane = MAIN_LN_SWPr_LOG0_TO_PHY_LNSWAP_SELf_GET(reg_lane_swap);
            break;
        case 1:
            *rx_phy_lane = MAIN_LN_SWPr_LOG1_TO_PHY_LNSWAP_SELf_GET(reg_lane_swap);
            break;
        case 2:
            *rx_phy_lane = MAIN_LN_SWPr_LOG2_TO_PHY_LNSWAP_SELf_GET(reg_lane_swap);
            break;
        case 3:
            *rx_phy_lane = MAIN_LN_SWPr_LOG3_TO_PHY_LNSWAP_SELf_GET(reg_lane_swap);
            break;
        default:
           return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int qmod_synce_mode_set(PHYMOD_ST *pc, int mode0, int mode1)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN_SYNCE_CTLr_t reg_main_synce_ctl;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_qmod_rx_phy_lane_get(pc, start_lane, &phy_lane));

    MAIN_SYNCE_CTLr_CLR(reg_main_synce_ctl);
    PHYMOD_IF_ERR_RETURN(READ_MAIN_SYNCE_CTLr(pc, &reg_main_synce_ctl));

    switch(phy_lane) {
        case 0:
            MAIN_SYNCE_CTLr_SYNCE_MODE0_PHY_LN0f_SET(reg_main_synce_ctl, mode0);
            MAIN_SYNCE_CTLr_SYNCE_MODE1_PHY_LN0f_SET(reg_main_synce_ctl, mode1);
            break;
        case 1:
            MAIN_SYNCE_CTLr_SYNCE_MODE0_PHY_LN1f_SET(reg_main_synce_ctl, mode0);
            MAIN_SYNCE_CTLr_SYNCE_MODE1_PHY_LN1f_SET(reg_main_synce_ctl, mode1);
            break;
        case 2:
            MAIN_SYNCE_CTLr_SYNCE_MODE0_PHY_LN2f_SET(reg_main_synce_ctl, mode0);
            MAIN_SYNCE_CTLr_SYNCE_MODE1_PHY_LN2f_SET(reg_main_synce_ctl, mode1);
            break;
        case 3:
            MAIN_SYNCE_CTLr_SYNCE_MODE0_PHY_LN3f_SET(reg_main_synce_ctl, mode0);
            MAIN_SYNCE_CTLr_SYNCE_MODE1_PHY_LN3f_SET(reg_main_synce_ctl, mode1);
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN_SYNCE_CTLr(pc, reg_main_synce_ctl));

    return PHYMOD_E_NONE;
}

int qmod_synce_clk_ctrl_set(PHYMOD_ST *pc, uint32_t val)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN_SYNCE_DIVISOR0_PHY_LN0r_t reg_synce_divisor_lane0;
    MAIN_SYNCE_DIVISOR1_PHY_LN1r_t reg_synce_divisor_lane1;
    MAIN_SYNCE_DIVISOR2_PHY_LN2r_t reg_synce_divisor_lane2;
    MAIN_SYNCE_DIVISOR3_PHY_LN3r_t reg_synce_divisor_lane3;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_qmod_rx_phy_lane_get(pc, start_lane, &phy_lane));

    switch(phy_lane) {
        case 0:
            MAIN_SYNCE_DIVISOR0_PHY_LN0r_CLR(reg_synce_divisor_lane0);
            MAIN_SYNCE_DIVISOR0_PHY_LN0r_SET(reg_synce_divisor_lane0, val);
            PHYMOD_IF_ERR_RETURN(WRITE_MAIN_SYNCE_DIVISOR0_PHY_LN0r(pc, reg_synce_divisor_lane0));
            break;
        case 1:
            MAIN_SYNCE_DIVISOR1_PHY_LN1r_CLR(reg_synce_divisor_lane1);
            MAIN_SYNCE_DIVISOR1_PHY_LN1r_SET(reg_synce_divisor_lane1, val);
            PHYMOD_IF_ERR_RETURN(WRITE_MAIN_SYNCE_DIVISOR1_PHY_LN1r(pc, reg_synce_divisor_lane1));
            break;
        case 2:
            MAIN_SYNCE_DIVISOR2_PHY_LN2r_CLR(reg_synce_divisor_lane2);
            MAIN_SYNCE_DIVISOR2_PHY_LN2r_SET(reg_synce_divisor_lane2, val);
            PHYMOD_IF_ERR_RETURN(WRITE_MAIN_SYNCE_DIVISOR2_PHY_LN2r(pc, reg_synce_divisor_lane2));
            break;
        case 3:
            MAIN_SYNCE_DIVISOR3_PHY_LN3r_CLR(reg_synce_divisor_lane3);
            MAIN_SYNCE_DIVISOR3_PHY_LN3r_SET(reg_synce_divisor_lane3, val);
            PHYMOD_IF_ERR_RETURN(WRITE_MAIN_SYNCE_DIVISOR3_PHY_LN3r(pc, reg_synce_divisor_lane3));
            break;
        default:
           return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int qmod_synce_mode_get(PHYMOD_ST *pc, int *mode0, int *mode1)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN_SYNCE_CTLr_t reg_main_synce_ctl;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_qmod_rx_phy_lane_get(pc, start_lane, &phy_lane));

    MAIN_SYNCE_CTLr_CLR(reg_main_synce_ctl);
    PHYMOD_IF_ERR_RETURN(READ_MAIN_SYNCE_CTLr(pc, &reg_main_synce_ctl));

    switch(phy_lane) {
         case 0:
            *mode0 = MAIN_SYNCE_CTLr_SYNCE_MODE0_PHY_LN0f_GET(reg_main_synce_ctl);
            *mode1 = MAIN_SYNCE_CTLr_SYNCE_MODE1_PHY_LN0f_GET(reg_main_synce_ctl);
            break;
         case 1:
            *mode0 = MAIN_SYNCE_CTLr_SYNCE_MODE0_PHY_LN1f_GET(reg_main_synce_ctl);
            *mode1 = MAIN_SYNCE_CTLr_SYNCE_MODE1_PHY_LN1f_GET(reg_main_synce_ctl);
            break;
         case 2:
            *mode0 = MAIN_SYNCE_CTLr_SYNCE_MODE0_PHY_LN2f_GET(reg_main_synce_ctl);
            *mode1 = MAIN_SYNCE_CTLr_SYNCE_MODE1_PHY_LN2f_GET(reg_main_synce_ctl);
            break;
         case 3:
            *mode0 = MAIN_SYNCE_CTLr_SYNCE_MODE0_PHY_LN3f_GET(reg_main_synce_ctl);
            *mode1 = MAIN_SYNCE_CTLr_SYNCE_MODE1_PHY_LN3f_GET(reg_main_synce_ctl);
            break;
         default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int qmod_synce_clk_ctrl_get(PHYMOD_ST *pc, uint32_t *val)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN_SYNCE_DIVISOR0_PHY_LN0r_t reg_synce_divisor_lane0;
    MAIN_SYNCE_DIVISOR1_PHY_LN1r_t reg_synce_divisor_lane1;
    MAIN_SYNCE_DIVISOR2_PHY_LN2r_t reg_synce_divisor_lane2;
    MAIN_SYNCE_DIVISOR3_PHY_LN3r_t reg_synce_divisor_lane3;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_qmod_rx_phy_lane_get(pc, start_lane, &phy_lane));

    switch(phy_lane) {
        case 0:
            PHYMOD_IF_ERR_RETURN(READ_MAIN_SYNCE_DIVISOR0_PHY_LN0r(pc, &reg_synce_divisor_lane0));
            *val = MAIN_SYNCE_DIVISOR0_PHY_LN0r_GET(reg_synce_divisor_lane0);
            break;
        case 1:
            PHYMOD_IF_ERR_RETURN(READ_MAIN_SYNCE_DIVISOR1_PHY_LN1r(pc, &reg_synce_divisor_lane1));
            *val = MAIN_SYNCE_DIVISOR1_PHY_LN1r_GET(reg_synce_divisor_lane1);
            break;
        case 2:
            PHYMOD_IF_ERR_RETURN(READ_MAIN_SYNCE_DIVISOR2_PHY_LN2r(pc, &reg_synce_divisor_lane2));
            *val = MAIN_SYNCE_DIVISOR2_PHY_LN2r_GET(reg_synce_divisor_lane2);
            break;
        case 3:
            PHYMOD_IF_ERR_RETURN(READ_MAIN_SYNCE_DIVISOR3_PHY_LN3r(pc, &reg_synce_divisor_lane3));
            *val = MAIN_SYNCE_DIVISOR3_PHY_LN3r_GET(reg_synce_divisor_lane3);
            break;
        default:
           return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}
