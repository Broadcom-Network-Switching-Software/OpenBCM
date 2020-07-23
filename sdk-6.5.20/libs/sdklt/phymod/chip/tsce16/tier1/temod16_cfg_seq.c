/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : temod16_cfg_seq.c
 * Description: c functions implementing Tier1s for TEMod Serdes Driver
 *---------------------------------------------------------------------*/

#ifndef _DV_TB_
 #define _SDK_TEMOD16_ 1 
#endif

#ifdef _DV_TB_
#include <stdint.h>
#include "temod16_main.h"
#include "temod16_defines.h"
#include "te16PCSRegEnums.h"
#include "te16PMDRegEnums.h"
#include "phy_tsc_iblk.h"
#include "bcmi_tsce_xgxs_defs.h"
#endif /* _DV_TB_ */


#ifdef _SDK_TEMOD16_
#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_tsce16_xgxs_defs.h>
#include "temod16_enum_defines.h"
#include "temod16.h"
#include "temod16_sc_lkup_table.h"
#include "te16PCSRegEnums.h"
#include "../../merlin16/tier1/merlin16_internal.h"
#include "../../merlin16/tier1/merlin16_interface.h"

#endif /* _SDK_TEMOD16_ */

#ifndef PHYMOD_ST  
#ifdef _SDK_TEMOD16_
  #define PHYMOD_ST  const phymod_access_t
#else
  #define PHYMOD_ST  temod16_st
#endif /* _SDK_TEMOD16_ */
#endif /* PHYMOD_ST */


#ifdef _SDK_TEMOD16_
  #define TEMOD16_DBG_FUNC       (1L << 0)
  #define TMOD_DBG_IN_FUNC_INFO(pc) \
    PHYMOD_VDBG(TEMOD16_DBG_FUNC,pc,("%-22s: Adr:%08x Ln:%02d\n", __func__, (unsigned int)pc->addr, (int)pc->lane_mask))
#endif

#ifdef _DV_TB_
  #define TMOD_DBG_IN_FUNC_INFO(pc) \
    PHYMOD_VDBG(TEMOD16_DBG_FUNC, pc, \
      ("TEMOD16 IN Function : %s Port Add : %d Lane No: %d\n", \
      __func__, pc->prt_ad, pc->this_lane))
  #define TEFMOD_DBG_OUT_FUNC_INFO(pc) \
    PHYMOD_VDBG(TEMOD16_DBG_FUNC, pc, \
      ("TEMOD16 OUT of Function : %s Port Add : %d Lane No: %d\n", \
      __func__, pc->prt_ad, pc->this_lane))
int phymod_debug_check(uint32_t flags, PHYMOD_ST *pc);
#endif

#ifdef _SDK_TEMOD16_
static merlin16_sc_pmd_entry_st merlin16_sc_pmd_entry[] = {

/*SPEED_10M :                     0*/  { 1, TEMOD16_PMA_OS_MODE_10,    TEMOD16_PLL_MODE_DIV_80, 0, 0, 1},
/*SPEED_10M :                     1*/  { 1, TEMOD16_PMA_OS_MODE_10,    TEMOD16_PLL_MODE_DIV_80, 0, 0, 1},
/*SPEED_100M :                    2*/  { 1, TEMOD16_PMA_OS_MODE_10,    TEMOD16_PLL_MODE_DIV_80, 0, 0, 1},
/*SPEED_1000M :                   3*/  { 1, TEMOD16_PMA_OS_MODE_10,    TEMOD16_PLL_MODE_DIV_80, 0, 0, 1},
/*SPEED_1G_CX1 :TBD               4*/  { 1, TEMOD16_PMA_OS_MODE_10,    TEMOD16_PLL_MODE_DIV_80, 0, 0, 1},
/*SPEED_1G_KX1 :                  5*/  { 1, TEMOD16_PMA_OS_MODE_8_25,  TEMOD16_PLL_MODE_DIV_66, 0, 0, 1},
/*SPEED_2p5G :                    6*/  { 1, TEMOD16_PMA_OS_MODE_4,     TEMOD16_PLL_MODE_DIV_80, 0, 0, 1},
/*SPEED_5G_X1 :                   7*/  { 1, TEMOD16_PMA_OS_MODE_2,     TEMOD16_PLL_MODE_DIV_80, 1, 0, 0},
/*SPEED_10G_CX4 :                 8*/  { 4, TEMOD16_PMA_OS_MODE_4,     TEMOD16_PLL_MODE_DIV_80, 0, 0, 0},
/*SPEED_10G_KX4:                  9*/  { 4, TEMOD16_PMA_OS_MODE_4,     TEMOD16_PLL_MODE_DIV_80, 0, 0, 0},
/*SPEED_10G_X4: TBD          10/0xa*/  { 4, TEMOD16_PMA_OS_MODE_4,     TEMOD16_PLL_MODE_DIV_80, 0, 0, 1},
/*SPEED_13G_X4 :NA           11/0xb*/  { 4, TEMOD16_PMA_OS_MODE_2,     TEMOD16_PLL_MODE_DIV_52, 0, 0, 1},
/*SPEED_15G_X4 :             12/0xc*/  { 4, TEMOD16_PMA_OS_MODE_2,     TEMOD16_PLL_MODE_DIV_60, 0, 0, 1},
/*SPEED_16G_X4 :             13/0xd*/  { 4, TEMOD16_PMA_OS_MODE_2,     TEMOD16_PLL_MODE_DIV_64, 0, 0, 1},
/*SPEED_20G_CX4:             14/0xe*/  { 4, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_80, 0, 0, 1},
/*SPEED_10G_CX2 : TBD        15/0xf*/  { 2, TEMOD16_PMA_OS_MODE_2,     TEMOD16_PLL_MODE_DIV_80, 0, 0, 1},
/*SPEED_10G_X2 :            16/0x10*/  { 2, TEMOD16_PMA_OS_MODE_2,     TEMOD16_PLL_MODE_DIV_80, 1, 0, 1},
/*SPEED_20G_X4:  TBD        17/0x11*/  { 4, TEMOD16_PMA_OS_MODE_2,     TEMOD16_PLL_MODE_DIV_80, 1, 0, 1},
/*SPEED_10p5G_X2:NA         18/0x12*/  { 2, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_42, 1, 0, 0},
/*SPEED_21G_X4:NA           19/0x13*/  { 4, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_42, 1, 0, 0},
/*SPEED_12p7G_X2 :NA        20/0x14*/  { 2, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_42, 1, 0, 0},
/*SPEED_25p45G_X4:NA        21/0x15*/  { 4, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_42, 1, 0, 0},
/*SPEED_15p75G_X2:NA        22/0x16*/  { 2, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_52, 1, 0, 0},
/*SPEED_31p5G_X4:NA         23/0x17*/  { 4, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_52, 1, 0, 0},
/*SPEED_31p5G_KR4:NA        24/0x18*/  { 4, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_52, 1, 0, 0},
/*SPEED_20G_CX2:            25/0x19*/  { 2, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_66, 1, 0, 0},
/*SPEED_20G_X2:             26/0x1a*/  { 2, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_66, 1, 0, 0},
/*SPEED_40G_X4:             27/0x1b*/  { 2, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_66, 1, 0, 0},
/*SPEED_10G_KR1:            28/0x1c*/  { 0, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_66, 1, 0, 0},
/*SPEED_10p6_X1:            29/0x1d*/  { 0, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_70, 1, 0, 0},
/*SPEED_20G_KR2             30/0x1e*/  { 2, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_66, 1, 0, 0},
/*SPEED_20G_CR2             31/0x1f*/  { 2, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_66, 1, 0, 0},
/*SPEED_21G_X2 TBD          32/0x20*/  { 2, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_70, 1, 0, 0},
/*SPEED_40G_KR4             33/0x21*/  { 4, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_66, 1, 0, 0},
/*SPEED_40G_CR4             34/0x22*/  { 4, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_66, 1, 0, 0},
/*SPEED_42G_X4              35/0x23*/  { 4, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_70, 1, 0, 0},
/*SPEED_100G_CR10 TBD       36/0x24*/  {10, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_66, 1, 0, 0},
/*SPEED_107G_CR10 TBD       37/0x25*/  {10, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_70, 1, 0, 0},
/*SPEED_120G_X12 TBD        38/0x26*/  {12, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_66, 1, 0, 0},
/*SPEED_127G_X12 TBD        39/0x27*/  {12, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_70, 1, 0, 0},
/*SPEED_12.12G_KR1:         40/0x28*/  { 0, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_80, 1, 0, 0},
/*SPEED_24.24G_KR2          41/0x29*/  { 2, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_80, 1, 0, 0},
/*SPEED_48.48G_KR4          42/0x2a*/  { 4, TEMOD16_PMA_OS_MODE_1,     TEMOD16_PLL_MODE_DIV_80, 1, 0, 0},
/*SPEED_ILLEGAL             43/0x2b*/  { 0, TEMOD16_PMA_OS_MODE_5,     TEMOD16_PLL_MODE_DIV_40, 0, 0, 0},
/*SPEED_ILLEGAL             44/0x2c*/  { 0, TEMOD16_PMA_OS_MODE_5,     TEMOD16_PLL_MODE_DIV_40, 0, 0, 0},
/*SPEED_ILLEGAL             45/0x2d*/  { 0, TEMOD16_PMA_OS_MODE_5,     TEMOD16_PLL_MODE_DIV_40, 0, 0, 0},
/*SPEED_ILLEGAL             46/0x2e*/  { 0, TEMOD16_PMA_OS_MODE_5,     TEMOD16_PLL_MODE_DIV_40, 0, 0, 0},
/*SPEED_ILLEGAL             47/0x2f*/  { 0, TEMOD16_PMA_OS_MODE_5,     TEMOD16_PLL_MODE_DIV_40, 0, 0, 0},
/*SPEED_ILLEGAL             48/0x30*/  { 0, TEMOD16_PMA_OS_MODE_5,     TEMOD16_PLL_MODE_DIV_40, 0, 0, 0},
/*SPEED_5G_KR1              49/0x31*/  { 1, TEMOD16_PMA_OS_MODE_2,     TEMOD16_PLL_MODE_DIV_66, 0, 0, 0},
/*SPEED_10p5G_X4            50/0x32*/  { 2, TEMOD16_PMA_OS_MODE_2,     TEMOD16_PLL_MODE_DIV_42, 0, 0, 0},
/*SPEED_ILLEGAL             51/0x33*/  { 0, TEMOD16_PMA_OS_MODE_5,     TEMOD16_PLL_MODE_DIV_40, 0, 0, 0},
/*SPEED_ILLEGAL             52/0x33*/  { 0, TEMOD16_PMA_OS_MODE_5,     TEMOD16_PLL_MODE_DIV_40, 0, 0, 0},
/*SPEED_10M_10p3125         53/0x35*/  { 1, TEMOD16_PMA_OS_MODE_8_25,  TEMOD16_PLL_MODE_DIV_66, 0, 0, 0},
/*SPEED_100M_10p3125        54/0x36*/  { 1, TEMOD16_PMA_OS_MODE_8_25,  TEMOD16_PLL_MODE_DIV_66, 0, 0, 0},
/*SPEED_1000M_10p3125       55/0x37*/  { 1, TEMOD16_PMA_OS_MODE_8_25,  TEMOD16_PLL_MODE_DIV_66, 0, 0, 0},
/*SPEED_2p5G_X1_10p3125     56/0x38*/  { 1, TEMOD16_PMA_OS_MODE_3_3,   TEMOD16_PLL_MODE_DIV_66, 0, 0, 0},
/*SPEED_10G_KX4_10p3125:    57/0x39*/  { 4, TEMOD16_PMA_OS_MODE_3_3,   TEMOD16_PLL_MODE_DIV_66, 0, 0, 0},
/*SPEED_10G_CX4_10p3125:    58/0x3a*/  { 4, TEMOD16_PMA_OS_MODE_3_3,   TEMOD16_PLL_MODE_DIV_66, 0, 0, 0},
/*SPEED_10G_X4_10p3125:     59/0x3b*/  { 4, TEMOD16_PMA_OS_MODE_3_3,   TEMOD16_PLL_MODE_DIV_66, 0, 0, 0},
/*SPEED_ILLEGAL             60/0x3c*/  { 0, TEMOD16_PMA_OS_MODE_5,     TEMOD16_PLL_MODE_DIV_40, 0, 0, 0}

};
#endif

int _temod16_set_port_mode_select(PHYMOD_ST *pc);
int _configure_sc_speed_configuration(PHYMOD_ST* pc);
int _init_pcs_fs(PHYMOD_ST* pc);
int _init_pcs_an(PHYMOD_ST* pc);
int _temod16_wait_sc_stats_set(PHYMOD_ST* pc);
int _configure_st_entry(int st_entry_num, int st_entry_speed, PHYMOD_ST* pc);

#ifdef _DV_TB_
int temod16_set_an_port_mode(PHYMOD_ST* pc);
int temod16_autoneg_set(PHYMOD_ST* pc);
int temod16_autoneg_control(PHYMOD_ST* pc);
int temod16_clause72_control(PHYMOD_ST* pc, cl72_type_t cl_72_type);
int temod16_prbs_check(PHYMOD_ST* pc, int real_check, int *prbs_status);
int temod16_prbs_mode(PHYMOD_ST* pc, int port, int prbs_inv, int pat, int check_mode);
int temod16_prbs_control(PHYMOD_ST* pc, int prbs_enable);
int temod16_prbs_rx_enable_set(PHYMOD_ST* pc, int enable);
int temod16_prbs_tx_enable_set(PHYMOD_ST* pc, int enable);
int temod16_prbs_rx_invert_data_set(PHYMOD_ST* pc, int invert_data);
int temod16_prbs_rx_check_mode_set(PHYMOD_ST* pc, int check_mode);
int temod16_prbs_tx_invert_data_set(PHYMOD_ST* pc, int invert_data);
int temod16_prbs_tx_polynomial_set(PHYMOD_ST* pc, prbs_polynomial_type_t prbs_polynomial);
int temod16_prbs_rx_polynomial_set(PHYMOD_ST* pc, prbs_polynomial_type_t prbs_polynomial);
int temod16_pmd_lane_swap(PHYMOD_ST *pc);
int temod16_check_sc_stats(PHYMOD_ST* pc);
int temod16_pmd_native_mode_set(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf);
#endif /* _DV_TB_ */

/*!
@brief   This function reads TX-PLL PLL_LOCK bit.
@param   pc  handle to current TSCE context (#PHYMOD_ST)
@param   lockStatus reference which is updated with lock status.
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Read PLL lock status. Returns  1 or 0 (locked/not)
*/

int temod16_pll_lock_get(PHYMOD_ST* pc, int* lockStatus)
{
  PMD_X1_STSr_t  reg_pmd_x1_sts;
  TMOD_DBG_IN_FUNC_INFO(pc);
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

int temod16_pmd_lock_get(PHYMOD_ST* pc, uint32_t* lockStatus)
{
    PMD_X4_STSr_t  reg_pmd_x4_sts;
    int i = 0, start_lane = 0, num_lane = 0;
    phymod_access_t pa_copy;

    TMOD_DBG_IN_FUNC_INFO(pc);
    *lockStatus = 1;
    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    for(i=0; i<num_lane; i++) {
        if(((pc->lane_mask >> (start_lane + i)) & 0x1) == 1) {
            pa_copy.lane_mask |= 0x1 << (start_lane + i);
            READ_PMD_X4_STSr(&pa_copy, &reg_pmd_x4_sts);
            *lockStatus = *lockStatus & PMD_X4_STSr_RX_LOCK_STSf_GET(reg_pmd_x4_sts);
        }
    }

    return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/*
@brief   This function waits for TX-PLL PLL_LOCK bit. The register is one copy
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Wait for PLL to lock. Cannot be used in SDK.
*/

int temod16_pll_lock_wait(PHYMOD_ST* pc, int timeOutValue)
{
   int rv;
   TMOD_DBG_IN_FUNC_INFO(pc);

   rv = temod16_regbit_set_wait_check(pc,
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
int temod16_wait_pmd_lock(PHYMOD_ST* pc, int timeOutValue, int* lockStatus)
{
   int rv; 
   PMD_X4_STSr_t reg_pmd_x4_stats;
   TMOD_DBG_IN_FUNC_INFO(pc);

   PMD_X4_STSr_CLR(reg_pmd_x4_stats);

   if (pc->per_lane_control) {
      rv = temod16_regbit_set_wait_check(pc, 0xc012, 1,1,timeOutValue);
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
      if (PMD_X4_STSr_RX_LOCK_STSf_GET(reg_pmd_x4_stats)) {
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
int temod16_pcs_bypass_ctl (PHYMOD_ST* pc, int cntl)
{
  SC_X4_BYPASSr_t reg_sc_bypass;
  
  TMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_BYPASSr_CLR(reg_sc_bypass);

  if (cntl==TEMOD16_SC_MODE_BYPASS) {
    SC_X4_BYPASSr_SC_BYPASSf_SET(reg_sc_bypass, 1);  
  } else {
    SC_X4_BYPASSr_SC_BYPASSf_SET(reg_sc_bypass, 0);  
  }

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X4_BYPASSr( pc, reg_sc_bypass));

  return PHYMOD_E_NONE;
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
int temod16_pmd_reset_bypass (PHYMOD_ST* pc, int cntl)     /* PMD_RESET_BYPASS */
{
  int lnCntl, coreCntl;
  lnCntl   = cntl & 0x10 ; 
  coreCntl = cntl & 0x1 ; 
  PMD_X4_OVRRr_t reg_pmd_x4_or;
  PMD_X4_CTLr_t  reg_pmd_x4_ctrl;
  PMD_X1_OVRRr_t reg_pmd_x1_or;
  PMD_X1_CTLr_t  reg_pmd_x1_ctrl;
  SC_X4_CTLr_t reg_sc_ctrl;

  TMOD_DBG_IN_FUNC_INFO(pc);
  PMD_X4_OVRRr_CLR(reg_pmd_x4_or);
  PMD_X4_CTLr_CLR(reg_pmd_x4_ctrl);
  PMD_X1_OVRRr_CLR(reg_pmd_x1_or);
  PMD_X1_CTLr_CLR(reg_pmd_x1_ctrl);
  SC_X4_CTLr_CLR(reg_sc_ctrl);

  if (lnCntl) {
    PMD_X4_OVRRr_LN_RX_DP_H_RSTB_OENf_SET(reg_pmd_x4_or, 1);
    PMD_X4_OVRRr_LN_TX_DP_H_RSTB_OENf_SET(reg_pmd_x4_or, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_OVRRr(pc, reg_pmd_x4_or));

    PMD_X4_CTLr_LN_RX_DP_H_RSTBf_SET(reg_pmd_x4_ctrl, 0);
    PMD_X4_CTLr_LN_TX_DP_H_RSTBf_SET(reg_pmd_x4_ctrl, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, reg_pmd_x4_ctrl));

    /* toggle added */
    PMD_X4_OVRRr_CLR(reg_pmd_x4_or);
    PMD_X4_CTLr_CLR(reg_pmd_x4_ctrl);

    PMD_X4_CTLr_LN_RX_DP_H_RSTBf_SET(reg_pmd_x4_ctrl, 1);
    PMD_X4_CTLr_LN_TX_DP_H_RSTBf_SET(reg_pmd_x4_ctrl, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, reg_pmd_x4_ctrl));

    PMD_X4_OVRRr_LN_RX_DP_H_RSTB_OENf_SET(reg_pmd_x4_or, 0);
    PMD_X4_OVRRr_LN_TX_DP_H_RSTB_OENf_SET(reg_pmd_x4_or, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_OVRRr(pc, reg_pmd_x4_or));

  } else {
    PMD_X4_OVRRr_LN_RX_DP_H_RSTB_OENf_SET(reg_pmd_x4_or, 0);
    PMD_X4_OVRRr_LN_TX_DP_H_RSTB_OENf_SET(reg_pmd_x4_or, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_OVRRr(pc, reg_pmd_x4_or));

    PMD_X4_CTLr_LN_RX_DP_H_RSTBf_SET(reg_pmd_x4_ctrl, 1);
    PMD_X4_CTLr_LN_TX_DP_H_RSTBf_SET(reg_pmd_x4_ctrl, 1);
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

#ifdef _SDK_TEMOD16_
/*!
\brief Controls the init setting/resetting of autoneg  registers.
\param  pc handle to current TSCE context (#PHYMOD_ST)
\param  temod16_an_init_t structure with all the onr time autoneg init cfg.
\returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
\details Get aneg features via #temod16_an_init_t.
  This does not start the autoneg. That is done in temod16_autoneg_control

*/
int temod16_autoneg_set_init(PHYMOD_ST* pc, temod16_an_init_t *an_init_st) /* AUTONEG_SET */
{
  AN_X4_CTLSr_t    reg_an_ctrl;
  DIGITAL_CTL1000X2r_t   reg_ctrl1000x2;
  AN_X4_LOC_DEV_CL73_BASE_ABIL1r_t reg_an_cl73_abl_1;
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_t    reg_cl73_abiilities;

  TMOD_DBG_IN_FUNC_INFO(pc);
  AN_X4_CTLSr_CLR(reg_an_ctrl);
  AN_X4_CTLSr_AN_FAIL_COUNT_LIMITf_SET(reg_an_ctrl, an_init_st->an_fail_cnt);
  AN_X4_CTLSr_OUI_CONTROLf_SET(reg_an_ctrl, an_init_st->an_oui_ctrl);
  AN_X4_CTLSr_LINKFAILTIMER_DISf_SET(reg_an_ctrl, an_init_st->linkfailtimer_dis);
  AN_X4_CTLSr_LINKFAILTIMERQUAL_ENf_SET(reg_an_ctrl, an_init_st->linkfailtimerqua_en);
  AN_X4_CTLSr_AN_GOOD_CHECK_TRAPf_SET(reg_an_ctrl, an_init_st->an_good_check_trap);
  AN_X4_CTLSr_AN_GOOD_TRAPf_SET(reg_an_ctrl,  an_init_st->an_good_trap);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_CTLSr(pc, reg_an_ctrl));

  DIGITAL_CTL1000X2r_CLR(reg_ctrl1000x2);
  DIGITAL_CTL1000X2r_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_ctrl1000x2, an_init_st->disable_rf_report);
  PHYMOD_IF_ERR_RETURN(MODIFY_DIGITAL_CTL1000X2r(pc, reg_ctrl1000x2));

  AN_X4_LOC_DEV_CL73_BASE_ABIL1r_CLR(reg_an_cl73_abl_1);
  AN_X4_LOC_DEV_CL73_BASE_ABIL1r_CL73_NONCE_MATCH_OVERf_SET(reg_an_cl73_abl_1, an_init_st->cl73_nonce_match_over);
  AN_X4_LOC_DEV_CL73_BASE_ABIL1r_CL73_NONCE_MATCH_VALf_SET(reg_an_cl73_abl_1, an_init_st->cl73_nonce_match_val);
  AN_X4_LOC_DEV_CL73_BASE_ABIL1r_BASE_SELECTORf_SET(reg_an_cl73_abl_1, an_init_st->base_selector);
  AN_X4_LOC_DEV_CL73_BASE_ABIL1r_TRANSMIT_NONCEf_SET(reg_an_cl73_abl_1, ( an_init_st->cl73_transmit_nonce & 0x1f));
  PHYMOD_IF_ERR_RETURN ( MODIFY_AN_X4_LOC_DEV_CL73_BASE_ABIL1r(pc, reg_an_cl73_abl_1));

  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_CLR(reg_cl73_abiilities);
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_CL73_REMOTE_FAULTf_SET(reg_cl73_abiilities, ( an_init_st->cl73_remote_fault & 1)); 
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL73_BASE_ABIL0r(pc, reg_cl73_abiilities));

  return PHYMOD_E_NONE;
} 
#endif

#ifdef _SDK_TEMOD16_
/*!
\brief Controls the setting/resetting of autoneg timers.
\param  pc handle to current TSCE context (#PHYMOD_ST)
\returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
\details
  Get aneg features via #PHYMOD_ST->an_tech_ability and write the pages
  This does not start the autoneg. That is done in temod16_autoneg_control

*/
int temod16_autoneg_timer_init(PHYMOD_ST* pc)               /* AUTONEG timer set*/
{

   AN_X1_CL37_RESTARTr_t    reg_cl37_restart;
   AN_X1_CL37_ACKr_t        reg_cl37_ack;
   AN_X1_CL73_BRK_LNKr_t reg_cl73_break_link;
   AN_X1_CL73_DME_LOCKr_t   reg_cl73_dme_lock;
   AN_X1_LNK_UPr_t         reg_link_up;
   AN_X1_LNK_FAIL_INHBT_TMR_CL72r_t     reg_inhibit_timer;
   AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_t reg_inhibit_not_timer;
   AN_X1_PD_SD_TMRr_t                      reg_pd_sd_timer;
   AN_X1_CL37_SYNC_STS_FILTER_TMRr_t    reg_sync_status_filter_timer;
   AN_X1_PD_TO_CL37_LNK_WAIT_TMRr_t       reg_link_wait_timer;
   AN_X1_IGNORE_LNK_TMRr_t                reg_ignore_link_timer;
   AN_X1_DME_PAGE_TMRr_t                   reg_dme_page_timer;
   AN_X1_SGMII_CL73_TMR_TYPEr_t            reg_sgmii_cl73_timer;
   
  TMOD_DBG_IN_FUNC_INFO(pc);
  /*0x9250 AN_X1_TIMERS_cl37_restart */
  AN_X1_CL37_RESTARTr_CLR(reg_cl37_restart);
  AN_X1_CL37_RESTARTr_SET(reg_cl37_restart, 0x29a );
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL37_RESTARTr(pc, reg_cl37_restart));

  /*0x9251 AN_X1_TIMERS_cl37_ack */
  AN_X1_CL37_ACKr_CLR(reg_cl37_ack);
  AN_X1_CL37_ACKr_SET(reg_cl37_ack,  0x29a);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL37_ACKr(pc, reg_cl37_ack));

   /*0x9253 AN_X1_TIMERS_cl73_break_link */
  AN_X1_CL73_BRK_LNKr_CLR(reg_cl73_break_link);
  AN_X1_CL73_BRK_LNKr_SET(reg_cl73_break_link, 0x10ed);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_BRK_LNKr(pc, reg_cl73_break_link));

/*TBD::0x9255 AN_X1_TIMERS_cl73_dme_lock*/ 
  AN_X1_CL73_DME_LOCKr_CLR(reg_cl73_dme_lock);
  AN_X1_CL73_DME_LOCKr_SET(reg_cl73_dme_lock, 0x14d4);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_DME_LOCKr(pc, reg_cl73_dme_lock));

/*TBD::0x9256 AN_X1_TIMERS_link_up*/ 
  AN_X1_LNK_UPr_CLR(reg_link_up);
  AN_X1_LNK_UPr_SET(reg_link_up, 0x29a);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_UPr(pc, reg_link_up));

/*0x9257 AN_X1_TIMERS_link_fail_inhibit_timer_cl72*/ 
  AN_X1_LNK_FAIL_INHBT_TMR_CL72r_CLR(reg_inhibit_timer);
  AN_X1_LNK_FAIL_INHBT_TMR_CL72r_SET(reg_inhibit_timer, 0x8382);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_FAIL_INHBT_TMR_CL72r(pc, reg_inhibit_timer));

/*0x9258 AN_X1_TIMERS_link_fail_inhibit_timer_not_cl72*/ 
  AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_CLR(reg_inhibit_not_timer);
  AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_SET(reg_inhibit_not_timer, 0x1ee0);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r(pc, reg_inhibit_not_timer));

/*0x9259 AN_X1_TIMERS_pd_sd_timer*/ 
 AN_X1_PD_SD_TMRr_CLR(reg_pd_sd_timer);
 AN_X1_PD_SD_TMRr_SET(reg_pd_sd_timer, 0xa6a);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_PD_SD_TMRr(pc, reg_pd_sd_timer));

/*0x925a AN_X1_TIMERS_cl72_max_wait_timer*/ 
  AN_X1_CL37_SYNC_STS_FILTER_TMRr_CLR(reg_sync_status_filter_timer);
  AN_X1_CL37_SYNC_STS_FILTER_TMRr_SET(reg_sync_status_filter_timer, 0x29a);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL37_SYNC_STS_FILTER_TMRr(pc, reg_sync_status_filter_timer));

/*0x925b AN_X1_TIMERS_PD_TO_CL37_LINK_WAIT_TIMER*/ 
  AN_X1_PD_TO_CL37_LNK_WAIT_TMRr_CLR(reg_link_wait_timer);
  AN_X1_PD_TO_CL37_LNK_WAIT_TMRr_SET(reg_link_wait_timer, 0xa6a);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_PD_TO_CL37_LNK_WAIT_TMRr(pc, reg_link_wait_timer));

  /*0x925c AN_X1_TIMERS_ignore_link_timer*/ 
  AN_X1_IGNORE_LNK_TMRr_CLR(reg_ignore_link_timer);
  AN_X1_IGNORE_LNK_TMRr_SET(reg_ignore_link_timer, 0x29a);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_IGNORE_LNK_TMRr(pc, reg_ignore_link_timer));

  /*0x925d AN_X1_TIMERS_dme_page_timer*/  
  AN_X1_DME_PAGE_TMRr_CLR(reg_dme_page_timer);
  AN_X1_DME_PAGE_TMRr_SET(reg_dme_page_timer, 0x3b5f);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_DME_PAGE_TMRr(pc, reg_dme_page_timer));

  /*0x925e AN_X1_TIMERS_sgmii_cl73_timer_type*/ 
  AN_X1_SGMII_CL73_TMR_TYPEr_CLR(reg_sgmii_cl73_timer);
  AN_X1_SGMII_CL73_TMR_TYPEr_SET(reg_sgmii_cl73_timer, 0x6b);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_SGMII_CL73_TMR_TYPEr(pc, reg_sgmii_cl73_timer));
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
  This does not start the autoneg. That is done in temod16_autoneg_control

*/
int temod16_autoneg_set(PHYMOD_ST* pc)               /* AUTONEG_SET */
{

  AN_X4_CTLSr_t                     reg_an_ctrl;
  DIGITAL_CTL1000X2r_t                  reg_ctrl1000x2;
  AN_X4_LOC_DEV_CL37_BASE_ABILr_t   reg_cl37_base_abilities;
  AN_X4_LOC_DEV_CL37_BAM_ABILr_t    reg_cl37_bam_abilities;
  AN_X4_LOC_DEV_OVER1G_ABIL0r_t     reg_over1g_abilities;
  AN_X4_LOC_DEV_OVER1G_ABIL1r_t     reg_over1g_abilities1;
  AN_X4_LOC_DEV_CL73_BAM_ABILr_t    reg_cl73_bam_abilities;
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_t  reg_cl73_base_abilities;

  TMOD_DBG_IN_FUNC_INFO(pc);
  AN_X4_CTLSr_CLR(reg_an_ctrl);

  AN_X4_CTLSr_AN_FAIL_COUNT_LIMITf_SET(reg_an_ctrl, pc->an_fail_cnt);
  AN_X4_CTLSr_OUI_CONTROLf_SET(reg_an_ctrl, pc->an_oui_ctrl);
  AN_X4_CTLSr_PD_KX_ENf_SET(reg_an_ctrl, pc->pd_kx_en);
  AN_X4_CTLSr_PD_KX4_ENf_SET(reg_an_ctrl, pc->pd_kx4_en);
  AN_X4_CTLSr_LINKFAILTIMER_DISf_SET(reg_an_ctrl, pc->linkfailtimer_dis);
  AN_X4_CTLSr_LINKFAILTIMERQUAL_ENf_SET(reg_an_ctrl, pc->linkfailtimerqua_en);
  AN_X4_CTLSr_AN_GOOD_CHECK_TRAPf_SET(reg_an_ctrl, pc->an_good_check_trap);
  AN_X4_CTLSr_AN_GOOD_TRAPf_SET(reg_an_ctrl,  pc->an_good_trap);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_CTLSr(pc, reg_an_ctrl));


  DIGITAL_CTL1000X2r_CLR(reg_ctrl1000x2);
  DIGITAL_CTL1000X2r_DISABLE_REMOTE_FAULT_REPORTINGf_SET(reg_ctrl1000x2, pc->disable_rf_report);
  PHYMOD_IF_ERR_RETURN(MODIFY_DIGITAL_CTL1000X2r(pc, reg_ctrl1000x2));

  if (pc->sc_mode == TEMOD16_SC_MODE_AN_CL37) {
      /* an37 */
      if (!(pc->cl37_bam_en)) {
       AN_X4_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities);
       AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities, 1);
       AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_SET (reg_cl37_base_abilities, 1);     
       PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities));
      } 
      
      /*Local device cl37 base abilities setting*/
      /******* Pause Settings ********/
      AN_X4_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities);
      if (pc->cl37_an_pause) { 
          AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_SET(reg_cl37_base_abilities, (pc->cl37_an_pause & 3));
      }
      /******* Half duplex Settings ********/
      if (pc->cl37_an_hd) {
         AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_HALF_DUPLEXf_SET(reg_cl37_base_abilities, (pc->cl37_an_hd & 1));
      }   
      /******* SGMII Master mode Settings ********/
      if (pc->cl37_sgmii_master_mode) { 
               AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_cl37_base_abilities, (pc->cl37_sgmii_master_mode & 1));
      }
      /******* SGMII Speed Settings ********/
      /*if (pc->cl37_sgmii_speed) { */
              AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_SET(reg_cl37_base_abilities, (pc->cl37_sgmii_speed & 3));
      /*}*/
      /******* CL37 Next page setting ********/
      if (pc->cl37_an_np) { 
               AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_cl37_base_abilities, (pc->cl37_an_np & 1));
      }
      /******* CL37 full duplex ********/
      if (pc->cl37_an_fd) { 
               AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(reg_cl37_base_abilities, (pc->cl37_an_fd & 1));
      }
      /******* SGMII duplex Settings ********/
      if (pc->cl37_sgmii_duplex) { 
               AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities, (pc->cl37_sgmii_duplex & 1));
      }
          

      /***** Setting AN_X4_ABILITIES_ld_base_abilities_1 0xC181 *******/
       PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities));

      /*Local device cl37 bam abilities setting*/

      /******* CL37 Bam AN_X4_ABILITIES_local_device_over1g_abilities_0 setting********/
      /* cl37 bam abilities 0xc182 */
      AN_X4_LOC_DEV_CL37_BAM_ABILr_CLR(reg_cl37_bam_abilities);
         AN_X4_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_cl37_bam_abilities, (pc->cl37_bam_code & 0x1ff));
         AN_X4_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_cl37_bam_abilities, (pc->cl37_bam_ovr1g_en & 1));
         AN_X4_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_cl37_bam_abilities, (pc->cl37_bam_ovr1g_pgcnt & 3));

      PHYMOD_IF_ERR_RETURN 
             (MODIFY_AN_X4_LOC_DEV_CL37_BAM_ABILr(pc, reg_cl37_bam_abilities));


      AN_X4_LOC_DEV_OVER1G_ABIL0r_CLR(reg_over1g_abilities); 
      if (pc->cl37_bam_speed) 
          AN_X4_LOC_DEV_OVER1G_ABIL0r_SET(reg_over1g_abilities, (pc->cl37_bam_speed & 0x7ff));    
      /***** Setting AN_X4_ABILITIES_local_device_over1g_abilities_0  0xC184 *******/
      PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LOC_DEV_OVER1G_ABIL0r(pc, reg_over1g_abilities));

      /******* CL37 Bam AN_X4_ABILITIES_local_device_over1g_abilities_1 setting********/
      AN_X4_LOC_DEV_OVER1G_ABIL1r_CLR(reg_over1g_abilities1);
      if (pc->cl37_bam_speed)  
           AN_X4_LOC_DEV_OVER1G_ABIL1r_SET(reg_over1g_abilities1,pc->cl37_bam_speed >> 11);
        /* AN_LOCAL_DEVICE_OVER1G_ABILITIES_1r_BAM_13GBASE_X4f_SET(reg_over1g_abilities1, (pc->cl37_bam_speed >> 11)); */
      if (pc->cl37_bam_fec)
              AN_X4_LOC_DEV_OVER1G_ABIL1r_FECf_SET(reg_over1g_abilities1, (pc->cl37_bam_fec & 1));
     if (pc->cl37_bam_hg2) 
               AN_X4_LOC_DEV_OVER1G_ABIL1r_HG2f_SET(reg_over1g_abilities1, (pc->cl37_bam_hg2 & 1));
      if (pc->an_abilities_CL72) 
               AN_X4_LOC_DEV_OVER1G_ABIL1r_CL72f_SET(reg_over1g_abilities1, (pc->an_abilities_CL72 & 1));
      /***** Setting AN_X4_ABILITIES_local_device_over1g_abilities_0  0xC183 *******/
      PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LOC_DEV_OVER1G_ABIL1r(pc, reg_over1g_abilities1));

  }/*Completion of CL37 abilities*/
  /*CL73 abilities*/
  else if (pc->sc_mode == TEMOD16_SC_MODE_AN_CL73) {
      /*PD to CL37 Enable*/
      if (pc->an_pd_to_cl37_enable) {
        AN_X4_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities);
        AN_X4_LOC_DEV_CL37_BASE_ABILr_AN_PD_TO_CL37_ENABLEf_SET(reg_cl37_base_abilities, 0x1);
        PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities));
        AN_X4_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities);
        if (pc->cl37_an_pause) { 
                 AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_SET(reg_cl37_base_abilities, (pc->cl37_an_pause & 3));
        }
        /******* Half duplex Settings ********/
        if (pc->cl37_an_hd) {
                 AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_HALF_DUPLEXf_SET(reg_cl37_base_abilities, (pc->cl37_an_hd & 1));
        }   
        /******* SGMII Master mode Settings ********/
        if (pc->cl37_sgmii_master_mode) { 
                 AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_cl37_base_abilities, (pc->cl37_sgmii_master_mode & 1));
        }
        /******* SGMII Speed Settings ********/
        if (pc->cl37_sgmii_speed) { 
                 AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_SET(reg_cl37_base_abilities, (pc->cl37_sgmii_speed & 3));
        }
        /******* CL37 Next page setting ********/
        if (pc->cl37_an_np) { 
                 AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_cl37_base_abilities, (pc->cl37_an_np & 1));
        }
        /******* CL37 full duplex ********/
        if (pc->cl37_an_fd) { 
                 AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(reg_cl37_base_abilities, (pc->cl37_an_fd & 1));
        }
        /******* SGMII duplex Settings ********/
        if (pc->cl37_sgmii_duplex) { 
                 AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities, (pc->cl37_sgmii_duplex & 1));
        }

        /***** Setting AN_X4_ABILITIES_ld_base_abilities_1 0xC181 *******/
        PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities));
      }
      /*CL73 bam abilities setting*/
      AN_X4_LOC_DEV_CL73_BAM_ABILr_CLR(reg_cl73_bam_abilities);
      AN_X4_LOC_DEV_CL73_BAM_ABILr_BAM_20GBASE_KR2f_SET(reg_cl73_bam_abilities,( pc->cl73_bam_speed & 1));
      AN_X4_LOC_DEV_CL73_BAM_ABILr_BAM_20GBASE_CR2f_SET(reg_cl73_bam_abilities,((pc->cl73_bam_speed >> 1) & 0x1));
      AN_X4_LOC_DEV_CL73_BAM_ABILr_HPAM_20GKR2f_SET(reg_cl73_bam_abilities,  ((pc->cl73_bam_speed & 4) >> 2));
      AN_X4_LOC_DEV_CL73_BAM_ABILr_CL73_BAM_CODEf_SET(reg_cl73_bam_abilities, (pc->cl73_bam_code & 0x1ff));
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL73_BAM_ABILr(pc, reg_cl73_bam_abilities));
      /*CL73 base abilities_0 setting*/
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_CLR(reg_cl73_base_abilities);
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_NEXT_PAGEf_SET(reg_cl73_base_abilities, (pc->cl73_nxt_page & 1));
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_100GBASE_CR10f_SET(reg_cl73_base_abilities, ((pc->cl73_speed >> 0)& 0x1));
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_40GBASE_CR4f_SET(reg_cl73_base_abilities,   ((pc->cl73_speed >> 1)& 0x1));
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_40GBASE_KR4f_SET(reg_cl73_base_abilities,   ((pc->cl73_speed >> 2)& 0x1));
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_10GBASE_KRf_SET(reg_cl73_base_abilities,    ((pc->cl73_speed >> 3)& 0x1));
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_10GBASE_KX4f_SET(reg_cl73_base_abilities,   ((pc->cl73_speed >> 4)& 0x1));
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_1000BASE_KXf_SET(reg_cl73_base_abilities,   ((pc->cl73_speed >> 5)& 0x1));

      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_CL73_PAUSEf_SET(reg_cl73_base_abilities, ( pc->cl73_pause & 3));
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_FECf_SET(reg_cl73_base_abilities, ( pc->cl73_fec & 3));
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_CL73_REMOTE_FAULTf_SET(reg_cl73_base_abilities, ( pc->cl73_remote_fault & 1));
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL73_BASE_ABIL0r(pc, reg_cl73_base_abilities));
  }

  return PHYMOD_E_NONE;

}
#endif
#ifdef _SDK_TEMOD16_
int temod16_autoneg_set(PHYMOD_ST* pc,
    temod16_an_ability_t *an_ability_st) /* AUTONEG_SET */
 {

  AN_X4_LOC_DEV_CL37_BASE_ABILr_t    reg_cl37_base_abilities;
  AN_X4_LOC_DEV_CL37_BAM_ABILr_t     reg_cl37_bam_abilities;
  AN_X4_LOC_DEV_OVER1G_ABIL0r_t     reg_over1g_abilities;
  AN_X4_LOC_DEV_OVER1G_ABIL1r_t     reg_over1g_abilities1;
  AN_X4_LOC_DEV_CL73_BAM_ABILr_t     reg_cl73_bam_abilities;
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_t  reg_cl73_base_abilities;
  AN_X4_LOC_DEV_CL73_BASE_ABIL1r_t  reg_an_cl73_abl_1;

  TMOD_DBG_IN_FUNC_INFO(pc);

  /* an37 */
  AN_X4_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities);
  AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities, 1);
  if((an_ability_st->cl37_adv.an_bam_speed) || (an_ability_st->cl37_adv.an_bam_speed1)) {
      AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_cl37_base_abilities, 0);
  }
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities));

  /*Local device cl37 base abilities setting*/
  AN_X4_LOC_DEV_CL37_BASE_ABILr_CLR(reg_cl37_base_abilities);
  AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_SET(reg_cl37_base_abilities, (an_ability_st->cl37_adv.an_pause & 3));
  AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_SET(reg_cl37_base_abilities, (an_ability_st->cl37_adv.cl37_sgmii_speed & 3));

  AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_SET(reg_cl37_base_abilities, 1);
  AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_SET(reg_cl37_base_abilities, 1);

  /***** Setting AN_X4_ABILITIES_ld_base_abilities_1 0xC181 *******/
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL37_BASE_ABILr(pc, reg_cl37_base_abilities));

  /*Local device cl37 bam abilities setting*/
  /******* CL37 Bam AN_X4_ABILITIES_local_device_over1g_abilities_0 setting********/
  /* cl37 bam abilities 0xc182 */
  /* Once we put this code in init, can remove teh over1g* cfg from here */
  AN_X4_LOC_DEV_CL37_BAM_ABILr_CLR(reg_cl37_bam_abilities);
  AN_X4_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_cl37_bam_abilities, 0x2);
  AN_X4_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_cl37_bam_abilities, 0x1);
  AN_X4_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_cl37_bam_abilities, 0x5);

  PHYMOD_IF_ERR_RETURN
      (MODIFY_AN_X4_LOC_DEV_CL37_BAM_ABILr(pc, reg_cl37_bam_abilities));

  AN_X4_LOC_DEV_OVER1G_ABIL0r_CLR(reg_over1g_abilities);
  AN_X4_LOC_DEV_OVER1G_ABIL1r_CLR(reg_over1g_abilities1);

  if (an_ability_st->cl37_adv.an_bam_speed)
      AN_X4_LOC_DEV_OVER1G_ABIL0r_SET(reg_over1g_abilities, (an_ability_st->cl37_adv.an_bam_speed & 0x7ff));
  if (an_ability_st->cl37_adv.an_bam_speed1)
      AN_X4_LOC_DEV_OVER1G_ABIL1r_SET(reg_over1g_abilities1, (an_ability_st->cl37_adv.an_bam_speed1 & 0x1fff ));


  /******* CL37 Bam AN_X4_ABILITIES_local_device_over1g_abilities_1 setting********/
  if (an_ability_st->cl37_adv.an_fec == TEMOD16_FEC_CL74_SUPRTD_REQSTD)
      AN_X4_LOC_DEV_OVER1G_ABIL1r_FECf_SET(reg_over1g_abilities1, (an_ability_st->cl37_adv.an_fec & 1));
  if (an_ability_st->cl37_adv.an_hg2)
      AN_X4_LOC_DEV_OVER1G_ABIL1r_HG2f_SET(reg_over1g_abilities1, (an_ability_st->cl37_adv.an_hg2 & 1));
  if (an_ability_st->cl37_adv.an_cl72)
      AN_X4_LOC_DEV_OVER1G_ABIL1r_CL72f_SET(reg_over1g_abilities1, (an_ability_st->cl37_adv.an_cl72 & 1));
  /***** Setting AN_X4_ABILITIES_local_device_over1g_abilities_0  0xC183 *******/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LOC_DEV_OVER1G_ABIL0r(pc, reg_over1g_abilities));
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LOC_DEV_OVER1G_ABIL1r(pc, reg_over1g_abilities1));
  /*Completion of CL37 abilities*/

  /*CL73 abilities*/
  /*CL73 bam abilities setting*/
  AN_X4_LOC_DEV_CL73_BAM_ABILr_CLR(reg_cl73_bam_abilities);
  AN_X4_LOC_DEV_CL73_BAM_ABILr_BAM_20GBASE_KR2f_SET(reg_cl73_bam_abilities,( an_ability_st->cl73_adv.an_bam_speed & 1));
  AN_X4_LOC_DEV_CL73_BAM_ABILr_BAM_20GBASE_CR2f_SET(reg_cl73_bam_abilities,((an_ability_st->cl73_adv.an_bam_speed >> 1) & 0x1));
  AN_X4_LOC_DEV_CL73_BAM_ABILr_HPAM_20GKR2f_SET(reg_cl73_bam_abilities,  ((an_ability_st->cl73_adv.an_bam_speed & 4) >> 2));
  AN_X4_LOC_DEV_CL73_BAM_ABILr_CL73_BAM_CODEf_SET(reg_cl73_bam_abilities,  0x4);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL73_BAM_ABILr(pc, reg_cl73_bam_abilities));
  /*CL73 base abilities_0 setting*/
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_CLR(reg_cl73_base_abilities);
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_100GBASE_CR10f_SET(reg_cl73_base_abilities, ((an_ability_st->cl73_adv.an_base_speed >> 0)& 0x1));
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_40GBASE_CR4f_SET(reg_cl73_base_abilities,   ((an_ability_st->cl73_adv.an_base_speed >> 1)& 0x1));
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_40GBASE_KR4f_SET(reg_cl73_base_abilities,   ((an_ability_st->cl73_adv.an_base_speed >> 2)& 0x1));
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_10GBASE_KRf_SET(reg_cl73_base_abilities,    ((an_ability_st->cl73_adv.an_base_speed >> 3)& 0x1));
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_10GBASE_KX4f_SET(reg_cl73_base_abilities,   ((an_ability_st->cl73_adv.an_base_speed >> 4)& 0x1));
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_1000BASE_KXf_SET(reg_cl73_base_abilities,   ((an_ability_st->cl73_adv.an_base_speed >> 5)& 0x1));

  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_CL73_PAUSEf_SET(reg_cl73_base_abilities, ( an_ability_st->cl73_adv.an_pause & 3));

  if (an_ability_st->cl73_adv.an_fec == TEMOD16_FEC_CL74_SUPRTD_REQSTD) {
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_FECf_SET(reg_cl73_base_abilities, 0x3);
  } else if (an_ability_st->cl73_adv.an_fec == TEMOD16_FEC_SUPRTD_NOT_REQSTD) {
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_FECf_SET(reg_cl73_base_abilities, 0x1);
  } else {
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_FECf_SET(reg_cl73_base_abilities, 0x0);
  }
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL73_BASE_ABIL0r(pc, reg_cl73_base_abilities));

  /* set cl73 nonce set 0xc340 */
  AN_X4_LOC_DEV_CL73_BASE_ABIL1r_CLR(reg_an_cl73_abl_1);
  AN_X4_LOC_DEV_CL73_BASE_ABIL1r_BASE_SELECTORf_SET(reg_an_cl73_abl_1, 0x1);
  PHYMOD_IF_ERR_RETURN ( MODIFY_AN_X4_LOC_DEV_CL73_BASE_ABIL1r(pc, reg_an_cl73_abl_1));

  /* CL72 Enable logic
         if cl72_en = 1, use the default HT settings, no overrides.
         If cl72_en=0, then override to disable cl72.
   */
  if((an_ability_st->cl73_adv.an_cl72 & 0x1) == 1) {
     temod16_override_set(pc, TEMOD16_OVERRIDE_CL72, 0x1);
  } else {
     temod16_override_set(pc, TEMOD16_OVERRIDE_CL72, 0x0);
  }

  return PHYMOD_E_NONE;
}
#endif /* _SDK_TEMOD16_*/


/**
@brief   To get autoneg control registers.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   an_control details
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Upper layer software calls this function to get autoneg
         info. 

<B>How to call:</B><br>
<p>Call directly </p>
*/
int temod16_autoneg_control_get(PHYMOD_ST* pc, temod16_an_control_t *an_control, int *an_complete) 
{
    AN_X4_LOC_DEV_CL37_BASE_ABILr_t reg_an_cl37_abl;
    AN_X4_ENSr_t reg_an_enb;
    AN_X4_CTLSr_t an_x4_abl_ctrl;
    AN_X4_AN_MISC_STSr_t an_misc_sts;

    TMOD_DBG_IN_FUNC_INFO(pc);
    READ_AN_X4_LOC_DEV_CL37_BASE_ABILr(pc, &reg_an_cl37_abl);
    /* Only one an_property_tpe is returned type, so the sgmii maybe replaced by the code later in this tier1 */
    if (AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(reg_an_cl37_abl) == 1) {
      an_control->an_property_type = TEMOD16_AN_PROPERTY_ENABLE_SGMII_MASTER_MODE;
    }

    READ_AN_X4_CTLSr(pc, &an_x4_abl_ctrl);
    an_control->pd_kx_en = AN_X4_CTLSr_PD_KX_ENf_GET(an_x4_abl_ctrl);
    an_control->pd_kx4_en = AN_X4_CTLSr_PD_KX4_ENf_GET(an_x4_abl_ctrl);

    /*Setting X4 abilities*/
    READ_AN_X4_ENSr(pc, &reg_an_enb);
    if (AN_X4_ENSr_CL37_BAM_ENABLEf_GET(reg_an_enb) == 1) {
       an_control->an_type = TEMOD16_AN_MODE_CL37BAM;
       an_control->enable = 1;
    } else if (AN_X4_ENSr_CL73_BAM_ENABLEf_GET(reg_an_enb) == 1) {
       an_control->an_type = TEMOD16_AN_MODE_CL73BAM;
       an_control->enable = 1;
    } else if (AN_X4_ENSr_CL73_HPAM_ENABLEf_GET(reg_an_enb) == 1) {
       an_control->an_type = TEMOD16_AN_MODE_HPAM;
       an_control->enable = 1;
    } else if ( AN_X4_ENSr_CL73_ENABLEf_GET(reg_an_enb) == 1) {
       an_control->an_type = TEMOD16_AN_MODE_CL73;
       an_control->enable = 1;
    } else if ( AN_X4_ENSr_CL37_SGMII_ENABLEf_GET(reg_an_enb) == 1) {
       an_control->an_type = TEMOD16_AN_MODE_SGMII;
       an_control->enable = 1;
    } else if ( AN_X4_ENSr_CL37_ENABLEf_GET(reg_an_enb) == 1) {
       an_control->an_type = TEMOD16_AN_MODE_CL37;;
       an_control->enable = 1;
    } else {
       an_control->an_type = TEMOD16_AN_MODE_NONE;
       an_control->enable = 0;
    }

    if (AN_X4_ENSr_HPAM_TO_CL73_AUTO_ENABLEf_GET(reg_an_enb) == 1) {
      an_control->an_property_type = TEMOD16_AN_PROPERTY_ENABLE_HPAM_TO_CL73_AUTO;
    } else if (AN_X4_ENSr_CL73_BAM_TO_HPAM_AUTO_ENABLEf_GET(reg_an_enb) == 1) {
      an_control->an_property_type = TEMOD16_AN_PROPERTY_ENABLE_CL73_BAM_TO_HPAM_AUTO;
    } else if (AN_X4_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_GET(reg_an_enb) == 1) {
      an_control->an_property_type = TEMOD16_AN_PROPERTY_ENABLE_SGMII_TO_CL37_AUTO;
    } else if (AN_X4_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_GET(reg_an_enb) == 1) {
      an_control->an_property_type = TEMOD16_AN_PROPERTY_ENABLE_CL37_BAM_to_SGMII_AUTO;
    }

    an_control->num_lane_adv = AN_X4_ENSr_NUM_ADVERTISED_LANESf_GET(reg_an_enb);

    /* an_complete status */
    AN_X4_AN_MISC_STSr_CLR(an_misc_sts);
    READ_AN_X4_AN_MISC_STSr(pc, &an_misc_sts);
    *an_complete = AN_X4_AN_MISC_STSr_AN_COMPLETEf_GET(an_misc_sts);

    return PHYMOD_E_NONE;
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
   
int temod16_autoneg_local_ability_get(PHYMOD_ST* pc, temod16_an_ability_t *an_ability_st)
{

  AN_X4_LOC_DEV_CL37_BASE_ABILr_t    reg_cl37_base_abilities;
  AN_X4_LOC_DEV_OVER1G_ABIL0r_t      reg_over1g_abilities;
  AN_X4_LOC_DEV_OVER1G_ABIL1r_t      reg_over1g_abilities1;
  AN_X4_LOC_DEV_CL73_BAM_ABILr_t     reg_cl73_bam_abilities;
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_t   reg_cl73_base_abilities;
  int an_fec;

  TMOD_DBG_IN_FUNC_INFO(pc);
  READ_AN_X4_LOC_DEV_CL37_BASE_ABILr(pc, &reg_cl37_base_abilities);
  an_ability_st->cl37_adv.an_pause = AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_GET(reg_cl37_base_abilities);
  an_ability_st->cl37_adv.cl37_sgmii_speed = AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(reg_cl37_base_abilities); 

  READ_AN_X4_LOC_DEV_OVER1G_ABIL0r(pc, &reg_over1g_abilities);
  an_ability_st->cl37_adv.an_bam_speed = AN_X4_LOC_DEV_OVER1G_ABIL0r_GET(reg_over1g_abilities) & 0x7ff;

  READ_AN_X4_LOC_DEV_OVER1G_ABIL1r(pc, &reg_over1g_abilities1);
  an_ability_st->cl37_adv.an_bam_speed1 = AN_X4_LOC_DEV_OVER1G_ABIL1r_GET(reg_over1g_abilities1) & 0xfff;

  if (AN_X4_LOC_DEV_OVER1G_ABIL1r_FECf_GET(reg_over1g_abilities1)) {
      an_ability_st->cl37_adv.an_fec = TEMOD16_FEC_CL74_SUPRTD_REQSTD;
  } else {
      an_ability_st->cl37_adv.an_fec = TEMOD16_FEC_SUPRTD_NOT_REQSTD;
  }
  an_ability_st->cl37_adv.an_cl72 = AN_X4_LOC_DEV_OVER1G_ABIL1r_CL72f_GET(reg_over1g_abilities1);
  an_ability_st->cl37_adv.an_hg2 = AN_X4_LOC_DEV_OVER1G_ABIL1r_HG2f_GET(reg_over1g_abilities1);
  
  READ_AN_X4_LOC_DEV_CL73_BAM_ABILr(pc, &reg_cl73_bam_abilities); 
  an_ability_st->cl73_adv.an_bam_speed = AN_X4_LOC_DEV_CL73_BAM_ABILr_GET(reg_cl73_bam_abilities) & 0x7;
  READ_AN_X4_LOC_DEV_CL73_BASE_ABIL0r(pc, &reg_cl73_base_abilities);
  an_ability_st->cl73_adv.an_base_speed = AN_X4_LOC_DEV_CL73_BASE_ABIL0r_GET(reg_cl73_base_abilities) & 0x3f;

  an_ability_st->cl73_adv.an_pause = AN_X4_LOC_DEV_CL73_BASE_ABIL0r_CL73_PAUSEf_GET(reg_cl73_base_abilities);
  an_fec = AN_X4_LOC_DEV_CL73_BASE_ABIL0r_FECf_GET(reg_cl73_base_abilities);
  if (an_fec == 0x3) {
      an_ability_st->cl73_adv.an_fec = TEMOD16_FEC_CL74_SUPRTD_REQSTD;
  } else if (an_fec == 0x1) {
      an_ability_st->cl73_adv.an_fec = TEMOD16_FEC_SUPRTD_NOT_REQSTD;
  } else {
      an_ability_st->cl73_adv.an_fec = TEMOD16_FEC_NOT_SUPRTD;
  }

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

#define AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_20G_CX4f_GET(r) ((((r).an_x4_lp_mp1024_up1[0]) >> 10) & 0x1)
#define AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_16G_X4f_GET(r) ((((r).an_x4_lp_mp1024_up1[0]) >> 9) & 0x1)
#define AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_15G_X4f_GET(r) ((((r).an_x4_lp_mp1024_up1[0]) >> 8) & 0x1)
#define AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_13G_X4f_GET(r) ((((r).an_x4_lp_mp1024_up1[0]) >> 7) & 0x1)
#define AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_12P5G_X4f_GET(r) ((((r).an_x4_lp_mp1024_up1[0]) >> 6) & 0x1)
#define AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_12G_X4f_GET(r) ((((r).an_x4_lp_mp1024_up1[0]) >> 5) & 0x1)
#define AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_10G_CX4f_GET(r) ((((r).an_x4_lp_mp1024_up1[0]) >> 4) & 0x1)
#define AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_10G_X4f_GET(r) ((((r).an_x4_lp_mp1024_up1[0]) >> 3) & 0x1)
#define AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_6G_X4f_GET(r) ((((r).an_x4_lp_mp1024_up1[0]) >> 2) & 0x1)
#define AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_5G_X4f_GET(r) ((((r).an_x4_lp_mp1024_up1[0]) >> 1) & 0x1)
#define AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_2P5G_X1f_GET(r) (((r).an_x4_lp_mp1024_up1[0]) & 0x1)

#define AN_X4_LP_BASE_PAGE2r_SPEED_100G_CR10f_GET(r) ((((r).an_x4_lp_base_page2[0]) >> 10) & 0x1)
#define AN_X4_LP_BASE_PAGE2r_SPEED_40G_CR4f_GET(r) ((((r).an_x4_lp_base_page2[0]) >> 9) & 0x1)
#define AN_X4_LP_BASE_PAGE2r_SPEED_40G_KR4f_GET(r) ((((r).an_x4_lp_base_page2[0]) >> 8) & 0x1)
#define AN_X4_LP_BASE_PAGE2r_SPEED_10G_KR_ABILITYf_GET(r) ((((r).an_x4_lp_base_page2[0]) >> 7) & 0x1)
#define AN_X4_LP_BASE_PAGE2r_SPEED_10G_KX4_ABILITYf_GET(r) ((((r).an_x4_lp_base_page2[0]) >> 6) & 0x1)
#define AN_X4_LP_BASE_PAGE2r_SPEED_1G_KX_ABILITYf_GET(r) ((((r).an_x4_lp_base_page2[0]) >> 5) & 0x1)

#define AN_X4_LP_MP1024_UP3r_SPEED_ABILITY_21G_X4f_GET(r) ((((r).an_x4_lp_mp1024_up3[0]) >> 9) & 0x1)
#define AN_X4_LP_MP1024_UP3r_SPEED_ABILITY_25P45G_X4f_GET(r) ((((r).an_x4_lp_mp1024_up3[0]) >> 8) & 0x1)
#define AN_X4_LP_MP1024_UP3r_SPEED_ABILITY_31P5G_X4f_GET(r) ((((r).an_x4_lp_mp1024_up3[0]) >> 7) & 0x1)
#define AN_X4_LP_MP1024_UP3r_SPEED_ABILITY_32P7Gf_GET(r) ((((r).an_x4_lp_mp1024_up3[0]) >> 6) & 0x1)
#define AN_X4_LP_MP1024_UP3r_SPEED_ABILITY_40G_X4f_GET(r) ((((r).an_x4_lp_mp1024_up3[0]) >> 5) & 0x1)

#define AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_1G_CX1f_GET(r) ((((r).an_x4_lp_mp1024_up4[0]) >> 9) & 0x1)
#define AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_10G_CX1f_GET(r) ((((r).an_x4_lp_mp1024_up4[0]) >> 8) & 0x1)
#define AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_15P75G_X2f_GET(r) ((((r).an_x4_lp_mp1024_up4[0]) >> 7) & 0x1)
#define AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_20G_CX2f_GET(r) ((((r).an_x4_lp_mp1024_up4[0]) >> 6) & 0x1)
#define AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_20G_X2f_GET(r) ((((r).an_x4_lp_mp1024_up4[0]) >> 5) & 0x1)
#define AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_12P7G_X2f_GET(r) ((((r).an_x4_lp_mp1024_up4[0]) >> 4) & 0x1)
#define AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_10P5G_X2f_GET(r) ((((r).an_x4_lp_mp1024_up4[0]) >> 3) & 0x1)
#define AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_10G_CX2f_GET(r) ((((r).an_x4_lp_mp1024_up4[0]) >> 2) & 0x1)
#define AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_10G_X2f_GET(r) ((((r).an_x4_lp_mp1024_up4[0]) >> 1) & 0x1)
#define AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_20G_X4f_GET(r) (((r).an_x4_lp_mp1024_up4[0]) & 0x1)

#define AN_X4_LP_MP5_UP3r_USER_DEFINED_CODE_19_11f_GET(r) (((r).an_x4_lp_mp5_up3[0]) & 0x1ff)
#define AN_X4_LP_MP5_UP4r_USER_DEFINED_CODE_10_0f_GET(r) (((r).an_x4_lp_mp5_up4[0]) & 0x7ff)
#define AN_X4_LP_MP1024_UP3r_HIGIG_II_ABILITYf_GET(r) (((r).an_x4_lp_mp1024_up3[0]) & 0x1)
#define AN_X4_LP_MP1024_UP3r_CL74_FEC_ABILITYf_GET(r) ((((r).an_x4_lp_mp1024_up3[0]) >> 1) & 0x1)
#define AN_X4_LP_MP1024_UP3r_CL72_TRAINING_ABILITYf_GET(r) ((((r).an_x4_lp_mp1024_up3[0]) >> 2) & 0x1)

int temod16_autoneg_remote_ability_get(PHYMOD_ST* pc, temod16_an_ability_t *an_ability_st)
{
  AN_X4_LP_MP1024_UP1r_t an_x4_up1;
  AN_X4_LP_MP1024_UP3r_t an_x4_up3;
  AN_X4_LP_MP1024_UP4r_t an_x4_up4;
  AN_X4_LP_BASE_PAGE1r_t an_x4_pg1;
  AN_X4_LP_BASE_PAGE2r_t an_x4_pg2;
  AN_X4_LP_BASE_PAGE3r_t an_x4_pg3;
  AN_X4_LP_MP5_UP3r_t an_x4_mp5_up3;
  AN_X4_LP_MP5_UP4r_t an_x4_mp5_up4;

  uint32_t data;
  uint32_t user_code;

  TMOD_DBG_IN_FUNC_INFO(pc);

  AN_X4_LP_MP1024_UP1r_CLR(an_x4_up1);
  AN_X4_LP_MP1024_UP3r_CLR(an_x4_up3);
  AN_X4_LP_MP1024_UP4r_CLR(an_x4_up4);
  AN_X4_LP_BASE_PAGE1r_CLR(an_x4_pg1);
  AN_X4_LP_BASE_PAGE2r_CLR(an_x4_pg2);
  AN_X4_LP_BASE_PAGE3r_CLR(an_x4_pg3);
  AN_X4_LP_MP5_UP3r_CLR(an_x4_mp5_up3);
  AN_X4_LP_MP5_UP4r_CLR(an_x4_mp5_up4);

  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_MP1024_UP1r(pc, &an_x4_up1));
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_MP1024_UP3r(pc, &an_x4_up3));
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_MP1024_UP4r(pc, &an_x4_up4));
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE_PAGE1r(pc, &an_x4_pg1));
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE_PAGE2r(pc, &an_x4_pg2));
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE_PAGE3r(pc, &an_x4_pg3));
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_MP5_UP3r(pc, &an_x4_mp5_up3));
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_MP5_UP4r(pc, &an_x4_mp5_up4));

  /* data:  get the   BAM_12GBASE_X4 and   BAM_12p5GBASE_X4 */
  data = AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_12G_X4f_GET(an_x4_up1);
  data <<= TEMOD16_CL37_BAM_12GBASE_X4;
  data |= (AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_12P5G_X4f_GET(an_x4_up1) << TEMOD16_CL37_BAM_12p5GBASE_X4);

  /* data: get BAM_10GBASE_X2_CX4, BAM_10GBASE_X2_CX4 and BAM_10p5GBASE_X2 */
  data |= (AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_10P5G_X2f_GET(an_x4_up4) << TEMOD16_CL37_BAM_BAM_10p5GBASE_X2);
  data |= (AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_10G_CX2f_GET(an_x4_up4) << TEMOD16_CL37_BAM_10GBASE_X2_CX4);
  data |= (AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_10G_X2f_GET(an_x4_up4) << TEMOD16_CL37_BAM_10GBASE_X2);

  /* data: get BAM_12p7GBASE_X2 */
  data |= (AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_12P7G_X2f_GET(an_x4_up4) << TEMOD16_CL37_BAM_12p7GBASE_X2);

  /* data: get BAM_10GBASE_X4_CX4,BAM_10GBASE_X4,BAM_6GBASE_X4,BAM_5GBASE_X4 and BAM_2P5GBASE_X1 */
  data |= AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_2P5G_X1f_GET(an_x4_up1) << TEMOD16_CL37_BAM_2p5GBASE_X;
  data |= AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_5G_X4f_GET(an_x4_up1) << TEMOD16_CL37_BAM_5GBASE_X4;
  data |= AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_6G_X4f_GET(an_x4_up1) << TEMOD16_CL37_BAM_6GBASE_X4;
  data |= AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_10G_X4f_GET(an_x4_up1) << TEMOD16_CL37_BAM_10GBASE_X4;
  data |= AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_10G_CX4f_GET(an_x4_up1) << TEMOD16_CL37_BAM_10GBASE_X4_CX4;
  an_ability_st->cl37_adv.an_bam_speed = data;

  /* data - get   BAM_13GBASE_X4, BAM_15GBASE_X4,   BAM_16GBASE_X4 and   BAM_20GBASE_CX4 */
  data = AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_13G_X4f_GET(an_x4_up1) << TEMOD16_CL37_BAM_13GBASE_X4;
  data |= (AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_15G_X4f_GET(an_x4_up1) << TEMOD16_CL37_BAM_15GBASE_X4);
  data |= (AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_16G_X4f_GET(an_x4_up1) << TEMOD16_CL37_BAM_16GBASE_X4);
  data |= (AN_X4_LP_MP1024_UP1r_SPEED_ABILITY_20G_CX4f_GET(an_x4_up1) << TEMOD16_CL37_BAM_20GBASE_X4_CX4);
  an_ability_st->cl37_adv.an_bam_speed1 = data;

  /* data -    BAM_15p75GBASE_X2 ,   BAM_20GBASE_X2*/
  data = AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_15P75G_X2f_GET(an_x4_up4) << TEMOD16_CL37_BAM_15p75GBASE_X2;
  data |= (AN_X4_LP_MP1024_UP4r_SPEED_ABILITY_20G_CX2f_GET(an_x4_up4) << TEMOD16_CL37_BAM_20GBASE_X2);
  an_ability_st->cl37_adv.an_bam_speed1 |= data;

  /* data -       -  21G-X4 ,   25.45G-X4,  31.5G-X4,   32.7G, 40G-X4 */
  data = AN_X4_LP_MP1024_UP3r_SPEED_ABILITY_21G_X4f_GET(an_x4_up3) << TEMOD16_CL37_BAM_21GBASE_X4;
  data |= (AN_X4_LP_MP1024_UP3r_SPEED_ABILITY_25P45G_X4f_GET(an_x4_up3) << TEMOD16_CL37_BAM_25p455GBASE_X4);
  data |= (AN_X4_LP_MP1024_UP3r_SPEED_ABILITY_31P5G_X4f_GET(an_x4_up3) << TEMOD16_CL37_BAM_31p5GBASE_X4);
  data |= (AN_X4_LP_MP1024_UP3r_SPEED_ABILITY_32P7Gf_GET(an_x4_up3) << TEMOD16_CL37_BAM_32p7GBASE_X4);
  data |= (AN_X4_LP_MP1024_UP3r_SPEED_ABILITY_40G_X4f_GET(an_x4_up3) << TEMOD16_CL37_BAM_40GBASE_X4);
  an_ability_st->cl37_adv.an_bam_speed1 |= data;

  an_ability_st->cl73_adv.an_bam_speed = 0x0;

  /* User-Defined Code[19:11] contains BAM code */
  user_code = AN_X4_LP_MP5_UP3r_USER_DEFINED_CODE_19_11f_GET(an_x4_mp5_up3) << 11;
  /* User-Defined Code[10:0] contains BAM abilities */
  data = AN_X4_LP_MP5_UP4r_USER_DEFINED_CODE_10_0f_GET(an_x4_mp5_up4);

  if (data & (1<<1)) {
     an_ability_st->cl73_adv.an_bam_speed |= (1 << TEMOD16_CL73_BAM_20GBASE_KR2);
  } else {
     user_code |= data;
     if(user_code == 0xabe20){
        an_ability_st->cl73_adv.an_bam_speed |= (1 << TEMOD16_CL73_BAM_20GBASE_KR2);
     }
  }
  if (data & (1<<0)){
      an_ability_st->cl73_adv.an_bam_speed |= (1<< TEMOD16_CL73_BAM_20GBASE_CR2);
  }

  data = AN_X4_LP_BASE_PAGE2r_SPEED_100G_CR10f_GET(an_x4_pg2) << TEMOD16_CL73_100GBASE_CR10;
  data |= AN_X4_LP_BASE_PAGE2r_SPEED_40G_CR4f_GET(an_x4_pg2) << TEMOD16_CL73_40GBASE_CR4;
  data |= AN_X4_LP_BASE_PAGE2r_SPEED_40G_KR4f_GET(an_x4_pg2) << TEMOD16_CL73_40GBASE_KR4;
  data |= AN_X4_LP_BASE_PAGE2r_SPEED_10G_KR_ABILITYf_GET(an_x4_pg2) << TEMOD16_CL73_10GBASE_KR;
  data |= AN_X4_LP_BASE_PAGE2r_SPEED_10G_KX4_ABILITYf_GET(an_x4_pg2) << TEMOD16_CL73_10GBASE_KX4;
  data |= AN_X4_LP_BASE_PAGE2r_SPEED_1G_KX_ABILITYf_GET(an_x4_pg2) << TEMOD16_CL73_1000BASE_KX;
  an_ability_st->cl73_adv.an_base_speed |= data;

   /* check the speed ability and pass it as well */
  an_ability_st->cl37_adv.cl37_sgmii_speed = (AN_X4_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(an_x4_pg1) >> 10) & 0x3;
  an_ability_st->cl37_adv.an_pause = (AN_X4_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(an_x4_pg1) >> 7) & 0x3;
  an_ability_st->cl37_adv.an_hg2 = AN_X4_LP_MP1024_UP3r_HIGIG_II_ABILITYf_GET(an_x4_up3);
  an_ability_st->cl37_adv.an_fec = AN_X4_LP_MP1024_UP3r_CL74_FEC_ABILITYf_GET(an_x4_up3);
  an_ability_st->cl37_adv.an_cl72 = AN_X4_LP_MP1024_UP3r_CL72_TRAINING_ABILITYf_GET(an_x4_up3);

  an_ability_st->cl73_adv.an_pause = (AN_X4_LP_BASE_PAGE1r_LP_BASE_PAGE1_PAGE_DATAf_GET(an_x4_pg1) >> 10) & 0x3;
  an_ability_st->cl73_adv.an_fec = (AN_X4_LP_BASE_PAGE3r_LP_BASE_PAGE3_PAGE_DATAf_GET(an_x4_pg3) >> 14) & 0x3;

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

\li TEMOD16_AN_NONE   (disable AN)
\li TEMOD16_CL73
\li TEMOD16_CL73_BAM
\li TEMOD16_HPAM
*/

#ifdef _DV_TB_
int temod16_autoneg_control(PHYMOD_ST* pc)
{
  uint16_t data ;
  uint16_t an_setup_enable, num_advertised_lanes, cl37_bam_enable, cl73_bam_enable  ;
  uint16_t cl73_hpam_enable, cl73_enable, cl37_sgmii_enable, cl37_enable;
  uint16_t cl73_nonce_match_over, cl73_nonce_match_val  ,cl73_transmit_nonce;
  uint16_t base_selector, cl73_bam_code  ;
  uint16_t hppam_20gkr2, next_page  ;
  uint16_t cl37_next_page,  cl37_full_duplex; 
  uint16_t sgmii_full_duplex ;
  uint16_t cl37_bam_code, over1g_ability, over1g_page_count;
  uint16_t cl37_restart, cl73_restart ;
  int    no_an ;
AN_X4_LOC_DEV_CL37_BAM_ABILr_t reg_an_cl37_bam_abilities;
/*AN_X4_LOC_DEV_CL73_BASE_ABIL0r_t reg_an_cl73_abl_0; */
AN_X4_LOC_DEV_CL73_BASE_ABIL1r_t reg_an_cl73_abl_1;
/* AN_X4_LOC_DEV_CL73_BAM_ABILr_t reg_an_cl73_abl; */
/* UC_COMMAND3r_t reg_com3; */
AN_X4_ENSr_t reg_an_enb;
  TMOD_DBG_IN_FUNC_INFO(pc);

/* TSCE_COMMANDr_t reg_com;
*/
 
  no_an                         = 0 ; 

  an_setup_enable               = 0x1;
  num_advertised_lanes          = 0x2;   
  cl37_bam_enable               = 0x0;
  cl73_bam_enable               = 0x0;
  cl73_hpam_enable              = 0x0;
  cl73_enable                   = 0x0;
  cl37_sgmii_enable             = 0x0;
  cl37_enable                   = 0x0;
  cl73_nonce_match_over         = 0x1;
  cl73_transmit_nonce           = 0x0;
  cl73_nonce_match_val          = 0x0;
  base_selector                 = 0x1;
  cl73_bam_code                 = 0x0;
  hppam_20gkr2                  = 0x0;
  next_page                     = 0x0;
  cl37_next_page                = 0x0;
  cl37_full_duplex              = 0x1; 
  sgmii_full_duplex             = 0x1;
  cl37_bam_code                 = 0x0;
  over1g_ability                = 0x0;
  over1g_page_count             = 0x0;
  cl37_restart                  = 0x0;
  cl73_restart                  = 0x0;

  num_advertised_lanes = pc->no_of_lanes;

  /*if (pc-> an_type == TSCMOD_CL73) {
    cl73_restart                = 0x1;
    cl73_enable                 = 0x1;
    cl73_nonce_match_over       = 0x1;
    cl73_nonce_match_val        = 0x0;
    base_selector               = 0x1;
  } else if (pc-> an_type == TSCMOD_CL37) {
    cl37_restart                = 0x1;
    cl37_enable                 = 0x1;
    cl37_bam_code               = 0x0;
    over1g_ability              = 0x0;
    over1g_page_count           = 0x0;
    sgmii_full_duplex           = 0x0;
  } else if (pc-> an_type == TSCMOD_CL37_BAM) {
    cl37_restart                = 0x1;
    cl37_enable                 = 0x1;
    cl37_bam_enable             = 0x1;
    cl37_bam_code               = 0x1;
    over1g_ability              = 0x1;
    over1g_page_count           = 0x1; */  /* PHY-863: =1: 5 pages; =0: 3 pages; WC uses 3 pages */
    /*sgmii_full_duplex           = 0x0;
    cl37_next_page              = 0x1;
  } else if (pc-> an_type == TSCMOD_CL37_SGMII) {
    cl37_restart                = 0x1;
    cl37_sgmii_enable           = 0x1;
    cl37_enable                 = 0x1;
    cl37_bam_code               = 0x0;
    over1g_ability              = 0x0;
    over1g_page_count           = 0x0;
  } else if (pc-> an_type == TSCMOD_CL73_BAM) {
    cl73_restart                = 0x1;
    cl73_enable                 = 0x1; 
    cl73_bam_enable             = 0x1;
    cl73_bam_code               = 0x3;
    next_page                   = 0x1;
    cl73_nonce_match_over       = 0x1;
    cl73_nonce_match_val        = 0x0;
    base_selector               = 0x1;
  } else if (pc-> an_type == TSCMOD_HPAM) {
    cl73_restart                = 0x1;
    cl73_enable                 = 0x1; 
    cl73_hpam_enable            = 0x1;
    cl73_bam_code               = 0x3;
    hppam_20gkr2                = 0x1;
    next_page                   = 0x1;
    cl73_nonce_match_over       = 0x1;
    cl73_nonce_match_val        = 0x0;
    base_selector               = 0x1;
  } else {
     no_an = 1 ; 
     printf("%-22s ERROR: u=%0d p=%0d Autoneg mode not defined\n", __func__, pc->unit, pc->port);
  }*/
#if defined (_DV_TB_)
  if (pc->sc_mode == TEMOD16_SC_MODE_AN_CL37) {
    cl37_restart                = 0x0;
    cl37_enable                 = pc->cl37_an_en;
    cl37_bam_enable             = pc->cl37_bam_en;
    cl37_sgmii_enable           = pc->cl37_sgmii_en;
    cl37_bam_code               = pc->cl37_bam_code & 0x1ff;
    over1g_ability              = pc->cl37_bam_ovr1g_en & 1;
    over1g_page_count           = pc->cl37_bam_ovr1g_pgcnt & 3;   /* PHY-863: =1: 5 pages; =0: 3 pages; WC uses 3 pages */
    sgmii_full_duplex           = pc->cl37_sgmii_duplex & 1;
    cl37_next_page              = pc->cl37_an_np & 1;
  } else if (pc->sc_mode == TEMOD16_SC_MODE_AN_CL73) {
    cl73_restart                = 0x0;
    cl73_enable                 = pc->cl73_an_en; 
    cl73_bam_enable             = pc->cl73_bam_en;
    cl73_hpam_enable            = pc->cl73_hpam_en;
    cl73_bam_code               = pc->cl73_bam_code & 0x1ff;
    next_page                   = pc->cl73_nxt_page & 1;
    cl73_nonce_match_over       = pc->cl73_nonce_match_over & 1;
    cl73_transmit_nonce         = pc->transmit_nonce;
    cl73_nonce_match_val        = pc->cl73_nonce_match_val & 1;
    base_selector               = pc->base_selector & 0x1f;
  } else {
     no_an = 1 ; 
     printf("%-22s ERROR: u=%0d p=%0d Autoneg mode not defined\n", __func__, pc->unit, pc->port);
     return PHYMOD_E_FAIL;
  }
#endif /*_DV_TB_*/

 
  /* cl37 bam abilities 0xc182 */
  AN_X4_LOC_DEV_CL37_BAM_ABILr_CLR(reg_an_cl37_bam_abilities);
  AN_X4_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_an_cl37_bam_abilities, cl37_bam_code);
  AN_X4_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_SET(reg_an_cl37_bam_abilities, over1g_ability);
  AN_X4_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_SET(reg_an_cl37_bam_abilities, over1g_page_count);

  PHYMOD_IF_ERR_RETURN 
         (MODIFY_AN_X4_LOC_DEV_CL37_BAM_ABILr(pc, reg_an_cl37_bam_abilities));

  /* set next page  bit set 0xc186 */
  /*
  TSCE_READ_AN_LOCAL_DEVICE_CL73_BASE_ABILITIES_0r(pc, &reg_an_cl73_abl_0);
  TSCE_AN_LOCAL_DEVICE_CL73_BASE_ABILITIES_0r_NEXT_PAGEf_SET(reg_an_cl73_abl_0, next_page);
  PHYMOD_IF_ERR_RETURN ( TSCE_WRITE_AN_LOCAL_DEVICE_CL73_BASE_ABILITIES_0r(pc, reg_an_cl73_abl_0));
 */

  /* set cl73_bam_code set 0xc187 */
  /*

  TSCE_READ_AN_LOCAL_DEVICE_CL73_BAM_ABILITIESr(pc, &reg_an_cl73_abl);
  TSCE_AN_LOCAL_DEVICE_CL73_BAM_ABILITIESr_CL73_BAM_CODEf_SET(reg_an_cl73_abl, cl73_bam_code);
  PHYMOD_IF_ERR_RETURN ( TSCE_WRITE_AN_LOCAL_DEVICE_CL73_BAM_ABILITIESr(pc, reg_an_cl73_abl));
  */
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
   AN_X4_LOC_DEV_CL73_BASE_ABIL1r_CLR(reg_an_cl73_abl_1);
   AN_X4_LOC_DEV_CL73_BASE_ABIL1r_CL73_NONCE_MATCH_OVERf_SET(reg_an_cl73_abl_1, cl73_nonce_match_over);
   AN_X4_LOC_DEV_CL73_BASE_ABIL1r_CL73_NONCE_MATCH_VALf_SET(reg_an_cl73_abl_1, cl73_nonce_match_val);
   PHYMOD_IF_ERR_RETURN ( MODIFY_AN_X4_LOC_DEV_CL73_BASE_ABIL1r(pc, reg_an_cl73_abl_1)); 
  
   /* set cl73 base_selector to 802.3 set 0xc185 */
   AN_X4_LOC_DEV_CL73_BASE_ABIL1r_CLR(reg_an_cl73_abl_1); 
   AN_X4_LOC_DEV_CL73_BASE_ABIL1r_BASE_SELECTORf_SET(reg_an_cl73_abl_1, base_selector);
   PHYMOD_IF_ERR_RETURN ( MODIFY_AN_X4_LOC_DEV_CL73_BASE_ABIL1r(pc, reg_an_cl73_abl_1)); 
  
   AN_X4_LOC_DEV_CL73_BASE_ABIL1r_CLR(reg_an_cl73_abl_1); 
   AN_X4_LOC_DEV_CL73_BASE_ABIL1r_TRANSMIT_NONCEf_SET(reg_an_cl73_abl_1, ( cl73_transmit_nonce & 0x1f));
   PHYMOD_IF_ERR_RETURN ( MODIFY_AN_X4_LOC_DEV_CL73_BASE_ABIL1r(pc, reg_an_cl73_abl_1));

  /* 0x924a for PHY_892 TBD*/ 
  /*if ((no_an==0)&&(pc->port_type==TSCMOD_SINGLE_PORT)) {*/
     data = 0x1a0a ; /* 100ms TX_RESET_TIMER_PERIOD */
     data = 0xd05 ;  /* 50ms */
  /*} else {
     data = 0x5 ;
  }
    PHYMOD_IF_ERR_RETURN 
    TSC_REG_WRITE(pc->unit, pc, 0x00, 0x0000924a, data)) ;*/

    /*Disabling AN if already enabled required for dynamic speed change tests*/

    AN_X4_ENSr_CLR(reg_an_enb);

    AN_X4_ENSr_CL37_ENABLEf_SET(reg_an_enb,0);
    AN_X4_ENSr_CL73_ENABLEf_SET(reg_an_enb,0);
    PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ENSr(pc, reg_an_enb)); 

    /*Setting X4 abilities*/
    AN_X4_ENSr_CLR(reg_an_enb);
    AN_X4_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb,cl37_bam_enable);
    AN_X4_ENSr_CL73_BAM_ENABLEf_SET(reg_an_enb,cl73_bam_enable);
    AN_X4_ENSr_CL73_HPAM_ENABLEf_SET(reg_an_enb,cl73_hpam_enable);
    AN_X4_ENSr_CL73_ENABLEf_SET(reg_an_enb,cl73_enable);
    AN_X4_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb,cl37_sgmii_enable);
    AN_X4_ENSr_CL37_ENABLEf_SET(reg_an_enb,cl37_enable);
    AN_X4_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb,cl37_restart);
    AN_X4_ENSr_CL73_AN_RESTARTf_SET(reg_an_enb,cl73_restart);
    AN_X4_ENSr_HPAM_TO_CL73_AUTO_ENABLEf_SET(reg_an_enb,pc->hpam_to_cl73_auto_enable);
    AN_X4_ENSr_CL73_BAM_TO_HPAM_AUTO_ENABLEf_SET(reg_an_enb,pc->cl73_bam_to_hpam_auto_enable);
    AN_X4_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb,pc->sgmii_to_cl37_auto_enable);
    AN_X4_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb,pc->cl37_bam_to_sgmii_auto_enable);
    AN_X4_ENSr_NUM_ADVERTISED_LANESf_SET(reg_an_enb,num_advertised_lanes);

  PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ENSr(pc, reg_an_enb)); 

  return PHYMOD_E_NONE;
} /* temod16_autoneg_control */
#endif
#ifdef _SDK_TEMOD16_
int temod16_autoneg_control(PHYMOD_ST* pc, temod16_an_control_t *an_control)
{
  uint16_t num_advertised_lanes, cl37_bam_enable, cl73_bam_enable  ;
  uint16_t cl73_hpam_enable, cl73_enable, cl37_sgmii_enable, cl37_enable;
  uint16_t cl73_bam_code;
  uint16_t cl73_next_page;
  uint16_t cl37_next_page; 
  uint16_t cl37_bam_code;
  uint16_t cl37_restart, cl73_restart ;
  phymod_access_t pa_copy;
  int i=0, start_lane=0, num_lane=0, sgmii_to_cl37=0 ;
  uint32_t  enable=0;

  AN_X4_LOC_DEV_CL37_BASE_ABILr_t reg_an_cl37_abl;
  AN_X4_LOC_DEV_CL37_BAM_ABILr_t reg_an_cl37_bam_abilities;
  AN_X4_LOC_DEV_CL73_BASE_ABIL0r_t reg_an_cl73_abl_0;
  AN_X4_ENSr_t reg_an_enb;
  AN_X4_CTLSr_t an_x4_abl_ctrl;

  AN_X1_CL37_ERRr_t      reg_cl37_err;
  AN_X1_CL73_ERRr_t      reg_cl73_err;
  AN_X4_LOC_DEV_CL73_BAM_ABILr_t     reg_cl73_bam_abilities;
  TMOD_DBG_IN_FUNC_INFO(pc);

  PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
  PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
  pa_copy.lane_mask = 0;
  num_advertised_lanes          = an_control->num_lane_adv;

  switch (num_advertised_lanes) {
    case 3: num_lane = 4; break;
    case 2: num_lane = 4; break;
    case 1: num_lane = 2; break;
    case 0: num_lane = 1; break;
    default: num_lane = 1;
  }
  for (i=0; i<num_lane; i++) {
     pa_copy.lane_mask = pa_copy.lane_mask | 0x1 << (start_lane+i);
  }

  cl37_bam_enable               = 0x0;
  cl73_bam_enable               = 0x0;
  cl73_hpam_enable              = 0x0;
  cl73_enable                   = 0x0;
  cl37_sgmii_enable             = 0x0;
  cl37_enable                   = 0x0;
  cl73_next_page                = 0x0;
  cl37_next_page                = 0x0;
  cl37_bam_code                 = 0x0;
  cl37_restart                  = 0x0;
  cl73_restart                  = 0x0;
  cl73_bam_code                 = 0x0;

  switch (an_control->an_type) {
      case TEMOD16_CL73:  
      {  
        cl73_restart                = an_control->enable;
        cl73_enable                 = an_control->enable; 
        break;
      }
      case TEMOD16_CL37:
        cl37_restart                = an_control->enable;
        cl37_enable                 = an_control->enable;
        cl37_bam_code               = 0x0;
        break;
      case TEMOD16_CL37_BAM:
        cl37_restart                = an_control->enable;
        cl37_enable                 = an_control->enable;
        cl37_bam_enable             = an_control->enable;
        cl37_bam_code               = 0x1;
        cl37_next_page              = 0x1;
        break;
      case TEMOD16_CL37_SGMII:
        cl37_restart                = an_control->enable;
        cl37_sgmii_enable           = an_control->enable;
        cl37_enable                 = an_control->enable;
        cl37_bam_code               = 0x0;
        break;
      case TEMOD16_CL73_BAM:
        cl73_restart                = an_control->enable;
        cl73_enable                 = an_control->enable; 
        cl73_bam_enable             = an_control->enable;
        cl73_next_page              = 0x1;
        cl73_bam_code               = 0x3;
        break;
      case TEMOD16_HPAM:
        cl73_restart                = an_control->enable;
        cl73_enable                 = an_control->enable; 
        cl73_hpam_enable            = an_control->enable;
        cl73_next_page              = 0x1;
        break;
      case TEMOD16_AN_MODE_CL37_SGMII:
        cl37_restart                = an_control->enable;
        cl37_sgmii_enable           = an_control->enable;
        cl37_enable                 = an_control->enable;
        if(cl37_enable) {
            sgmii_to_cl37           = 1;
        }
        cl37_bam_code               = 0x0;
        break;
     default:
        return PHYMOD_E_FAIL;
        break;
  }

  /* RESET Speed Change bit */
  if(an_control->enable){
      temod16_disable_set(pc);
  }
    
     /* AN TIMERS */
     /*0x9252 AN_X1_TIMERS_cl37_error */
     AN_X1_CL37_ERRr_CLR(reg_cl37_err);
     if (an_control->an_type == TEMOD16_AN_MODE_CL37) {
       AN_X1_CL37_ERRr_SET(reg_cl37_err, 0);
     } else if (an_control->an_type == TEMOD16_AN_MODE_CL37BAM) {
       AN_X1_CL37_ERRr_SET(reg_cl37_err, 0x55d);
     }
     PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL37_ERRr(pc, reg_cl37_err));

     /*0x9254 AN_X1_TIMERS_cl73_error*/ 
      AN_X1_CL73_ERRr_CLR(reg_cl73_err);
      if (an_control->an_type == TEMOD16_AN_MODE_CL73) {
         AN_X1_CL73_ERRr_SET(reg_cl73_err, 0);
      } else if (an_control->an_type == TEMOD16_AN_MODE_HPAM) {
         AN_X1_CL73_ERRr_SET(reg_cl73_err, 0xfff0);
      } else if (an_control->an_type == TEMOD16_AN_MODE_CL73BAM) {
         AN_X1_CL73_ERRr_SET(reg_cl73_err, 0x1a10);
      }
      PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_ERRr(pc, reg_cl73_err));


      /* an37 an-restart_reset_disable setting */
      /* DOne  in autoneg_set, so commenting here */
      /*
      AN_X4_LOC_DEV_CL37_BASE_ABILr_CLR(reg_an_cl37_abl); 
      AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_SET(reg_an_cl37_abl, cl37_an_restart_reset_disable);
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL37_BASE_ABILr(pc, reg_an_cl37_abl));
      */

      /* cl37 bam abilities 0xc182 */
      AN_X4_LOC_DEV_CL37_BAM_ABILr_CLR(reg_an_cl37_bam_abilities);
      AN_X4_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_SET(reg_an_cl37_bam_abilities, cl37_bam_code);
      PHYMOD_IF_ERR_RETURN
           (MODIFY_AN_X4_LOC_DEV_CL37_BAM_ABILr(pc, reg_an_cl37_bam_abilities));
      
      /*need to set the next page for cl37bam */
      AN_X4_LOC_DEV_CL37_BASE_ABILr_CLR(reg_an_cl37_abl);
      if (an_control->an_property_type & TEMOD16_AN_PROPERTY_ENABLE_SGMII_MASTER_MODE )
         AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_an_cl37_abl, 0x1);
      else
         AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_SET(reg_an_cl37_abl, 0x0);
      AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_SET(reg_an_cl37_abl, cl37_next_page);
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL37_BASE_ABILr(pc, reg_an_cl37_abl));

      /*need to set cl73 BAM next page probably*/
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_CLR(reg_an_cl73_abl_0);
      AN_X4_LOC_DEV_CL73_BASE_ABIL0r_NEXT_PAGEf_SET(reg_an_cl73_abl_0, cl73_next_page & 1); 
      PHYMOD_IF_ERR_RETURN ( MODIFY_AN_X4_LOC_DEV_CL73_BASE_ABIL0r(pc, reg_an_cl73_abl_0));
      PHYMOD_IF_ERR_RETURN( READ_AN_X4_LOC_DEV_CL73_BAM_ABILr(pc, &reg_cl73_bam_abilities));
      AN_X4_LOC_DEV_CL73_BAM_ABILr_CL73_BAM_CODEf_SET(reg_cl73_bam_abilities, (cl73_bam_code & 0x1ff));
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LOC_DEV_CL73_BAM_ABILr(pc, reg_cl73_bam_abilities));


      AN_X4_CTLSr_CLR(an_x4_abl_ctrl);
      AN_X4_CTLSr_PD_KX_ENf_SET(an_x4_abl_ctrl, an_control->pd_kx_en & 0x1);
      AN_X4_CTLSr_PD_KX4_ENf_SET(an_x4_abl_ctrl, an_control->pd_kx4_en & 0x1);
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_CTLSr(pc, an_x4_abl_ctrl));

      AN_X4_ENSr_CLR(reg_an_enb);

      AN_X4_ENSr_CL37_ENABLEf_SET(reg_an_enb,0);
      AN_X4_ENSr_CL73_ENABLEf_SET(reg_an_enb,0);
      AN_X4_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb,0);
      AN_X4_ENSr_CL73_AN_RESTARTf_SET(reg_an_enb,0);
      PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ENSr(pc, reg_an_enb));

      /*Setting X4 abilities*/
      AN_X4_ENSr_CLR(reg_an_enb);
      AN_X4_ENSr_CL37_BAM_ENABLEf_SET(reg_an_enb,cl37_bam_enable);
      AN_X4_ENSr_CL73_BAM_ENABLEf_SET(reg_an_enb,cl73_bam_enable);
      AN_X4_ENSr_CL73_HPAM_ENABLEf_SET(reg_an_enb,cl73_hpam_enable);
      AN_X4_ENSr_CL73_ENABLEf_SET(reg_an_enb,cl73_enable);
      AN_X4_ENSr_CL37_SGMII_ENABLEf_SET(reg_an_enb,cl37_sgmii_enable);
      AN_X4_ENSr_CL37_ENABLEf_SET(reg_an_enb,cl37_enable);
      AN_X4_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb,cl37_restart);
      AN_X4_ENSr_CL73_AN_RESTARTf_SET(reg_an_enb,cl73_restart);
      if (an_control->an_property_type & TEMOD16_AN_PROPERTY_ENABLE_HPAM_TO_CL73_AUTO )
        AN_X4_ENSr_HPAM_TO_CL73_AUTO_ENABLEf_SET(reg_an_enb,0x1);
      else
        AN_X4_ENSr_HPAM_TO_CL73_AUTO_ENABLEf_SET(reg_an_enb,0x0);
      if (an_control->an_property_type & TEMOD16_AN_PROPERTY_ENABLE_CL73_BAM_TO_HPAM_AUTO )
        AN_X4_ENSr_CL73_BAM_TO_HPAM_AUTO_ENABLEf_SET(reg_an_enb,0x1);
      else
        AN_X4_ENSr_CL73_BAM_TO_HPAM_AUTO_ENABLEf_SET(reg_an_enb,0x0);
      if ((an_control->an_property_type & TEMOD16_AN_PROPERTY_ENABLE_SGMII_TO_CL37_AUTO) || sgmii_to_cl37)
        AN_X4_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb,0x1);
      else
        AN_X4_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_SET(reg_an_enb,0x0);
      if (an_control->an_property_type & TEMOD16_AN_PROPERTY_ENABLE_CL37_BAM_to_SGMII_AUTO )
        AN_X4_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb,0x1);
      else
        AN_X4_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_SET(reg_an_enb,0x0);
      AN_X4_ENSr_NUM_ADVERTISED_LANESf_SET(reg_an_enb,num_advertised_lanes);

      PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ENSr(pc, reg_an_enb));

      /*next check if an enabled, restart bit needs to be cleared */
      if (an_control->enable) {
          AN_X4_ENSr_CLR(reg_an_enb);
          AN_X4_ENSr_CL37_AN_RESTARTf_SET(reg_an_enb,0);
          AN_X4_ENSr_CL73_AN_RESTARTf_SET(reg_an_enb,0);
          PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_ENSr(pc, reg_an_enb));
      }

      /* need to clean up the FIFO */
      if (an_control->enable == 0) {
          pa_copy.lane_mask = 1 << start_lane ;
          temod16_disable_get(&pa_copy,&enable);
          if (enable == 0x1) {
              PHYMOD_IF_ERR_RETURN (temod16_trigger_speed_change(&pa_copy));
          }
      }
  return PHYMOD_E_NONE;
} /* temod16_autoneg_control */
#endif /* _SDK_TEMOD16_*/

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
int temod16_afe_speed_up_dsc_vga(PHYMOD_ST* pc, int pmd_touched)   /* AFE_SPEED_UP_DSC_VGA */
{
PLL_CALCTL0r_t reg_pll_calctl0;
PLL_CALCTL1r_t reg_pll_calctl1;
PLL_CALCTL2r_t reg_pll_calctl2;
UC_CTL0r_t     reg_uc_ctl0;
/*
PLL_CAL_CTL0r_t    reg_ctrl0;
PLL_CAL_CTL1r_t    reg_ctrl1;
PLL_CAL_CTL2r_t    reg_ctrl2;
PLL_CAL_CTL5r_t    reg_ctrl5;
PLL_CAL_CTL6r_t    reg_ctrl6;
*/
DSC_SM_CTL4r_t    reg_ctrl4;

TMOD_DBG_IN_FUNC_INFO(pc);

PLL_CALCTL0r_CLR(reg_pll_calctl0);
PLL_CALCTL1r_CLR(reg_pll_calctl1);
PLL_CALCTL2r_CLR(reg_pll_calctl2);
UC_CTL0r_CLR(reg_uc_ctl0);
/*
PLL_CAL_CTL5r_CLR(reg_ctrl5);
PLL_CAL_CTL6r_CLR(reg_ctrl6);
PLL_CAL_CTL0r_CLR(reg_ctrl0);
PLL_CAL_CTL1r_CLR(reg_ctrl1);
PLL_CAL_CTL2r_CLR(reg_ctrl2);
*/
DSC_SM_CTL4r_CLR(reg_ctrl4);

  if (pmd_touched == 0) {
/* PLL speed_up begin */
  /* write ( "calib_step_time",                          0x52);*/
  PLL_CALCTL0r_CALIB_STEP_TIMEf_SET(reg_pll_calctl0, 0x52);
  PHYMOD_IF_ERR_RETURN (MODIFY_PLL_CALCTL0r(pc, reg_pll_calctl0));

  /* write ( "freqdet_time",                             0x28F);*/
  PLL_CALCTL1r_FREQDET_TIMEf_SET(reg_pll_calctl1, 0x28f);
  PHYMOD_IF_ERR_RETURN (MODIFY_PLL_CALCTL1r(pc, reg_pll_calctl1));

  /* write ( "calib_cap_charge_time",                    0x28);*/
  PLL_CALCTL2r_CALIB_CAP_CHARGE_TIMEf_SET(reg_pll_calctl2, 0x28);
  PHYMOD_IF_ERR_RETURN (MODIFY_PLL_CALCTL2r(pc, reg_pll_calctl2));
  }
  /* PLL speed_up end */

  /* write ( "tx_disable_timer_ctrl",                       0);*/
  UC_CTL0r_TX_DISABLE_TIMER_CTRLf_SET(reg_uc_ctl0, 0x0);
  PHYMOD_IF_ERR_RETURN (MODIFY_UC_CTL0r(pc, reg_uc_ctl0));

  /* DSC speed_up begin */

  DSC_SM_CTL4r_HW_TUNE_TIMEOUTf_SET(reg_ctrl4, 0x1);
  DSC_SM_CTL4r_CDR_SETTLE_TIMEOUTf_SET(reg_ctrl4, 0x1);
  DSC_SM_CTL4r_ACQ_CDR_TIMEOUTf_SET(reg_ctrl4, 0x4);

  PHYMOD_IF_ERR_RETURN (MODIFY_DSC_SM_CTL4r(pc, reg_ctrl4));

  /* DSC speed_up end */
  return PHYMOD_E_NONE;
}
#endif /* _DV_TB_ */

static int _temod16_getRevDetails(PHYMOD_ST* pc)
{
  MAIN0_SERDESIDr_t reg_serdesid;

  MAIN0_SERDESIDr_CLR(reg_serdesid);
  PHYMOD_IF_ERR_RETURN(READ_MAIN0_SERDESIDr(pc,&reg_serdesid));
  return (MAIN0_SERDESIDr_GET(reg_serdesid));
}

/**
@brief   Init routine sets various operating modes of TSCE.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   pmd_touched  
@param   spd_intf  #temod16_spd_intfc_type
@param   pll_mode # to override the pll div 
@returns PHYMOD_E_NONE if successful. PHYMOD_E_FAIL else.
@details

This function is called once per TSCE. It cannot be called multiple times
and should be called immediately after reset. Elements of #PHYMOD_ST should be
initialized to required values prior to calling this function. The following
sub-configurations are performed here.

\li Set pll divider for VCO setting in PMD. pll divider is calculated from max_speed. 
*/

int temod16_set_pll_mode(PHYMOD_ST* pc, int pmd_touched, temod16_spd_intfc_type spd_intf, int pll_mode)           /* SET_PLL_MODE */
{
  PLL_CTL8r_t    reg_ctl8;
  PLL_CTL9r_t    reg_ctl9;
  int speed;
  int pll_ndiv;
  int pll_mode_lcl;

  TMOD_DBG_IN_FUNC_INFO(pc);
  PLL_CTL8r_CLR(reg_ctl8);
  PLL_CTL9r_CLR(reg_ctl9);

  if (pmd_touched == 0) {
    temod16_get_mapped_speed(spd_intf, &speed);
    /*Support Override PLL DIV */
    if (pll_mode & 0x80000000) {
      pll_mode_lcl = pll_mode & 0x3ff;
      temod16_get_pll_ndiv(pll_mode_lcl, &pll_ndiv);
      PLL_CTL8r_I_NDIV_INTf_SET(reg_ctl8, (pll_ndiv) & 0x0000ffff);
    } else {
       temod16_get_pll_ndiv(merlin16_sc_pmd_entry[speed].pll_mode, &pll_ndiv);
       PLL_CTL8r_I_NDIV_INTf_SET(reg_ctl8, pll_ndiv); 
    }

#ifdef _DV_TB_
    if ( pc->verbosity) {
      printf("%-22s: plldiv:%d data:%x\n", __func__, pc->plldiv, PLL_CTL8r_I_NDIV_INTf_GET(reg_ctl8));
      printf("%-22s: main0_setup=%x\n", __func__, PLL_CTL8r_I_NDIV_INTf_GET(reg_ctl8));
    }
#endif

    PHYMOD_IF_ERR_RETURN (MODIFY_PLL_CTL8r(pc, reg_ctl8));

    /* write ( "mmd_resetb"                    0);  */
    PLL_CTL9r_MMD_RESETBf_SET(reg_ctl9, 0);

    /* dummy read to kill some time* */
    PHYMOD_IF_ERR_RETURN (READ_PLL_CTL8r(pc, &reg_ctl8));

    /* write ( "mmd_resetb"                    1);  */
    /* mmd_resetb should be asserted and deasserted every time a different divider value is programmed. */
    PLL_CTL9r_MMD_RESETBf_SET(reg_ctl9, 1);

  }

  return PHYMOD_E_NONE;

} /* temod16_set_pll_mode(PHYMOD_ST* pc) */

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
int temod16_tick_override_set(PHYMOD_ST* pc, int tick_override, int numerator, int denominator)
{
  MAIN0_TICK_CTL1r_t    reg_ctrl1;
  MAIN0_TICK_CTL0r_t    reg_ctrl0;

  TMOD_DBG_IN_FUNC_INFO(pc);
  MAIN0_TICK_CTL1r_CLR(reg_ctrl1);
  MAIN0_TICK_CTL0r_CLR(reg_ctrl0);

  /*Tick over_write enable*/
  MAIN0_TICK_CTL1r_TICK_OVERRIDEf_SET(reg_ctrl1, tick_override);
  MAIN0_TICK_CTL1r_TICK_NUMERATOR_UPPERf_SET(reg_ctrl1, (numerator & 0x7fff0));
  PHYMOD_IF_ERR_RETURN(WRITE_MAIN0_TICK_CTL1r(pc, reg_ctrl1));

  MAIN0_TICK_CTL0r_TICK_DENOMINATORf_SET(reg_ctrl0, (denominator & 0x3ff) );
  MAIN0_TICK_CTL0r_TICK_NUMERATOR_LOWERf_SET(reg_ctrl0, (numerator & 0xf));
  PHYMOD_IF_ERR_RETURN(WRITE_MAIN0_TICK_CTL0r(pc, reg_ctrl0));

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
int temod16_set_an_port_mode(PHYMOD_ST* pc)    /* SET_AN_PORT_MODE */
{
  uint16_t data, mask,rdata;
  PHYMOD_ST  pc_10G_X1;
  PHYMOD_ST*  pc_10G_X1_tmp;
  PHYMOD_ST  pc_40G_X4;
  PHYMOD_ST*  pc_40G_X4_tmp;
  PHYMOD_ST  pc_100G_X4;
  PHYMOD_ST*  pc_100G_X4_tmp;

  MAIN0_SETUPr_t reg_main0_setup; 
  AN_X1_CL37_RESTARTr_t reg_cl37_restart_timers;
  AN_X1_CL37_ACKr_t reg_cl37_ack_timers;
  AN_X1_CL37_ERRr_t reg_cl37_err_timers;
  AN_X1_CL73_BRK_LNKr_t reg_cl73_brk_lnk_timers;
  AN_X1_CL73_ERRr_t reg_cl73_err_timers;
  MAIN0_TICK_CTL1r_t reg_tick_ctrl;
  MAIN0_TICK_CTL0r_t reg_tick_ctrl0;

  AN_X1_CFG_CTLr_t reg_an_retry_cnt;
  AN_X1_BAM_SPD_PRI_35_30r_t reg_pri35_30;
  AN_X1_BAM_SPD_PRI_29_24r_t reg_pri29_24;
  AN_X1_BAM_SPD_PRI_23_18r_t reg_pri23_18;
  AN_X1_BAM_SPD_PRI_17_12r_t reg_pri17_12;
  AN_X1_BAM_SPD_PRI_11_6r_t reg_pri11_6;
  AN_X1_BAM_SPD_PRI_5_0r_t reg_bam_pri5;
  AN_X1_OUI_LWRr_t reg_an_oui_lower;
  AN_X1_OUI_UPRr_t reg_an_oui_upper;
  SC_X1_TX_RST_CNTr_t reg_sc_rst_cnt;
  AN_X1_SGMII_CL73_TMR_TYPEr_t reg_an_cl73_sgmii_tmr;
  AN_X1_IGNORE_LNK_TMRr_t reg_an_ig_link_tmr;
  AN_X1_PD_TO_CL37_LNK_WAIT_TMRr_t reg_an_pd_to_cl37;
  AN_X1_CL37_SYNC_STS_FILTER_TMRr_t reg_an_sync_stat_fil_timer;
  AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_t reg_lk_fail_not72;
  AN_X1_CL73_DME_LOCKr_t reg_cl73_dme;
  AN_X1_LNK_UPr_t reg_an_lp;
  AN_X1_LNK_FAIL_INHBT_TMR_CL72r_t reg_lk_fail;
  AN_X1_PD_SD_TMRr_t reg_an_pd_sd_tmr;

  TMOD_DBG_IN_FUNC_INFO(pc);
  MAIN0_SETUPr_CLR(reg_main0_setup);

  data=0;
  mask=0;
  /*Ref clock selection*/
  data = main0_refClkSelect_clk_156p25MHz;
  /*Selectng div 40 for CL37*/
  /*if (pc->sc_mode == TEMOD16_SC_MODE_AN_CL37)
    data = data | (MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div40 <<
    MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_SHIFT); */
  /*Selectng div 66 for CL77*/
  /*if (pc->sc_mode == TEMOD16_SC_MODE_AN_CL73)
    data = data | (MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div66 <<
    MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_SHIFT);*/
    
  /*For AN port mode should be always QUAD*/

  PHYMOD_IF_ERR_RETURN (READ_MAIN0_SETUPr(pc, &reg_main0_setup));
  rdata = MAIN0_SETUPr_PORT_MODE_SELf_GET(reg_main0_setup);
  
  MAIN0_SETUPr_REFCLK_SELf_SET(reg_main0_setup, data);
  data = 0;
  if ((pc->no_of_lanes == 2) || (pc->no_of_lanes == 3)) {
    MAIN0_SETUPr_PORT_MODE_SELf_SET(reg_main0_setup, 0);
  } else {
    if (pc->this_lane == 0 || pc->this_lane == 1 || pc->this_lane == 4 || pc->this_lane == 5 || pc->this_lane == 8 || pc->this_lane == 9) {
      if (rdata == 0x1 || rdata == 0x3) {
        data = data | 0x1;
      }  
      if (rdata == 0x4 || rdata == 0x2) {
        data = data | 0x0;
      }  
    }
    if (pc->this_lane == 2 || pc->this_lane == 3 || pc->this_lane == 6 || pc->this_lane == 7 || pc->this_lane == 10 || pc->this_lane == 11) {
      if (rdata == 0x2 || rdata == 0x3) {
        data = data | 0x2 ;
      }  
      if (rdata == 0x1 || rdata == 0x4) {
        data = data | 0x0 ;
      }  
    }
    MAIN0_SETUPr_PORT_MODE_SELf_SET(reg_main0_setup, data);
  }
  
  PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc,reg_main0_setup));

  /*Tick over_write enable*/
  MAIN0_TICK_CTL1r_CLR(reg_tick_ctrl);
  MAIN0_TICK_CTL0r_CLR(reg_tick_ctrl0);

  MAIN0_TICK_CTL1r_TICK_OVERRIDEf_SET(reg_tick_ctrl, pc->an_tick_override);
  MAIN0_TICK_CTL1r_TICK_NUMERATOR_UPPERf_SET(reg_tick_ctrl, (pc->an_tick_numerator & 0x7fff0) >> 4);
  PHYMOD_IF_ERR_RETURN(WRITE_MAIN0_TICK_CTL1r(pc, reg_tick_ctrl));

  data=0;
  if ((pc->sc_mode == TEMOD16_SC_MODE_AN_CL37))
    {
       /*Setting numerator to 0x10 and denominator to 0x1 for CL37*/
       MAIN0_TICK_CTL0r_TICK_DENOMINATORf_SET(reg_tick_ctrl0,(pc->an_tick_denominator & 0x3ff));
       MAIN0_TICK_CTL0r_TICK_NUMERATOR_LOWERf_SET(reg_tick_ctrl0,(pc->an_tick_numerator & 0xf));
       PHYMOD_IF_ERR_RETURN(WRITE_MAIN0_TICK_CTL0r(pc, reg_tick_ctrl0));
    }
  if ((pc->sc_mode == TEMOD16_SC_MODE_AN_CL73))
    {
       /*Setting numerator to 0x80 and denominator to 0x1 for CL37*/
       MAIN0_TICK_CTL0r_TICK_DENOMINATORf_SET(reg_tick_ctrl0,(pc->an_tick_denominator & 0x3ff));
       MAIN0_TICK_CTL0r_TICK_NUMERATOR_LOWERf_SET(reg_tick_ctrl0,(pc->an_tick_numerator & 0xf));
       PHYMOD_IF_ERR_RETURN(WRITE_MAIN0_TICK_CTL0r(pc, reg_tick_ctrl0));
    }


#if defined (_DV_TB_)
/*AN from ST */
  if (pc->per_lane_control & 0x4) {
     pc_10G_X1 = *pc;
     pc_10G_X1.speed = digital_operationSpeeds_SPEED_10G_KR1; 
     pc_10G_X1_tmp = &pc_10G_X1;
     get_ht_entries(pc_10G_X1_tmp);
     if (pc->cl72_en & 0x80000000)
       pc_10G_X1_tmp->cl72_en = 0x0;
     else  
       pc_10G_X1_tmp->cl72_en = 0x1;
     pc_10G_X1_tmp->cl72_en = pc_10G_X1_tmp->cl72_en | 0x40000000;
     _configure_st_entry(1, digital_operationSpeeds_SPEED_40G_KR4, pc_10G_X1_tmp); 
  }
  else if (pc->per_lane_control & 0x8) {
     pc_40G_X4 = *pc;
     pc_40G_X4.speed = digital_operationSpeeds_SPEED_40G_KR4; 
     pc_40G_X4_tmp = &pc_40G_X4;
     get_ht_entries(pc_40G_X4_tmp);
     if (pc->cl72_en & 0x80000000)
       pc_40G_X4_tmp->cl72_en = 0x0;
     else  
       pc_40G_X4_tmp->cl72_en = 0x1;
     pc_40G_X4_tmp->cl72_en = pc_40G_X4_tmp->cl72_en | 0x40000000;
     _configure_st_entry(1, digital_operationSpeeds_SPEED_10G_KR1, pc_40G_X4_tmp); 
  }
  else if (pc->per_lane_control & 0x20) {
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


/*0x9250 AN_X1_TIMERS_cl37_restart */
  AN_X1_CL37_RESTARTr_CLR(reg_cl37_restart_timers);
  data = pc->cl37_restart_timer_period;
  if (data == 0)
    data = 0x1f;
  AN_X1_CL37_RESTARTr_CL37_RESTART_TIMER_PERIODf_SET(reg_cl37_restart_timers, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL37_RESTARTr(pc, reg_cl37_restart_timers));
/*0x9251 AN_X1_TIMERS_cl37_ack */
  AN_X1_CL37_ACKr_CLR(reg_cl37_ack_timers);
  data = pc->cl37_ack_timer_period;
  if (data == 0)
    data = 0x1f;
  AN_X1_CL37_ACKr_CL37_ACK_TIMER_PERIODf_SET(reg_cl37_ack_timers, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL37_ACKr(pc, reg_cl37_ack_timers));
/*0x9252 AN_X1_TIMERS_cl37_error */
  AN_X1_CL37_ERRr_CLR(reg_cl37_err_timers);
  data = pc->cl37_error_timer_period;
  AN_X1_CL37_ERRr_CL37_ERROR_TIMER_PERIODf_SET(reg_cl37_err_timers, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL37_ERRr(pc, reg_cl37_err_timers));
/*0x9253 AN_X1_TIMERS_cl73_break_link */
  AN_X1_CL73_BRK_LNKr_CLR(reg_cl73_brk_lnk_timers);
  data = pc->cl73_break_link_timer_period;
  AN_X1_CL73_BRK_LNKr_TX_DISABLE_TIMER_PERIODf_SET(reg_cl73_brk_lnk_timers, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_BRK_LNKr(pc, reg_cl73_brk_lnk_timers));
/*0x9254 AN_X1_TIMERS_cl73_error*/ 
  AN_X1_CL73_ERRr_CLR(reg_cl73_err_timers);
  data = pc->cl73_error_timer_period;
  AN_X1_CL73_ERRr_CL73_ERROR_TIMER_PERIODf_SET(reg_cl73_err_timers, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_ERRr(pc, reg_cl73_err_timers));
/*TBD::0x9255 AN_X1_TIMERS_cl73_dme_lock*/ 
  data = 100;
  AN_X1_CL73_DME_LOCKr_CLR(reg_cl73_dme);
  AN_X1_CL73_DME_LOCKr_PD_DME_LOCK_TIMER_PERIODf_SET(reg_cl73_dme, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_DME_LOCKr(pc, reg_cl73_dme));
/*TBD::0x9256 AN_X1_TIMERS_link_up*/ 
  data = 32;
  AN_X1_LNK_UPr_CLR(reg_an_lp);
  AN_X1_LNK_UPr_CL73_LINK_UP_TIMER_PERIODf_SET(reg_an_lp, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_UPr(pc, reg_an_lp));
/*0x9257 AN_X1_TIMERS_link_fail_inhibit_timer_cl72*/ 
  data = pc->link_fail_inhibit_timer_cl72_period;
  AN_X1_LNK_FAIL_INHBT_TMR_CL72r_CLR(reg_lk_fail);
  AN_X1_LNK_FAIL_INHBT_TMR_CL72r_LINK_FAIL_INHIBIT_TIMER_CL72_PERIODf_SET(reg_lk_fail, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_FAIL_INHBT_TMR_CL72r(pc, reg_lk_fail));
/*0x9258 AN_X1_TIMERS_link_fail_inhibit_timer_not_cl72*/ 
  data = pc->link_fail_inhibit_timer_not_cl72_period;
  AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_CLR(reg_lk_fail_not72);
  AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_LINK_FAIL_INHIBIT_TIMER_NCL72_PERIODf_SET(reg_lk_fail_not72, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r(pc, reg_lk_fail_not72));
/*0x9259 AN_X1_TIMERS_pd_sd_timer*/ 
  data = pc->pd_sd_timer_period;
  AN_X1_PD_SD_TMRr_CLR(reg_an_pd_sd_tmr);
  AN_X1_PD_SD_TMRr_PD_SD_TIMER_PERIODf_SET(reg_an_pd_sd_tmr, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_PD_SD_TMRr(pc, reg_an_pd_sd_tmr));
/*0x925a AN_X1_TIMERS_cl72_max_wait_timer*/ 
  data = pc->cl72_max_wait_timer;
  AN_X1_CL37_SYNC_STS_FILTER_TMRr_CLR(reg_an_sync_stat_fil_timer);
  AN_X1_CL37_SYNC_STS_FILTER_TMRr_CL37_SYNC_STATUS_FILTER_TIMER_PERIODf_SET(reg_an_sync_stat_fil_timer, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL37_SYNC_STS_FILTER_TMRr(pc, reg_an_sync_stat_fil_timer));
/*0x925b AN_X1_TIMERS_PD_TO_CL37_LINK_WAIT_TIMER*/ 
  data = pc->pd_to_cl37_wait_timer;
  AN_X1_PD_TO_CL37_LNK_WAIT_TMRr_CLR(reg_an_pd_to_cl37);
  AN_X1_PD_TO_CL37_LNK_WAIT_TMRr_PD_TO_CL37_LINK_WAIT_TIMERf_SET(reg_an_pd_to_cl37, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_PD_TO_CL37_LNK_WAIT_TMRr(pc, reg_an_pd_to_cl37));
/*0x925c AN_X1_TIMERS_ignore_link_timer*/ 
  data = pc->an_ignore_link_timer;
  AN_X1_IGNORE_LNK_TMRr_CLR(reg_an_ig_link_tmr);
  AN_X1_IGNORE_LNK_TMRr_IGNORE_LINK_TIMER_PERIODf_SET(reg_an_ig_link_tmr, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_IGNORE_LNK_TMRr(pc, reg_an_ig_link_tmr));
/*TBD::0x925d AN_X1_TIMERS_dme_page_timer*/  
/*  data = 100;
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_TIMERS_DME_PAGE_TIMERr(pc->unit, pc, data));
*/  
/*TBD::0x925e AN_X1_TIMERS_sgmii_cl73_timer_type*/ 
  data = 100;
  AN_X1_SGMII_CL73_TMR_TYPEr_CLR(reg_an_cl73_sgmii_tmr);
  AN_X1_SGMII_CL73_TMR_TYPEr_SGMII_TIMERf_SET(reg_an_cl73_sgmii_tmr, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_SGMII_CL73_TMR_TYPEr(pc, reg_an_cl73_sgmii_tmr));
/*TBD::0x9263 AN_X1_TIMERS_sgmii_cl73_timer_type*/ 
  data = pc->tx_reset_count;
  SC_X1_TX_RST_CNTr_CLR(reg_sc_rst_cnt);
  SC_X1_TX_RST_CNTr_TX_RESET_COUNTf_SET(reg_sc_rst_cnt, data);
  PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_TX_RST_CNTr(pc, reg_sc_rst_cnt));
/*TBD::0x924a AN_X1_CONTROL_tx_reset_timer_period*/ 
  /*data = 20;
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CONTROL_TX_RESET_TIMER_PERIODr(pc->unit, pc,
  data)); */
/*TBD::0x924a AN_X1_CONTROL_pll_reset_timer */
  /* data = 20;
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CONTROL_TX_RESET_TIMER_PERIODr(pc->unit, pc,
  data)); */
/*0x9240 AN_X1_CONTROL_oui_upper  */
  data =  (pc->oui_upper_data & 0xff);
  AN_X1_OUI_UPRr_CLR(reg_an_oui_upper);
  AN_X1_OUI_UPRr_OUI_UPPER_DATAf_SET(reg_an_oui_upper, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_OUI_UPRr(pc, reg_an_oui_upper));
/*0x9241 AN_X1_CONTROL_oui_lower  */
  data =  ((pc->oui_lower_data));
  AN_X1_OUI_LWRr_CLR(reg_an_oui_lower);
  AN_X1_OUI_LWRr_OUI_LOWER_DATAf_SET(reg_an_oui_lower, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_OUI_LWRr(pc, reg_an_oui_lower));
/*0x9242 AN_X1_CONTROL_BAM_SPEED_PRI_5_0  */
  data =  ((pc->bam_spd_pri_5_0));
  AN_X1_BAM_SPD_PRI_5_0r_CLR(reg_bam_pri5);
  AN_X1_BAM_SPD_PRI_5_0r_SET(reg_bam_pri5, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_BAM_SPD_PRI_5_0r(pc, reg_bam_pri5));
/*0x9243 AN_X1_CONTROL_BAM_SPEED_PRI_11_6  */
  data =  ((pc->bam_spd_pri_11_6));
  AN_X1_BAM_SPD_PRI_11_6r_CLR(reg_pri11_6);
  AN_X1_BAM_SPD_PRI_11_6r_SET(reg_pri11_6, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_BAM_SPD_PRI_11_6r(pc, reg_pri11_6));
/*0x9244 AN_X1_CONTROL_BAM_SPEED_PRI_17_12  */
  data =  ((pc->bam_spd_pri_17_12));
  AN_X1_BAM_SPD_PRI_17_12r_CLR(reg_pri17_12);
  AN_X1_BAM_SPD_PRI_17_12r_SET(reg_pri17_12, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_BAM_SPD_PRI_17_12r(pc, reg_pri17_12));
/*0x9245 AN_X1_CONTROL_BAM_SPEED_PRI_23_18  */
  data =  ((pc->bam_spd_pri_23_18) );
  AN_X1_BAM_SPD_PRI_23_18r_CLR(reg_pri23_18);
  AN_X1_BAM_SPD_PRI_23_18r_SET(reg_pri23_18, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_BAM_SPD_PRI_23_18r(pc, reg_pri23_18));
/*0x9246 AN_X1_CONTROL_BAM_SPEED_PRI_29_24  */
  data =  ((pc->bam_spd_pri_29_24));
  AN_X1_BAM_SPD_PRI_29_24r_CLR(reg_pri29_24);
  AN_X1_BAM_SPD_PRI_29_24r_SET(reg_pri29_24, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_BAM_SPD_PRI_29_24r(pc, reg_pri29_24));
/*0x9247 AN_X1_CONTROL_BAM_SPEED_PRI_35_30  */
  data =  ((pc->bam_spd_pri_35_30));
  AN_X1_BAM_SPD_PRI_35_30r_CLR(reg_pri35_30);
  AN_X1_BAM_SPD_PRI_35_30r_SET(reg_pri35_30, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_BAM_SPD_PRI_35_30r(pc, reg_pri35_30));
/*0x9248 AN_X1_CONTROL_CONFIG_CONTROL  */
  data =  pc->pd_to_cl37_retry_cnt;
  AN_X1_CFG_CTLr_CLR(reg_an_retry_cnt);
  AN_X1_CFG_CTLr_SET(reg_an_retry_cnt, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CFG_CTLr(pc, reg_an_retry_cnt));
  return PHYMOD_E_NONE;
} /* temod16_set_an_port_mode(PHYMOD_ST* pc) */
#endif
#ifdef _SDK_TEMOD16_
int temod16_set_an_port_mode(PHYMOD_ST* pc, int enable, int nl_encoded, int starting_lane, int single_port)    /* SET_AN_PORT_MODE */
{
  uint16_t rdata;
  MAIN0_SETUPr_t  reg_setup;
  int             new_portmode = -1;
  temod16_pll_mode_type current_pll = TEMOD16_PLL_MODE_DIV_ZERO;

  TMOD_DBG_IN_FUNC_INFO(pc);
  MAIN0_SETUPr_CLR(reg_setup);
  PHYMOD_IF_ERR_RETURN (READ_MAIN0_SETUPr(pc, &reg_setup));

  /*Ref clock selection tied to 156.25MHz */
  MAIN0_SETUPr_REFCLK_SELf_SET(reg_setup, 0x3);

  rdata = MAIN0_SETUPr_PORT_MODE_SELf_GET(reg_setup);

  /* nl_encoded: encoded num of lanes adv: say 0->KR, 1->KR2, 2->KR4, 3->CR10*/
  if((nl_encoded == 2) || (nl_encoded == 3)) {
    MAIN0_SETUPr_PORT_MODE_SELf_SET(reg_setup, 0);
  } else {
      if(starting_lane == 0 || starting_lane == 1){
          switch(rdata) {
          case 0x2: new_portmode = 0; break ;
          case 0x3: new_portmode = 1; break ;
          case 0x4: new_portmode = 1; break ;
          default: break ;
          }
      }
      if(starting_lane == 2 || starting_lane == 3){
          switch(rdata) {
          case 0x1: new_portmode = 0; break ;
          case 0x3: new_portmode = 2; break ;
          case 0x4: new_portmode = 2; break ;
          default: break ;
          }
      }
      if((enable>0) && (new_portmode>=0)) { /* only AN enable to change this field */
          MAIN0_SETUPr_PORT_MODE_SELf_SET(reg_setup, new_portmode);
      }
  }

  /*Setting single port mode*/  
  MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(reg_setup, single_port);

  PHYMOD_IF_ERR_RETURN(temod16_pll_config_get(pc, &current_pll));
  if (current_pll == TEMOD16_PLL_MODE_DIV_80) {
      MAIN0_SETUPr_CL37_HIGH_VCOf_SET(reg_setup, 1);
  } else {
      MAIN0_SETUPr_CL37_HIGH_VCOf_SET(reg_setup, 0);
  }

  PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc,reg_setup));

  return PHYMOD_E_NONE;
} /* temod16_set_an_port_mode(PHYMOD_ST* pc, int num_of_lanes, int starting_lane, int single_port) */
#endif /* _SDK_TEMOD16_ */

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
int temod16_tx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int num_lanes)           /* TX_LOOPBACK_CONTROL  */
{
  MAIN0_LPBK_CTLr_t          reg_lb_ctrl;
  PMD_X4_OVRRr_t             reg_pmd_ovr;
  PMD_X4_CTLr_t              reg_pmd_ctrl;
  uint16_t                   lane_mask, data, i, data_temp;

  TMOD_DBG_IN_FUNC_INFO(pc);
  data = 0;
  lane_mask = 0;
  data_temp = 0;
  MAIN0_LPBK_CTLr_CLR(reg_lb_ctrl);
  PMD_X4_OVRRr_CLR(reg_pmd_ovr);
  PMD_X4_CTLr_CLR(reg_pmd_ctrl);

  for (i = 0; i < num_lanes; i++) {
    if (!PHYMOD_LANEPBMP_MEMBER(pc->lane_mask, starting_lane + i)) {
        continue;
    }
    lane_mask |= 1 << (starting_lane + i);
    data |= enable << (starting_lane + i);
  }

   PHYMOD_IF_ERR_RETURN
      (READ_MAIN0_LPBK_CTLr(pc, &reg_lb_ctrl));

   data_temp = MAIN0_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_GET(reg_lb_ctrl);
   data_temp &= ~lane_mask;
   data |= data_temp;

   MAIN0_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_SET(reg_lb_ctrl, data);
   PHYMOD_IF_ERR_RETURN
      (MODIFY_MAIN0_LPBK_CTLr(pc, reg_lb_ctrl));

/* signal_detect and rx_lock */
  if (enable) {
       PMD_X4_OVRRr_RX_LOCK_OVRDf_SET(reg_pmd_ovr, 1);
       PMD_X4_OVRRr_SIGNAL_DETECT_OVRDf_SET(reg_pmd_ovr, 1);
       PMD_X4_OVRRr_TX_DISABLE_OENf_SET(reg_pmd_ovr, 1);
  } else {
       PMD_X4_OVRRr_RX_LOCK_OVRDf_SET(reg_pmd_ovr, 0);
       PMD_X4_OVRRr_SIGNAL_DETECT_OVRDf_SET(reg_pmd_ovr, 0);
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
   PHYMOD_IF_ERR_RETURN
      (MODIFY_PMD_X4_CTLr(pc, reg_pmd_ctrl));

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
int temod16_tx_pmd_loopback_control(PHYMOD_ST* pc, int cntl)           /* TX_PMD_LOOPBACK_CONTROL  */
{

  TLB_RX_DIG_LPBK_CFGr_t    reg_lpbk_cfg;

  TMOD_DBG_IN_FUNC_INFO(pc);
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
@brief   pcs -> rloop
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion

This function sets the RX-to-TX(PCS)  Remote loopback mode, based on port_type.

\li 0:1 : Enable  RX->TX loopback
\li 0:0 : Disable RX->TX loopback

Note that this function can program <B>multiple lanes simultaneously</B>.
As an example, to enable gloop on all lanes,
set #PHYMOD_ST::per_lane_control to 0x01010101

*/
int temod16_rx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type)           /* RX_LOOPBACK_CONTROL  */
{
   MAIN0_LPBK_CTLr_t                 reg_lb_ctrl;
   TX_PI_CTL5r_t                     reg_tx_pi_ctl5;

   /*TLB_TX_TX_PI_LOOP_TIMING_CFGr_t   reg_lb_timing;*/
   TX_PI_CTL0r_t                     reg_com_ctrl;
   /* Need to find equivalent register for TLB_TX_TX_PI_LOOP_TIMING_CFGr_t in Merlin and fix this. RLG*/
   uint16_t lane_mask, data, data_temp;

   TMOD_DBG_IN_FUNC_INFO(pc);
   MAIN0_LPBK_CTLr_CLR(reg_lb_ctrl);
   TX_PI_CTL5r_CLR(reg_tx_pi_ctl5);

   /*TLB_TX_TX_PI_LOOP_TIMING_CFGr_CLR(reg_lb_timing);*/
   TX_PI_CTL0r_CLR(reg_com_ctrl);

   lane_mask = 0;
   switch (port_type) {
       case TEMOD16_MULTI_PORT:   lane_mask = (1 << (starting_lane%4)); break;
       case TEMOD16_TRI1_PORT:    lane_mask = ((starting_lane%4)==2)?0xc : (1 << (starting_lane%4)); break;
       case TEMOD16_TRI2_PORT:    lane_mask = ((starting_lane%4)==0)?0x3 : (1 << (starting_lane%4)); break;
       case TEMOD16_DXGXS:        lane_mask = ((starting_lane%4)==0)?0x3 : 0xc; break;
       case TEMOD16_SINGLE_PORT:  lane_mask = 0xf; break;
       default: break ;
   }
   data = enable ? lane_mask : 0;

   PHYMOD_IF_ERR_RETURN
      (READ_MAIN0_LPBK_CTLr(pc, &reg_lb_ctrl));

   data_temp = MAIN0_LPBK_CTLr_REMOTE_PCS_LOOPBACK_ENABLEf_GET(reg_lb_ctrl);
   data_temp = data_temp & ~lane_mask;
   data = data | data_temp;


   TX_PI_CTL5r_TX_PI_REPEATER_MODE_ENf_SET(reg_tx_pi_ctl5, 0);
   PHYMOD_IF_ERR_RETURN
        (MODIFY_TX_PI_CTL5r(pc, reg_tx_pi_ctl5));

   /* set Tx_PI */
/*   TLB_TX_TX_PI_LOOP_TIMING_CFGr_TX_PI_LOOP_TIMING_SRC_SELf_SET(reg_lb_timing, 1);*/

/*   PHYMOD_IF_ERR_RETURN
        (MODIFY_TLB_TX_TX_PI_LOOP_TIMING_CFGr(pc, reg_lb_timing));*/

   TX_PI_CTL0r_TX_PI_ENf_SET(reg_com_ctrl, 1);
   TX_PI_CTL0r_TX_PI_JITTER_FILTER_ENf_SET(reg_com_ctrl, 1);
   PHYMOD_IF_ERR_RETURN
        (MODIFY_TX_PI_CTL0r(pc, reg_com_ctrl));

  /* wait for 25 us here*/

   TX_PI_CTL0r_CLR(reg_com_ctrl);
   TX_PI_CTL0r_TX_PI_EXT_CTRL_ENf_SET(reg_com_ctrl, 1);
   PHYMOD_IF_ERR_RETURN
        (MODIFY_TX_PI_CTL0r(pc, reg_com_ctrl));

   MAIN0_LPBK_CTLr_REMOTE_PCS_LOOPBACK_ENABLEf_SET(reg_lb_ctrl, data );
   PHYMOD_IF_ERR_RETURN
        (MODIFY_MAIN0_LPBK_CTLr(pc, reg_lb_ctrl));

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
int temod16_encode_set(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf)         /* ENCODE_SET */
{
  uint16_t  hg2_message_invalid_code_enable, hg2_codec, hg2_en ;
  TX_X4_ENC0r_t    reg_encode;
  int cntl;

  TMOD_DBG_IN_FUNC_INFO(pc);
  TX_X4_ENC0r_CLR(reg_encode);

  printf("HiGig_Info : %-22s : pc->per_lane_control %08x : tx_higig2_en %08x : tx_higig2_codec %08x\n", __func__, pc->per_lane_control, pc->tx_higig2_en, pc->tx_higig2_codec) ;

  cntl = pc->per_lane_control;

  hg2_message_invalid_code_enable = 1 ;
  hg2_en            = 0 ;    /* 001 cl48 8b10b
                                010 cl48 8b10b rxaui
                                011 cl36 8b10b
                                100 cl82 64b66b
                                101 cl49 64b66b
                                110 brcm 64b66b */
  hg2_codec         = 0 ;

  if ((spd_intf == TEMOD16_SPD_10_SGMII) |
     (spd_intf == TEMOD16_SPD_100_SGMII) |
     (spd_intf == TEMOD16_SPD_1000_SGMII) |
     (spd_intf == TEMOD16_SPD_2500)) {
      hg2_message_invalid_code_enable = 0x0 ;
  } else if ((spd_intf == TEMOD16_SPD_10000_XFI)||
            (spd_intf == TEMOD16_SPD_10600_XFI_HG)||
            (spd_intf == TEMOD16_SPD_5000)) {
     hg2_message_invalid_code_enable = 1 ;
  } else if (spd_intf == TEMOD16_SPD_10000 ) {   /* 10G XAUI */
     hg2_message_invalid_code_enable = 0 ;
  } else if (spd_intf == TEMOD16_SPD_10000_HI ) {   /* 10.6G HI XAUI */
     hg2_message_invalid_code_enable = 0 ;
  } else if (spd_intf == TEMOD16_SPD_20G_MLD_DXGXS ) {
     hg2_message_invalid_code_enable = 1 ;
  } else if ((spd_intf ==TEMOD16_SPD_21G_HI_MLD_DXGXS)||
             (spd_intf ==TEMOD16_SPD_12773_HI_DXGXS)) {
     hg2_message_invalid_code_enable = 1 ;
     hg2_en            = 1 ;
     hg2_codec         = 1 ;
  } else if ((spd_intf == TEMOD16_SPD_20G_DXGXS)||
             (spd_intf == TEMOD16_SPD_21G_HI_DXGXS)) {
     hg2_message_invalid_code_enable = 0 ;
  } else if ((spd_intf == TEMOD16_SPD_42G_X4)||
             (spd_intf == TEMOD16_SPD_40G_X4)||
             (spd_intf == TEMOD16_SPD_25455) ||
             (spd_intf == TEMOD16_SPD_12773_DXGXS)) {
     hg2_message_invalid_code_enable = 0 ;
  } else if (spd_intf == TEMOD16_SPD_40G_XLAUI ) {
     hg2_message_invalid_code_enable = 1 ;
  } else if (spd_intf == TEMOD16_SPD_42G_XLAUI ) {
     hg2_message_invalid_code_enable = 1 ;
     hg2_en            = 1 ;
     hg2_codec         = 1 ;
  } else if (spd_intf == TEMOD16_SPD_127G_HG_CR12) {
     hg2_message_invalid_code_enable = 1 ;
     hg2_en            = 1 ;
     hg2_codec         = 1 ;
  } else if (spd_intf == TEMOD16_SPD_107G_HG_CR10) {
     hg2_message_invalid_code_enable = 1 ;
     hg2_en            = 1 ;
     hg2_codec         = 1 ;
  }

  /* NICK */
  if (cntl & 0x80000000) {
    READ_TX_X4_ENC0r(pc, &reg_encode);
    printf("HiGig_Info : %-22s : pc->per_lane_control %08x : TX_X4_CONTROL0_ENCODE_0 0x%04x : tx_higig2_en %01x : tx_higig2_codec %01x\n", __func__, pc->per_lane_control,
               reg_encode, (TX_X4_ENC0r_HG2_ENABLEf_GET(reg_encode)) & 0x1, (TX_X4_ENC0r_HG2_CODECf_GET(reg_encode)) & 0x1) ;
     hg2_en    = pc->tx_higig2_en ;
     hg2_codec = pc->tx_higig2_codec ;
  }
  /* set encoder  0xc111 */
  TX_X4_ENC0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(reg_encode, hg2_message_invalid_code_enable);
     TX_X4_ENC0r_HG2_CODECf_SET(reg_encode, hg2_codec);
     TX_X4_ENC0r_HG2_ENABLEf_SET(reg_encode, hg2_en);

   if (pc->scramble_idle_tx_en) {
     TX_X4_ENC0r_T_TEST_MODE_CFGf_SET(reg_encode,pc->scramble_idle_tx_en);
     /* TX_X4_ENC0r_CL49_TX_TL_MODEf_SET(reg_encode,2); */
   }
  PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_ENC0r(pc, reg_encode));

  return PHYMOD_E_NONE;
}
#endif /* _DV_TB_ */

#ifdef _SDK_TEMOD16_

int temod16_encode_set(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, uint16_t enable)         /* ENCODE_SET */
{
  uint16_t  hg2_message_invalid_code_enable, hg2_codec, hg2_en ;
  TX_X4_ENC0r_t    reg_encode;

  TMOD_DBG_IN_FUNC_INFO(pc);
  TX_X4_ENC0r_CLR(reg_encode);

  hg2_message_invalid_code_enable = 0;
  hg2_en            = 0 ;
  hg2_codec         = 0 ;

  if(enable == 1) {
      hg2_message_invalid_code_enable = 1;
      hg2_en = 1;
      hg2_codec = 1;
  }

  /* set encoder  0xc111 */
  TX_X4_ENC0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(reg_encode, hg2_message_invalid_code_enable);
  TX_X4_ENC0r_HG2_CODECf_SET(reg_encode, hg2_codec);
  TX_X4_ENC0r_HG2_ENABLEf_SET(reg_encode, hg2_en);

  PHYMOD_IF_ERR_RETURN
      (MODIFY_TX_X4_ENC0r(pc, reg_encode));

  return PHYMOD_E_NONE;

}

#endif /* _SDK_TEMOD16_ */

/**
@brief   set rx decoder of any particular lane
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details This function is used to select decoder of any receive lane.

\li select decodeMode

*/

#ifdef _DV_TB_
int temod16_decode_set(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf)         /* DECODE_SET */
{

 uint16_t hg2_message_ivalid_code_enable, hg2_en, hg2_codec;
  RX_X4_DEC_CTL0r_t    reg_decode_ctrl;
  int cntl;

  TMOD_DBG_IN_FUNC_INFO(pc);
  RX_X4_DEC_CTL0r_CLR(reg_decode_ctrl);

  /* NICK */
  printf("HiGig_Info : %-22s : pc->per_lane_control %08x : rx_higig2_en %08x : rx_higig2_codec %08x\n", __func__, pc->per_lane_control, pc->rx_higig2_en, pc->rx_higig2_codec);

  /* NICK */
  cntl = pc->per_lane_control | 0x1 ;
  /* cntl = 1; */

  hg2_message_ivalid_code_enable = 0x1 ;
  hg2_en          = 0 ;
  hg2_codec       = 0 ;
  if (cntl) { /* set decoder */


    if (spd_intf == TEMOD16_SPD_10_SGMII) {
        hg2_message_ivalid_code_enable = 0x0 ;
    } else if (spd_intf == TEMOD16_SPD_100_SGMII) {
        hg2_message_ivalid_code_enable = 0x0 ;
    } else if (spd_intf == TEMOD16_SPD_1000_SGMII) {
        hg2_message_ivalid_code_enable = 0x0 ;
    } else if (spd_intf == TEMOD16_SPD_2500) {
        hg2_message_ivalid_code_enable = 0x0 ;
    } else if ((spd_intf == TEMOD16_SPD_10000_XFI)||
              (spd_intf == TEMOD16_SPD_10600_XFI_HG)||
              (spd_intf == TEMOD16_SPD_5000)) {
        hg2_message_ivalid_code_enable = 0x1 ;
    } else if ( spd_intf == TEMOD16_SPD_10000 ) {  /* 10G XAUI */
        hg2_message_ivalid_code_enable = 0x0 ;
    } else if ( spd_intf == TEMOD16_SPD_10000_HI ) {  /* 10G XAUI */
        hg2_message_ivalid_code_enable = 0x0 ;
    } else if ( spd_intf == TEMOD16_SPD_20G_MLD_DXGXS ) {
        hg2_message_ivalid_code_enable = 0x1 ;
    } else if ((spd_intf ==TEMOD16_SPD_21G_HI_MLD_DXGXS)||
              (spd_intf ==TEMOD16_SPD_12773_HI_DXGXS)) {
        hg2_message_ivalid_code_enable = 0x1 ;
        hg2_en          = 1 ;
        hg2_codec       = 1 ;
    } else if ((spd_intf == TEMOD16_SPD_20G_DXGXS)||
              (spd_intf == TEMOD16_SPD_21G_HI_DXGXS)) {
        hg2_message_ivalid_code_enable = 0x0 ;
    } else if ((spd_intf == TEMOD16_SPD_42G_X4) ||
              (spd_intf == TEMOD16_SPD_40G_X4) ||
              (spd_intf == TEMOD16_SPD_25455) ||
              (spd_intf == TEMOD16_SPD_12773_DXGXS)) {
        hg2_message_ivalid_code_enable = 0x1 ;
    } else if ( spd_intf == TEMOD16_SPD_40G_XLAUI ) {
        hg2_message_ivalid_code_enable = 0x1 ;
    } else if ( spd_intf == TEMOD16_SPD_42G_XLAUI ) {  /* 40 MLD Hig2 */
        hg2_message_ivalid_code_enable = 0x1 ;
        hg2_en          = 1 ;
        hg2_codec       = 1 ;
    } else if ( spd_intf == TEMOD16_SPD_127G_HG_CR12 ) {
        hg2_message_ivalid_code_enable = 0x1 ;
        hg2_en          = 1 ;
        hg2_codec       = 1 ;
    } else if ( spd_intf == TEMOD16_SPD_107G_HG_CR10 ) {
        hg2_message_ivalid_code_enable = 0x1 ;
        hg2_en          = 1 ;
        hg2_codec       = 1 ;
    } else {
       printf("%-22s ERROR: p=%0d undefined spd_intf=%0d(%s)\n", __func__, pc->port, spd_intf,
              e2s_temod16_spd_intfc_type[spd_intf]) ;
    }
    /* HG setting */
    if ((pc->ctrl_type & TEMOD16_CTRL_TYPE_HG)) {
       /* not CL48 */
       hg2_en          = 1 ;
       hg2_codec       = 1 ;
    }

    /* NICK */
    if (cntl & 0x80000000) {
     READ_RX_X4_DEC_CTL0r(pc, &reg_decode_ctrl);
#ifdef _DV_TB_
     printf("HiGig_Info : %-22s : pc->per_lane_control %08x : RX_X4_CONTROL0_DECODE_CONTROL_0 0x%04x : rx_higig2_en %01x : rx_higig2_codec %01x\n", __func__, pc->per_lane_control, reg_decode_ctrl,
               (RX_X4_DEC_CTL0r_HG2_ENABLEf_GET(reg_decode_ctrl)) & 0x1, (RX_X4_DEC_CTL0r_HG2_CODECf_GET(reg_decode_ctrl)) & 0x1) ;
#endif

      hg2_en    = pc->rx_higig2_en ;
      hg2_codec = pc->rx_higig2_codec ;
    }
   
    
    if (pc->sc_mode != TEMOD16_SC_MODE_AN_CL37 && pc->sc_mode !=TEMOD16_SC_MODE_AN_CL73) {

      RX_X4_DEC_CTL0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(reg_decode_ctrl, hg2_message_ivalid_code_enable);
         RX_X4_DEC_CTL0r_HG2_ENABLEf_SET(reg_decode_ctrl, hg2_en);
         RX_X4_DEC_CTL0r_HG2_CODECf_SET(reg_decode_ctrl, hg2_codec);
      PHYMOD_IF_ERR_RETURN
          (MODIFY_RX_X4_DEC_CTL0r(pc, reg_decode_ctrl));
    }
  } else {  /* if ~cntl */
    /* no op */
     return PHYMOD_E_NONE;
  }

  if (pc->scramble_idle_rx_en) {
    RX_X4_DEC_CTL0r_R_TEST_MODE_CFGf_SET(reg_decode_ctrl,pc->scramble_idle_rx_en);
  }
  if ((pc->pmd_reset_control) != 0) {
    temod16_pmd_reset_bypass(pc, 1);
  }
  
  return PHYMOD_E_NONE;
}  /* DECODE_SET */
#endif

#ifdef _SDK_TEMOD16_
int temod16_decode_set(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, uint16_t enable)         /* DECODE_SET */
{

 uint16_t hg2_message_ivalid_code_enable, hg2_en, hg2_codec;
 RX_X4_DEC_CTL0r_t    reg_decode_ctrl;

  TMOD_DBG_IN_FUNC_INFO(pc);
  RX_X4_DEC_CTL0r_CLR(reg_decode_ctrl);

  hg2_message_ivalid_code_enable = 0x0;
  hg2_en          = 0 ;
  hg2_codec       = 0 ;
  if(enable == 1) {
      hg2_message_ivalid_code_enable = 1;
      hg2_en = 1;
      hg2_codec = 1;
  }


  RX_X4_DEC_CTL0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(reg_decode_ctrl, hg2_message_ivalid_code_enable);
  RX_X4_DEC_CTL0r_HG2_ENABLEf_SET(reg_decode_ctrl, hg2_en);
  RX_X4_DEC_CTL0r_HG2_CODECf_SET(reg_decode_ctrl, hg2_codec);
  PHYMOD_IF_ERR_RETURN
      (MODIFY_RX_X4_DEC_CTL0r(pc, reg_decode_ctrl));
  return PHYMOD_E_NONE;
}  /* DECODE_SET */

#endif /* SDK_TEMOD16_ */

/*!
@brief   get  port speed configured
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   get the resolved speed cfg in the spd_intf
@returns The value PHYMOD_E_NONE upon successful completion.
@details get  port speed configured
*/
int temod16_spd_intf_get(PHYMOD_ST* pc, int *spd_intf)         /* SET_SPD_INTF */
{
  SC_X4_RSLVD_SPDr_t sc_final_resolved_speed;

  TMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_RSLVD_SPDr_CLR(sc_final_resolved_speed);
  READ_SC_X4_RSLVD_SPDr(pc,&sc_final_resolved_speed);
  temod16_get_actual_speed(SC_X4_RSLVD_SPDr_SPEEDf_GET(sc_final_resolved_speed), spd_intf);
  return PHYMOD_E_NONE;
}

/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   *revid int ref. to return revision id to calling function.
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This  fucntion reads the revid register that contains core number, revision
number and stores the 16-bit value in the field #PHYMOD_ST::accData.

*/
int temod16_revid_read(PHYMOD_ST* pc, uint32_t *revid) 
{
  /*
  int laneselect;
  laneselect = pc->lane_select;
  pc->lane_select=TEMOD16_LANE_0_0_0_1;
  */
  TMOD_DBG_IN_FUNC_INFO(pc);
  *revid=_temod16_getRevDetails(pc);
  /*
  pc->lane_select=laneselect;
  */

  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init

*/
int temod16_init_pcs(PHYMOD_ST* pc)              /* INIT_PCS */
{
  TMOD_DBG_IN_FUNC_INFO(pc);
  _temod16_set_port_mode_select(pc);
  temod16_set_bit_repli(pc,pc->spd_intf);
  
  temod16_rx_lane_control_set(pc, 1);
  temod16_tx_lane_control_set(pc, 1, 0);         /* TX_LANE_CONTROL */

  /*Re-Visit, Assuming the value is non xero in case mld only */
  temod16_mld_am_timers_set(pc, pc->rx_am_timer_init_val, pc->tx_am_timer_init_val);

  if ((pc->sc_mode == TEMOD16_SC_MODE_AN_CL37) || (pc->sc_mode == TEMOD16_SC_MODE_AN_CL73))
    _init_pcs_an(pc);
  else /* Forced Speed */
    _init_pcs_fs(pc);
  
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
int temod16_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched) /* PMD_RESET_SEQ */
{
  PMD_X1_CTLr_t reg_pmd_x1_ctrl;
#ifdef KEEP_THIS
  SC_X4_BYPASSr_t reg_bypass;
  SC_X4_BYPASSr_CLR(reg_bypass);
#endif
  TMOD_DBG_IN_FUNC_INFO(pc);
  PMD_X1_CTLr_CLR(reg_pmd_x1_ctrl);
  
  if (pmd_touched == 0) {
    PMD_X1_CTLr_POR_H_RSTBf_SET(reg_pmd_x1_ctrl,1);
    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(reg_pmd_x1_ctrl,1);
    PHYMOD_IF_ERR_RETURN(WRITE_PMD_X1_CTLr(pc,reg_pmd_x1_ctrl));
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
int temod16_wait_sc_done(PHYMOD_ST* pc, uint16_t *data)
{
  static int timeOut = 15;
  int localTimeOut = timeOut;
  SC_X4_STSr_t reg_sc_ctrl_sts;

  TMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_STSr_CLR(reg_sc_ctrl_sts);

  while(1) {
    localTimeOut--;
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_STSr(pc, &reg_sc_ctrl_sts));
    if ((SC_X4_STSr_SW_SPEED_CHANGE_DONEf_GET(reg_sc_ctrl_sts) == 1) || localTimeOut < 1)
      break;
  }
  return PHYMOD_E_NONE;
}

/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init
*/
int _temod16_wait_sc_stats_set(PHYMOD_ST* pc)
{
  uint16_t data;
  SC_X4_STSr_t reg_sc_ctrl_sts;

  SC_X4_STSr_CLR(reg_sc_ctrl_sts);
   
  while(1) {
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_STSr(pc, &reg_sc_ctrl_sts));
   data = SC_X4_STSr_SW_SPEED_CONFIG_VLDf_GET(reg_sc_ctrl_sts);
   if (data == 1)
     break;
  }
  return PHYMOD_E_NONE;
}

/**
@brief   Read PCS Link status
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   *link Reference for Status of PCS Link
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init

*/
int temod16_get_pcs_link_status(PHYMOD_ST* pc, uint32_t *link)
{
  RX_X4_PCS_LIVE_STSr_t reg_pcs_live_sts;
  
  TMOD_DBG_IN_FUNC_INFO(pc);
  RX_X4_PCS_LIVE_STSr_CLR(reg_pcs_live_sts);
  PHYMOD_IF_ERR_RETURN (READ_RX_X4_PCS_LIVE_STSr(pc, &reg_pcs_live_sts));
  *link = RX_X4_PCS_LIVE_STSr_LINK_STATUSf_GET(reg_pcs_live_sts);
  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init

*/
int temod16_mld_am_timers_set(PHYMOD_ST* pc, int rx_am_timer_init_val, int tx_am_timer_init_val) /*MLD_AM_TIMERS_SET */
{
  CL82_RX_AM_TMRr_t reg_rx_am_timer;
  TX_X2_MLD_SWP_CNTr_t reg_mld_swap_cnt;
    
  TMOD_DBG_IN_FUNC_INFO(pc);
  CL82_RX_AM_TMRr_CLR(reg_rx_am_timer);
  TX_X2_MLD_SWP_CNTr_CLR(reg_mld_swap_cnt);

  if (rx_am_timer_init_val != 0) {
    CL82_RX_AM_TMRr_SET(reg_rx_am_timer, rx_am_timer_init_val);
    PHYMOD_IF_ERR_RETURN (WRITE_CL82_RX_AM_TMRr(pc, reg_rx_am_timer));
  }
  if (tx_am_timer_init_val != 0) {
    TX_X2_MLD_SWP_CNTr_SET(reg_mld_swap_cnt, tx_am_timer_init_val);
    PHYMOD_IF_ERR_RETURN (WRITE_TX_X2_MLD_SWP_CNTr(pc, reg_mld_swap_cnt));
  }
  return PHYMOD_E_NONE;
}
#endif

/**
@brief   Disable teh Port(Forced Speed Mode)
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init

*/
int temod16_disable_set(PHYMOD_ST* pc)              /* INIT_PCS */
{
  SC_X4_CTLr_t reg_sc_ctrl;
  
  TMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_CTLr_CLR(reg_sc_ctrl); 

  /* write 0 to the speed change */
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 0);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc,reg_sc_ctrl));

  return PHYMOD_E_NONE;
}

/**
@brief   Select the ILKN path and bypass TSCE PCS
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details This will enable ILKN path. PCS is set to bypass to relinquish PMD
control. Expect PMD to be programmed elsewhere. If we need the QSGMII PCS expect
the QSGMII to be already programmed at the port layer. If not, we will feed
garbage to the ILKN path.

*/
int temod16_init_pcs_ilkn(PHYMOD_ST* pc)              /* INIT_PCS */
{
  SC_X4_BYPASSr_t reg_sc_bypass;
  ILKN_CTL0r_t reg_ilkn_ctrl0;

  TMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_BYPASSr_CLR(reg_sc_bypass);
  ILKN_CTL0r_CLR(reg_ilkn_ctrl0);

  SC_X4_BYPASSr_SC_BYPASSf_SET(reg_sc_bypass, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_BYPASSr(pc,reg_sc_bypass));

  ILKN_CTL0r_ILKN_SELf_SET(reg_ilkn_ctrl0, 1);
  ILKN_CTL0r_CREDIT_ENf_SET(reg_ilkn_ctrl0, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_ILKN_CTL0r(pc, reg_ilkn_ctrl0));

  return PHYMOD_E_NONE;
}

/**
@brief   PMD per lane reset
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per lane PMD ln_rst and ln_dp_rst by writing to PMD_X4_CONTROL in pcs space

*/
int temod16_pmd_x4_reset(PHYMOD_ST* pc)              /* PMD_X4_RESET */
{
  PMD_X4_CTLr_t reg_pmd_x4_ctrl;

  TMOD_DBG_IN_FUNC_INFO(pc);
  PMD_X4_CTLr_CLR(reg_pmd_x4_ctrl);
  PMD_X4_CTLr_LN_RX_H_RSTBf_SET(reg_pmd_x4_ctrl,0);
  PMD_X4_CTLr_LN_TX_H_RSTBf_SET(reg_pmd_x4_ctrl,0);
  PMD_X4_CTLr_LN_RX_DP_H_RSTBf_SET(reg_pmd_x4_ctrl,0);
  PMD_X4_CTLr_LN_TX_DP_H_RSTBf_SET(reg_pmd_x4_ctrl,0);
  PHYMOD_IF_ERR_RETURN (MODIFY_PMD_X4_CTLr(pc, reg_pmd_x4_ctrl));
  
  PMD_X4_CTLr_CLR(reg_pmd_x4_ctrl);
  PMD_X4_CTLr_LN_RX_H_RSTBf_SET(reg_pmd_x4_ctrl,1);
  PMD_X4_CTLr_LN_TX_H_RSTBf_SET(reg_pmd_x4_ctrl,1);
  PMD_X4_CTLr_LN_RX_DP_H_RSTBf_SET(reg_pmd_x4_ctrl,1);
  PMD_X4_CTLr_LN_TX_DP_H_RSTBf_SET(reg_pmd_x4_ctrl,1);
  PHYMOD_IF_ERR_RETURN (MODIFY_PMD_X4_CTLr(pc, reg_pmd_x4_ctrl));
  
  return PHYMOD_E_NONE;
}

/**
@brief   Initialize the PMD in independent mode. (No PCS involved)
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   pmd_touched 
@param   uc_active 
@param   spd_intf #temod16_spd_intfc_type 
@param   t_pma_os_mode per lane OS Mode in PMD
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PMD Init done by software

*/
int temod16_init_pmd_sw(PHYMOD_ST* pc, int pmd_touched, int uc_active,
                      temod16_spd_intfc_type spd_intf,  int t_pma_os_mode)  /* INIT_PMD_SW */
{
  TMOD_DBG_IN_FUNC_INFO(pc);
  temod16_init_pmd(pc, pmd_touched,  uc_active);

#ifdef _DV_TB_
  temod16_pmd_native_mode_set(pc, spd_intf);
#endif

  temod16_pmd_osmode_set(pc, spd_intf,  t_pma_os_mode);

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
int temod16_init_pmd(PHYMOD_ST* pc, int pmd_touched, int uc_active)   /* INIT_PMD */
{
  /* TXFIR_MISC_CTL1r_t                         reg_misc1_ctrl; */
  RXTXCOM_LN_CLK_RST_N_PWRDWN_CTLr_t    reg_pwrdwn_ctrl;
  TOP_USER_CTL0r_t                    reg_usr_ctrl;

  TMOD_DBG_IN_FUNC_INFO(pc);
  RXTXCOM_LN_CLK_RST_N_PWRDWN_CTLr_CLR(reg_pwrdwn_ctrl);
  TOP_USER_CTL0r_CLR(reg_usr_ctrl);

  /* REMOVED NOT NEEDED 
  TXFIR_MISC_CTL1r_CLR(reg_misc1_ctrl);
  TXFIR_MISC_CTL1r_DP_RESET_TX_DISABLE_DISf_SET(reg_misc1_ctrl, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_TXFIR_MISC_CTL1r(pc,reg_misc1_ctrl));
  */

  RXTXCOM_LN_CLK_RST_N_PWRDWN_CTLr_LN_DP_S_RSTBf_SET(reg_pwrdwn_ctrl, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_RXTXCOM_LN_CLK_RST_N_PWRDWN_CTLr(pc, reg_pwrdwn_ctrl));

  if (pmd_touched == 0) {
    if (uc_active == 1) {
      TOP_USER_CTL0r_UC_ACTIVEf_SET(reg_usr_ctrl, 1);
      TOP_USER_CTL0r_CORE_DP_S_RSTBf_SET(reg_usr_ctrl, 1);
      /* release reset to pll data path. TBD need for all lanes  and uc_active set */
      PHYMOD_IF_ERR_RETURN (MODIFY_TOP_USER_CTL0r(pc, reg_usr_ctrl));
    } else{
      TOP_USER_CTL0r_CORE_DP_S_RSTBf_SET(reg_usr_ctrl, 1);
      PHYMOD_IF_ERR_RETURN (MODIFY_TOP_USER_CTL0r(pc, reg_usr_ctrl));
    }
  }
  return PHYMOD_E_NONE;
}

/**
@brief   Set Per lane OS mode set in PMD
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   spd_intf speed type #temod16_spd_intfc_type
@param   os_mode over sample rate.
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PMD Init

*/
int temod16_pmd_osmode_set(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, int os_mode)   /* INIT_PMD */
{
  RXTXCOM_OSR_MODE_CTLr_t    reg_osr_mode;
  int speed;

  TMOD_DBG_IN_FUNC_INFO(pc);
  RXTXCOM_OSR_MODE_CTLr_CLR(reg_osr_mode);
  temod16_get_mapped_speed(spd_intf, &speed);

  /*os_mode         = 0x0; */ /* 0= OS MODE 1;  1= OS MODE 2; 2=OS MODE 3; 
                             3=OS MODE 3.3; 4=OS MODE 4; 5=OS MODE 5; 
                             6=OS MODE 8;   7=OS MODE 8.25; 8: OS MODE 10*/
  if (os_mode & 0x80000000) {
    os_mode =  (os_mode) & 0x0000ffff;
  }
 else {
    os_mode =  merlin16_sc_pmd_entry[speed].t_pma_os_mode;
  }

  RXTXCOM_OSR_MODE_CTLr_OSR_MODE_FRCf_SET(reg_osr_mode, 1);
  RXTXCOM_OSR_MODE_CTLr_OSR_MODE_FRC_VALf_SET(reg_osr_mode, os_mode);

  PHYMOD_IF_ERR_RETURN
     (MODIFY_RXTXCOM_OSR_MODE_CTLr(pc, reg_osr_mode));

  return PHYMOD_E_NONE;
}

/**
@brief   Get Per lane OS mode set in PMD
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   os_mode over sample rate.
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PMD Init

*/
int temod16_pmd_osmode_get(PHYMOD_ST* pc, int *os_mode)   /* INIT_PMD */
{
  RXTXCOM_OSR_MODE_CTLr_t    reg_osr_mode;

  TMOD_DBG_IN_FUNC_INFO(pc);
  RXTXCOM_OSR_MODE_CTLr_CLR(reg_osr_mode);

  PHYMOD_IF_ERR_RETURN
     (READ_RXTXCOM_OSR_MODE_CTLr(pc, &reg_osr_mode));

 if(RXTXCOM_OSR_MODE_CTLr_OSR_MODE_FRCf_GET(reg_osr_mode) == 1)
   *os_mode = RXTXCOM_OSR_MODE_CTLr_OSR_MODE_FRC_VALf_GET(reg_osr_mode);
 else
   *os_mode = 0xff;

  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
int temod16_pmd_native_mode_set(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf)
{
  TLB_TX_TLB_TX_MISC_CFGr_t    reg_tx_misc;

  TMOD_DBG_IN_FUNC_INFO(pc);
  TLB_TX_TLB_TX_MISC_CFGr_CLR(reg_tx_misc);

  if (
             spd_intf == TEMOD16_SPD_10000 || 
             spd_intf == TEMOD16_SPD_15000 || 
             spd_intf == TEMOD16_SPD_16000 ||
             spd_intf == TEMOD16_SPD_10000_DXGXS ||
             spd_intf == TEMOD16_SPD_20000
     ) {
     TLB_TX_TLB_TX_MISC_CFGr_TX_PCS_NATIVE_ANA_FRMT_ENf_SET(reg_tx_misc,pc->bit_rep_native_mode);      
  }

  PHYMOD_IF_ERR_RETURN (MODIFY_TLB_TX_TLB_TX_MISC_CFGr(pc, reg_tx_misc));

  return PHYMOD_E_NONE;
}
#endif
/**
@brief   Enable FEC for forced Speeds. Will be rewritten in FEC_control
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Enable FEC for forced speeds.

*/
int temod16_fecmode_set(PHYMOD_ST* pc, int fec_enable)              /* FECMODE_SET */
{
  TX_X4_MISCr_t                reg_misc;
  RX_X4_DEC_CTL0r_t            reg_decode;
  SC_X4_CTLr_t                 reg;
  phymod_access_t pa_copy;
  int start_lane = 0, num_lane = 0;


  TMOD_DBG_IN_FUNC_INFO(pc);
  TX_X4_MISCr_CLR(reg_misc);
  RX_X4_DEC_CTL0r_CLR(reg_decode);
  SC_X4_CTLr_CLR(reg);

  PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
  PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
  pa_copy.lane_mask = 0x1 << start_lane;

  TX_X4_MISCr_FEC_ENABLEf_SET(reg_misc, fec_enable);
  PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, reg_misc));

  RX_X4_DEC_CTL0r_BLOCK_SYNC_MODEf_SET(reg_decode, (fec_enable << 2));
  PHYMOD_IF_ERR_RETURN (MODIFY_RX_X4_DEC_CTL0r(pc, reg_decode));
  
 
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(&pa_copy, &reg));
  if (SC_X4_CTLr_SW_SPEED_CHANGEf_GET(reg) == 1) {
    PHYMOD_IF_ERR_RETURN (temod16_trigger_speed_change(&pa_copy)); 
  }

  return PHYMOD_E_NONE;
}

int temod16_fecmode_get(PHYMOD_ST* pc, uint32_t *fec_enable)
{
  TX_X4_MISCr_t                   reg_misc;
  SC_X4_CTLr_t                    reg;
  AN_X4_ENSr_t                    reg_an_enb;
  AN_X4_AN_ABIL_RESOLUTION_STSr_t an_hcd_status;

  TMOD_DBG_IN_FUNC_INFO(pc);
  TX_X4_MISCr_CLR(reg_misc);
  SC_X4_CTLr_CLR(reg);
  AN_X4_ENSr_CLR(reg_an_enb);

  PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc, &reg));
  PHYMOD_IF_ERR_RETURN (READ_AN_X4_ENSr(pc, &reg_an_enb));

  *fec_enable = 0;
  if(AN_X4_ENSr_CL73_ENABLEf_GET(reg_an_enb) == 1){
    READ_AN_X4_AN_ABIL_RESOLUTION_STSr(pc,&an_hcd_status);
    *fec_enable = AN_X4_AN_ABIL_RESOLUTION_STSr_AN_HCD_FECf_GET(an_hcd_status);
  } else if(SC_X4_CTLr_SW_SPEED_CHANGEf_GET(reg) == 1) {
    PHYMOD_IF_ERR_RETURN (READ_TX_X4_MISCr(pc, &reg_misc));
    *fec_enable = TX_X4_MISCr_FEC_ENABLEf_GET(reg_misc);
  }

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
          FEC_ENABLE_OEN |
          DESKEWMODE_OEN |
          DESC2_MODE_OEN |
          CL36BYTEDELETEMOD16E_OEN |
          BRCM64B66_DESCRAMBLER_ENABLE_OEN |
          CHK_END_EN_OEN |
          BLOCK_SYNC_MODE_OEN |
          DECODERMODE_OEN |
          ENCODEMODE_OEN |
          DESCRAMBLERMODE_OEN |
          SCR_MODE_OEN

*/
int temod16_override_set(PHYMOD_ST* pc, override_type_t or_type, uint16_t or_val)   /* OVERRIDE_SET */
{
  int or_value;
  SC_X4_FLD_OVRR_EN0_TYPEr_t reg_or_en0;
  SC_X4_FLD_OVRR_EN1_TYPEr_t reg_or_en1;
  LNK_CTLr_t reg_cl72_link_ctrl;
  SC_X4_LN_NUM_OVRRr_t  reg_ln_num_or_val;
  RX_X4_PMA_CTL0r_t reg_os_mode_or_val;
  TX_X4_MISCr_t reg_misc_or_val;
  RX_X4_PCS_CTL0r_t reg_pcs_or_val;
  RX_X2_MISC0r_t reg_misc0_or_val;
  RX_X4_DEC_CTL0r_t reg_dec_or_val;
  RX_X4_CL36_RX0r_t reg_cl36_or_val;
  TX_X4_ENC0r_t reg_encode_or_val;

  TMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_FLD_OVRR_EN0_TYPEr_CLR(reg_or_en0);
  SC_X4_FLD_OVRR_EN1_TYPEr_CLR(reg_or_en1);

  or_value = or_val & 0x0000ffff;
  switch (or_type) {
  case TEMOD16_OVERRIDE_RESET :
   /* reset credits */
   WRITE_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);

   SC_X4_FLD_OVRR_EN1_TYPEr_CL72_EN_OENf_SET(reg_or_en1, 0);
   SC_X4_FLD_OVRR_EN1_TYPEr_REORDER_EN_OENf_SET(reg_or_en1, 0);
   SC_X4_FLD_OVRR_EN1_TYPEr_CL36_EN_OENf_SET(reg_or_en1, 0);
   MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc,reg_or_en1);
  break;
  case TEMOD16_OVERRIDE_ALL :
    /* get info. from table and apply it to all credit regs. */
  break;
  case TEMOD16_OVERRIDE_CL72 :
       LNK_CTLr_CLR(reg_cl72_link_ctrl);
       LNK_CTLr_LINK_CONTROL_FORCEVALf_SET(reg_cl72_link_ctrl, or_value);
       MODIFY_LNK_CTLr(pc,reg_cl72_link_ctrl);
       /*Set the override enable*/
       if (or_value == 0) {
           SC_X4_FLD_OVRR_EN1_TYPEr_CL72_EN_OENf_SET(reg_or_en1, 1);
       } else {
           SC_X4_FLD_OVRR_EN1_TYPEr_CL72_EN_OENf_SET(reg_or_en1, 0);
       }
       MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc,reg_or_en1);
  break;
  case TEMOD16_OVERRIDE_NUM_LANES :
       SC_X4_LN_NUM_OVRRr_CLR(reg_ln_num_or_val);
       SC_X4_LN_NUM_OVRRr_NUM_LANES_OVERRIDE_VALUEf_SET(reg_ln_num_or_val,or_value);
       MODIFY_SC_X4_LN_NUM_OVRRr(pc,reg_ln_num_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_NUM_LANES_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_OS_MODE:
       RX_X4_PMA_CTL0r_CLR(reg_os_mode_or_val);
       RX_X4_PMA_CTL0r_OS_MODEf_SET(reg_os_mode_or_val,or_value);
       MODIFY_RX_X4_PMA_CTL0r(pc,reg_os_mode_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_OS_MODE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_FEC_EN :
       TX_X4_MISCr_CLR(reg_misc_or_val);
       TX_X4_MISCr_FEC_ENABLEf_SET(reg_misc_or_val,or_value);
       MODIFY_TX_X4_MISCr(pc,reg_misc_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_FEC_ENABLE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_DESKEW_MODE:
       RX_X4_PCS_CTL0r_CLR(reg_pcs_or_val);
       RX_X4_PCS_CTL0r_DESKEWMODEf_SET(reg_pcs_or_val,or_value);
       MODIFY_RX_X4_PCS_CTL0r(pc,reg_pcs_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_DESKEWMODE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_DESC2_MODE :
       RX_X4_PCS_CTL0r_CLR(reg_pcs_or_val);
       RX_X4_PCS_CTL0r_DESC2_MODEf_SET(reg_pcs_or_val,or_value);
       MODIFY_RX_X4_PCS_CTL0r(pc,reg_pcs_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_DESC2_MODE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_CL36BYTEDEL_MODE:
       RX_X4_PCS_CTL0r_CLR(reg_pcs_or_val);
       RX_X4_PCS_CTL0r_CL36BYTEDELETEMODEf_SET(reg_pcs_or_val,or_value);
       MODIFY_RX_X4_PCS_CTL0r(pc,reg_pcs_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_CL36BYTEDELETEMODE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_BRCM64B66_DESCR_MODE:
       RX_X4_PCS_CTL0r_CLR(reg_pcs_or_val);
       RX_X4_PCS_CTL0r_BRCM64B66_DESCRAMBLER_ENABLEf_SET(reg_pcs_or_val,or_value);
       MODIFY_RX_X4_PCS_CTL0r(pc,reg_pcs_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_BRCM64B66_DESCRAMBLER_ENABLE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_CHKEND_EN:
       RX_X2_MISC0r_CLR(reg_misc0_or_val);
       RX_X2_MISC0r_CHK_END_ENf_SET(reg_misc0_or_val, or_value);
       MODIFY_RX_X2_MISC0r(pc,reg_misc0_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_CHK_END_EN_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_BLKSYNC_MODE:
       RX_X4_DEC_CTL0r_CLR(reg_dec_or_val);
       RX_X4_DEC_CTL0r_BLOCK_SYNC_MODEf_SET(reg_dec_or_val,or_value);
       MODIFY_RX_X4_DEC_CTL0r(pc, reg_dec_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_BLOCK_SYNC_MODE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_REORDER_EN :
       RX_X4_CL36_RX0r_CLR(reg_cl36_or_val);
       RX_X4_CL36_RX0r_REORDER_ENf_SET(reg_cl36_or_val, or_value);
       MODIFY_RX_X4_CL36_RX0r(pc,reg_cl36_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN1_TYPEr_REORDER_EN_OENf_SET(reg_or_en1, 1);
       MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc,reg_or_en1);
  break;
  case TEMOD16_OVERRIDE_CL36_EN: 
       RX_X4_CL36_RX0r_CLR(reg_cl36_or_val);
       RX_X4_CL36_RX0r_CL36_ENf_SET(reg_cl36_or_val, or_value);
       MODIFY_RX_X4_CL36_RX0r(pc,reg_cl36_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN1_TYPEr_CL36_EN_OENf_SET(reg_or_en1, 1);
       MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc,reg_or_en1);
  break;
  case TEMOD16_OVERRIDE_SCR_MODE:
       TX_X4_MISCr_CLR(reg_misc_or_val);
       TX_X4_MISCr_SCR_MODEf_SET(reg_misc_or_val,or_value);
       MODIFY_TX_X4_MISCr(pc,reg_misc_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_SCR_MODE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_DESCR_MODE:
       RX_X4_PCS_CTL0r_CLR(reg_pcs_or_val);
       RX_X4_PCS_CTL0r_DESCRAMBLERMODEf_SET(reg_pcs_or_val,or_value);
       MODIFY_RX_X4_PCS_CTL0r(pc,reg_pcs_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_DESCRAMBLERMODE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_ENCODE_MODE:
       TX_X4_ENC0r_CLR(reg_encode_or_val);
       TX_X4_ENC0r_ENCODEMODEf_SET(reg_encode_or_val, or_value);
       MODIFY_TX_X4_ENC0r(pc,reg_encode_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_ENCODEMODE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_DECODE_MODE: 
       RX_X4_PCS_CTL0r_CLR(reg_pcs_or_val);
       RX_X4_PCS_CTL0r_DECODERMODEf_SET(reg_pcs_or_val,or_value);
       MODIFY_RX_X4_PCS_CTL0r(pc,reg_pcs_or_val);
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN0_TYPEr_DECODERMODE_OENf_SET(reg_or_en0,1);
       MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0);
  break;
  case TEMOD16_OVERRIDE_SPDID :
    /* get info. from table and apply it to all credit regs. */
  break;
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
int temod16_credit_override_set(PHYMOD_ST* pc, credit_type_t credit_type, int userCredit)   /* CREDIT_OVERRIDE_SET */
{
  SC_X4_FLD_OVRR_EN1_TYPEr_t    reg_enable_type;
  TX_X4_CRED0r_t          reg_credit0;
  TX_X4_CRED1r_t          reg_credit1;
  TX_X4_LOOPCNTr_t                        reg_loopcnt;
  TX_X4_MAC_CREDGENCNTr_t               reg_maccreditgen;
  TX_X4_PCS_CLKCNT0r_t                  reg_pcs_clk_cnt;
  TX_X4_PCS_CREDGENCNTr_t               reg_pcs_creditgen;

  TMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_FLD_OVRR_EN1_TYPEr_CLR(reg_enable_type);
  TX_X4_CRED0r_CLR(reg_credit0);
  TX_X4_CRED1r_CLR(reg_credit1);
  TX_X4_LOOPCNTr_CLR(reg_loopcnt);
  TX_X4_MAC_CREDGENCNTr_CLR(reg_maccreditgen);
  TX_X4_PCS_CLKCNT0r_CLR(reg_pcs_clk_cnt);
  TX_X4_PCS_CREDGENCNTr_CLR(reg_pcs_creditgen);

  switch ( credit_type ) {
    case TEMOD16_CREDIT_RESET: 
    /* reset credits */
       SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT0_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT1_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT0_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT1_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN1_TYPEr_REPLICATION_CNT_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN1_TYPEr_MAC_CREDITGENCNT_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN1_TYPEr_PCS_CREDITENABLE_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN1_TYPEr_PCS_CLOCKCNT0_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN1_TYPEr_PCS_CREDITGENCNT_OENf_SET(reg_enable_type, 0);
       SC_X4_FLD_OVRR_EN1_TYPEr_SGMII_SPD_SWITCH_OENf_SET(reg_enable_type, 0);

       PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_FLD_OVRR_EN1_TYPEr(pc, reg_enable_type));
      break;
    case TEMOD16_CREDIT_TABLE: 
    /* get info. from table and apply it to all credit regs. */
    break;
    case  TEMOD16_CREDIT_CLOCK_COUNT_0:
       /* write to clockcnt0 */
       TX_X4_CRED0r_CLOCKCNT0f_SET(reg_credit0, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED0r(pc, reg_credit0));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT0_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, reg_enable_type));
      break;
    case  TEMOD16_CREDIT_CLOCK_COUNT_1:  /* (cntl & 0x00020000) */
       /* write to clockcnt1 */
       TX_X4_CRED1r_CLOCKCNT1f_SET(reg_credit1, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED1r(pc, reg_credit1));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT1_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, reg_enable_type));
       break;
    case  TEMOD16_CREDIT_LOOP_COUNT_0: 
       /* write to loopcnt0 */
       TX_X4_LOOPCNTr_LOOPCNT0f_SET(reg_loopcnt, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_LOOPCNTr(pc, reg_loopcnt));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT0_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, reg_enable_type));
       break;
    case TEMOD16_CREDIT_LOOP_COUNT_1:   /* (cntl & 0x00080000) */
       /* write to pcs_clockcnt0 */
       TX_X4_LOOPCNTr_LOOPCNT1f_SET(reg_loopcnt, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_LOOPCNTr(pc, reg_loopcnt));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT1_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, reg_enable_type));
       break;
     case TEMOD16_CREDIT_MAC:   /* (cntl & 0x00100000) */
       /* write to mac_creditgencnt */
       TX_X4_MAC_CREDGENCNTr_MAC_CREDITGENCNTf_SET(reg_maccreditgen, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MAC_CREDGENCNTr(pc, reg_maccreditgen));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN1_TYPEr_MAC_CREDITGENCNT_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, reg_enable_type));
      break;
    case  TEMOD16_CREDIT_PCS_CLOCK_COUNT_0:   /* (cntl & 0x00200000) */
       /* write to pcs_clockcnt0 */
       TX_X4_PCS_CLKCNT0r_PCS_CLOCKCNT0f_SET(reg_pcs_clk_cnt, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_PCS_CLKCNT0r(pc, reg_pcs_clk_cnt));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN1_TYPEr_PCS_CLOCKCNT0_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, reg_enable_type));
      break;
    case TEMOD16_CREDIT_PCS_GEN_COUNT:   /* (cntl & 0x00400000) */
       /* write to pcs_creditgencnt */
       TX_X4_PCS_CREDGENCNTr_PCS_CREDITGENCNTf_SET(reg_pcs_creditgen, userCredit) ;
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_PCS_CREDGENCNTr(pc, reg_pcs_creditgen));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN1_TYPEr_PCS_CREDITGENCNT_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, reg_enable_type));
      break;
    case TEMOD16_CREDIT_EN: /* (cntl & 0x00800000) */
       /* write to pcs_crden */
       TX_X4_PCS_CLKCNT0r_PCS_CREDITENABLEf_SET(reg_pcs_clk_cnt, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_PCS_CLKCNT0r(pc, reg_pcs_clk_cnt));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN1_TYPEr_PCS_CREDITENABLE_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, reg_enable_type));
       break;
    case  TEMOD16_CREDIT_PCS_REPCNT:    /* (cntl & 0x01000000) */
       /* write to pcs_repcnt */
       TX_X4_PCS_CLKCNT0r_REPLICATION_CNTf_SET(reg_pcs_clk_cnt, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_PCS_CLKCNT0r(pc, reg_pcs_clk_cnt));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN1_TYPEr_REPLICATION_CNT_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, reg_enable_type));
       break;
    case  TEMOD16_CREDIT_SGMII_SPD: /* (cntl & 0x02000000) */
       /* write to sgmii */
       TX_X4_CRED0r_SGMII_SPD_SWITCHf_SET(reg_credit0, userCredit);
       PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED0r(pc, reg_credit0));
       /*Set the override enable*/
       SC_X4_FLD_OVRR_EN1_TYPEr_SGMII_SPD_SWITCH_OENf_SET(reg_enable_type, 1);
       PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, reg_enable_type));
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
int temod16_pmd_lane_reset(PHYMOD_ST* pc)              /* PMD_LANE_RESET */
{
  TMOD_DBG_IN_FUNC_INFO(pc);
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
int temod16_clause72_control(PHYMOD_ST* pc, cl72_type_t cl_72_type)                /* CLAUSE_72_CONTROL */
{
  return PHYMOD_E_NONE;
} /* clause72_control */
#endif /* _DV_TB_ */

#ifdef _DV_TB_
/**
@brief   Check PRBS sync status
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   real_check 
@param   prbs_status 
@returns PHYMOD_E_NONE if rbs lock with no errors; SOC_E_TIMEOUT for a timeout,
meaning that either the prbs did not lock, or that prbs had errors, or both
@details
This function reads from one of PATGEN0_PRBSSTAT0 register within 4 lanes.
Retunr value reads 0x8000, meaning that prbs has locked and there are no 
prbs errors, OR no lock (bit[15]=0) or errors (bit[14:0]!=0)

The errors are stored in #PHYMOD_ST::accData, as -2 if lost lock, -1 if no lock,
0 is locked and error free.  1-n means erros found.

\li PATGEN0_PRBSSTAT0 (0xC032)
\li PATGEN0_PRBSSTAT1 (0xC033)
\li PATGEN0_TESTMISCSTATUSr (0xC034)

*/
int temod16_prbs_check(PHYMOD_ST* pc, int real_check, int *prbs_status)
{
   TLB_RX_PRBS_CHK_LOCK_STSr_t      reg_prbs_chk_lock;
   TLB_RX_PRBS_CHK_ERR_CNT_MSB_STSr_t    reg_prbs_err_cnt_msb;
   TLB_RX_PRBS_CHK_ERR_CNT_LSB_STSr_t     reg_prbs_err_cnt_lsb;
   int lock, lock_lost, err ;

   TMOD_DBG_IN_FUNC_INFO(pc);
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
      if (TLB_RX_PRBS_CHK_ERR_CNT_MSB_STSr_PRBS_CHK_LOCK_LOST_LHf_GET(reg_prbs_err_cnt_msb)) {
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
/**
@brief   program the prbs settings
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   port 
@param   prbs_inv 
@param   pattern_type 
@param   prbs_check_mode 
@returns The value PHYMOD_E_NONE upon successful completion

The 1st byte of #PHYMOD_ST::per_lane_control is for TX and 2nd byte is 
for RX. The 3rd byte sets the prbs_check_mode. The bits of each byte control
their lanes as follows.

\li 3:3 invert_data
\li 2:0 polynomial mode (prbs7, prbs9, prbs11, prbs15, prbs23, prbs31, prbs58).

Third byte sets prbs_check_modes

\li 2:0 prbs_check_mode 
\li 0:self_sync_mode with hysteresis, PRBS seed register is continuously seeded with previously received bits.
\li 1:initial seed mode with hysteresis, PRBS seed register is seeded with previous received bits only till PRBS lock.
\li 2:initial seed mode without hysteresis, Similar to above mode except once locked it stays locked untill PRBS is disabled.

*/
int temod16_prbs_mode(PHYMOD_ST* pc, int port, int prbs_inv, int pattern_type, int prbs_check_mode)    /* PRBS_MODE */
{
  TLB_TX_PRBS_GEN_CFGr_t    reg_prbs_gen_cfg;
  TLB_RX_PRBS_CHK_CFGr_t    reg_prbs_chk_cfg;
  /* SC_X4_BYPASSr_t      reg_bypass; */

  /*int prbs_check_mode ; */
  int prbs_sel_tx, prbs_inv_tx ;
  int prbs_sel_rx, prbs_inv_rx ;

/* setting PCS in bypass mode */
  TMOD_DBG_IN_FUNC_INFO(pc);
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
/**
@brief   program the prbs settings
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   prbs_enable 
@returns The value PHYMOD_E_NONE upon successful completion

The 1st (TX), 2nd(RX) byte of #PHYMOD_ST::per_lane_control is associated
with TX and RX respectively. The bits of each byte control their
lanes as follows.

<ol>
<li> the  first byte [3:0] (0x-f) is for TX enable
<li> the  first byte [7:4] (0xf-) is for TX enable mask ;
<li> the second byte [3:0] is for RX enable
<li> the second byte [7:4] is for RX enable mask ;
</ol>

Port mode should become multiple lane.  Flex port support is TBD.

*/
int temod16_prbs_control(PHYMOD_ST* pc, int prbs_enable)                /* PRBS_CONTROL */
{
  TLB_TX_PRBS_GEN_CFGr_t    reg_prbs_gen_cfg;
  TLB_RX_PRBS_CHK_CFGr_t    reg_prbs_chk_cfg;
  int prbs_rx_en, prbs_tx_en ;

  TMOD_DBG_IN_FUNC_INFO(pc);
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
#temod16_spd_intfc_type and set in the
#PHYMOD_ST::spd_intf field.  To enable CJPAT, set #PHYMOD_ST::per_lane_control to
the value #USE_CJPAT; to enable CRPAT, set #PHYMOD_ST::per_lane_control to the
value #USE_CRPAT.  This function is used in tandem with temod16_cjpat_crpat_check().

*/

int temod16_cjpat_crpat_mode_set(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, int port, int pattern_type, int pkt_number, int pkt_size, int ipg_size)   /* CJPAT_CRPAT_MODE_SET  */
{
   int number_pkt, crc_check, intf_type, rx_port_sel, tx_port_sel, pkt_or_prtp ;
   int pkt_size_int, ipg_size_int, payload_type, prtp_data_pattern_sel, rx_prtp_en ;
   int tx_prtp_en;
   uint16_t data16 ;

   int rand_num;

   PKTGEN_CTL1r_t    reg_pktctrl1;
   PKTGEN_CTL2r_t    reg_pktctrl2;
   PKTGEN_PRTPCTLr_t    reg_prtpctrl;
   PKTGEN_PCS_SEEDA0r_t     reg_seeda0;
   PKTGEN_PCS_SEEDA1r_t     reg_seeda1;
   PKTGEN_PCS_SEEDA2r_t     reg_seeda2;
   PKTGEN_PCS_SEEDA3r_t     reg_seeda3;
   PKTGEN_PCS_SEEDB0r_t     reg_seedb0;
   PKTGEN_PCS_SEEDB1r_t     reg_seedb1;
   PKTGEN_PCS_SEEDB2r_t     reg_seedb2;
   PKTGEN_PCS_SEEDB3r_t     reg_seedb3;
   PKTGEN_PAYLOADBYTESr_t reg_payload_b;

   TMOD_DBG_IN_FUNC_INFO(pc);
   PKTGEN_CTL1r_CLR(reg_pktctrl1);
   PKTGEN_CTL2r_CLR(reg_pktctrl2);
   PKTGEN_PRTPCTLr_CLR(reg_prtpctrl);
   PKTGEN_PCS_SEEDA0r_CLR(reg_seeda0);
   PKTGEN_PCS_SEEDA1r_CLR(reg_seeda1);
   PKTGEN_PCS_SEEDA2r_CLR(reg_seeda2);
   PKTGEN_PCS_SEEDA3r_CLR(reg_seeda3);
   PKTGEN_PCS_SEEDB0r_CLR(reg_seedb0);
   PKTGEN_PCS_SEEDB1r_CLR(reg_seedb1);
   PKTGEN_PCS_SEEDB2r_CLR(reg_seedb2);
   PKTGEN_PCS_SEEDB3r_CLR(reg_seedb3);
   PKTGEN_PAYLOADBYTESr_CLR(reg_payload_b);

   number_pkt = pkt_number ; /* 2: unlimited, 0: idles, 1: single pkt */
   crc_check  = 1 ;
   rx_port_sel= port ;
   tx_port_sel= port ;
   pkt_or_prtp = 0 ; /* 0: pakcet generator;  1: PRTP(pseudo random test pattern) */
   rx_prtp_en = 0;
   tx_prtp_en = 0;
   if ((pattern_type == 0x9) | (pattern_type == 0xA)) {
     pkt_or_prtp = 1;
     rx_prtp_en = 1;
     tx_prtp_en = 1;
   }

   if (pattern_type == 0xA) {
     prtp_data_pattern_sel = 1;
   } else {
     prtp_data_pattern_sel = 0;
   }

/*   pkt_size_int   = pkt_size ; */
   pkt_size_int   = 1;
   ipg_size_int   = 8 ;
 /*  ipg_size_int   = ipg_size ; */

   /* need to check the interface type */
   if ((pattern_type == 0x9) | (pattern_type == 0xA)) {
     payload_type = 0;
   } else {
     payload_type = pattern_type;
   }


   /* intf_type  0: XLGMII/XGMII;  1: MIII/GMII */

   intf_type = ((spd_intf == TEMOD16_SPD_10_X1_SGMII) |
     (spd_intf == TEMOD16_SPD_100_X1_SGMII) |
     (spd_intf == TEMOD16_SPD_1000_X1_SGMII) |
     (spd_intf == TEMOD16_SPD_10_SGMII) |
     (spd_intf == TEMOD16_SPD_100_SGMII) |
     (spd_intf == TEMOD16_SPD_1000_SGMII) |
     (spd_intf == TEMOD16_SPD_2500) |
     (spd_intf == TEMOD16_SPD_2500_X1)) ? 1 : 0;

 /* if payload type is Repeat 2 Bytes in 0x9040*/

  if (payload_type == 0) {
    data16 = 0x2323;
    PKTGEN_PAYLOADBYTESr_BYTE1f_SET(reg_payload_b, ((data16 >> 8)  & 0xff));
    PKTGEN_PAYLOADBYTESr_BYTE0f_SET(reg_payload_b, (data16 & 0xff));
    WRITE_PKTGEN_PAYLOADBYTESr(pc, reg_payload_b);
  }

   /* 0x9030 */
   PKTGEN_CTL1r_PKT_OR_PRTPf_SET(reg_pktctrl1, pkt_or_prtp);
   PKTGEN_CTL1r_TX_TEST_PORT_SELf_SET(reg_pktctrl1, tx_port_sel);
   PKTGEN_CTL1r_RX_PORT_SELf_SET(reg_pktctrl1, rx_port_sel);
   PKTGEN_CTL1r_RX_PKT_CHECK_ENf_SET(reg_pktctrl1, crc_check);
   PKTGEN_CTL1r_RX_MSBUS_TYPEf_SET(reg_pktctrl1, intf_type);
   PKTGEN_CTL1r_NUMBER_PKTf_SET(reg_pktctrl1, number_pkt);
   PKTGEN_CTL1r_TX_PRTP_ENf_SET(reg_pktctrl1, tx_prtp_en);
   PKTGEN_CTL1r_PRTP_DATA_PATTERN_SELf_SET(reg_pktctrl1, prtp_data_pattern_sel);

   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGEN_CTL1r(pc, reg_pktctrl1));

   /* 0x9031 */
   PKTGEN_CTL2r_IPG_SIZEf_SET(reg_pktctrl2, ipg_size_int);
   PKTGEN_CTL2r_PKT_SIZEf_SET(reg_pktctrl2, pkt_size_int);
   PKTGEN_CTL2r_PAYLOAD_TYPEf_SET(reg_pktctrl2, payload_type);
   PKTGEN_CTL2r_TX_MSBUS_TYPEf_SET(reg_pktctrl2, intf_type);

   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGEN_CTL2r(pc, reg_pktctrl2));

   /* 0x9032 */
   PKTGEN_PRTPCTLr_RX_PRTP_ENf_SET(reg_prtpctrl,rx_prtp_en);
   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGEN_PRTPCTLr(pc, reg_prtpctrl));

/* program seeds if necessary */
   rand_num = 1234;
#ifdef _DV_TB_
   rand_num = rand ();
#endif

   data16 = rand_num & 0xFFFF;
   if ((pattern_type == 0x9) | (pattern_type == 0xA)) {
   PKTGEN_PCS_SEEDA0r_SEEDA0f_SET(reg_seeda0, data16);
   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGEN_PCS_SEEDA0r(pc, reg_seeda0));
   data16 = data16 + 1;
   PKTGEN_PCS_SEEDA1r_SEEDA1f_SET(reg_seeda1, data16);
   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGEN_PCS_SEEDA1r(pc, reg_seeda1));
   data16 = data16 + 1;
   PKTGEN_PCS_SEEDA2r_SEEDA2f_SET(reg_seeda2, data16);
   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGEN_PCS_SEEDA2r( pc, reg_seeda2));
   data16 = data16 + 1;
   PKTGEN_PCS_SEEDA3r_SEEDA3f_SET(reg_seeda3, data16);
   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGEN_PCS_SEEDA3r(pc, reg_seeda3));
   data16 = data16 + 1;
   PKTGEN_PCS_SEEDB0r_SEEDB0f_SET(reg_seedb0, data16);
   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGEN_PCS_SEEDB0r( pc, reg_seedb0));
   data16 = data16 + 1;
   PKTGEN_PCS_SEEDB1r_SEEDB1f_SET(reg_seedb1, data16);
   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGEN_PCS_SEEDB1r( pc, reg_seedb1));
   data16 = data16 + 1;
   PKTGEN_PCS_SEEDB2r_SEEDB2f_SET(reg_seedb2, data16);
   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGEN_PCS_SEEDB2r(pc, reg_seedb2));
   data16 = data16 + 1;
   PKTGEN_PCS_SEEDB3r_SEEDB3f_SET(reg_seedb3, data16);
   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGEN_PCS_SEEDB3r( pc, reg_seedb3));
   }
   return PHYMOD_E_NONE;
} /* int temod16_cjpat_crpat_mode_set */

/**
@brief   Checks CJPAT/CRPAT parameters for a particular port
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion

This function checks for CJPAT or CRPAT for a particular port.  The speed mode
of the lane must be chosen from the enumerated type #temod16_spd_intfc_type and
set in the #PHYMOD_ST::spd_intf field.  CJPAT/CRPAT must first enabled using
#temod16_cjpat_crpat_control().  The duration of packet transmission is controlled
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

int temod16_cjpat_crpat_check(PHYMOD_ST* pc)     /* CJPAT_CRPAT_CHECK */
{
   uint32_t tx_cnt, rx_cnt ;

   TXPKTCNT_Ur_t    reg_tx_u;
   TXPKTCNT_Lr_t    reg_tx_l;
   RXPKTCNT_Ur_t    reg_rx_u;
   RXPKTCNT_Lr_t    reg_rx_l;
   PKTGEN_CRCERRCNTr_t reg_crc_cnt;

   TMOD_DBG_IN_FUNC_INFO(pc);
   TXPKTCNT_Ur_CLR(reg_tx_u);
   READ_TXPKTCNT_Ur(pc, &reg_tx_u);
   TXPKTCNT_Lr_CLR(reg_tx_l);
   READ_TXPKTCNT_Lr(pc, &reg_tx_l);
   RXPKTCNT_Ur_CLR(reg_rx_u);
   READ_RXPKTCNT_Ur(pc, &reg_rx_u);
   RXPKTCNT_Lr_CLR(reg_rx_l);
   READ_RXPKTCNT_Lr(pc, &reg_rx_l);
   PKTGEN_CRCERRCNTr_CLR(reg_crc_cnt);
   READ_PKTGEN_CRCERRCNTr(pc, &reg_crc_cnt);

   tx_cnt = TXPKTCNT_Ur_GET(reg_tx_u) <<16;
   tx_cnt |= TXPKTCNT_Lr_GET(reg_tx_l);

   rx_cnt = RXPKTCNT_Ur_GET(reg_rx_u) <<16;
   rx_cnt |= RXPKTCNT_Lr_GET(reg_rx_l);

#ifdef _DV_TB_
   pc->accData = 1 ;
#endif /* _DV_TB_ */
   if ( (tx_cnt != rx_cnt) ||(tx_cnt==0) ) {
#ifdef _DV_TB_
  /*Check if teh SDK needs more info on failure */
      PHYMOD_DEBUG_ERROR(("%-22s ERROR: u=%0d p=%0d tx_cnt=%0d rx_cnt=%0d mismatch\n", __func__, pc->unit, pc->port, tx_cnt, rx_cnt));
      pc->accData = 0;
#endif /* _DV_TB_ */
      return PHYMOD_E_FAIL;
   }

   if (reg_crc_cnt.pktgen_crcerrcnt[0]) {
#ifdef _DV_TB_
  /*Check if teh SDK needs more info on failure */
      PHYMOD_DEBUG_ERROR(("%-22s ERROR: u=%0d p=%0d crc error=%0d\n", __func__, pc->unit, pc->port, reg_crc_cnt.pktgen_crcerrcnt[0]));
      pc->accData = 0;
#endif /* _DV_TB_ */

      return PHYMOD_E_FAIL;
   }
   return PHYMOD_E_NONE;
}

/**
@brief    Enables or disables the bit error rate testing for a particular lane.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable 
@returns The value PHYMOD_E_NONE upon successful completion.
@details

This function enables or disables bit-error-rate testing (BERT)
for a particular lane as indicated by #PHYMOD_ST::this_lane from data
generated by PRBS, CJPAT, or CRPAT.  This function must be used in
conjunction with #temod16_prbs_control() or #temod16_cjpat_crpat_control().

Using PRBS without this function results in traffic that is
continuous, but not packetized; using PRBS with this function
results in traffic that is packetized.  Currently only enabling CJPAT/CRPAT
is supported.

*/
int temod16_cjpat_crpat_control(PHYMOD_ST* pc, int enable)         /* CJPAT_CRPAT_CONTROL  */
{
  PKTGEN_CTL2r_t    reg;

  TMOD_DBG_IN_FUNC_INFO(pc);
  PKTGEN_CTL2r_CLR(reg);
  /* 0x9031 */
  PHYMOD_IF_ERR_RETURN
     (READ_PKTGEN_CTL2r(pc, &reg));

  if (enable) {
    PKTGEN_CTL2r_PKTGEN_ENf_SET(reg,  1);
  } else {
    PKTGEN_CTL2r_PKTGEN_ENf_SET(reg, 0);
  }
   PHYMOD_IF_ERR_RETURN
      (WRITE_PKTGEN_CTL2r(pc, reg));

  return PHYMOD_E_NONE;
}

/**
@brief   sets both the RX and TX lane swap values for all lanes simultaneously.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   lane_map  
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
int temod16_pcs_lane_swap(PHYMOD_ST *pc, int lane_map)
{
  unsigned int tx_lane_map_0, tx_lane_map_1, tx_lane_map_2, tx_lane_map_3;
  unsigned int rx_lane_map_0, rx_lane_map_1, rx_lane_map_2, rx_lane_map_3;
  MAIN0_LN_SWPr_t reg;
  TMOD_DBG_IN_FUNC_INFO(pc);
/*  lane_map = lane_map >> 16;*/

  MAIN0_LN_SWPr_CLR(reg);

  tx_lane_map_0 = (((lane_map >> 0)  & 0x3) );
  tx_lane_map_1 = (((lane_map >> 4)  & 0x3) );
  tx_lane_map_2 = (((lane_map >> 8)  & 0x3) );
  tx_lane_map_3 = (((lane_map >> 12) & 0x3) );

  rx_lane_map_0 = (((lane_map >> 16) & 0x3) );
  rx_lane_map_1 = (((lane_map >> 20) & 0x3) );
  rx_lane_map_2 = (((lane_map >> 24) & 0x3) );
  rx_lane_map_3 = (((lane_map >> 28) & 0x3) );

  MAIN0_LN_SWPr_LOG0_TO_PHY_TX_LNSWAP_SELf_SET(reg, tx_lane_map_0);
  MAIN0_LN_SWPr_LOG1_TO_PHY_TX_LNSWAP_SELf_SET(reg, tx_lane_map_1);
  MAIN0_LN_SWPr_LOG2_TO_PHY_TX_LNSWAP_SELf_SET(reg, tx_lane_map_2);
  MAIN0_LN_SWPr_LOG3_TO_PHY_TX_LNSWAP_SELf_SET(reg, tx_lane_map_3);

  MAIN0_LN_SWPr_LOG0_TO_PHY_RX_LNSWAP_SELf_SET(reg, rx_lane_map_0);
  MAIN0_LN_SWPr_LOG1_TO_PHY_RX_LNSWAP_SELf_SET(reg, rx_lane_map_1);
  MAIN0_LN_SWPr_LOG2_TO_PHY_RX_LNSWAP_SELf_SET(reg, rx_lane_map_2);
  MAIN0_LN_SWPr_LOG3_TO_PHY_RX_LNSWAP_SELf_SET(reg, rx_lane_map_3);

  PHYMOD_IF_ERR_RETURN
      (WRITE_MAIN0_LN_SWPr(pc, reg)) ;

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
int temod16_pmd_lane_swap(PHYMOD_ST *pc)
{
  LN_ADDR0r_t reg_adr0;
  LN_ADDR1r_t reg_adr1;
  LN_ADDR2r_t reg_adr2;
  LN_ADDR3r_t reg_adr3;

  unsigned int map;
  unsigned int rx_lane_addr_0, rx_lane_addr_1, rx_lane_addr_2, rx_lane_addr_3;
  unsigned int tx_lane_addr_0, tx_lane_addr_1, tx_lane_addr_2, tx_lane_addr_3;

  TMOD_DBG_IN_FUNC_INFO(pc);
  LN_ADDR0r_CLR(reg_adr0);
  LN_ADDR1r_CLR(reg_adr1);
  LN_ADDR2r_CLR(reg_adr2);
  LN_ADDR3r_CLR(reg_adr3);

  map = (unsigned int)pc->per_lane_control;

  tx_lane_addr_0 = (map >> 0) & 0xf;
  tx_lane_addr_1 = (map >> 4) & 0xf;
  tx_lane_addr_2 = (map >> 8) & 0xf;
  tx_lane_addr_3 = (map >> 12) & 0xf;
  rx_lane_addr_0 = (map >> 16) & 0xf;
  rx_lane_addr_1 = (map >> 20) & 0xf;
  rx_lane_addr_2 = (map >> 24) & 0xf;
  rx_lane_addr_3 = (map >> 28) & 0xf;

#ifdef _DV_TB_
  pc->adjust_port_mode = 1;
#endif

  LN_ADDR0r_TX_LANE_ADDR_0f_SET(reg_adr0, tx_lane_addr_0);
  LN_ADDR0r_RX_LANE_ADDR_0f_SET(reg_adr0, rx_lane_addr_0);
  PHYMOD_IF_ERR_RETURN
     (MODIFY_LN_ADDR0r(pc, reg_adr0));

  LN_ADDR1r_TX_LANE_ADDR_1f_SET(reg_adr1, tx_lane_addr_1);
  LN_ADDR1r_RX_LANE_ADDR_1f_SET(reg_adr1, rx_lane_addr_1);
  PHYMOD_IF_ERR_RETURN
     (MODIFY_LN_ADDR1r(pc, reg_adr1));

  LN_ADDR2r_TX_LANE_ADDR_2f_SET(reg_adr2, tx_lane_addr_2);
  LN_ADDR2r_RX_LANE_ADDR_2f_SET(reg_adr2, rx_lane_addr_2);
  PHYMOD_IF_ERR_RETURN
     (MODIFY_LN_ADDR2r(pc, reg_adr2));

  LN_ADDR3r_TX_LANE_ADDR_3f_SET(reg_adr3, tx_lane_addr_3);
  LN_ADDR3r_RX_LANE_ADDR_3f_SET(reg_adr3, rx_lane_addr_3);
  PHYMOD_IF_ERR_RETURN
     (MODIFY_LN_ADDR3r(pc, reg_adr3));


#ifdef _DV_TB_
  pc->adjust_port_mode = 0;
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
int temod16_pmd_lane_swap_tx(PHYMOD_ST *pc, uint32_t tx_lane_map_0, uint32_t tx_lane_map_1, uint32_t tx_lane_map_2, uint32_t tx_lane_map_3)
{
  return PHYMOD_E_NONE ;
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
int temod16_power_control(PHYMOD_ST* pc)
{
  TMOD_DBG_IN_FUNC_INFO(pc);
  /* To be implemented */
  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful
@details PCS Init calls this Per Port PCS Init routine
*/
int _temod16_set_port_mode_select(PHYMOD_ST* pc)
{

 int         port_mode_sel ;
 uint16_t      single_port_mode;
 MAIN0_SETUPr_t    reg     ;

 MAIN0_SETUPr_CLR(reg);
 port_mode_sel = 0 ;
  if (pc->tsc_touched == 1) {
     single_port_mode = 0 ;
     MAIN0_SETUPr_CLR(reg);
  } else {
     MAIN0_SETUPr_MASTER_PORT_NUMf_SET(reg, pc->master_portnum);
     MAIN0_SETUPr_PLL_RESET_ENf_SET(reg, 1);
  }

 switch (pc->port_type) {
   case TEMOD16_MULTI_PORT:   port_mode_sel = 0;  break ;
   case TEMOD16_TRI1_PORT:    port_mode_sel = 1;  break ;
   case TEMOD16_TRI2_PORT:    port_mode_sel = 2;  break ;
   case TEMOD16_DXGXS:        port_mode_sel = 3;  break ;
   case TEMOD16_SINGLE_PORT:  port_mode_sel = 4;  break ;
   default: break ;
 }

  if ((pc->sc_mode == TEMOD16_SC_MODE_AN_CL37) || (pc->sc_mode == TEMOD16_SC_MODE_AN_CL73)) {
      /*data = port_mode_sel << MAIN0_SETUP_PORT_MODE_SEL_SHIFT |*/
   MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(reg, pc->single_port_mode);
   MAIN0_SETUPr_AN_HIGH_VCOf_SET(reg, pc->cl37_high_vco);
   MAIN0_SETUPr_CL73_LOW_VCOf_SET(reg, pc->cl73_low_vco);
   MAIN0_SETUPr_REFCLK_SELf_SET(reg, pc->refclk);

    PHYMOD_IF_ERR_RETURN (MODIFY_MAIN0_SETUPr(pc, reg));
  } else {
   MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(reg, pc->single_port_mode);
   MAIN0_SETUPr_AN_HIGH_VCOf_SET(reg, pc->cl37_high_vco);
   MAIN0_SETUPr_CL73_LOW_VCOf_SET(reg, pc->cl73_low_vco);
   MAIN0_SETUPr_REFCLK_SELf_SET(reg, pc->refclk);
   MAIN0_SETUPr_PORT_MODE_SELf_SET(reg, port_mode_sel);

   PHYMOD_IF_ERR_RETURN (MODIFY_MAIN0_SETUPr(pc, reg));
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
  SC_X1_SPD_OVRR0_7r_t        reg0_ovr7;
  SC_X1_SPD_OVRR0_8r_t        reg0_ovr8;

  SC_X1_SPD_OVRR1_SPDr_t    reg1_ovr_spd;
  SC_X1_SPD_OVRR1_0r_t        reg1_ovr;
  SC_X1_SPD_OVRR1_1r_t        reg1_ovr1;
  SC_X1_SPD_OVRR1_2r_t        reg1_ovr2;
  SC_X1_SPD_OVRR1_3r_t        reg1_ovr3;
  SC_X1_SPD_OVRR1_4r_t        reg1_ovr4;
  SC_X1_SPD_OVRR1_5r_t        reg1_ovr5;
  SC_X1_SPD_OVRR1_6r_t        reg1_ovr6;
  SC_X1_SPD_OVRR1_7r_t        reg1_ovr7;
  SC_X1_SPD_OVRR1_8r_t        reg1_ovr8;

  SC_X1_SPD_OVRR2_SPDr_t    reg2_ovr_spd;
  SC_X1_SPD_OVRR2_0r_t        reg2_ovr;
  SC_X1_SPD_OVRR2_1r_t        reg2_ovr1;
  SC_X1_SPD_OVRR2_2r_t        reg2_ovr2;
  SC_X1_SPD_OVRR2_3r_t        reg2_ovr3;
  SC_X1_SPD_OVRR2_4r_t        reg2_ovr4;
  SC_X1_SPD_OVRR2_5r_t        reg2_ovr5;
  SC_X1_SPD_OVRR2_6r_t        reg2_ovr6;
  SC_X1_SPD_OVRR2_7r_t        reg2_ovr7;
  SC_X1_SPD_OVRR2_8r_t        reg2_ovr8;

  SC_X1_SPD_OVRR3_SPDr_t    reg3_ovr_spd;
  SC_X1_SPD_OVRR3_0r_t        reg3_ovr;
  SC_X1_SPD_OVRR3_1r_t        reg3_ovr1;
  SC_X1_SPD_OVRR3_2r_t        reg3_ovr2;
  SC_X1_SPD_OVRR3_3r_t        reg3_ovr3;
  SC_X1_SPD_OVRR3_4r_t        reg3_ovr4;
  SC_X1_SPD_OVRR3_5r_t        reg3_ovr5;
  SC_X1_SPD_OVRR3_6r_t        reg3_ovr6;
  SC_X1_SPD_OVRR3_7r_t        reg3_ovr7;
  SC_X1_SPD_OVRR3_8r_t        reg3_ovr8;

if (st_entry_num == 0) {

  SC_X1_SPD_OVRR0_SPDr_CLR(reg0_ovr_spd);
  SC_X1_SPD_OVRR0_SPDr_NUM_LANESf_SET(reg0_ovr_spd, pc->num_lanes);
  SC_X1_SPD_OVRR0_SPDr_SPEEDf_SET(reg0_ovr_spd, st_entry_speed);

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_SPDr(pc, reg0_ovr_spd));

  SC_X1_SPD_OVRR0_0r_CLR(reg0_ovr);
  SC_X1_SPD_OVRR0_1r_CLR(reg0_ovr1);
  SC_X1_SPD_OVRR0_2r_CLR(reg0_ovr2);
  SC_X1_SPD_OVRR0_3r_CLR(reg0_ovr3);
  SC_X1_SPD_OVRR0_4r_CLR(reg0_ovr4);
  SC_X1_SPD_OVRR0_5r_CLR(reg0_ovr5);
  SC_X1_SPD_OVRR0_6r_CLR(reg0_ovr6);
  SC_X1_SPD_OVRR0_7r_CLR(reg0_ovr7);
  SC_X1_SPD_OVRR0_8r_CLR(reg0_ovr8);

  SC_X1_SPD_OVRR0_0r_ENCODEMODEf_SET(reg0_ovr, pc->t_encode_mode);
  SC_X1_SPD_OVRR0_0r_SCR_MODEf_SET(reg0_ovr, pc->t_scr_mode);
  SC_X1_SPD_OVRR0_0r_OS_MODEf_SET(reg0_ovr, pc->t_pma_os_mode);

/* can remove this once cl72 is part of regular flow testing. Just for AN
 * id picked from ST tests*/
  if (pc->cl72_en &  0x40000000)
      SC_X1_SPD_OVRR0_0r_CL72_ENABLEf_SET(reg0_ovr, pc->cl72_en) ;

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_0r(pc, reg0_ovr));

  SC_X1_SPD_OVRR0_1r_BRCM64B66_DESCRAMBLER_ENABLEf_SET(reg0_ovr1, pc->r_dec1_brcm64b66_descr);
  SC_X1_SPD_OVRR0_1r_CL36BYTEDELETEMOD16Ef_SET(reg0_ovr1, pc->r_desc2_byte_deletion);
  SC_X1_SPD_OVRR0_1r_DESC2_MODEf_SET(reg0_ovr1, pc->r_desc2_mode);
  SC_X1_SPD_OVRR0_1r_DESKEWMODEf_SET(reg0_ovr1, pc->r_deskew_mode);
  SC_X1_SPD_OVRR0_1r_DECODERMODEf_SET(reg0_ovr1, pc->r_dec1_mode);
  SC_X1_SPD_OVRR0_1r_DESCRAMBLERMODEf_SET(reg0_ovr1, pc->r_descr1_mode);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_1r(pc, reg0_ovr1));

  SC_X1_SPD_OVRR0_2r_CL36_ENf_SET(reg0_ovr2, pc->cl36_en);
  SC_X1_SPD_OVRR0_2r_REORDER_ENf_SET(reg0_ovr2, pc->r_reorder_mode);
  SC_X1_SPD_OVRR0_2r_BLOCK_SYNC_MODEf_SET(reg0_ovr2, pc->blk_sync_mode);
  SC_X1_SPD_OVRR0_2r_CHK_END_ENf_SET(reg0_ovr2, pc->cl48_check_end);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_2r(pc, reg0_ovr2));

  SC_X1_SPD_OVRR0_3r_CLOCKCNT0f_SET(reg0_ovr3, pc->clkcnt0);
  SC_X1_SPD_OVRR0_3r_SGMII_SPD_SWITCHf_SET(reg0_ovr3, pc->sgmii);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_3r(pc, reg0_ovr3));

  SC_X1_SPD_OVRR0_4r_CLOCKCNT1f_SET(reg0_ovr4, pc->clkcnt1);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_4r(pc, reg0_ovr4));

  SC_X1_SPD_OVRR0_5r_LOOPCNT1f_SET(reg0_ovr5, pc->lpcnt1);
  SC_X1_SPD_OVRR0_5r_LOOPCNT0f_SET(reg0_ovr5, pc->lpcnt0);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_5r(pc, reg0_ovr5));

  SC_X1_SPD_OVRR0_6r_MAC_CREDITGENCNTf_SET(reg0_ovr6, pc->mac_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_6r(pc, reg0_ovr6));
  SC_X1_SPD_OVRR0_7r_PCS_CLOCKCNT0f_SET(reg0_ovr7, pc->pcs_clkcnt);
  SC_X1_SPD_OVRR0_7r_PCS_CREDITENABLEf_SET(reg0_ovr7, pc->pcs_crdten);
  SC_X1_SPD_OVRR0_7r_REPLICATION_CNTf_SET(reg0_ovr7, pc->pcs_repcnt);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_7r(pc, reg0_ovr7));

  SC_X1_SPD_OVRR0_8r_PCS_CREDITGENCNTf_SET(reg0_ovr8, pc->pcs_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_8r(pc, reg0_ovr8));
} /* (st_entry_num == 0) */

else if (st_entry_num == 1) {


 SC_X1_SPD_OVRR1_SPDr_CLR(reg1_ovr_spd);
  SC_X1_SPD_OVRR1_SPDr_NUM_LANESf_SET(reg1_ovr_spd, pc->num_lanes);
  SC_X1_SPD_OVRR1_SPDr_SPEEDf_SET(reg1_ovr_spd, st_entry_speed);

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_SPDr(pc, reg1_ovr_spd));

  SC_X1_SPD_OVRR1_0r_CLR(reg1_ovr);
  SC_X1_SPD_OVRR1_1r_CLR(reg1_ovr1);
  SC_X1_SPD_OVRR1_2r_CLR(reg1_ovr2);
  SC_X1_SPD_OVRR1_3r_CLR(reg1_ovr3);
  SC_X1_SPD_OVRR1_4r_CLR(reg1_ovr4);
  SC_X1_SPD_OVRR1_5r_CLR(reg1_ovr5);
  SC_X1_SPD_OVRR1_6r_CLR(reg1_ovr6);
  SC_X1_SPD_OVRR1_7r_CLR(reg1_ovr7);
  SC_X1_SPD_OVRR1_8r_CLR(reg1_ovr8);

  SC_X1_SPD_OVRR1_0r_ENCODEMODEf_SET(reg1_ovr, pc->t_encode_mode);
  SC_X1_SPD_OVRR1_0r_SCR_MODEf_SET(reg1_ovr, pc->t_scr_mode);
  SC_X1_SPD_OVRR1_0r_OS_MODEf_SET(reg1_ovr, pc->t_pma_os_mode);

/* can remove this once cl72 is part of regular flow testing. Just for AN
 * id picked from ST tests*/
  if (pc->cl72_en &  0x40000000)
      SC_X1_SPD_OVRR1_0r_CL72_ENABLEf_SET(reg1_ovr, pc->cl72_en) ;

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_0r(pc, reg1_ovr));

  SC_X1_SPD_OVRR1_1r_BRCM64B66_DESCRAMBLER_ENABLEf_SET(reg1_ovr1, pc->r_dec1_brcm64b66_descr);
  SC_X1_SPD_OVRR1_1r_CL36BYTEDELETEMOD16Ef_SET(reg1_ovr1, pc->r_desc2_byte_deletion);
  SC_X1_SPD_OVRR1_1r_DESC2_MODEf_SET(reg1_ovr1, pc->r_desc2_mode);
  SC_X1_SPD_OVRR1_1r_DESKEWMODEf_SET(reg1_ovr1, pc->r_deskew_mode);
  SC_X1_SPD_OVRR1_1r_DECODERMODEf_SET(reg1_ovr1, pc->r_dec1_mode);
  SC_X1_SPD_OVRR1_1r_DESCRAMBLERMODEf_SET(reg1_ovr1, pc->r_descr1_mode);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_1r(pc, reg1_ovr1));

  SC_X1_SPD_OVRR1_2r_CL36_ENf_SET(reg1_ovr2, pc->cl36_en);
  SC_X1_SPD_OVRR1_2r_REORDER_ENf_SET(reg1_ovr2, pc->r_reorder_mode);
  SC_X1_SPD_OVRR1_2r_BLOCK_SYNC_MODEf_SET(reg1_ovr2, pc->blk_sync_mode);
  SC_X1_SPD_OVRR1_2r_CHK_END_ENf_SET(reg1_ovr2, pc->cl48_check_end);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_2r(pc, reg1_ovr2));

  SC_X1_SPD_OVRR1_3r_CLOCKCNT0f_SET(reg1_ovr3, pc->clkcnt0);
  SC_X1_SPD_OVRR1_3r_SGMII_SPD_SWITCHf_SET(reg1_ovr3, pc->sgmii);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_3r(pc, reg1_ovr3));

  SC_X1_SPD_OVRR1_4r_CLOCKCNT1f_SET(reg1_ovr4, pc->clkcnt1);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_4r(pc, reg1_ovr4));

  SC_X1_SPD_OVRR1_5r_LOOPCNT1f_SET(reg1_ovr5, pc->lpcnt1);
  SC_X1_SPD_OVRR1_5r_LOOPCNT0f_SET(reg1_ovr5, pc->lpcnt0);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_5r(pc, reg1_ovr5));

  SC_X1_SPD_OVRR1_6r_MAC_CREDITGENCNTf_SET(reg1_ovr6, pc->mac_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_6r(pc, reg1_ovr6));
  SC_X1_SPD_OVRR1_7r_PCS_CLOCKCNT0f_SET(reg1_ovr7, pc->pcs_clkcnt);
  SC_X1_SPD_OVRR1_7r_PCS_CREDITENABLEf_SET(reg1_ovr7, pc->pcs_crdten);
  SC_X1_SPD_OVRR1_7r_REPLICATION_CNTf_SET(reg1_ovr7, pc->pcs_repcnt);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_7r(pc, reg1_ovr7));

  SC_X1_SPD_OVRR1_8r_PCS_CREDITGENCNTf_SET(reg1_ovr8, pc->pcs_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_8r(pc, reg1_ovr8));



} /* (st_entry_num == 1) */

else if (st_entry_num == 2) {

 SC_X1_SPD_OVRR2_SPDr_CLR(reg2_ovr_spd);
  SC_X1_SPD_OVRR2_SPDr_NUM_LANESf_SET(reg2_ovr_spd, pc->num_lanes);
  SC_X1_SPD_OVRR2_SPDr_SPEEDf_SET(reg2_ovr_spd, st_entry_speed);

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_SPDr(pc, reg2_ovr_spd));

  SC_X1_SPD_OVRR2_0r_CLR(reg2_ovr);
  SC_X1_SPD_OVRR2_1r_CLR(reg2_ovr1);
  SC_X1_SPD_OVRR2_2r_CLR(reg2_ovr2);
  SC_X1_SPD_OVRR2_3r_CLR(reg2_ovr3);
  SC_X1_SPD_OVRR2_4r_CLR(reg2_ovr4);
  SC_X1_SPD_OVRR2_5r_CLR(reg2_ovr5);
  SC_X1_SPD_OVRR2_6r_CLR(reg2_ovr6);
  SC_X1_SPD_OVRR2_7r_CLR(reg2_ovr7);
  SC_X1_SPD_OVRR2_8r_CLR(reg2_ovr8);

  SC_X1_SPD_OVRR2_0r_ENCODEMODEf_SET(reg2_ovr, pc->t_encode_mode);
  SC_X1_SPD_OVRR2_0r_SCR_MODEf_SET(reg2_ovr, pc->t_scr_mode);
  SC_X1_SPD_OVRR2_0r_OS_MODEf_SET(reg2_ovr, pc->t_pma_os_mode);

/* can remove this once cl72 is part of regular flow testing. Just for AN
 * id picked from ST tests*/
  if (pc->cl72_en &  0x40000000)
      SC_X1_SPD_OVRR2_0r_CL72_ENABLEf_SET(reg2_ovr, pc->cl72_en) ;

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_0r(pc, reg2_ovr));

  SC_X1_SPD_OVRR2_1r_BRCM64B66_DESCRAMBLER_ENABLEf_SET(reg2_ovr1, pc->r_dec1_brcm64b66_descr);
  SC_X1_SPD_OVRR2_1r_CL36BYTEDELETEMOD16Ef_SET(reg2_ovr1, pc->r_desc2_byte_deletion);
  SC_X1_SPD_OVRR2_1r_DESC2_MODEf_SET(reg2_ovr1, pc->r_desc2_mode);
  SC_X1_SPD_OVRR2_1r_DESKEWMODEf_SET(reg2_ovr1, pc->r_deskew_mode);
  SC_X1_SPD_OVRR2_1r_DECODERMODEf_SET(reg2_ovr1, pc->r_dec1_mode);
  SC_X1_SPD_OVRR2_1r_DESCRAMBLERMODEf_SET(reg2_ovr1, pc->r_descr1_mode);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_1r(pc, reg2_ovr1));

  SC_X1_SPD_OVRR2_2r_CL36_ENf_SET(reg2_ovr2, pc->cl36_en);
  SC_X1_SPD_OVRR2_2r_REORDER_ENf_SET(reg2_ovr2, pc->r_reorder_mode);
  SC_X1_SPD_OVRR2_2r_BLOCK_SYNC_MODEf_SET(reg2_ovr2, pc->blk_sync_mode);
  SC_X1_SPD_OVRR2_2r_CHK_END_ENf_SET(reg2_ovr2, pc->cl48_check_end);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_2r(pc, reg2_ovr2));

  SC_X1_SPD_OVRR2_3r_CLOCKCNT0f_SET(reg2_ovr3, pc->clkcnt0);
  SC_X1_SPD_OVRR2_3r_SGMII_SPD_SWITCHf_SET(reg2_ovr3, pc->sgmii);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_3r(pc, reg2_ovr3));

  SC_X1_SPD_OVRR2_4r_CLOCKCNT1f_SET(reg2_ovr4, pc->clkcnt1);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_4r(pc, reg2_ovr4));

  SC_X1_SPD_OVRR2_5r_LOOPCNT1f_SET(reg2_ovr5, pc->lpcnt1);
  SC_X1_SPD_OVRR2_5r_LOOPCNT0f_SET(reg2_ovr5, pc->lpcnt0);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_5r(pc, reg2_ovr5));

  SC_X1_SPD_OVRR2_6r_MAC_CREDITGENCNTf_SET(reg2_ovr6, pc->mac_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_6r(pc, reg2_ovr6));
  SC_X1_SPD_OVRR2_7r_PCS_CLOCKCNT0f_SET(reg2_ovr7, pc->pcs_clkcnt);
  SC_X1_SPD_OVRR2_7r_PCS_CREDITENABLEf_SET(reg2_ovr7, pc->pcs_crdten);
  SC_X1_SPD_OVRR2_7r_REPLICATION_CNTf_SET(reg2_ovr7, pc->pcs_repcnt);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_7r(pc, reg2_ovr7));

  SC_X1_SPD_OVRR2_8r_PCS_CREDITGENCNTf_SET(reg2_ovr8, pc->pcs_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_8r(pc, reg2_ovr8));
} /* (st_entry_num == 2) */

else  /*(st_entry_num == 3) */ {

 SC_X1_SPD_OVRR3_SPDr_CLR(reg3_ovr_spd);
  SC_X1_SPD_OVRR3_SPDr_NUM_LANESf_SET(reg3_ovr_spd, pc->num_lanes);
  SC_X1_SPD_OVRR3_SPDr_SPEEDf_SET(reg3_ovr_spd, st_entry_speed);

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_SPDr(pc, reg3_ovr_spd));

  SC_X1_SPD_OVRR3_0r_CLR(reg3_ovr);
  SC_X1_SPD_OVRR3_1r_CLR(reg3_ovr1);
  SC_X1_SPD_OVRR3_2r_CLR(reg3_ovr2);
  SC_X1_SPD_OVRR3_3r_CLR(reg3_ovr3);
  SC_X1_SPD_OVRR3_4r_CLR(reg3_ovr4);
  SC_X1_SPD_OVRR3_5r_CLR(reg3_ovr5);
  SC_X1_SPD_OVRR3_6r_CLR(reg3_ovr6);
  SC_X1_SPD_OVRR3_7r_CLR(reg3_ovr7);
  SC_X1_SPD_OVRR3_8r_CLR(reg3_ovr8);

  SC_X1_SPD_OVRR3_0r_ENCODEMODEf_SET(reg3_ovr, pc->t_encode_mode);
  SC_X1_SPD_OVRR3_0r_SCR_MODEf_SET(reg3_ovr, pc->t_scr_mode);
  SC_X1_SPD_OVRR3_0r_OS_MODEf_SET(reg3_ovr, pc->t_pma_os_mode);

/*can remove this once cl72 is part of regular flow testing. Just for AN
 * id picked from ST tests*/
  if (pc->cl72_en &  0x40000000)
      SC_X1_SPD_OVRR3_0r_CL72_ENABLEf_SET(reg3_ovr, pc->cl72_en) ;

  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_0r(pc, reg3_ovr));

  SC_X1_SPD_OVRR3_1r_BRCM64B66_DESCRAMBLER_ENABLEf_SET(reg3_ovr1, pc->r_dec1_brcm64b66_descr);
  SC_X1_SPD_OVRR3_1r_CL36BYTEDELETEMOD16Ef_SET(reg3_ovr1, pc->r_desc2_byte_deletion);
  SC_X1_SPD_OVRR3_1r_DESC2_MODEf_SET(reg3_ovr1, pc->r_desc2_mode);
  SC_X1_SPD_OVRR3_1r_DESKEWMODEf_SET(reg3_ovr1, pc->r_deskew_mode);
  SC_X1_SPD_OVRR3_1r_DECODERMODEf_SET(reg3_ovr1, pc->r_dec1_mode);
  SC_X1_SPD_OVRR3_1r_DESCRAMBLERMODEf_SET(reg3_ovr1, pc->r_descr1_mode);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_1r(pc, reg3_ovr1));

  SC_X1_SPD_OVRR3_2r_CL36_ENf_SET(reg3_ovr2, pc->cl36_en);
  SC_X1_SPD_OVRR3_2r_REORDER_ENf_SET(reg3_ovr2, pc->r_reorder_mode);
  SC_X1_SPD_OVRR3_2r_BLOCK_SYNC_MODEf_SET(reg3_ovr2, pc->blk_sync_mode);
  SC_X1_SPD_OVRR3_2r_CHK_END_ENf_SET(reg3_ovr2, pc->cl48_check_end);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_2r(pc, reg3_ovr2));

  SC_X1_SPD_OVRR3_3r_CLOCKCNT0f_SET(reg3_ovr3, pc->clkcnt0);
  SC_X1_SPD_OVRR3_3r_SGMII_SPD_SWITCHf_SET(reg3_ovr3, pc->sgmii);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_3r(pc, reg3_ovr3));

  SC_X1_SPD_OVRR3_4r_CLOCKCNT1f_SET(reg3_ovr4, pc->clkcnt1);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_4r(pc, reg3_ovr4));

  SC_X1_SPD_OVRR3_5r_LOOPCNT1f_SET(reg3_ovr5, pc->lpcnt1);
  SC_X1_SPD_OVRR3_5r_LOOPCNT0f_SET(reg3_ovr5, pc->lpcnt0);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_5r(pc, reg3_ovr5));

  SC_X1_SPD_OVRR3_6r_MAC_CREDITGENCNTf_SET(reg3_ovr6, pc->mac_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_6r(pc, reg3_ovr6));
  SC_X1_SPD_OVRR3_7r_PCS_CLOCKCNT0f_SET(reg3_ovr7, pc->pcs_clkcnt);
  SC_X1_SPD_OVRR3_7r_PCS_CREDITENABLEf_SET(reg3_ovr7, pc->pcs_crdten);
  SC_X1_SPD_OVRR3_7r_REPLICATION_CNTf_SET(reg3_ovr7, pc->pcs_repcnt);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_7r(pc, reg3_ovr7));

  SC_X1_SPD_OVRR3_8r_PCS_CREDITGENCNTf_SET(reg3_ovr8, pc->pcs_cgc);
  PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_8r(pc, reg3_ovr8));
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
int _configure_sc_speed_configuration(PHYMOD_ST* pc)
{
  int data =0;
  int mask =0;
  int st_entry =0;
  int speed_entry = 0;
  PHYMOD_ST pc_st_tmp;
  PHYMOD_ST* pc_st;

  if (pc->sc_mode == TEMOD16_SC_MODE_HT_WITH_BASIC_OVERRIDE) {
    /* Configure the ffec_eb regsiter */
    data |= (1 << 10);
    mask |= 0x0400;

  }
  else if (pc->sc_mode == TEMOD16_SC_MODE_ST_OVERRIDE) {
  }
  else if (pc->sc_mode == TEMOD16_SC_MODE_ST) {
     st_entry = pc->per_lane_control & 0xff;
     speed_entry = (pc->per_lane_control & 0xff00) >> 8;

/*
    A pc_st_tmp is created so the overriden expected values passed from SV are not 
    loaded in ST, and teh ST has the same HT values
*/
     init_temod16_st(&pc_st_tmp);
     pc_st_tmp.spd_intf = pc->spd_intf;
     pc_st_tmp.prt_ad = pc->prt_ad;
     pc_st_tmp.unit = pc->unit;
     pc_st_tmp.port_type = pc->port_type;
     pc_st_tmp.this_lane = pc->this_lane;
     pc_st_tmp.sc_mode = pc->sc_mode;
     pc_st_tmp.cl72_en = pc->cl72_en;
     pc_st = &pc_st_tmp;
     temod16_sc_lkup_table(pc_st);
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
int _init_pcs_an(PHYMOD_ST* pc)             
{
  UC_CLK_CTL0r_t reg_ctrl0;
  UC_RST_CTL0r_t reg_rst_ctrl0;

  UC_CLK_CTL0r_CLR(reg_ctrl0);
  UC_RST_CTL0r_CLR(reg_rst_ctrl0);

  UC_CLK_CTL0r_MICRO_MASTER_CLK_ENf_SET(reg_ctrl0, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_UC_CLK_CTL0r(pc, reg_ctrl0));

  UC_RST_CTL0r_MICRO_MASTER_RSTBf_SET(reg_rst_ctrl0, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_UC_RST_CTL0r(pc, reg_rst_ctrl0));

  temod16_set_an_port_mode(pc);
  pc->port_type=TEMOD16_MULTI_PORT;
  temod16_autoneg_set(pc);
  temod16_autoneg_control(pc);

   return PHYMOD_E_NONE;
}
#endif /* _DV_TB_ */
#ifdef _SDK_TEMOD16_TEMP_ 
int _init_pcs_an(PHYMOD_ST* pc)             
{
  temod16_an_set_type_t ata_type;
  temod16_tech_ability an_tech_ability = 0; 
  temod16_cl37bam_ability an_cl37bam_ability = 0;
  temod16_an_mode_type_t an_mode = 0;
  int data;
    
  /* temod16_autoneg_init(pc); */
  temod16_autoneg_timer_init(pc); 
  /* temod16_tick_override_set(pc, pc->an_tick_override, pc->an_tick_numerator, pc->an_tick_denominator); */
  temod16_set_an_port_mode(pc, 1, pc->no_of_lanes, pc->this_lane, pc->single_port);

  /* TBD: add locgic to port the info from pc to an_ability_st and an_control */
  temod16_autoneg_set( pc, an_ability_st);
  temod16_autoneg_control(pc, an_control)

  return PHYMOD_E_NONE;
}
#endif /* _SDK_TEMOD16_TEMP_ */

/**
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init
*/

int temod16_trigger_speed_change(PHYMOD_ST* pc)
{
  SC_X4_CTLr_t    reg; 
  
  TMOD_DBG_IN_FUNC_INFO(pc);
#ifdef _DV_TB_
  pc->adjust_port_mode = 1; 
#endif /* _DV_TB_ */

  /* write 0 to the speed change */
  SC_X4_CTLr_CLR(reg);
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc, &reg));
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg, 0);
  PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_CTLr(pc, reg));

  /* write 1 to the speed change. No need to read again before write*/
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg, 1);
  PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_CTLr(pc, reg));

#ifdef _DV_TB_
  pc->adjust_port_mode = 0; 
#endif /* _DV_TB_ */

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
   SC_X4_RSLVD_SPDr_t    reg_resolved_spd;
   SC_X4_RSLVD0r_t        reg_resolved_0;
   SC_X4_RSLVD1r_t        reg_resolved_1;
   SC_X4_RSLVD2r_t        reg_resolved_2;
   SC_X4_RSLVD3r_t        reg_resolved_3;
   SC_X4_RSLVD4r_t        reg_resolved_4;
   SC_X4_RSLVD5r_t        reg_resolved_5;
   SC_X4_RSLVD6r_t        reg_resolved_6;
   SC_X4_RSLVD7r_t        reg_resolved_7;
   SC_X4_RSLVD8r_t        reg_resolved_8;

   SC_X4_RSLVD_SPDr_CLR(reg_resolved_spd);
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD_SPDr(pc, &reg_resolved_spd));
   sc_final_stats_actual.num_lanes = SC_X4_RSLVD_SPDr_NUM_LANESf_GET(reg_resolved_spd);
   actual_speed = SC_X4_RSLVD_SPDr_SPEEDf_GET(reg_resolved_spd);
   sc_final_stats_actual.speed = actual_speed;

   SC_X4_RSLVD0r_CLR(reg_resolved_0);
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD0r(pc, &reg_resolved_0));
   sc_final_stats_actual.t_encode_mode = SC_X4_RSLVD0r_ENCODEMODEf_GET(reg_resolved_0);
   sc_final_stats_actual.cl72_en = SC_X4_RSLVD0r_CL72_ENABLEf_GET(reg_resolved_0);
   sc_final_stats_actual.fec_en = SC_X4_RSLVD0r_FEC_ENABLEf_GET(reg_resolved_0);
   sc_final_stats_actual.t_scr_mode = SC_X4_RSLVD0r_SCR_MODEf_GET(reg_resolved_0);
   sc_final_stats_actual.t_pma_os_mode = SC_X4_RSLVD0r_OS_MODEf_GET(reg_resolved_0);

   SC_X4_RSLVD1r_CLR(reg_resolved_1);
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD1r(pc, &reg_resolved_1));
   sc_final_stats_actual.r_dec1_brcm64b66_descr = SC_X4_RSLVD1r_BRCM64B66_DESCRAMBLER_ENABLEf_GET(reg_resolved_1);
   sc_final_stats_actual.r_desc2_byte_deletion = SC_X4_RSLVD1r_CL36BYTEDELETEMOD16Ef_GET(reg_resolved_1);
   sc_final_stats_actual.r_desc2_mode = SC_X4_RSLVD1r_DESC2_MODEf_GET(reg_resolved_1);
   sc_final_stats_actual.r_deskew_mode = SC_X4_RSLVD1r_DESKEWMODEf_GET(reg_resolved_1);
   sc_final_stats_actual.r_dec1_mode = SC_X4_RSLVD1r_DECODERMODEf_GET(reg_resolved_1);
   sc_final_stats_actual.r_descr1_mode = SC_X4_RSLVD1r_DESCRAMBLERMODEf_GET(reg_resolved_1);

   SC_X4_RSLVD2r_CLR(reg_resolved_2);
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD2r(pc, &reg_resolved_2));
   sc_final_stats_actual.cl36_en = SC_X4_RSLVD2r_CL36_ENf_GET(reg_resolved_2);
   sc_final_stats_actual.r_reorder_mode = SC_X4_RSLVD2r_REORDER_ENf_GET(reg_resolved_2);
   sc_final_stats_actual.blk_sync_mode =  SC_X4_RSLVD2r_BLOCK_SYNC_MODEf_GET(reg_resolved_2);
   sc_final_stats_actual.cl48_check_end = SC_X4_RSLVD2r_CHK_END_ENf_GET(reg_resolved_2);

   SC_X4_RSLVD3r_CLR(reg_resolved_3);
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD3r(pc, &reg_resolved_3));
   sc_final_stats_actual.clkcnt0 = SC_X4_RSLVD3r_CLOCKCNT0f_GET(reg_resolved_3);
   sc_final_stats_actual.sgmii =  SC_X4_RSLVD3r_SGMII_SPD_SWITCHf_GET(reg_resolved_3);

   SC_X4_RSLVD4r_CLR(reg_resolved_4);
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD4r(pc, &reg_resolved_4));
   sc_final_stats_actual.clkcnt1 = SC_X4_RSLVD4r_CLOCKCNT1f_GET(reg_resolved_4);

   SC_X4_RSLVD5r_CLR(reg_resolved_5);
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD5r(pc, &reg_resolved_5));
   sc_final_stats_actual.lpcnt1 = SC_X4_RSLVD5r_LOOPCNT1f_GET(reg_resolved_5);
   sc_final_stats_actual.lpcnt0 = SC_X4_RSLVD5r_LOOPCNT0f_GET(reg_resolved_5);

   SC_X4_RSLVD6r_CLR(reg_resolved_6);
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD6r(pc, &reg_resolved_6));
   sc_final_stats_actual.mac_cgc = SC_X4_RSLVD6r_MAC_CREDITGENCNTf_GET(reg_resolved_6);

   SC_X4_RSLVD7r_CLR(reg_resolved_7);
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD7r(pc, &reg_resolved_7));
   sc_final_stats_actual.pcs_clkcnt = SC_X4_RSLVD7r_PCS_CLOCKCNT0f_GET(reg_resolved_7);
   sc_final_stats_actual.pcs_crdten = SC_X4_RSLVD7r_PCS_CREDITENABLEf_GET(reg_resolved_7);
   sc_final_stats_actual.pcs_repcnt = SC_X4_RSLVD7r_REPLICATION_CNTf_GET(reg_resolved_7);

   SC_X4_RSLVD8r_CLR(reg_resolved_8);
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD8r(pc, &reg_resolved_8));
   sc_final_stats_actual.pcs_cgc = SC_X4_RSLVD8r_PCS_CREDITGENCNTf_GET(reg_resolved_8);

   /* Compare the actual and expected stats */
   fail = 0;


   if (pc->sc_mode == TEMOD16_SC_MODE_ST) {
     if (actual_speed != (pc->per_lane_control >> 8))
        fail = 1; 
   } else if ((pc->per_lane_control & 0xffff) == (0x004 | (TEMOD16_SPD_10000_XFI << 8))) { /* DV Verification Only */
     if (actual_speed != digital_operationSpeeds_SPEED_10G_KR1)
        fail = 1; 
   } else if ((pc->per_lane_control & 0xffff) == (0x004 | (TEMOD16_SPD_10_SGMII << 8)))  { /* DV Verification Only */
     if (actual_speed != digital_operationSpeeds_SPEED_10M)
        fail = 1; 
   } else if ((pc->per_lane_control & 0xffff) == (0x004 | (TEMOD16_SPD_5000_XFI << 8)))  { /* DV Verification Only */
     if (actual_speed != digital_operationSpeeds_SPEED_5G_KR1)
        fail = 1; 
   } else {
     if (actual_speed != pc->speed)
        fail = 1; 
   }

   if (sc_final_stats_actual.num_lanes != pc->num_lanes)
      fail = 1;
   if (sc_final_stats_actual.t_pma_os_mode != pc->t_pma_os_mode)
      fail = 1;
   if (sc_final_stats_actual.t_scr_mode != pc->t_scr_mode)
      fail = 1;
   if (sc_final_stats_actual.fec_en != pc->fec_en)
      fail = 1;
   if (sc_final_stats_actual.t_encode_mode != pc->t_encode_mode)
      fail = 1;

    /* TEMP CHK for CL72*/
    if (pc->cl72_en  & 0x08000000) {
     if (sc_final_stats_actual.cl72_en != (pc->cl72_en & 0xffff))
         fail = 1;
    }

   if (sc_final_stats_actual.r_descr1_mode != pc->r_descr1_mode)
      fail = 1;
   if (sc_final_stats_actual.r_dec1_mode != pc->r_dec1_mode)
      fail = 1;
   if (sc_final_stats_actual.r_deskew_mode != pc->r_deskew_mode)
      fail = 1;
   if (sc_final_stats_actual.r_desc2_mode != pc->r_desc2_mode)
      fail = 1;
   if (sc_final_stats_actual.r_desc2_byte_deletion != pc->r_desc2_byte_deletion)
      fail = 1;
   if (sc_final_stats_actual.r_dec1_brcm64b66_descr != pc->r_dec1_brcm64b66_descr)
      fail = 1;

   if (sc_final_stats_actual.cl48_check_end != pc->cl48_check_end)
      fail = 1;
   if (sc_final_stats_actual.blk_sync_mode != pc->blk_sync_mode)
      fail = 1;
   if (sc_final_stats_actual.r_reorder_mode != pc->r_reorder_mode)
      fail = 1;
   if (sc_final_stats_actual.cl36_en != pc->cl36_en)
      fail = 1;

   if (sc_final_stats_actual.sgmii != pc->sgmii)
      fail = 1;
   if (sc_final_stats_actual.clkcnt0 != pc->clkcnt0)
      fail = 1;
   if (sc_final_stats_actual.clkcnt1 != pc->clkcnt1)
      fail = 1;
   if (sc_final_stats_actual.lpcnt0 != pc->lpcnt0)
      fail = 1;
   if (sc_final_stats_actual.lpcnt1 != pc->lpcnt1)
      fail = 1;
   if (sc_final_stats_actual.mac_cgc != pc->mac_cgc)
      fail = 1;
   if (sc_final_stats_actual.pcs_repcnt != pc->pcs_repcnt)
      fail = 1;
   if (sc_final_stats_actual.pcs_crdten != pc->pcs_crdten)
      fail = 1;
   if (sc_final_stats_actual.pcs_clkcnt != pc->pcs_clkcnt)
      fail = 1;
   if (sc_final_stats_actual.pcs_cgc != pc->pcs_cgc)
      fail = 1;

   pc->accData   = fail;
   if (fail == 1) {
#ifdef _DV_TB_
      printf("Actual stats:\n");
      sc_final_stats_actual_tmp = &sc_final_stats_actual;
      print_temod16_sc_lkup_table(sc_final_stats_actual_tmp);
      printf("Expected stats:\n");
      print_temod16_sc_lkup_table(pc);
#endif
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
int _init_pcs_fs(PHYMOD_ST* pc)             
{
  uint16_t data;
  SC_X4_CTLr_t    reg_swmode;

  _get_sc_speed_configuration(pc);
  _configure_sc_speed_configuration(pc);
   
  /* Set speed */
  if (pc->sc_mode == TEMOD16_SC_MODE_ST) 
    data = (pc->per_lane_control & 0xff00) >> 8;
  else if ((pc->per_lane_control & 0xffff) == (0x004 | (TEMOD16_SPD_10000_XFI << 8))) /* DV Verification Only */
    data = digital_operationSpeeds_SPEED_10G_KR1;
  else if ((pc->per_lane_control & 0xffff) == (0x004 | (TEMOD16_SPD_10_SGMII << 8))) /* DV Verification Only */
    data = digital_operationSpeeds_SPEED_10M;
  else if ((pc->per_lane_control & 0xffff) == (0x004 | (TEMOD16_SPD_5000_XFI << 8))) /* DV Verification Only */
    data = digital_operationSpeeds_SPEED_5G_KR1;
  else
    data = pc->speed;

   SC_X4_CTLr_CLR(reg_swmode);
   SC_X4_CTLr_SW_SPEEDf_SET(reg_swmode, data);
   PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, reg_swmode));

   temod16_trigger_speed_change(pc); 
   _temod16_wait_sc_stats_set(pc);

  return PHYMOD_E_NONE;
}
#endif

/**
@brief   Initialize the PMD for QSGMII PCS. (different from TSCE PCS)
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Details pending. 
*/
int temod16_init_pmd_qsgmii(PHYMOD_ST* pc, int pmd_touched, int uc_active) /* INIT_PMD_SGMII */
{
 return PHYMOD_E_NONE;
}

/**
@brief   TSC12 DV stuff. Incomplete
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details Called to Configure tsc12 for 100G+ speeds

*/

int temod16_tsc12_control(PHYMOD_ST* pc, int cl82_multi_pipe_mode, int cl82_mld_phys_map)   /* TSC12_CONTROL */
{
  MAIN0_MISCr_t reg_misc;
  TMOD_DBG_IN_FUNC_INFO(pc);
  MAIN0_MISCr_CLR(reg_misc);

  MAIN0_MISCr_CL82_MULTI_PIPE_MODEf_SET(reg_misc, (cl82_multi_pipe_mode & 0x3));
  MAIN0_MISCr_CL82_MLD_PHYS_MAPf_SET(reg_misc, (cl82_mld_phys_map & 0x3));

  PHYMOD_IF_ERR_RETURN (MODIFY_MAIN0_MISCr(pc, reg_misc));
  return PHYMOD_E_NONE;
}

/*
int temod16_set_bit_repli(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf )
{
  MAIN0_MISCr_t reg_misc;
  TMOD_DBG_IN_FUNC_INFO(pc);
  MAIN0_MISCr_CLR(reg_misc);

  MAIN0_MISCr_TX_PCS_BIT_REPL_ENf_SET(reg_misc, pc->bit_rep_native_mode);
  PHYMOD_IF_ERR_RETURN (MODIFY_MAIN0_MISCr(pc, reg_misc));

  return PHYMOD_E_NONE;
}
*/

#ifdef _DV_TB_
/**
@brief   Checks to ensure SC has programmed the PCS as expected
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details Called to Configure tsc12 for 100G+ speeds

*/

int temod16_check_sc_stats(PHYMOD_ST* pc)   /* CHECK_SC_STATS */
{
  uint16_t data ;

  TMOD_DBG_IN_FUNC_INFO(pc);
  data = _read_and_compare_final_status(pc); 
  return PHYMOD_E_NONE;
}
#endif /* _DV_TB_ */


/**
@brief   get the rx lane reset  staus for any lane
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   get the status as *value
@returns The value PHYMOD_E_NONE upon successful completion.

This function enables/disables rx lane (RSTB_LANE) or read back control bit for
that based on per_lane_control being 1 or 0. If per_lane_control is 0xa, only
read back RSTB_LANE.

*/
int temod16_rx_lane_control_get(PHYMOD_ST* pc, int *value)         /* RX_LANE_CONTROL */
{
  RX_X4_PMA_CTL0r_t    reg_pma_ctrl;

  TMOD_DBG_IN_FUNC_INFO(pc);
  RX_X4_PMA_CTL0r_CLR(reg_pma_ctrl);

  PHYMOD_IF_ERR_RETURN (READ_RX_X4_PMA_CTL0r(pc, &reg_pma_ctrl));
  if ( RX_X4_PMA_CTL0r_RSTB_LANEf_GET(reg_pma_ctrl) )
    *value = 1;
  else
    *value = 0;

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
int temod16_rx_lane_control_set(PHYMOD_ST* pc, int enable)         /* RX_LANE_CONTROL */
{
  RX_X4_PMA_CTL0r_t    reg_pma_ctrl;

  TMOD_DBG_IN_FUNC_INFO(pc);
  RX_X4_PMA_CTL0r_CLR(reg_pma_ctrl);
  if (enable) {
    RX_X4_PMA_CTL0r_RSTB_LANEf_SET(reg_pma_ctrl, 1);
    PHYMOD_IF_ERR_RETURN (MODIFY_RX_X4_PMA_CTL0r(pc, reg_pma_ctrl));
  } else {
     /* bit set to 0 for disabling RXP */
    RX_X4_PMA_CTL0r_RSTB_LANEf_SET( reg_pma_ctrl, 0);
    PHYMOD_IF_ERR_RETURN (MODIFY_RX_X4_PMA_CTL0r(pc, reg_pma_ctrl));
  }
  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
int temod16_prbs_rx_enable_set(PHYMOD_ST* pc, int enable)
{
  TLB_RX_PRBS_CHK_CFGr_t    reg_prbs_chk_cfg;

  TMOD_DBG_IN_FUNC_INFO(pc);
  TLB_RX_PRBS_CHK_CFGr_CLR(reg_prbs_chk_cfg);

  TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_ENf_SET(reg_prbs_chk_cfg, enable);
  PHYMOD_IF_ERR_RETURN (MODIFY_TLB_RX_PRBS_CHK_CFGr(pc, reg_prbs_chk_cfg));

  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
int temod16_prbs_tx_enable_set(PHYMOD_ST* pc, int enable)                /* PRBS_MODE */
{
  TLB_TX_PRBS_GEN_CFGr_t    reg_prbs_gen_cfg;

  TMOD_DBG_IN_FUNC_INFO(pc);
  TLB_TX_PRBS_GEN_CFGr_CLR(reg_prbs_gen_cfg);

  TLB_TX_PRBS_GEN_CFGr_PRBS_GEN_ENf_SET(reg_prbs_gen_cfg, enable);
  PHYMOD_IF_ERR_RETURN (MODIFY_TLB_TX_PRBS_GEN_CFGr(pc, reg_prbs_gen_cfg));

  return PHYMOD_E_NONE;

} /* PRBS_MODE */
#endif

#ifdef _DV_TB_
int temod16_prbs_rx_invert_data_set(PHYMOD_ST* pc, int invert_data)                /* PRBS_MODE */
{
  TLB_RX_PRBS_CHK_CFGr_t    reg_prbs_chk_cfg;

  TMOD_DBG_IN_FUNC_INFO(pc);
  TLB_RX_PRBS_CHK_CFGr_CLR(reg_prbs_chk_cfg);

  TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_INVf_SET(reg_prbs_chk_cfg, invert_data) ;
  PHYMOD_IF_ERR_RETURN (MODIFY_TLB_RX_PRBS_CHK_CFGr(pc, reg_prbs_chk_cfg));

  return PHYMOD_E_NONE;

} /* PRBS_MODE */
#endif

#ifdef _DV_TB_
int temod16_prbs_rx_check_mode_set(PHYMOD_ST* pc, int check_mode)                /* PRBS_MODE */
{
  TLB_RX_PRBS_CHK_CFGr_t    reg_prbs_chk_cfg;

  TMOD_DBG_IN_FUNC_INFO(pc);
  TLB_RX_PRBS_CHK_CFGr_CLR(reg_prbs_chk_cfg);

  TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_MODEf_SET(reg_prbs_chk_cfg, check_mode);
  PHYMOD_IF_ERR_RETURN (MODIFY_TLB_RX_PRBS_CHK_CFGr(pc, reg_prbs_chk_cfg));

  return PHYMOD_E_NONE;

} /* PRBS_MODE */
#endif

#ifdef _DV_TB_
int temod16_prbs_tx_invert_data_set(PHYMOD_ST* pc, int invert_data)                /* PRBS_MODE */
{
  TLB_TX_PRBS_GEN_CFGr_t    reg_prbs_gen_cfg;

  TMOD_DBG_IN_FUNC_INFO(pc);
  TLB_TX_PRBS_GEN_CFGr_CLR(reg_prbs_gen_cfg);

  TLB_TX_PRBS_GEN_CFGr_PRBS_GEN_INVf_SET(reg_prbs_gen_cfg, invert_data);
  PHYMOD_IF_ERR_RETURN(MODIFY_TLB_TX_PRBS_GEN_CFGr(pc, reg_prbs_gen_cfg));

  return PHYMOD_E_NONE;

} /* PRBS_MODE */
#endif

#ifdef _DV_TB_
int temod16_prbs_tx_polynomial_set(PHYMOD_ST* pc, prbs_polynomial_type_t prbs_polynomial)                /* PRBS_MODE */
{
  TLB_TX_PRBS_GEN_CFGr_t    reg_prbs_gen_cfg;

  TMOD_DBG_IN_FUNC_INFO(pc);
  TLB_TX_PRBS_GEN_CFGr_CLR(reg_prbs_gen_cfg);

  TLB_TX_PRBS_GEN_CFGr_PRBS_GEN_MODE_SELf_SET(reg_prbs_gen_cfg, prbs_polynomial);
  PHYMOD_IF_ERR_RETURN (MODIFY_TLB_TX_PRBS_GEN_CFGr(pc, reg_prbs_gen_cfg));

  return PHYMOD_E_NONE;

} /* PRBS_MODE */
#endif

#ifdef _DV_TB_
int temod16_prbs_rx_polynomial_set(PHYMOD_ST* pc, prbs_polynomial_type_t prbs_polynomial)                /* PRBS_MODE */
{
  TLB_RX_PRBS_CHK_CFGr_t    reg_prbs_chk_cfg;

  TMOD_DBG_IN_FUNC_INFO(pc);
  TLB_RX_PRBS_CHK_CFGr_CLR(reg_prbs_chk_cfg);

  TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_MODE_SELf_SET(reg_prbs_chk_cfg, prbs_polynomial);
  PHYMOD_IF_ERR_RETURN (MODIFY_TLB_RX_PRBS_CHK_CFGr(pc, reg_prbs_chk_cfg));

  return PHYMOD_E_NONE;

} /* PRBS_MODE */
#endif


int temod16_tx_loopback_get(PHYMOD_ST *pc, uint32_t *enable)           /* TX_LOOPBACK_get  */
{
    MAIN0_LPBK_CTLr_t loopReg;
    TMOD_DBG_IN_FUNC_INFO(pc);

    PHYMOD_IF_ERR_RETURN(READ_MAIN0_LPBK_CTLr(pc, &loopReg));
    *enable = MAIN0_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_GET(loopReg);

    return PHYMOD_E_NONE;
}

int temod16_update_port_mode(PHYMOD_ST *pc, int *pll_restart)
{
    MAIN0_SETUPr_t mode_reg;
    int port_mode_sel, port_mode_sel_reg, temp_pll_restart;
    uint32_t single_port_mode;
    uint32_t first_couple_mode = 0, second_couple_mode = 0;
    TMOD_DBG_IN_FUNC_INFO(pc);

    port_mode_sel = 0 ;
    single_port_mode = 0 ;
    temp_pll_restart = 0; 

    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUPr(pc, &mode_reg));
    port_mode_sel_reg = MAIN0_SETUPr_PORT_MODE_SELf_GET(mode_reg);

    if (pc->lane_mask == 0xf) {
        port_mode_sel = 4;
        if ( port_mode_sel_reg != 4) {
            temp_pll_restart = 1;
        }
    } else {  
        first_couple_mode  = ((port_mode_sel_reg == 2) || (port_mode_sel_reg == 3) || (port_mode_sel_reg == 4));
        second_couple_mode = ((port_mode_sel_reg == 1) || (port_mode_sel_reg == 3) || (port_mode_sel_reg == 4));
        switch (pc->lane_mask) {
        case 1:
        case 2:
            first_couple_mode = 0;
            break;
        case 4:
        case 8:
            second_couple_mode = 0;
            break;
        case 3:
            first_couple_mode = 1;
            break;
        case 0xc:
            second_couple_mode = 1;
            break;
        default:
        /* dprintf("%-22s: ERROR port_mode_sel=%0d undefined\n", 
             __func__, port_mode_sel_reg); */
            break ;
        }
        
        if (first_couple_mode) {
            port_mode_sel =(second_couple_mode)? 3: 2;
        }
        else if (second_couple_mode) {
            port_mode_sel = 1;
        }
        else{
            port_mode_sel = 0 ;
        }
    }
    
    *pll_restart = temp_pll_restart;

    MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(mode_reg, single_port_mode);
    MAIN0_SETUPr_PORT_MODE_SELf_SET(mode_reg, port_mode_sel);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc, mode_reg));
    return PHYMOD_E_NONE ; 
}

int temod16_tx_lane_control_set(PHYMOD_ST* pc, tx_lane_disable_type_t tx_dis_type)         /* TX_LANE_CONTROL */
{
    TX_X4_MISCr_t    reg_misc_ctrl;
    TMOD_DBG_IN_FUNC_INFO(pc);

    TX_X4_MISCr_CLR(reg_misc_ctrl);

    switch (tx_dis_type) {
        case TEMOD16_TX_LANE_RESET: 
            TX_X4_MISCr_RSTB_TX_LANEf_SET(reg_misc_ctrl, 0);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, reg_misc_ctrl));
            TX_X4_MISCr_CLR(reg_misc_ctrl);
            TX_X4_MISCr_RSTB_TX_LANEf_SET(reg_misc_ctrl, 1);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, reg_misc_ctrl));
            break;
        case TEMOD16_TX_LANE_TRAFFIC_ENABLE: 
            TX_X4_MISCr_ENABLE_TX_LANEf_SET(reg_misc_ctrl, 1);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, reg_misc_ctrl));
            break;
        case TEMOD16_TX_LANE_TRAFFIC_DISABLE: 
            TX_X4_MISCr_ENABLE_TX_LANEf_SET(reg_misc_ctrl, 0);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, reg_misc_ctrl));
            break;
        case TEMOD16_TX_LANE_RESET_TRAFFIC_ENABLE: 
            TX_X4_MISCr_RSTB_TX_LANEf_SET(reg_misc_ctrl, 1);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, reg_misc_ctrl));
            TX_X4_MISCr_CLR(reg_misc_ctrl);
            TX_X4_MISCr_ENABLE_TX_LANEf_SET(reg_misc_ctrl, 1);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, reg_misc_ctrl));
            break;
        case TEMOD16_TX_LANE_RESET_TRAFFIC_DISABLE: 
            TX_X4_MISCr_RSTB_TX_LANEf_SET(reg_misc_ctrl, 0);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, reg_misc_ctrl));
            TX_X4_MISCr_CLR(reg_misc_ctrl);
            TX_X4_MISCr_ENABLE_TX_LANEf_SET(reg_misc_ctrl, 0);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, reg_misc_ctrl));
            break;
        default: 
            break;
    }
    return PHYMOD_E_NONE;
}

int temod16_refclk_set(PHYMOD_ST* pc, phymod_ref_clk_t ref_clock) 
{
    MAIN0_SETUPr_t  reg_setup;
    TOP_USER_CTL0r_t dig_top_user_reg;
    TMOD_DBG_IN_FUNC_INFO(pc);
 
    MAIN0_SETUPr_CLR(reg_setup);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUPr(pc, &reg_setup));
    PHYMOD_IF_ERR_RETURN(READ_TOP_USER_CTL0r(pc, &dig_top_user_reg));

    if (ref_clock == phymodRefClk125Mhz) {
        MAIN0_SETUPr_REFCLK_SELf_SET(reg_setup, main0_refClkSelect_clk_125MHz);
        TOP_USER_CTL0r_HEARTBEAT_COUNT_1USf_SET(dig_top_user_reg, 0x1f4);
    } else {
        MAIN0_SETUPr_REFCLK_SELf_SET(reg_setup, main0_refClkSelect_clk_156p25MHz);
        TOP_USER_CTL0r_HEARTBEAT_COUNT_1USf_SET(dig_top_user_reg, 0x271);
    }

    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc,reg_setup));
    PHYMOD_IF_ERR_RETURN(MODIFY_TOP_USER_CTL0r(pc, dig_top_user_reg));

    return PHYMOD_E_NONE;
}

int temod16_pcs_ilkn_mode_set(PHYMOD_ST* pc)              /* INIT_PCS_ILKN */
{
    SC_X4_BYPASSr_t reg_sc_bypass;

    TMOD_DBG_IN_FUNC_INFO(pc);
    SC_X4_BYPASSr_CLR(reg_sc_bypass);
    SC_X4_BYPASSr_SC_BYPASSf_SET(reg_sc_bypass, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_BYPASSr(pc,reg_sc_bypass));

    return PHYMOD_E_NONE;
}

int temod16_master_port_num_set( PHYMOD_ST *pc,  int port_num)
{
    MAIN0_SETUPr_t main_reg;
    TMOD_DBG_IN_FUNC_INFO(pc);

    MAIN0_SETUPr_CLR(main_reg);
    MAIN0_SETUPr_MASTER_PORT_NUMf_SET(main_reg, port_num);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc, main_reg));

    return PHYMOD_E_NONE;
}

int temod16_cl48_lfrfli_init(PHYMOD_ST* pc)               /* AUTONEG timer set*/
{
    TX_X2_CL48_0r_t       reg_cl48_ctl ;
    TMOD_DBG_IN_FUNC_INFO(pc);

    TX_X2_CL48_0r_CLR(reg_cl48_ctl) ;
    TX_X2_CL48_0r_CL48_TX_LI_ENABLEf_SET(reg_cl48_ctl,1) ;
    TX_X2_CL48_0r_CL48_TX_LF_ENABLEf_SET(reg_cl48_ctl,1) ;
    TX_X2_CL48_0r_CL48_TX_RF_ENABLEf_SET(reg_cl48_ctl,1) ;
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X2_CL48_0r(pc, reg_cl48_ctl));

    return PHYMOD_E_NONE;
}

int temod16_mld_am_timers_set(PHYMOD_ST* pc) /*MLD_AM_TIMERS_SET */
{
    TX_X2_MLD_SWP_CNTr_t reg_mld_swap_cnt;
    TMOD_DBG_IN_FUNC_INFO(pc);

    TX_X2_MLD_SWP_CNTr_CLR(reg_mld_swap_cnt);

    TX_X2_MLD_SWP_CNTr_SET(reg_mld_swap_cnt, 0xfffc);
    PHYMOD_IF_ERR_RETURN (WRITE_TX_X2_MLD_SWP_CNTr(pc, reg_mld_swap_cnt));
    return PHYMOD_E_NONE;
}

int temod16_cl74_chng_default(PHYMOD_ST* pc)
{
    RX_X4_FEC2r_t RX_X4_FEC2reg;
    phymod_access_t pa_copy;
    int i;
    TMOD_DBG_IN_FUNC_INFO(pc);

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    RX_X4_FEC2r_CLR(RX_X4_FEC2reg);

    for (i = 0; i <4; i++) {
        pa_copy.lane_mask = 0x1 << i;
        PHYMOD_IF_ERR_RETURN(WRITE_RX_X4_FEC2r(&pa_copy, RX_X4_FEC2reg));
    }
    return PHYMOD_E_NONE;
}

int temod16_speed_id_get(PHYMOD_ST* pc, int *speed_id)
{
    SC_X4_RSLVD_SPDr_t sc_final_resolved_speed;
    TMOD_DBG_IN_FUNC_INFO(pc);

    SC_X4_RSLVD_SPDr_CLR(sc_final_resolved_speed);
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD_SPDr(pc,&sc_final_resolved_speed));
    *speed_id = SC_X4_RSLVD_SPDr_SPEEDf_GET(sc_final_resolved_speed);

    return PHYMOD_E_NONE;
}

int temod16_hg_enable_get(PHYMOD_ST* pc, int *enable)
{
    TX_X4_ENC0r_t    reg_encode;
    TMOD_DBG_IN_FUNC_INFO(pc);

    TX_X4_ENC0r_CLR(reg_encode);

    PHYMOD_IF_ERR_RETURN
        (READ_TX_X4_ENC0r(pc, &reg_encode));
    *enable = TX_X4_ENC0r_HG2_ENABLEf_GET(reg_encode);

    return PHYMOD_E_NONE;
}

int temod16_pll_config_set(const phymod_access_t *pa, temod16_pll_mode_type  pll_mode, phymod_ref_clk_t ref_clk)
{
  enum merlin16_pll_refclk_enum refclk;
  enum merlin16_pll_div_enum pll_cfg;

  switch (pll_mode) {

  case TEMOD16_PLL_MODE_DIV_54:
    pll_cfg = MERLIN16_PLL_DIV_54P4;
    break;
  case TEMOD16_PLL_MODE_DIV_60:
    pll_cfg = MERLIN16_PLL_DIV_60;
    break;
  case TEMOD16_PLL_MODE_DIV_64:
    pll_cfg = MERLIN16_PLL_DIV_64;
    break;
  case TEMOD16_PLL_MODE_DIV_66:
    pll_cfg = MERLIN16_PLL_DIV_66;
    break;
  case TEMOD16_PLL_MODE_DIV_70:
    pll_cfg = MERLIN16_PLL_DIV_70;
    break;
  case TEMOD16_PLL_MODE_DIV_80:
    pll_cfg = MERLIN16_PLL_DIV_80;
    break;
  case TEMOD16_PLL_MODE_DIV_92:
    pll_cfg = MERLIN16_PLL_DIV_92;
    break;
  default:
    pll_cfg = MERLIN16_PLL_DIV_66;
    break;
  }

  switch (ref_clk) {
    case phymodRefClk156Mhz:
        refclk = MERLIN16_PLL_REFCLK_156P25MHZ;
        break;
    default:
        refclk = MERLIN16_PLL_REFCLK_156P25MHZ;
        break;
  }

  return merlin16_configure_pll_refclk_div(pa, refclk, pll_cfg);
}

int temod16_pll_config_get(const phymod_access_t *pa, temod16_pll_mode_type  *pll_mode)
{
    uint32_t pll_div;

    PHYMOD_IF_ERR_RETURN
        (merlin16_INTERNAL_read_pll_div(pa, &pll_div));

    /* merlin16_INTERNAL_read_pll_div() puts one of merlin16_pll_div_enum
     * as the returned value in pll_div
     */
    switch (pll_div) {
        case MERLIN16_PLL_DIV_70: *pll_mode = TEMOD16_PLL_MODE_DIV_70;
            break;
        case MERLIN16_PLL_DIV_66: *pll_mode = TEMOD16_PLL_MODE_DIV_66;
            break;
        case MERLIN16_PLL_DIV_64: *pll_mode = TEMOD16_PLL_MODE_DIV_64;
            break;
        case MERLIN16_PLL_DIV_60: *pll_mode = TEMOD16_PLL_MODE_DIV_60;
            break;
        case MERLIN16_PLL_DIV_92: *pll_mode = TEMOD16_PLL_MODE_DIV_92;
            break;
        case MERLIN16_PLL_DIV_80: *pll_mode = TEMOD16_PLL_MODE_DIV_80;
            break;
        case MERLIN16_PLL_DIV_54P4: *pll_mode = TEMOD16_PLL_MODE_DIV_54;
            break;
        default: return PHYMOD_E_NONE;
            break;
    }

    return PHYMOD_E_NONE;
}

int temod16_pcs_ilkn_chk(PHYMOD_ST* pc, int *ilkn_set)              /* INIT_PCS_ILKN */
{
    ILKN_CTL0r_t ILKN_CONTROL0r_reg;
    TMOD_DBG_IN_FUNC_INFO(pc);

    PHYMOD_IF_ERR_RETURN(READ_ILKN_CTL0r(pc, &ILKN_CONTROL0r_reg));

    *ilkn_set = ILKN_CTL0r_ILKN_SELf_GET(ILKN_CONTROL0r_reg);

    return PHYMOD_E_NONE;
}

int temod16_clause72_control(PHYMOD_ST* sa__, int cl72_en)                /* CLAUSE_72_CONTROL */
{
    err_code_t __err;
    phymod_access_t pa_copy;
    int start_lane = 0, num_lane = 0;
    uint32_t enable=0;

    PHYMOD_MEMCPY(&pa_copy, sa__, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(sa__, &start_lane, &num_lane));
    pa_copy.lane_mask = 0x1 << start_lane;

    __err=wr_cl72_ieee_training_enable(cl72_en); if (__err) return(__err);
    __err=wr_cl72_ieee_restart_training(cl72_en); if (__err) return(__err);

    pa_copy.lane_mask = 0x1 << start_lane;
    temod16_disable_get(&pa_copy, &enable);
    if (cl72_en & TEMOD16_CL72_CONTROL_NO_TRIGER) {

    } else {
      if (enable == 0x1) {
          PHYMOD_IF_ERR_RETURN (temod16_trigger_speed_change(&pa_copy));
      }
    }

    return PHYMOD_E_NONE;
}

int temod16_cl37_high_vco_set(PHYMOD_ST* pc, int value)
{
    MAIN0_SETUPr_t  reg_setup;

    MAIN0_SETUPr_CLR(reg_setup);
    MAIN0_SETUPr_CL37_HIGH_VCOf_SET(reg_setup, value);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc,reg_setup));

    return PHYMOD_E_NONE;
}

int temod16_osdfe_on_lkup_get(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, uint32_t* osdfe_on)
{
    int speed_id;

    temod16_get_mapped_speed(spd_intf, &speed_id);
    *osdfe_on = merlin16_sc_pmd_entry[speed_id].osdfe_on;

    return PHYMOD_E_NONE;
}

int temod16_scrambling_dis_lkup_get(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, uint32_t* scrambling_dis)
{
    int speed_id;

    temod16_get_mapped_speed(spd_intf, &speed_id);
    *scrambling_dis = merlin16_sc_pmd_entry[speed_id].scrambling_dis;

    return PHYMOD_E_NONE;
}

int temod16_override_clear(PHYMOD_ST* pc, override_type_t or_type)
{
    SC_X4_FLD_OVRR_EN0_TYPEr_t reg_or_en0;
    TMOD_DBG_IN_FUNC_INFO(pc);

    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(reg_or_en0);
    switch (or_type) {
    case TEMOD16_OVERRIDE_SCR_MODE:
        /*Set the override disable*/
        SC_X4_FLD_OVRR_EN0_TYPEr_SCR_MODE_OENf_SET(reg_or_en0,0);
        PHYMOD_IF_ERR_RETURN (MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0));
        break;
    case TEMOD16_OVERRIDE_DESCR_MODE:
        /*Set the override disable*/
        SC_X4_FLD_OVRR_EN0_TYPEr_DESCRAMBLERMODE_OENf_SET(reg_or_en0,0);
        PHYMOD_IF_ERR_RETURN (MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc,reg_or_en0));
        break;
    default:
        return PHYMOD_E_UNAVAIL;
    }
    return PHYMOD_E_NONE;
}

int temod16_set_spd_intf(PHYMOD_ST *pc, temod16_spd_intfc_type spd_intf, int no_trig)
{
    SC_X4_CTLr_t xgxs_x4_ctrl;
    phymod_access_t pa_copy;
    int speed_id = 0, start_lane = 0, num_lane = 0;
    TMOD_DBG_IN_FUNC_INFO(pc);

    /* need to figure out what's the starting lane */
    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
       (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
    pa_copy.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(temod16_get_mapped_speed(spd_intf, &speed_id));
    /* write the speed_id into the speed_change register */
    SC_X4_CTLr_CLR(xgxs_x4_ctrl);
    SC_X4_CTLr_SW_SPEEDf_SET(xgxs_x4_ctrl, speed_id);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, xgxs_x4_ctrl));
    /*next call the speed_change routine */
    if (no_trig) {
    } else {
        PHYMOD_IF_ERR_RETURN (temod16_trigger_speed_change(&pa_copy));
        /*check the speed_change_done nit*/
        PHYMOD_IF_ERR_RETURN (_temod16_wait_sc_stats_set(pc));
    }

    return PHYMOD_E_NONE;
}

int temod16_rx_squelch_set(PHYMOD_ST *pc, int enable)
{
    SIGDET_CTL1r_t sigdet_ctl;

    SIGDET_CTL1r_CLR(sigdet_ctl);
    PHYMOD_IF_ERR_RETURN(READ_SIGDET_CTL1r(pc, &sigdet_ctl));
    if (enable) {
        SIGDET_CTL1r_SIGNAL_DETECT_FRCf_SET(sigdet_ctl, 1);
        SIGDET_CTL1r_SIGNAL_DETECT_FRC_VALf_SET(sigdet_ctl, 0);
    } else {
        SIGDET_CTL1r_SIGNAL_DETECT_FRCf_SET(sigdet_ctl, 0);
        SIGDET_CTL1r_SIGNAL_DETECT_FRC_VALf_SET(sigdet_ctl, 0);
    }
    PHYMOD_IF_ERR_RETURN(MODIFY_SIGDET_CTL1r(pc, sigdet_ctl));

    return PHYMOD_E_NONE;
}

int temod16_rx_squelch_get(PHYMOD_ST *pc, int *val)
{
    SIGDET_CTL1r_t sigdet_ctl;

    SIGDET_CTL1r_CLR(sigdet_ctl);
    PHYMOD_IF_ERR_RETURN(READ_SIGDET_CTL1r(pc, &sigdet_ctl));
    *val = SIGDET_CTL1r_SIGNAL_DETECT_FRCf_GET(sigdet_ctl);

    return PHYMOD_E_NONE;
}

int temod16_port_enable_set(PHYMOD_ST *pc, int enable)
{
    if (enable) {
        PHYMOD_IF_ERR_RETURN(temod16_rx_squelch_set(pc, 0));
        PHYMOD_IF_ERR_RETURN(temod16_tx_squelch_set(pc, 0));
    } else {
        PHYMOD_IF_ERR_RETURN(temod16_rx_squelch_set(pc, 1));
        PHYMOD_IF_ERR_RETURN(temod16_tx_squelch_set(pc, 1));
    }

    return PHYMOD_E_NONE;
}

int temod16_tx_squelch_set(PHYMOD_ST *pc, int enable)
{
    MISC_CTL0r_t tx_misc_ctl;

    MISC_CTL0r_CLR(tx_misc_ctl);
    PHYMOD_IF_ERR_RETURN(READ_MISC_CTL0r(pc, &tx_misc_ctl));
    MISC_CTL0r_SDK_TX_DISABLEf_SET(tx_misc_ctl, enable);
    PHYMOD_IF_ERR_RETURN(MODIFY_MISC_CTL0r(pc, tx_misc_ctl));

    return PHYMOD_E_NONE;
}

int temod16_tx_squelch_get(PHYMOD_ST *pc, int *val)
{
    MISC_CTL0r_t tx_misc_ctl;

    MISC_CTL0r_CLR(tx_misc_ctl);
    PHYMOD_IF_ERR_RETURN(READ_MISC_CTL0r(pc, &tx_misc_ctl));
    *val = MISC_CTL0r_SDK_TX_DISABLEf_GET(tx_misc_ctl);

    return PHYMOD_E_NONE;
}

int temod16_tx_lane_control_get(PHYMOD_ST* pc, int *reset, int *enable)         /* TX_LANE_CONTROL_GET */
{
    TX_X4_MISCr_t    reg_misc_ctrl;

    TMOD_DBG_IN_FUNC_INFO(pc);
    TX_X4_MISCr_CLR(reg_misc_ctrl);

    PHYMOD_IF_ERR_RETURN (READ_TX_X4_MISCr(pc, &reg_misc_ctrl));
    *reset =  TX_X4_MISCr_RSTB_TX_LANEf_GET(reg_misc_ctrl);
    *enable = TX_X4_MISCr_ENABLE_TX_LANEf_GET(reg_misc_ctrl);

    return PHYMOD_E_NONE;
}

int temod16_get_pcs_latched_link_status(PHYMOD_ST* pc, uint32_t *link)
{
    RX_X4_PCS_LIVE_STSr_t   reg_pcs_live_sts;
    RX_X4_PCS_LATCH_STS0r_t latched_sts ;
    int                     latched_val ;
  
    TMOD_DBG_IN_FUNC_INFO(pc);
    RX_X4_PCS_LIVE_STSr_CLR(reg_pcs_live_sts);
    RX_X4_PCS_LATCH_STS0r_CLR(latched_sts) ;

    PHYMOD_IF_ERR_RETURN (READ_RX_X4_PCS_LIVE_STSr(pc, &reg_pcs_live_sts));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_PCS_LATCH_STS0r(pc, &latched_sts)) ;
    latched_val = RX_X4_PCS_LATCH_STS0r_LINK_STATUS_LLf_GET(latched_sts) ;
    if (latched_val) {
        *link = 0 ; 
    } else {
        *link = RX_X4_PCS_LIVE_STSr_LINK_STATUSf_GET(reg_pcs_live_sts);
    }

    return PHYMOD_E_NONE;
}

/* PMD per lane reset get */
int temod16_pmd_x4_reset_get(PHYMOD_ST* pc, int *is_in_reset)
{
    PMD_X4_CTLr_t reg_pmd_x4_ctrl;

    TMOD_DBG_IN_FUNC_INFO(pc);
    PMD_X4_CTLr_CLR(reg_pmd_x4_ctrl);
    PHYMOD_IF_ERR_RETURN(READ_PMD_X4_CTLr(pc, &reg_pmd_x4_ctrl));

    if (PMD_X4_CTLr_LN_TX_H_RSTBf_GET(reg_pmd_x4_ctrl) &&
        PMD_X4_CTLr_LN_RX_H_RSTBf_GET(reg_pmd_x4_ctrl) &&
        PMD_X4_CTLr_LN_TX_DP_H_RSTBf_GET(reg_pmd_x4_ctrl) &&
        PMD_X4_CTLr_LN_RX_DP_H_RSTBf_GET(reg_pmd_x4_ctrl)) {
        *is_in_reset = 0 ;
    } else {
        *is_in_reset = 1 ;
    }

    return PHYMOD_E_NONE;
}
/**
@brief   Get the plldiv from lookup table
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   spd_intf speed as specified by enum #temod_spd_intfc_type
@param   plldiv Receives the pll divider value
@param   speed_vec  Receives the speed id.
@returns The value PHYMOD_E_NONE upon successful completion
@details Get the plldiv from lookup table
*/
int temod16_plldiv_lkup_get(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, phymod_ref_clk_t ref_clk,  uint32_t *plldiv, uint16_t *speed_vec)
{
  int speed_id;

  temod16_get_mapped_speed(spd_intf, &speed_id);
  if (ref_clk == phymodRefClk156Mhz) {
    *plldiv = merlin16_sc_pmd_entry[speed_id].pll_mode;
  }
  *speed_vec = speed_id ;

  return PHYMOD_E_NONE;
}

int temod16_pll_reset_enable_set(PHYMOD_ST *pa , int enable)
{
    MAIN0_SETUPr_t main_reg;

    MAIN0_SETUPr_CLR(main_reg);
    MAIN0_SETUPr_PLL_RESET_ENf_SET(main_reg, enable);
    MODIFY_MAIN0_SETUPr(pa, main_reg);
    return PHYMOD_E_NONE;
}

int temod16_disable_get(PHYMOD_ST *pa, uint32_t *enable )
{
  SC_X4_CTLr_t reg_sc_ctrl;

  TMOD_DBG_IN_FUNC_INFO(pa);
  SC_X4_CTLr_CLR(reg_sc_ctrl);

  PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pa,&reg_sc_ctrl));
  *enable = SC_X4_CTLr_SW_SPEED_CHANGEf_GET(reg_sc_ctrl);

  return PHYMOD_E_NONE;
}

/**
@brief   EEE Control Set
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable enable or disable EEE control
@returns The value PHYMOD_E_NONE upon successful completion
@details EEE Control
         enable =1 : Allow LPI pass through. i.e. dont convert LPI.
         enable =0 : Convert LPI to idle. So  MAC will not see it.

*/
int temod16_eee_control_set(PHYMOD_ST* pc, uint32_t enable)
{
  RX_X4_PCS_CTL0r_t reg_pcs_ctrl0;

  TMOD_DBG_IN_FUNC_INFO(pc);
  RX_X4_PCS_CTL0r_CLR(reg_pcs_ctrl0);
  READ_RX_X4_PCS_CTL0r(pc, &reg_pcs_ctrl0);

  RX_X4_PCS_CTL0r_LPI_ENABLEf_SET(reg_pcs_ctrl0, enable & 0x1);
  PHYMOD_IF_ERR_RETURN (MODIFY_RX_X4_PCS_CTL0r(pc, reg_pcs_ctrl0));
  return PHYMOD_E_NONE;
}

/**
@brief   EEE Control Get
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable handle to pass back EEE control
@returns The value PHYMOD_E_NONE upon successful completion
@details EEE Control
         enable =1 : Allow LPI pass through. i.e. dont convert LPI.
         enable =0 : Convert LPI to idle. So  MAC will not see it.

*/
int temod16_eee_control_get(PHYMOD_ST* pc, uint32_t *enable)
{
  RX_X4_PCS_CTL0r_t reg_pcs_ctrl0;

  TMOD_DBG_IN_FUNC_INFO(pc);
  RX_X4_PCS_CTL0r_CLR(reg_pcs_ctrl0);

  PHYMOD_IF_ERR_RETURN (READ_RX_X4_PCS_CTL0r(pc, &reg_pcs_ctrl0));
  *enable = RX_X4_PCS_CTL0r_LPI_ENABLEf_GET(reg_pcs_ctrl0);
  return PHYMOD_E_NONE;
}

STATIC
int _temod16_rx_phy_lane_get(PHYMOD_ST *pc, int rx_logical_lane, int* rx_phy_lane)
{
    MAIN0_LN_SWPr_t reg_lane_swap;
    phymod_access_t pa_copy;

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));

    /*always use lane 0 copy since register is single copy */
    pa_copy.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (READ_MAIN0_LN_SWPr(&pa_copy, &reg_lane_swap));

    switch (rx_logical_lane) {
        case 0:
            *rx_phy_lane = MAIN0_LN_SWPr_LOG0_TO_PHY_RX_LNSWAP_SELf_GET(reg_lane_swap);
            break;
        case 1:
            *rx_phy_lane = MAIN0_LN_SWPr_LOG1_TO_PHY_RX_LNSWAP_SELf_GET(reg_lane_swap);
            break;
        case 2:
            *rx_phy_lane = MAIN0_LN_SWPr_LOG2_TO_PHY_RX_LNSWAP_SELf_GET(reg_lane_swap);
            break;
        case 3:
            *rx_phy_lane = MAIN0_LN_SWPr_LOG3_TO_PHY_RX_LNSWAP_SELf_GET(reg_lane_swap);
            break;
        default:
           return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int temod16_synce_stg0_mode_set(PHYMOD_ST *pc, int mode)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN0_SYNCE_CTL_STAGE0r_t reg_main0_synce_ctl_stage0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_temod16_rx_phy_lane_get(pc, start_lane, &phy_lane));

    MAIN0_SYNCE_CTL_STAGE0r_CLR(reg_main0_synce_ctl_stage0);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SYNCE_CTL_STAGE0r(pc, &reg_main0_synce_ctl_stage0));

    switch(phy_lane) {
        case 0:
           MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LN0f_SET(
                                           reg_main0_synce_ctl_stage0, mode);
           break;
        case 1:
           MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LN1f_SET(
                                           reg_main0_synce_ctl_stage0, mode);
           break;
        case 2:
           MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LN2f_SET(
                                           reg_main0_synce_ctl_stage0, mode);
           break;
        case 3:
           MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LN3f_SET(
                                           reg_main0_synce_ctl_stage0, mode);
           break;
        default:
           return PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SYNCE_CTL_STAGE0r(pc, reg_main0_synce_ctl_stage0));

    return PHYMOD_E_NONE;
}

int temod16_synce_stg1_mode_set(PHYMOD_ST *pc, int mode)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN0_SYNCE_CTLr_t reg_main0_synce_ctl;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_temod16_rx_phy_lane_get(pc, start_lane, &phy_lane));

    MAIN0_SYNCE_CTLr_CLR(reg_main0_synce_ctl);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SYNCE_CTLr(pc, &reg_main0_synce_ctl));

    switch(phy_lane) {
       case 0:
           MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LN0f_SET(reg_main0_synce_ctl, mode);
           break;
        case 1:
           MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LN1f_SET(reg_main0_synce_ctl, mode);
           break;
        case 2:
           MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LN2f_SET(reg_main0_synce_ctl, mode);
           break;
        case 3:
           MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LN3f_SET(reg_main0_synce_ctl, mode);
           break;
        default:
           return PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SYNCE_CTLr(pc, reg_main0_synce_ctl));

    return PHYMOD_E_NONE;
}

int temod16_synce_clk_ctrl_set(PHYMOD_ST *pc, uint32_t val)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    phymod_access_t pa_copy;
    RX_X4_SYNCE_CTLr_t reg_rx_x4_synce_ctl;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_temod16_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    pa_copy.lane_mask = 0x1 << phy_lane;

    RX_X4_SYNCE_CTLr_CLR(reg_rx_x4_synce_ctl);
    RX_X4_SYNCE_CTLr_SET(reg_rx_x4_synce_ctl, val);
    PHYMOD_IF_ERR_RETURN(WRITE_RX_X4_SYNCE_CTLr(&pa_copy, reg_rx_x4_synce_ctl));

    return PHYMOD_E_NONE;
}

int temod16_synce_stg0_mode_get(PHYMOD_ST *pc, int *mode)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN0_SYNCE_CTL_STAGE0r_t reg_main0_synce_ctl_stage0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_temod16_rx_phy_lane_get(pc, start_lane, &phy_lane));

    MAIN0_SYNCE_CTL_STAGE0r_CLR(reg_main0_synce_ctl_stage0);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SYNCE_CTL_STAGE0r(pc, &reg_main0_synce_ctl_stage0));

    switch(phy_lane) {
         case 0:
            *mode = MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LN0f_GET(
                                            reg_main0_synce_ctl_stage0);
            break;
         case 1:
            *mode = MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LN1f_GET(
                                            reg_main0_synce_ctl_stage0);
            break;
         case 2:
            *mode = MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LN2f_GET(
                                            reg_main0_synce_ctl_stage0);
            break;
         case 3:
            *mode = MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LN3f_GET(
                                            reg_main0_synce_ctl_stage0);
            break;
         default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int temod16_synce_stg1_mode_get(PHYMOD_ST *pc, int *mode)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN0_SYNCE_CTLr_t reg_main0_synce_ctl;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_temod16_rx_phy_lane_get(pc, start_lane, &phy_lane));

    MAIN0_SYNCE_CTLr_CLR(reg_main0_synce_ctl);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SYNCE_CTLr(pc, &reg_main0_synce_ctl));

    switch(phy_lane) {
         case 0:
            *mode = MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LN0f_GET(reg_main0_synce_ctl);
            break;
         case 1:
            *mode = MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LN1f_GET(reg_main0_synce_ctl);
            break;
         case 2:
            *mode = MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LN2f_GET(reg_main0_synce_ctl);
            break;
         case 3:
            *mode = MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LN3f_GET(reg_main0_synce_ctl);
            break;
         default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int temod16_synce_clk_ctrl_get(PHYMOD_ST *pc, uint32_t *val)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    phymod_access_t pa_copy;
    RX_X4_SYNCE_CTLr_t reg_rx_x4_synce_ctl;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_temod16_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    pa_copy.lane_mask = 0x1 << phy_lane;

    RX_X4_SYNCE_CTLr_CLR(reg_rx_x4_synce_ctl);
    PHYMOD_IF_ERR_RETURN(READ_RX_X4_SYNCE_CTLr(&pa_copy, &reg_rx_x4_synce_ctl));

    *val = RX_X4_SYNCE_CTLr_GET(reg_rx_x4_synce_ctl);

    return PHYMOD_E_NONE;
}
#ifdef SDK_TEMOD16_
int temod16_pll_sequencer_control(PHYMOD_ST *pa, int enable) { return 0; }
int temod16_firmware_load(PHYMOD_ST *pa) { return 0; }
/* int temod16_get_ht_entries ( PHYMOD_ST *pa,, sc_table_entry_t *exp_entry) { return 0; } */
int temod16_osmode_set(PHYMOD_ST *pa, int pmd_os_mode, int override_enable) { return 0; }
int temod16_pcs_lane_swap_get(PHYMOD_ST *pa,  uint32_t *tx_rx_swap) { return 0; }
int temod16_pll_lock_wait(PHYMOD_ST *pc, int timeOutValue) { return 0; } 
int temod16_pll_mode_set(PHYMOD_ST *pa, int pll_mode) { return 0; }
int temod16_pmd_lane_swap_tx_get(PHYMOD_ST *pa, uint32_t *tx_swap) { return 0; }
int temod16_rx_lane_control(PHYMOD_ST *pa,  int enable) { return 0; }
int temod16_tx_lane_disable_get(PHYMOD_ST *pa, uint32_t *enable) { return 0; }
int temod16_tx_lane_disable_set(PHYMOD_ST *pa, uint32_t *enable) { return 0; }
int temod16_tx_pi_control_get(PHYMOD_ST *pa,  uint32_t *value) { return 0; }
int temod16_update_port_mode_select(PHYMOD_ST *pa, int *pll_restart) { return 0; }
int _temod16_trigger_speed_change(PHYMOD_ST *pa) { return 0 ; }
int temod16_get_ht_entries(int speed, sc_table_entry_t *exp_entry) { return 0; } 
#endif


