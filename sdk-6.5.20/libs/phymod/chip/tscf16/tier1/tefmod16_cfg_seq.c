/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : tefmod16_cfg_seq.c
 * Description: c functions implementing Tier1s for TEFMod Serdes Driver
 *---------------------------------------------------------------------*/

#ifndef _DV_TB_
 #define _SDK_TEFMOD16_ 1 
#endif

#ifdef _DV_TB_
#include <stdint.h>
#include "tefmod_main.h"
#include "tefmod_defines.h"
#include "tefmod.h"
#include "tfPCSRegEnums.h"
#include "tfPMDRegEnums.h"
#include "phy_tsc_iblk.h"
#include "bcmi_tscf_16nm_xgxs_defs.h"
#endif /* _DV_TB_ */

#ifdef _SDK_TEFMOD16_
#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_tscf_16nm_xgxs_defs.h>
#include "tefmod16.h"
#include "tefmod16_enum_defines.h"
#include "tefmod16_sc_lkup_table.h"
#include "tf16PCSRegEnums.h"
#endif /* _SDK_TEFMOD16_ */

#ifdef _SDK_TEFMOD16_
#define PHYMOD_ST const phymod_access_t
#else
#define PHYMOD_ST tefmod_st
#endif

#define TEFMOD16_CL73_25GBASE_FEC_POS 24
#define TEFMOD16_CL73_25G_RS_FEC_POS 23
#define TEFMOD16_BAM_CL74_REQ 0x3
#define TEFMOD16_BAM_CL91_REQ 0x3
#define REV_A 0x0

#define _RD_SYMBOL_ERROR_CNTR(pc, index, counter) \
        do { \
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_CL91_FEC_SYM_ERR_CTR_LOW##index##r(pc,\
            &RX_X4_CL91_FEC_SYM_ERR_CTR_LOW##index##r_reg));\
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_CL91_FEC_SYM_ERR_CTR_UP##index##r(pc,\
            &RX_X4_CL91_FEC_SYM_ERR_CTR_UP##index##r_reg));\
            *(counter) = RX_X4_CL91_FEC_SYM_ERR_CTR_UP##index##r_FEC_SYMBOL_ERROR_COUNTER_UPPER_##index##f_GET\
            (RX_X4_CL91_FEC_SYM_ERR_CTR_UP##index##r_reg) << 16 |\
            RX_X4_CL91_FEC_SYM_ERR_CTR_LOW##index##r_FEC_SYMBOL_ERROR_COUNTER_LOWER_##index##f_GET\
            (RX_X4_CL91_FEC_SYM_ERR_CTR_LOW##index##r_reg);\
        } while(0)

#define TEFMOD16_RD_SYMBOL_ERROR_CNTR(pc, index, counter) \
        do {                                                       \
            switch (index) {                                       \
                case 0:                                            \
                    _RD_SYMBOL_ERROR_CNTR(pc, 0, counter);          \
                    break;                                         \
                case 1:                                            \
                    _RD_SYMBOL_ERROR_CNTR(pc, 1, counter);          \
                    break;                                         \
                case 2:                                            \
                    _RD_SYMBOL_ERROR_CNTR(pc, 2, counter);          \
                    break;                                         \
                case 3:                                            \
                    _RD_SYMBOL_ERROR_CNTR(pc, 3, counter);          \
                    break;                                         \
                default:                                           \
                return PHYMOD_E_PARAM;                             \
           }                                                       \
        } while (0)

#define TEFMOD16_SYMBOL_ERROR_CNTR_ADD(dst, src)      \
        do {  \
             /* Check overflow */                \
             if ((src) > (0xffffffff - (dst))) { \
                 (dst) = 0xffffffff;           \
             } else {                        \
                 (dst) += (src);                 \
             }                               \
        } while (0)

#ifdef _SDK_TEFMOD16_
  #define TEFMOD16_DBG_IN_FUNC_INFO(pc) \
    PHYMOD_VDBG(TEFMOD16_DBG_FUNC,pc,("-22%s: Adr:%08x Ln:%02d\n", __func__, (unsigned int)pc->addr, (int)pc->lane_mask))
  #define TEFMOD16_DBG_IN_FUNC_VIN_INFO(pc,_print_) \
    PHYMOD_VDBG(TEFMOD16_DBG_FUNCVALIN,pc,_print_)
  #define TEFMOD16_DBG_IN_FUNC_VOUT_INFO(pc,_print_) \
    PHYMOD_VDBG(TEFMOD16_DBG_FUNCVALOUT,pc,_print_)
#endif

#ifdef _DV_TB_
  #define TEFMOD_DBG_IN_FUNC_INFO(pc) \
    PHYMOD_VDBG(TEFMOD_DBG_FUNC, pc, \
      ("TEFMOD IN Function : %s Port Add : %d Lane No: %d\n", \
      __func__, pc->prt_ad, pc->this_lane))
  #define TEFMOD_DBG_IN_FUNC_VIN_INFO(pc,_print_) \
    PHYMOD_VDBG(TEFMOD16_DBG_FUNCVALIN,pc,_print_)
  #define TEFMOD_DBG_IN_FUNC_VOUT_INFO(pc,_print_) \
    PHYMOD_VDBG(TEFMOD_DBG_FUNCVALOUT,pc,_print_)
int phymod_debug_check(uint32_t flags, PHYMOD_ST *pc);
#endif

#define TEFMOD16_PKTGEN_ERR_INJ_EN_FIELD_SET(cl74_en, index) \
    do {\
        if (cl74_en) {\
             switch (index) {                                                                         \
                 case 0:                                                                                 \
                     PKTGEN_ERR_INJ_EN0r_ERRGEN_EN_PH0f_SET(PKTGEN_ERR_INJ_EN0r_reg, 0xf);          \
                     break;                                                                              \
                 case 1:                                                                                 \
                     PKTGEN_ERR_INJ_EN0r_ERRGEN_EN_PH1f_SET(PKTGEN_ERR_INJ_EN0r_reg, 0xf);          \
                     break;                                                                              \
                 case 2:                                                                                 \
                     PKTGEN_ERR_INJ_EN0r_ERRGEN_EN_PH2f_SET(PKTGEN_ERR_INJ_EN0r_reg, 0xf);          \
                     break;                                                                              \
                 case 3:                                                                                 \
                     PKTGEN_ERR_INJ_EN1r_ERRGEN_EN_PH3f_SET(PKTGEN_ERR_INJ_EN1r_reg, 0xf);          \
                     break;                                                                              \
                 default:                                                                                \
                     return PHYMOD_E_PARAM;                                                              \
            }                                                                                            \
       } else {\
             switch (index) {                                                                         \
                 case 0:                                                                                 \
                     PKTGEN_ERR_INJ_EN1r_CL91_40B_ERRGEN_EN_P0f_SET(PKTGEN_ERR_INJ_EN1r_reg, 0x1); \
                     break;                                                                              \
                 case 1:                                                                                 \
                     PKTGEN_ERR_INJ_EN1r_CL91_40B_ERRGEN_EN_P1f_SET(PKTGEN_ERR_INJ_EN1r_reg, 0x1); \
                     break;                                                                              \
                 case 2:                                                                                 \
                     PKTGEN_ERR_INJ_EN1r_CL91_40B_ERRGEN_EN_P2f_SET(PKTGEN_ERR_INJ_EN1r_reg, 0x1); \
                     break;                                                                              \
                 case 3:                                                                                 \
                     PKTGEN_ERR_INJ_EN1r_CL91_40B_ERRGEN_EN_P3f_SET(PKTGEN_ERR_INJ_EN1r_reg, 0x1); \
                     break;                                                                              \
                 default:                                                                                \
                     return PHYMOD_E_PARAM;                                                              \
            }                                                                                            \
       }\
    } while(0)

#define TEFMOD16_PKTGEN_ERR_INJ_EN_FIELD_GET(cl74_en, index, errgen_en) \
     do {\
         if (cl74_en) {\
              switch (index) {                                                                         \
                  case 0:                                                                                 \
                      *errgen_en = PKTGEN_ERR_INJ_EN0r_ERRGEN_EN_PH0f_GET(PKTGEN_ERR_INJ_EN0r_reg);          \
                      break;                                                                              \
                  case 1:                                                                                 \
                      *errgen_en = PKTGEN_ERR_INJ_EN0r_ERRGEN_EN_PH1f_GET(PKTGEN_ERR_INJ_EN0r_reg);          \
                      break;                                                                              \
                  case 2:                                                                                 \
                      *errgen_en = PKTGEN_ERR_INJ_EN0r_ERRGEN_EN_PH2f_GET(PKTGEN_ERR_INJ_EN0r_reg);          \
                      break;                                                                              \
                  case 3:                                                                                 \
                      *errgen_en = PKTGEN_ERR_INJ_EN1r_ERRGEN_EN_PH3f_GET(PKTGEN_ERR_INJ_EN1r_reg);          \
                      break;                                                                              \
                  default:                                                                                \
                      return PHYMOD_E_PARAM;                                                              \
             }                                                                                            \
        } else {\
              switch (index) {                                                                         \
                  case 0:                                                                                 \
                      *errgen_en = PKTGEN_ERR_INJ_EN1r_CL91_40B_ERRGEN_EN_P0f_GET(PKTGEN_ERR_INJ_EN1r_reg); \
                      break;                                                                              \
                  case 1:                                                                                 \
                      *errgen_en = PKTGEN_ERR_INJ_EN1r_CL91_40B_ERRGEN_EN_P1f_GET(PKTGEN_ERR_INJ_EN1r_reg); \
                      break;                                                                              \
                  case 2:                                                                                 \
                      *errgen_en = PKTGEN_ERR_INJ_EN1r_CL91_40B_ERRGEN_EN_P2f_GET(PKTGEN_ERR_INJ_EN1r_reg); \
                      break;                                                                              \
                  case 3:                                                                                 \
                      *errgen_en = PKTGEN_ERR_INJ_EN1r_CL91_40B_ERRGEN_EN_P3f_GET(PKTGEN_ERR_INJ_EN1r_reg); \
                      break;                                                                              \
                  default:                                                                                \
                      return PHYMOD_E_PARAM;                                                              \
             }                                                                                            \
        }\
     } while(0)

/**
@brief   getRevDetails , calls revid_read
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details
*/
/* internal function */
STATIC
int _tefmod16_getRevDetails(PHYMOD_ST* pc)
{
  MAIN0_SERDESIDr_t MAIN0_SERDESIDr_reg;
  MAIN0_SERDESIDr_CLR(MAIN0_SERDESIDr_reg);
  PHYMOD_IF_ERR_RETURN(READ_MAIN0_SERDESIDr(pc, &MAIN0_SERDESIDr_reg));
  return MAIN0_SERDESIDr_GET(MAIN0_SERDESIDr_reg);
}

/**
@brief Read the 16 bit rev. id value etc.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  revIdV receives the revid
@returns The value of the revid.
@details
This  fucntion reads the revid register that contains core number, revision
number and returns it.
*/
int tefmod16_revid_read(PHYMOD_ST* pc, uint32_t *revIdV)              /* REVID_READ */
{
#ifdef _DV_TB_
  int laneselect;
#endif
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
#ifdef _DV_TB_
  laneselect = pc->lane_select;
  pc->lane_select=TEFMOD16_LANE_0_0_0_1;

  PHYMOD_DEBUG_ERROR(("%-22s u=%0d p=%0d sel=%x ln=%0d dxgxs=%0d\n", __func__,
            pc->unit, pc->port, pc->lane_select, pc->this_lane, pc->dxgxs));
#endif
  *revIdV = _tefmod16_getRevDetails(pc);
#ifdef _DV_TB_
  pc->lane_select=laneselect;
#endif
  return PHYMOD_E_NONE;
}

/**
@brief Read tscf model_number.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value of the model_number.
@details
This  fucntion reads the revid register that contains core number, revision
number and returns it.
*/
int tefmod16_model_number_read(PHYMOD_ST* pc, uint32_t *model_number)              /* model_number */
{
  MAIN0_SERDESIDr_t MAIN0_SERDESIDr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  MAIN0_SERDESIDr_CLR(MAIN0_SERDESIDr_reg);
  PHYMOD_IF_ERR_RETURN(READ_MAIN0_SERDESIDr(pc, &MAIN0_SERDESIDr_reg));
  *model_number = MAIN0_SERDESIDr_MODEL_NUMBERf_GET(MAIN0_SERDESIDr_reg);
  
  return PHYMOD_E_NONE;
}

/**
@brief   This function reads TX-PLL PLL_LOCK bit.
@param   pc  handle to current TSCF context (#PHYMOD_ST)
@param   lockStatus reference which is updated with lock status.
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Read PLL lock status. Returns  1 or 0 (locked/not)
*/
int tefmod16_pll_lock_get(PHYMOD_ST* pc, int* lockStatus)
{
  PMD_X1_STSr_t  reg_pmd_x1_sts;
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  READ_PMD_X1_STSr(pc, &reg_pmd_x1_sts);

  *lockStatus =  PMD_X1_STSr_PLL_LOCK_STSf_GET(reg_pmd_x1_sts);
  /* TEFMOD16_DBG_IN_FUNC_VOUT_INFO(pc,("PLL lockStatus: %d", *lockStatus)); */
  return PHYMOD_E_NONE;
}

/**
@brief   This function reads RX-PMD PMD_LOCK bit.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   lockStatus reference which is updated with pmd_lock status
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Read PMD lock status Returns 1 or 0 (locked/not)
*/
#ifdef _DV_TB_
int tefmod_pmd_lock_get(PHYMOD_ST* pc, uint32_t* lockStatus)
{
  PMD_X4_STSr_t  reg_pmd_x4_sts;
  int i;
  int lane_mask;
  PHYMOD_ST* pc_copy;
  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  pc_copy = pc;
  pc_copy->port_type = TEFMOD16_MULTI_PORT;
  lane_mask = 0;
  switch(pc->port_type) {
    case TEFMOD_MULTI_PORT:   lane_mask = 0x1 << pc->this_lane;  break;
    case TEFMOD_TRI1_PORT:    if(pc->this_lane == 3) lane_mask = 0xc;  
                              else lane_mask = 0x1 << pc->this_lane;
                              break;
    case TEFMOD_TRI2_PORT:    if(pc->this_lane == 0) lane_mask = 0x3;
                              else lane_mask = 0x1 << pc->this_lane;
                              break;
    case TEFMOD_DXGXS:        if(pc->this_lane == 0)  lane_mask = 0x3;  
                              else lane_mask = 0xc;
                              break;
    case TEFMOD_SINGLE_PORT:  lane_mask = 0xf;  break;
    default: break;
  }
  
  *lockStatus = 0;
  for(i=0; i<4; i++) {
      if(((lane_mask >> i) & 0x1) == 1) {
         pc_copy->this_lane = i;
         READ_PMD_X4_STSr(pc_copy, &reg_pmd_x4_sts);
         *lockStatus = *lockStatus & PMD_X4_STSr_RX_LOCK_STSf_GET(reg_pmd_x4_sts);
      }
   }
  /* TEFMOD_DBG_IN_FUNC_VOUT_INFO(pc,("PMD lockStatus: %d", *lockStatus)); */

  return PHYMOD_E_NONE;
}
#endif

#ifdef _SDK_TEFMOD16_
int tefmod16_pmd_lock_get(PHYMOD_ST* pc, uint32_t* lockStatus)
{
  PMD_X4_STSr_t  reg_pmd_x4_sts;
  int i;
  phymod_access_t pa_copy;
  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  *lockStatus = 1;
   PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
   
   for(i=0; i<4; i++) {
      if(((pc->lane_mask >> i) & 0x1) == 1) {
         pa_copy.lane_mask = 0x1 << i;
         READ_PMD_X4_STSr(&pa_copy, &reg_pmd_x4_sts);
         *lockStatus = *lockStatus & PMD_X4_STSr_RX_LOCK_STSf_GET(reg_pmd_x4_sts);
      }
   }

  /* TEFMOD16_DBG_IN_FUNC_VOUT_INFO(pc,("PMD lockStatus: %d", *lockStatus)); */

  return PHYMOD_E_NONE;
}
#endif

/**
@brief   Init routine sets various operating modes of TSCF.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   pmd_touched   Is this the first time we are visiting the PMD.
@param   spd_intf  Input of enum type #tefmod16_spd_intfc_type_t
@param   pll_mode to override the pll div 
@returns PHYMOD_E_NONE if successful. PHYMOD_E_FAIL else.
@details

This function is called once per TSCF. It cannot be called multiple times
and should be called immediately after reset. Elements of #PHYMOD_ST should be
initialized to required values prior to calling this function. The following
sub-configurations are performed here.

\li Set pll divider for VCO setting in PMD. pll divider is calculated from max_speed. 
*/

int tefmod16_set_pll_mode(PHYMOD_ST* pc, int pmd_touched, tefmod16_spd_intfc_type_t spd_intf, int pll_mode)     /* SET_PLL_MODE */
{
  PLL_CAL_CTL7r_t    reg_ctl7;
  int speed;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  /* TEFMOD16_DBG_IN_FUNC_VIN_INFO(pc,("pmd_touched: %d, spd_intf: %d, pll_mode: %d", pmd_touched, spd_intf, pll_mode)); */

  PLL_CAL_CTL7r_CLR(reg_ctl7);
  if (pmd_touched == 0) {
    tefmod16_get_mapped_speed(spd_intf, &speed);
    /*Support Override PLL DIV */
    if(pll_mode & 0x80000000) {
      PLL_CAL_CTL7r_PLL_MODEf_SET(reg_ctl7, (pll_mode) & 0x0000ffff);
    } else {
       PLL_CAL_CTL7r_PLL_MODEf_SET(reg_ctl7, (sc_pmd16_entry[speed].pll_mode));
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

} /* tefmod16_set_pll_mode(PHYMOD_ST* pc) */

/*!
@brief   get  port speed id configured
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   speed_id Receives the resolved speed cfg in the speed_id
@returns The value PHYMOD_E_NONE upon successful completion.
@details get  port speed configured
*/

int tefmod16_speed_id_get(PHYMOD_ST* pc, int *speed_id)
{
  SC_X4_RSLVD_SPDr_t sc_final_resolved_speed;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  SC_X4_RSLVD_SPDr_CLR(sc_final_resolved_speed);
  READ_SC_X4_RSLVD_SPDr(pc,&sc_final_resolved_speed);
  *speed_id = SC_X4_RSLVD_SPDr_SPEEDf_GET(sc_final_resolved_speed);
  /* TEFMOD16_DBG_IN_FUNC_VOUT_INFO(pc,("speed_id: %d", *speed_id)); */

  return PHYMOD_E_NONE;
}

int tefmod16_refclk_set(PHYMOD_ST* pc, phymod_ref_clk_t ref_clock)
{
    DIG_TOP_USER_CTL0r_t dig_top_user_reg;
    READ_DIG_TOP_USER_CTL0r(pc, &dig_top_user_reg);
    switch  (ref_clock) {
        case phymodRefClk156Mhz:
            DIG_TOP_USER_CTL0r_HEARTBEAT_COUNT_1USf_SET(dig_top_user_reg, 0x271);
            break;
        case phymodRefClk125Mhz:
            DIG_TOP_USER_CTL0r_HEARTBEAT_COUNT_1USf_SET(dig_top_user_reg, 0x1f4);
            break;
        default:
            DIG_TOP_USER_CTL0r_HEARTBEAT_COUNT_1USf_SET(dig_top_user_reg, 0x271);
            break;
    }
    MODIFY_DIG_TOP_USER_CTL0r(pc, dig_top_user_reg);

    return PHYMOD_E_NONE;
}

/**
@brief   Init the PMD
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   pmd_touched If the PMD is already initialized
@returns The value PHYMOD_E_NONE upon successful completion
@details Per core PMD resets (both datapath and entire core)
We only intend to use this function if the PMD has never been initialized.
*/
int tefmod16_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched) /* PMD_RESET_SEQ */
{
  PMD_X1_CTLr_t reg_pmd_x1_ctrl;
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  /* TEFMOD16_DBG_IN_FUNC_VIN_INFO(pc,("pmd_touched: %x", pmd_touched)); */

  PMD_X1_CTLr_CLR(reg_pmd_x1_ctrl);

  if (pmd_touched == 0) {
    PMD_X1_CTLr_POR_H_RSTBf_SET(reg_pmd_x1_ctrl,1);
    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(reg_pmd_x1_ctrl,1);
    PHYMOD_IF_ERR_RETURN(WRITE_PMD_X1_CTLr(pc,reg_pmd_x1_ctrl));
  }
  return PHYMOD_E_NONE;
}

/**
@brief   Checks config valid status for the port 
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   port_num Port Number
@returns The value PHYMOD_E_NONE upon successful completion
@details This register bit indicates that PCS is now programmed as required by
the HT entry for that speed
*/

int tefmod16_master_port_num_set( PHYMOD_ST *pc,  int port_num)
{
  MAIN0_SETUPr_t main_reg;
  /* TEFMOD16_DBG_IN_FUNC_VIN_INFO(pc,("port_num: %d", port_num)); */

  MAIN0_SETUPr_CLR(main_reg);
  MAIN0_SETUPr_MASTER_PORT_NUMf_SET(main_reg, port_num);
  MODIFY_MAIN0_SETUPr(pc, main_reg);

  return PHYMOD_E_NONE;
}

/**
@brief   update the port mode 
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   pll_restart Receives info. on whether to restart pll.
@returns The value PHYMOD_E_NONE upon successful completion
*/
int tefmod16_update_port_mode( PHYMOD_ST *pc, int *pll_restart)
{
  MAIN0_SETUPr_t mode_reg;
  int port_mode_sel, port_mode_sel_reg, temp_pll_restart;
  uint32_t single_port_mode;
  uint32_t first_couple_mode = 0, second_couple_mode = 0;

  port_mode_sel = 0 ;
  single_port_mode = 0 ;
  temp_pll_restart = 0;

  READ_MAIN0_SETUPr(pc, &mode_reg);
  port_mode_sel_reg = MAIN0_SETUPr_PORT_MODE_SELf_GET(mode_reg);

  if(pc->lane_mask == 0xf){
    port_mode_sel = 4;
    if( port_mode_sel_reg != 4){
      temp_pll_restart = 1;
    }
  } else{
    first_couple_mode = ((port_mode_sel_reg == 2) || (port_mode_sel_reg == 3) || (port_mode_sel_reg == 4));
    second_couple_mode = ((port_mode_sel_reg == 1) || (port_mode_sel_reg == 3) || (port_mode_sel_reg == 4));
    switch(pc->lane_mask){
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
  
    if(first_couple_mode ){
      port_mode_sel =(second_couple_mode)? 3: 2;
    } else if(second_couple_mode) {
      port_mode_sel = 1;
    } else{
      port_mode_sel = 0 ;
    }
  }

  *pll_restart = temp_pll_restart;

  /*if(pc->verbosity & TEFMOD16_DBG_INIT)
  dprintf("%-22s u=%0d p=%0d port_mode_sel old=%0d new=%0d\n", __func__, 
            pc->unit, pc->port, port_mode_sel_reg, port_mode_sel) ; */

  MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(mode_reg, single_port_mode);
  MAIN0_SETUPr_PORT_MODE_SELf_SET(mode_reg, port_mode_sel);
  MODIFY_MAIN0_SETUPr(pc, mode_reg);
  /* TEFMOD16_DBG_IN_FUNC_VOUT_INFO(pc,("pll_restart: %d", *pll_restart)); */
  return PHYMOD_E_NONE ;
}

/**
@brief   Enable the pll reset bit
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   enable Controls whether to reset PLL
@returns The value PHYMOD_E_NONE upon successful completion
@details
Resets the PLL
*/
int tefmod16_pll_reset_enable_set (PHYMOD_ST *pc, int enable)
{
  MAIN0_SPD_CTLr_t main_reg; 

  MAIN0_SPD_CTLr_CLR(main_reg);
  MAIN0_SPD_CTLr_PLL_RESET_ENf_SET(main_reg, enable);
  MODIFY_MAIN0_SPD_CTLr(pc, main_reg);
 
  return PHYMOD_E_NONE ;
}

/**
@brief   Read PCS Link status
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   *link Reference for Status of PCS Link
@returns The value PHYMOD_E_NONE upon successful completion
@details Return the status of the PCS link. The link up implies the PCS is able
to decode the digital bits coming in on the serdes. It automatically implies
that the PLL is stable and that the PMD sublayer has successfully recovered the
clock on the receive line.
*/
int tefmod16_get_pcs_link_status(PHYMOD_ST* pc, uint32_t *link)
{
  RX_X4_PCS_LIVE_STS1r_t reg_pcs_live_sts;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  RX_X4_PCS_LIVE_STS1r_CLR(reg_pcs_live_sts);
  PHYMOD_IF_ERR_RETURN (READ_RX_X4_PCS_LIVE_STS1r(pc, &reg_pcs_live_sts));
  *link = RX_X4_PCS_LIVE_STS1r_LINK_STATUSf_GET(reg_pcs_live_sts);
  /* TEFMOD16_DBG_IN_FUNC_VOUT_INFO(pc,("pcs_live_stats_link: %d", *link)); */

  return PHYMOD_E_NONE;
}

int tefmod16_get_pcs_latched_link_status(PHYMOD_ST* pc, uint32_t *link)
{
  RX_X4_PCS_LIVE_STS1r_t reg_pcs_live_sts;
  RX_X4_PCS_LATCH_STS1r_t latched_sts ;
  int                     latched_val ;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  RX_X4_PCS_LIVE_STS1r_CLR(reg_pcs_live_sts);
  RX_X4_PCS_LATCH_STS1r_CLR(latched_sts) ;

  PHYMOD_IF_ERR_RETURN (READ_RX_X4_PCS_LIVE_STS1r(pc, &reg_pcs_live_sts));
  PHYMOD_IF_ERR_RETURN (READ_RX_X4_PCS_LATCH_STS1r(pc, &latched_sts)) ;
  latched_val = RX_X4_PCS_LATCH_STS1r_LINK_STATUS_LLf_GET(latched_sts) ;
  if(latched_val) {
      *link = 0 ;
  } else {
      *link = RX_X4_PCS_LIVE_STS1r_LINK_STATUSf_GET(reg_pcs_live_sts);
  }
  /* TEFMOD16_DBG_IN_FUNC_VOUT_INFO(pc,("pcs_live_stats_link: %d", *link)); */

  return PHYMOD_E_NONE;
}

/**
@brief   Get the Port status
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   enabled  Receives status on port enabled 
@returns The value PHYMOD_E_NONE upon successful completion
*/

int tefmod16_enable_get(PHYMOD_ST* pc, uint32_t* enabled)
{
  SC_X4_CTLr_t reg_sc_ctrl;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  SC_X4_CTLr_CLR(reg_sc_ctrl);

  PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc,&reg_sc_ctrl));
  *enabled = SC_X4_CTLr_SW_SPEED_CHANGEf_GET(reg_sc_ctrl);

  return PHYMOD_E_NONE;
}
/**
@brief   Get info on Disable status of the Port
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Disables the port by writing 0 to the speed config logic in PCS.
This makes the PCS to bring down the PCS blocks and also apply lane datapath
reset to the PMD. There is no control input to this function since it only does
one thing.
*/
int tefmod16_disable_set(PHYMOD_ST* pc)
{
  SC_X4_CTLr_t reg_sc_ctrl;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  SC_X4_CTLr_CLR(reg_sc_ctrl);
  READ_SC_X4_CTLr(pc, &reg_sc_ctrl);

  /* write 0 to the speed change */
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 0);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc,reg_sc_ctrl));

  return PHYMOD_E_NONE;
}

int tefmod16_enable_set(PHYMOD_ST* pc)
{
  SC_X4_CTLr_t reg_sc_ctrl;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  SC_X4_CTLr_CLR(reg_sc_ctrl);
  READ_SC_X4_CTLr(pc, &reg_sc_ctrl);

  /* write 0 to the speed change */
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc,reg_sc_ctrl));

  return PHYMOD_E_NONE;
}


/**
@brief  Get the plldiv from lookup table
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  spd_intf  Input of enum type #tefmod16_spd_intfc_type_t
@param  plldiv  Receives PLL Divider value
@returns The value PHYMOD_E_NONE upon successful completion
@details Get the plldiv from lookup table as a function of the speed.
*/

int tefmod16_plldiv_lkup_get(PHYMOD_ST* pc, tefmod16_spd_intfc_type_t spd_intf, phymod_ref_clk_t refclk,  uint32_t *plldiv)
{
  int speed_id;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  tefmod16_get_mapped_speed(spd_intf, &speed_id);
  if (refclk == phymodRefClk125Mhz) {
    *plldiv = sc_pmd16_entry_125M_ref[speed_id].pll_mode;
  } else {
    *plldiv = sc_pmd16_entry[speed_id].pll_mode;
  }
  /* TEFMOD16_DBG_IN_FUNC_VOUT_INFO(pc,("plldiv: %d", *plldiv)); */

  return PHYMOD_E_NONE;

}

/**
@brief   Get the osmode from lookup table
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   spd_intf  Input of enum type #tefmod16_spd_intfc_type_t
@param   osmode Receives the OS mode as assumed in the hard table.
@returns The value PHYMOD_E_NONE upon successful completion
@details Get the osmode from software version of Speed table as a function of
the speed
*/
int tefmod16_osmode_lkup_get(PHYMOD_ST* pc, tefmod16_spd_intfc_type_t spd_intf, uint32_t *osmode)
{
  int speed_id;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  tefmod16_get_mapped_speed(spd_intf, &speed_id);
  *osmode = sc_pmd16_entry[speed_id].t_pma_os_mode;

  return PHYMOD_E_NONE;
}

/**
@brief   sets both the RX and TX lane swap values for all lanes simultaneously.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   tx_rx_swap   Receives the PCS lane swap value 
@returns The value PHYMOD_E_NONE upon successful completion
@details
This function gets the TX lane swap values for all lanes simultaneously.
*/

int tefmod16_pcs_tx_lane_swap_get ( PHYMOD_ST *pc,  uint32_t *tx_swap)
{
  unsigned int pcs_map;
  TX_X1_TX_LN_SWPr_t reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  TX_X1_TX_LN_SWPr_CLR(reg);

  PHYMOD_IF_ERR_RETURN
      (READ_TX_X1_TX_LN_SWPr(pc, &reg));

  pcs_map = TX_X1_TX_LN_SWPr_GET(reg);

  *tx_swap = (((pcs_map >> 0)  & 0x3) << 0) |
                (((pcs_map >> 2)  & 0x3) << 4) |
                (((pcs_map >> 4)  & 0x3) << 8) |
                (((pcs_map >> 6)  & 0x3) << 12) ;

  return PHYMOD_E_NONE ;
}

int tefmod16_pcs_rx_lane_swap_get ( PHYMOD_ST *pc,  uint32_t *rx_swap)
{
  unsigned int pcs_map;
  RX_X1_RX_LN_SWPr_t reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  RX_X1_RX_LN_SWPr_CLR(reg);

  PHYMOD_IF_ERR_RETURN
      (READ_RX_X1_RX_LN_SWPr(pc, &reg)) ;

  pcs_map = RX_X1_RX_LN_SWPr_GET(reg);

  *rx_swap = (((pcs_map >> 0)  & 0x3) << 0) |
                (((pcs_map >> 2)  & 0x3) << 4) |
                (((pcs_map >> 4)  & 0x3) << 8) |
                (((pcs_map >> 6)  & 0x3) << 12) ;

  return PHYMOD_E_NONE ;
}


int tefmod16_pmd_lane_swap_tx_rx_get ( PHYMOD_ST *pc, uint32_t *tx_lane_map, uint32_t *rx_lane_map)
{
  uint16_t tx_lane_map_0, tx_lane_map_1, tx_lane_map_2, tx_lane_map_3;
  uint16_t rx_lane_map_0, rx_lane_map_1, rx_lane_map_2, rx_lane_map_3;
  LN_ADDR0r_t ln0_reg;
  LN_ADDR1r_t ln1_reg;
  LN_ADDR2r_t ln2_reg;
  LN_ADDR3r_t ln3_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  LN_ADDR0r_CLR(ln0_reg);
  LN_ADDR1r_CLR(ln1_reg);
  LN_ADDR2r_CLR(ln2_reg);
  LN_ADDR3r_CLR(ln3_reg);

  PHYMOD_IF_ERR_RETURN
     (READ_LN_ADDR0r(pc, &ln0_reg));
  PHYMOD_IF_ERR_RETURN
     (READ_LN_ADDR1r(pc, &ln1_reg));
  PHYMOD_IF_ERR_RETURN
     (READ_LN_ADDR2r(pc, &ln2_reg));
  PHYMOD_IF_ERR_RETURN
     (READ_LN_ADDR3r(pc, &ln3_reg));

  tx_lane_map_0 = LN_ADDR0r_TX_LANE_ADDR_0f_GET(ln0_reg);
  rx_lane_map_0 = LN_ADDR0r_RX_LANE_ADDR_0f_GET(ln0_reg);
  tx_lane_map_1 = LN_ADDR1r_TX_LANE_ADDR_1f_GET(ln1_reg);
  rx_lane_map_1 = LN_ADDR1r_RX_LANE_ADDR_1f_GET(ln1_reg);
  tx_lane_map_2 = LN_ADDR2r_TX_LANE_ADDR_2f_GET(ln2_reg);
  rx_lane_map_2 = LN_ADDR2r_RX_LANE_ADDR_2f_GET(ln2_reg);
  tx_lane_map_3 = LN_ADDR3r_TX_LANE_ADDR_3f_GET(ln3_reg);
  rx_lane_map_3 = LN_ADDR3r_RX_LANE_ADDR_3f_GET(ln3_reg);

  *tx_lane_map = ((tx_lane_map_0 & 0xf) << 0)
              | ((tx_lane_map_1 & 0xf) << 4)
              | ((tx_lane_map_2 & 0xf) << 8)
              | ((tx_lane_map_3 & 0xf) << 12);

  *rx_lane_map = ((rx_lane_map_0 & 0xf) << 0)
              | ((rx_lane_map_1 & 0xf) << 4)
              | ((rx_lane_map_2 & 0xf) << 8)
              | ((rx_lane_map_3 & 0xf) << 12);

  return PHYMOD_E_NONE ;
}


int tefmod16_pmd_lane_swap_tx_get ( PHYMOD_ST *pc, uint32_t *tx_lane_map)
{
/*TBD : check if PMD has lane swap control
  uint16_t tx_lane_map_0, tx_lane_map_1, tx_lane_map_2, tx_lane_map_3;
  DIG_TX_LN_MAP_0_1_2r_t              reg;
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_t    reg1;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
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
              | ((tx_lane_map_3 & 0xf) << 12);  */


  return PHYMOD_E_NONE ;
}


int tefmod16_pmd_lane_dp_reset(PHYMOD_ST* pc, uint16_t reset_value)
{
  return PHYMOD_E_NONE;
}


/**
@brief   rx lane reset and enable of any particular lane
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   enable to reset the lane.
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This function enables/disables rx lane (RSTB_LANE) or read back control bit for
that based on per_lane_control being 1 or 0. If per_lane_control is 0xa, only
read back RSTB_LANE.
*/
int tefmod16_rx_lane_control_set(PHYMOD_ST* pc, int enable)         /* RX_LANE_CONTROL */
{
  RX_X4_PMA_CTL0r_t    reg_pma_ctrl;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  RX_X4_PMA_CTL0r_CLR(reg_pma_ctrl);

  if (enable) {
    RX_X4_PMA_CTL0r_RSTB_LANEf_SET( reg_pma_ctrl, 0);
    PHYMOD_IF_ERR_RETURN (MODIFY_RX_X4_PMA_CTL0r(pc, reg_pma_ctrl));
    RX_X4_PMA_CTL0r_RSTB_LANEf_SET(reg_pma_ctrl, 1);
    PHYMOD_IF_ERR_RETURN (MODIFY_RX_X4_PMA_CTL0r(pc, reg_pma_ctrl));
  } else {
     /* bit set to 0 for disabling RXP */
    RX_X4_PMA_CTL0r_RSTB_LANEf_SET( reg_pma_ctrl, 0);
    PHYMOD_IF_ERR_RETURN (MODIFY_RX_X4_PMA_CTL0r(pc, reg_pma_ctrl));
  }
  return PHYMOD_E_NONE;
}

/**
@brief   rx lane reset and enable of any particular lane
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   Get the configured reset lane.
@returns The value PHYMOD_E_NONE upon successful completion.
@details
          This function read back control bit
*/
int tefmod16_rx_lane_control_get(PHYMOD_ST* pc, int *enable)         /* RX_LANE_CONTROL */
{
  RX_X4_PMA_CTL0r_t    reg_pma_ctrl;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  RX_X4_PMA_CTL0r_CLR(reg_pma_ctrl);
  PHYMOD_IF_ERR_RETURN (READ_RX_X4_PMA_CTL0r(pc, &reg_pma_ctrl));
  *enable =   RX_X4_PMA_CTL0r_RSTB_LANEf_GET( reg_pma_ctrl);

  return PHYMOD_E_NONE;
}



/**
@brief   Gets the TX And RX Polarity 
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   tx_polarity Receives the TX polarity
@param   rx_polarity Receives the RX polarity
@returns The value PHYMOD_E_NONE upon successful completion.
@details Gets the TX And RX Polarity from hardware.

*/
int tefmod16_tx_rx_polarity_get ( PHYMOD_ST *pc, uint32_t* tx_polarity, uint32_t* rx_polarity)
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
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   tx_polarity Controls the TX polarity
@param   rx_polarity Controls the RX polarity
@returns The value PHYMOD_E_NONE upon successful completion.
@details Sets the TX And RX Polarity
*/
int tefmod16_tx_rx_polarity_set ( PHYMOD_ST *pc, uint32_t tx_polarity, uint32_t rx_polarity)
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
@brief   Read the 16 bit rev. id value etc.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init
*/

int tefmod16_trigger_speed_change(PHYMOD_ST* pc)
{
  SC_X4_CTLr_t    reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  /* write 0 to the speed change */
  SC_X4_CTLr_CLR(reg);
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc, &reg));
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg, 0);
  PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_CTLr(pc, reg));

  /* write 1 to the speed change. No need to read again before write*/
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg, 1);
  PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_CTLr(pc, reg));

  return PHYMOD_E_NONE;
}

/*!
@brief Override TX lane output.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  tx control lanes to disable 
@returns PHYMOD_E_NONE if no errors. PHYMOD_ERROR else.
@details This function enables/disables, via override, transmission on a specific lane. No reset is
required to restart the transmission. Lane control is done through 'ovrr' and 'value' inputs.
<B>Set ovrr=0 & value=0 : No override of tx_disable to PMD lanes 0/1/2/3</B>
<B>Set ovrr=0 & value=1 : No override of tx_disable to PMD lanes 0/1/2/3</B>
<B>Set ovrr=1 & value=0 : Override tx_disable to inactive for PMD lanes 0/1/2/3</B>
<B>Set ovrr=1 & value=1 : Override tx_disable to active for PMD lanes 0/1/2/3</B>
*/

int tefmod16_tx_lane_disable (PHYMOD_ST* pc, int tx)
{
  PMD_X4_CTLr_t PMD_X4_PMD_X4_CONTROLr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);

  PMD_X4_CTLr_TX_DISABLEf_SET(PMD_X4_PMD_X4_CONTROLr_reg, tx);
  PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));
  return PHYMOD_E_NONE;
}

/**
@brief   Select the ILKN path and bypass TSCF PCS
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details This will enable ILKN path. PCS is set to bypass to relinquish PMD
control. Expect PMD to be programmed elsewhere.
*/
int tefmod16_init_pcs_ilkn(PHYMOD_ST* pc)              /* INIT_PCS_ILKN */
{
  ILKN_CTL0r_t ILKN_CONTROL0r_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  PHYMOD_IF_ERR_RETURN(READ_ILKN_CTL0r(pc, &ILKN_CONTROL0r_reg));
  ILKN_CTL0r_CREDIT_ENf_SET(ILKN_CONTROL0r_reg, 1);
  ILKN_CTL0r_ILKN_SELf_SET(ILKN_CONTROL0r_reg, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_ILKN_CTL0r(pc, ILKN_CONTROL0r_reg));

  return PHYMOD_E_NONE;
}

/**
@brief   Check if ILKN is set
@param   pc handle to current TSCF context (#PHYMOD_ST), ilkn_set status
@returns The value PHYMOD_E_NONE upon successful completion
@details Check if ILKN is set, ilkn_set = 1 if it is set.
*/
int tefmod16_pcs_ilkn_chk(PHYMOD_ST* pc, int *ilkn_set)              /* INIT_PCS_ILKN */
{
  ILKN_CTL0r_t ILKN_CONTROL0r_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  PHYMOD_IF_ERR_RETURN(READ_ILKN_CTL0r(pc, &ILKN_CONTROL0r_reg));
  *ilkn_set = ILKN_CTL0r_ILKN_SELf_GET(ILKN_CONTROL0r_reg);

  return PHYMOD_E_NONE;
}


#ifdef _DV_TB_
/*!
@brief Controls PMD reset pins irrespective of PCS is in Speed Control mode or not
aram  pc handle to current TSCF context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details Set the ::PHYMOD_ST::per_lane_control field of #PHYMOD_ST to <B>0 to
disable</B> PCS  <B>1 to enable</B>.
*/

/* Ensure that tier2 and phymod control exist to access this function */
int tefmod_pmd_lane_reset_bypass (PHYMOD_ST* pc, int pmd_reset_control)     /* PMD_LANE_RESET_BYPASS */
{
  int cntl;
  PMD_X4_OVRRr_t PMD_X4_OVERRIDEr_reg;
  PMD_X4_CTLr_t PMD_X4_PMD_X4_CONTROLr_reg;

  TEFMOD_DBG_IN_FUNC_INFO(pc);

  cntl = pmd_reset_control & 0x10;
 
  if (cntl) {
    PMD_X4_OVRRr_CLR(PMD_X4_OVERRIDEr_reg);
    PMD_X4_OVRRr_LN_DP_H_RSTB_OENf_SET(PMD_X4_OVERRIDEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_OVRRr(pc, PMD_X4_OVERRIDEr_reg));

    PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);
    PMD_X4_CTLr_LN_DP_H_RSTBf_SET(PMD_X4_PMD_X4_CONTROLr_reg,0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));

    /* toggle added */
    PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);
    PMD_X4_CTLr_LN_DP_H_RSTBf_SET(PMD_X4_PMD_X4_CONTROLr_reg,1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));

    PMD_X4_OVRRr_CLR(PMD_X4_OVERRIDEr_reg);
    PMD_X4_OVRRr_LN_DP_H_RSTB_OENf_SET(PMD_X4_OVERRIDEr_reg,0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_OVRRr(pc, PMD_X4_OVERRIDEr_reg));
  } else {
    PMD_X4_OVRRr_CLR(PMD_X4_OVERRIDEr_reg);
    PMD_X4_OVRRr_LN_DP_H_RSTB_OENf_SET(PMD_X4_OVERRIDEr_reg,0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_OVRRr(pc, PMD_X4_OVERRIDEr_reg));

    PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);
    PMD_X4_CTLr_LN_DP_H_RSTBf_SET(PMD_X4_PMD_X4_CONTROLr_reg,1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));
  }

   cntl = pmd_reset_control & 0x1;
  if (cntl) {

    PMD_X1_OVRRr_CLR(PMD_X1_OVERRIDEr_reg);
    PMD_X1_OVRRr_CORE_DP_H_RSTB_OENf_SET(PMD_X1_OVERRIDEr_reg,1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_OVRRr(pc, PMD_X1_OVERRIDEr_reg));

    PMD_X1_CTLr_CLR(PMD_X1_PMD_X1_CONTROLr_reg);
    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(PMD_X1_PMD_X1_CONTROLr_reg,0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_CTLr(pc, PMD_X1_PMD_X1_CONTROLr_reg));

    /* toggle added */
    PMD_X1_CTLr_CLR(PMD_X1_PMD_X1_CONTROLr_reg);
    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(PMD_X1_PMD_X1_CONTROLr_reg,1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_CTLr(pc, PMD_X1_PMD_X1_CONTROLr_reg));

    PMD_X1_OVRRr_CLR(PMD_X1_OVERRIDEr_reg);
    PMD_X1_OVRRr_CORE_DP_H_RSTB_OENf_SET(PMD_X1_OVERRIDEr_reg,0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_OVRRr(pc, PMD_X1_OVERRIDEr_reg));
  } else {
    PMD_X1_OVRRr_CLR(PMD_X1_OVERRIDEr_reg);
    PMD_X1_OVRRr_CORE_DP_H_RSTB_OENf_SET(PMD_X1_OVERRIDEr_reg,0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_OVRRr(pc, PMD_X1_OVERRIDEr_reg));

    PMD_X1_CTLr_CLR(PMD_X1_PMD_X1_CONTROLr_reg);
    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(PMD_X1_PMD_X1_CONTROLr_reg,1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_CTLr(pc, PMD_X1_PMD_X1_CONTROLr_reg));
  }

  SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg,0);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

  SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg,1);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

  return PHYMOD_E_NONE;
} /* PMD_RESET_BYPASS */


int tefmod_pmd_core_reset_bypass(PHYMOD_ST* pc, int pmd_reset_control) { /* PMD_CORE_RESET_BYPASS */

  int cntl;
  PMD_X1_OVRRr_t PMD_X1_OVERRIDEr_reg;
  PMD_X1_CTLr_t PMD_X1_PMD_X1_CONTROLr_reg;

  TEFMOD_DBG_IN_FUNC_INFO(pc);

  cntl = pmd_reset_control & 0x1; 

  if (cntl) {

    PMD_X1_OVRRr_CLR(PMD_X1_OVERRIDEr_reg);
    PMD_X1_OVRRr_CORE_DP_H_RSTB_OENf_SET(PMD_X1_OVERRIDEr_reg,1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_OVRRr(pc, PMD_X1_OVERRIDEr_reg));

    PMD_X1_CTLr_CLR(PMD_X1_PMD_X1_CONTROLr_reg);
    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(PMD_X1_PMD_X1_CONTROLr_reg,1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_CTLr(pc, PMD_X1_PMD_X1_CONTROLr_reg));

  } else {
    PMD_X1_OVRRr_CLR(PMD_X1_OVERRIDEr_reg);
    PMD_X1_OVRRr_CORE_DP_H_RSTB_OENf_SET(PMD_X1_OVERRIDEr_reg,0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_OVRRr(pc, PMD_X1_OVERRIDEr_reg));

    PMD_X1_CTLr_CLR(PMD_X1_PMD_X1_CONTROLr_reg);
    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(PMD_X1_PMD_X1_CONTROLr_reg,1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_CTLr(pc, PMD_X1_PMD_X1_CONTROLr_reg));
  }

  return PHYMOD_E_NONE;

}
#endif

 
/*!
@brief Controls the setting/resetting of autoneg advertisement registers.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  cl73_adv struct of type  #tefmod16_an_adv_ability_t 
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details
TBD
*/

#ifdef _DV_TB_
int tefmod16_autoneg_set(PHYMOD_ST* pc)               /* AUTONEG_SET */
{
  uint16_t data;
  uint16_t cl73_bam_code;
  /*  uint16_t tx_nonce; */
  AN_X1_OUI_LWRr_t AN_X1_CONTROL_OUI_LOWERr_reg;
  AN_X1_OUI_UPRr_t AN_X1_CONTROL_OUI_UPPERr_reg;
  AN_X4_CL73_CTLSr_t AN_X4_ABILITIES_CL73_CONTROLSr_reg;
  AN_X4_LD_BAM_ABILr_t AN_X4_ABILITIES_LD_BAM_ABILITIESr_reg;
  AN_X4_LD_BASE_ABIL1r_t AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg;
  AN_X4_LD_BASE_ABIL0r_t AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg;
  AN_X4_LD_UP1_ABIL0r_t AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg;
  AN_X4_LD_UP1_ABIL1r_t AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg;

  AN_X1_BAM_SPD_PRI0r_t   AN_X1_CONTROL_BAM_SPEED_PRI_0r_reg;
  AN_X1_BAM_SPD_PRI1r_t   AN_X1_CONTROL_BAM_SPEED_PRI_1r_reg;
  AN_X1_BAM_SPD_PRI2r_t   AN_X1_CONTROL_BAM_SPEED_PRI_2r_reg;
  AN_X1_BAM_SPD_PRI3r_t   AN_X1_CONTROL_BAM_SPEED_PRI_3r_reg;
  AN_X1_BAM_SPD_PRI4r_t   AN_X1_CONTROL_BAM_SPEED_PRI_4r_reg;
  AN_X1_IEEE_SPD_PRI1r_t  AN_X1_IEEE_SPD_PRI1r_reg;

  AN_X4_LD_BASE_ABIL2r_t AN_X4_LD_BASE_ABIL2r_reg; 
  AN_X4_LD_BASE_ABIL3r_t AN_X4_LD_BASE_ABIL3r_reg; 
  AN_X4_LD_BASE_ABIL4r_t AN_X4_LD_BASE_ABIL4r_reg; 

  
  AN_X4_LD_FEC_BASEPAGE_ABILr_t AN_X4_LD_FEC_BASEPAGE_ABILr_reg;
  
  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  /*****FEC Override ****/
  tefmod16_an_fec_override(pc);

  /***********Override programming ********/
  AN_X4_CL73_CTLSr_CLR(AN_X4_ABILITIES_CL73_CONTROLSr_reg);
  data = pc->an_misc_ctrls;
  AN_X4_CL73_CTLSr_SET(AN_X4_ABILITIES_CL73_CONTROLSr_reg, data);
  /********Setting AN_X4_ABILITIES_cl73_controls 0xC186*****/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_CL73_CTLSr(pc, AN_X4_ABILITIES_CL73_CONTROLSr_reg));

  if(pc->sw_an) {
    return PHYMOD_E_NONE;
  }
  AN_X1_OUI_LWRr_CLR(AN_X1_CONTROL_OUI_LOWERr_reg);
  AN_X1_OUI_LWRr_SET(AN_X1_CONTROL_OUI_LOWERr_reg, pc->oui & 0xffff);
  /********Setting AN_X1_OUI_LWR 0x9241*****/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_OUI_LWRr(pc, AN_X1_CONTROL_OUI_LOWERr_reg));
 
  AN_X1_OUI_UPRr_CLR(AN_X1_CONTROL_OUI_UPPERr_reg);
  AN_X1_OUI_UPRr_SET(AN_X1_CONTROL_OUI_UPPERr_reg, (pc->oui >> 16) & 0xffff);
  /********Setting AN_X1_OUI_UPR 0x9240*****/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_OUI_UPRr(pc, AN_X1_CONTROL_OUI_UPPERr_reg));




  /********BAM Code********/
  if(pc->an_type == TEFMOD16_CL73_BAM || pc->an_type == TEFMOD_HPAM)
    cl73_bam_code =3;
  else
   cl73_bam_code =0;

  AN_X4_LD_BAM_ABILr_CLR(AN_X4_ABILITIES_LD_BAM_ABILITIESr_reg);
  AN_X4_LD_BAM_ABILr_CL73_BAM_CODEf_SET(AN_X4_ABILITIES_LD_BAM_ABILITIESr_reg, cl73_bam_code);
  /********Setting AN_X4_ABILITIES_ld_bam_abilities 0xC185*****/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_BAM_ABILr(pc, AN_X4_ABILITIES_LD_BAM_ABILITIESr_reg));


  data=0;
  AN_X4_LD_BASE_ABIL1r_CLR(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg);

  /******* Base Abilities*****/
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_1G ) & 1 ) {
    AN_X4_LD_BASE_ABIL1r_BASE_1G_KX1f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_10G_KR ) & 1 ) {
    AN_X4_LD_BASE_ABIL1r_BASE_10G_KR1f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_40G_KR4 ) & 1 ) {
    AN_X4_LD_BASE_ABIL1r_BASE_40G_KR4f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_40G_CR4 ) & 1 ) {
    AN_X4_LD_BASE_ABIL1r_BASE_40G_CR4f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_100G_KR4 ) & 1 ) { 
    AN_X4_LD_BASE_ABIL1r_BASE_100G_KR4f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_100G_CR4 ) & 1 ) {
    AN_X4_LD_BASE_ABIL1r_BASE_100G_CR4f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }

  /******* Pause Settings ********/
  if(pc->an_pause == TEFMOD16_NO_PAUSE) 
    AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 0);
  if(pc->an_pause == TEFMOD16_ASYM_PAUSE) 
    AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  if(pc->an_pause == TEFMOD16_SYM_PAUSE) 
    AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 2);
  if(pc->an_pause == TEFMOD16_ASYM_SYM_PAUSE) 
    AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 3);

  /****** FEC Settings ********/
  if(pc->an_fec == TEFMOD16_FEC_NOT_SUPRTD) 
    AN_X4_LD_BASE_ABIL1r_FEC_REQf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 0);
  if(pc->an_fec == TEFMOD16_FEC_SUPRTD_NOT_REQSTD) {
    AN_X4_LD_BASE_ABIL1r_FEC_REQf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if(pc->an_fec == TEFMOD16_FEC_SUPRTD_REQSTD) {
    AN_X4_LD_BASE_ABIL1r_FEC_REQf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 3);
  }

  /****** Next page ******/
  if(pc->cl73_bam_enable || pc->cl73_hpam_enable) 
    AN_X4_LD_BASE_ABIL1r_NEXT_PAGEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);

  
  if(pc->an_rf == 1) {
    AN_X4_LD_BASE_ABIL1r_CL73_REMOTE_FAULTf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }

  /***** Setting AN_X4_ABILITIES_ld_base_abilities_1 0xC184 *******/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_BASE_ABIL1r(pc, AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg));


  data =0;
 
  AN_X4_LD_BASE_ABIL0r_CLR(AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg);
  /******tx nonce*****/
  AN_X4_LD_BASE_ABIL0r_TX_NONCEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg, pc->cl73_tx_nonce);
 
  /****** Base selector *****/
  AN_X4_LD_BASE_ABIL0r_CL73_BASE_SELECTORf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg, 1);
 
  /***** Setting AN_X4_ABILITIES_ld_base_abilities_0 0xC183 *******/
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_BASE_ABIL0r(pc, AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg));

  AN_X4_LD_BASE_ABIL3r_CLR(AN_X4_LD_BASE_ABIL3r_reg);
  AN_X4_LD_BASE_ABIL3r_BASE_25G_KR1_SELf_SET(AN_X4_LD_BASE_ABIL3r_reg, (pc->base_25g_kr1 & 0x1f));
  AN_X4_LD_BASE_ABIL3r_BASE_25G_KR1_ENf_SET(AN_X4_LD_BASE_ABIL3r_reg, (pc->base_25g_kr1 >> 16) & 1);
  AN_X4_LD_BASE_ABIL3r_BASE_25G_CR1_SELf_SET(AN_X4_LD_BASE_ABIL3r_reg, (pc->base_25g_cr1 & 0x1f));
  AN_X4_LD_BASE_ABIL3r_BASE_25G_CR1_ENf_SET(AN_X4_LD_BASE_ABIL3r_reg, (pc->base_25g_cr1 >> 16) & 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_BASE_ABIL3r(pc, AN_X4_LD_BASE_ABIL3r_reg ));

  AN_X4_LD_BASE_ABIL4r_CLR(AN_X4_LD_BASE_ABIL4r_reg);
  AN_X4_LD_BASE_ABIL4r_BASE_25G_KRS1_SELf_SET(AN_X4_LD_BASE_ABIL4r_reg, (pc->base_25g_krs & 0x1f));
  AN_X4_LD_BASE_ABIL4r_BASE_25G_KRS1_ENf_SET(AN_X4_LD_BASE_ABIL4r_reg, (pc->base_25g_krs >> 16) & 1);
  AN_X4_LD_BASE_ABIL4r_BASE_25G_CRS1_SELf_SET(AN_X4_LD_BASE_ABIL4r_reg, (pc->base_25g_crs & 0x1f));
  AN_X4_LD_BASE_ABIL4r_BASE_25G_CRS1_ENf_SET(AN_X4_LD_BASE_ABIL4r_reg, (pc->base_25g_crs >> 16) & 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_BASE_ABIL4r(pc, AN_X4_LD_BASE_ABIL4r_reg ));

/*
 * IEEE 50G Dual is not supported in TSCF16(TH2 and TD3) since it is removed from standard  
 *
  AN_X4_LD_BASE_ABIL2r_CLR(AN_X4_LD_BASE_ABIL2r_reg);
  AN_X4_LD_BASE_ABIL2r_BASE_50G_KR2_SELf_SET(AN_X4_LD_BASE_ABIL2r_reg, (pc->base_50g_kr2 & 0x1f));
  AN_X4_LD_BASE_ABIL2r_BASE_50G_KR2_ENf_SET(AN_X4_LD_BASE_ABIL2r_reg, (pc->base_50g_kr2 >> 16) & 1);
  AN_X4_LD_BASE_ABIL2r_BASE_50G_CR2_SELf_SET(AN_X4_LD_BASE_ABIL2r_reg, (pc->base_50g_cr2 & 0x1f));
  AN_X4_LD_BASE_ABIL2r_BASE_50G_CR2_ENf_SET(AN_X4_LD_BASE_ABIL2r_reg, (pc->base_50g_cr2 >> 16) & 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_BASE_ABIL2r(pc, AN_X4_LD_BASE_ABIL2r_reg ));
*/
 
  data =0;
  AN_X4_LD_UP1_ABIL0r_CLR(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg);
  /******* User page abilities*********/
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_20G_KR2) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_20G_KR2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_20G_CR2) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_20G_CR2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_40G_KR2) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_40G_KR2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_40G_CR2) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_40G_CR2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_50G_KR2) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_50G_KR2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_50G_CR2) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_50G_CR2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
/*
 * BAM 50G 4-lane is not supported in TSCF16(TH2 and TD3) chips.  
* 
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_50G_KR4) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_50G_KR4f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_50G_CR4) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_50G_CR4f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
*/ 
  AN_X4_LD_UP1_ABIL0r_BAM_HG2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, pc->an_higig2);
 
  /******** Setting AN_X4_ABILITIES_ld_up1_abilities_0 0xC181******/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_UP1_ABIL0r(pc, AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg));
 
 
  data =0;
  AN_X4_LD_UP1_ABIL1r_CLR(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg);
  /******* User page abilities*********/
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_20G_KR1) & 1 ) 
    AN_X4_LD_UP1_ABIL1r_BAM_20G_KR1f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_20G_CR1) & 1 ) 
    AN_X4_LD_UP1_ABIL1r_BAM_20G_CR1f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_25G_KR1) & 1 ) 
    AN_X4_LD_UP1_ABIL1r_BAM_25G_KR1f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD16_ABILITY_25G_CR1) & 1 ) 
    AN_X4_LD_UP1_ABIL1r_BAM_25G_CR1f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, 1);
  
  AN_X4_LD_UP1_ABIL1r_CL91_REQf_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, pc->bam_cl91_fec); 
  AN_X4_LD_UP1_ABIL1r_CL74_REQf_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, pc->bam_cl74_fec); 
 
  /******** Setting AN_X4_ABILITIES_ld_up1_abilities_1 0xC182******/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_UP1_ABIL1r(pc, AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg));

  AN_X4_LD_FEC_BASEPAGE_ABILr_CLR(AN_X4_LD_FEC_BASEPAGE_ABILr_reg);
  AN_X4_LD_FEC_BASEPAGE_ABILr_RS_FEC_REQ_SELf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, pc->rs_fec_req_sel);
  AN_X4_LD_FEC_BASEPAGE_ABILr_RS_FEC_REQ_ENf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, pc->rs_fec_req_en);
  AN_X4_LD_FEC_BASEPAGE_ABILr_BASE_R_FEC_REQ_SELf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, pc->base_r_fec_req_sel);
  AN_X4_LD_FEC_BASEPAGE_ABILr_BASE_R_FEC_REQ_ENf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, pc->base_r_fec_req_en);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_FEC_BASEPAGE_ABILr(pc, AN_X4_LD_FEC_BASEPAGE_ABILr_reg));

  AN_X1_BAM_SPD_PRI0r_CLR(AN_X1_CONTROL_BAM_SPEED_PRI_0r_reg);
  AN_X1_BAM_SPD_PRI1r_CLR(AN_X1_CONTROL_BAM_SPEED_PRI_1r_reg);
  AN_X1_BAM_SPD_PRI2r_CLR(AN_X1_CONTROL_BAM_SPEED_PRI_2r_reg);
  AN_X1_BAM_SPD_PRI3r_CLR(AN_X1_CONTROL_BAM_SPEED_PRI_3r_reg);
  AN_X1_BAM_SPD_PRI4r_CLR(AN_X1_CONTROL_BAM_SPEED_PRI_4r_reg);
 
  data = pc->an_priority_remap_0;
  /********Setting AN_X1_BAM_SPD_PRI0 0x9242*****/
  AN_X1_BAM_SPD_PRI0r_SET(AN_X1_CONTROL_BAM_SPEED_PRI_0r_reg, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_BAM_SPD_PRI0r(pc, AN_X1_CONTROL_BAM_SPEED_PRI_0r_reg));
 
  data = pc->an_priority_remap_1;
  AN_X1_BAM_SPD_PRI1r_SET(AN_X1_CONTROL_BAM_SPEED_PRI_1r_reg, data);
  /********Setting AN_X1_BAM_SPD_PRI1 0x9243*****/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_BAM_SPD_PRI1r(pc, AN_X1_CONTROL_BAM_SPEED_PRI_1r_reg));
 
  data = pc->an_priority_remap_2;
  AN_X1_BAM_SPD_PRI2r_SET(AN_X1_CONTROL_BAM_SPEED_PRI_2r_reg, data);
  /********Setting AN_X1_BAM_SPD_PRI2 0x9244*****/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_BAM_SPD_PRI2r(pc, AN_X1_CONTROL_BAM_SPEED_PRI_2r_reg));
 
  data = pc->an_priority_remap_3;
  AN_X1_BAM_SPD_PRI3r_SET(AN_X1_CONTROL_BAM_SPEED_PRI_3r_reg, data);
  /********Setting AN_X1_BAM_SPD_PRI3 0x9245*****/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_BAM_SPD_PRI3r(pc, AN_X1_CONTROL_BAM_SPEED_PRI_3r_reg));
 
  data = pc->an_priority_remap_4;
  AN_X1_BAM_SPD_PRI4r_SET(AN_X1_CONTROL_BAM_SPEED_PRI_4r_reg, data);
  /********Setting AN_X1_BAM_SPD_PRI4 0x9246*****/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_BAM_SPD_PRI4r(pc, AN_X1_CONTROL_BAM_SPEED_PRI_4r_reg));

  data = pc->an_priority_remap_5;
  AN_X1_IEEE_SPD_PRI1r_SET(AN_X1_IEEE_SPD_PRI1r_reg, data);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_IEEE_SPD_PRI1r(pc, AN_X1_IEEE_SPD_PRI1r_reg)); 

  return PHYMOD_E_NONE;
}
#endif

#ifdef _SDK_TEFMOD16_

int tefmod16_autoneg_set(PHYMOD_ST* pc, tefmod16_an_adv_ability_t *cl73_adv)
{
  uint32_t override_v;

  AN_X4_LD_BASE_ABIL1r_t AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg;
  AN_X4_LD_BASE_ABIL0r_t AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg;
  AN_X4_LD_UP1_ABIL0r_t AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg;
  AN_X4_LD_UP1_ABIL1r_t AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg;
  AN_X4_LD_BASE_ABIL3r_t AN_X4_LD_BASE_ABIL3r_reg;
  AN_X4_LD_BASE_ABIL4r_t AN_X4_LD_BASE_ABIL4r_reg;
  AN_X4_LD_FEC_BASEPAGE_ABILr_t AN_X4_LD_FEC_BASEPAGE_ABILr_reg;

  AN_X4_LD_BASE_ABIL1r_CLR(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg);

  /******* Base Abilities 0xC1C4[5:0] *****/
  if (cl73_adv->an_base_speed ) {
      AN_X4_LD_BASE_ABIL1r_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, cl73_adv->an_base_speed & 0x3f);
  }

  /******* Pause Settings 0xC1C4[7:6] ********/
  if (cl73_adv->an_pause == TEFMOD16_NO_PAUSE) {
      AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 0);
  }
  if (cl73_adv->an_pause == TEFMOD16_SYMM_PAUSE) {
      AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if (cl73_adv->an_pause == TEFMOD16_ASYM_PAUSE) {
      AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 2);
  }
  if (cl73_adv->an_pause == TEFMOD16_ASYM_SYMM_PAUSE) {
      AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 3);
  }

  /****** FEC Settings 0xC1C4 [9:8] ********/
  if (cl73_adv->an_fec == TEFMOD16_FEC_NOT_SUPRTD) {
      AN_X4_LD_BASE_ABIL1r_FEC_REQf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 0);
  }
  if (cl73_adv->an_fec == TEFMOD16_FEC_SUPRTD_NOT_REQSTD) {
      AN_X4_LD_BASE_ABIL1r_FEC_REQf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if ((cl73_adv->an_fec & TEFMOD16_FEC_CL74_SUPRTD_REQSTD) ||
      (cl73_adv->an_fec & TEFMOD16_FEC_CL91_SUPRTD_REQSTD)) {
      AN_X4_LD_BASE_ABIL1r_FEC_REQf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 3);
  }

  /***** Setting AN_X4_ABILITIES_ld_base_abilities_1 0xC1C4 *******/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_BASE_ABIL1r(pc, AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg));

  AN_X4_LD_BASE_ABIL3r_CLR(AN_X4_LD_BASE_ABIL3r_reg);
  AN_X4_LD_BASE_ABIL4r_CLR(AN_X4_LD_BASE_ABIL4r_reg);
  AN_X4_LD_FEC_BASEPAGE_ABILr_CLR(AN_X4_LD_FEC_BASEPAGE_ABILr_reg);
  AN_X4_LD_UP1_ABIL1r_CLR(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg);

  /* check 25G IEEE base speed 0xC1C8 0xC1C9 */
  if ((cl73_adv->an_base_speed  >> TEFMOD16_CL73_25GBASE_KR1 ) & 1) {
      AN_X4_LD_BASE_ABIL3r_BASE_25G_KR1_ENf_SET(AN_X4_LD_BASE_ABIL3r_reg, 1);
      /* IEEE Technology Ability Field A10 25GBASE-KR */
      AN_X4_LD_BASE_ABIL3r_BASE_25G_KR1_SELf_SET(AN_X4_LD_BASE_ABIL3r_reg, 10);
  }
  if ((cl73_adv->an_base_speed  >> TEFMOD16_CL73_25GBASE_CR1 ) & 1) {
      AN_X4_LD_BASE_ABIL3r_BASE_25G_CR1_ENf_SET(AN_X4_LD_BASE_ABIL3r_reg, 1);
      /* IEEE Technology Ability Field A10 25GBASE-CR */
      AN_X4_LD_BASE_ABIL3r_BASE_25G_CR1_SELf_SET(AN_X4_LD_BASE_ABIL3r_reg, 10);
  }
  if ((cl73_adv->an_base_speed  >> TEFMOD16_CL73_25GBASE_KRS1 ) & 1) {
      AN_X4_LD_BASE_ABIL4r_BASE_25G_KRS1_ENf_SET(AN_X4_LD_BASE_ABIL4r_reg, 1);
      /* IEEE Technology Ability Field A9 25GBASE-KR-S */
      AN_X4_LD_BASE_ABIL4r_BASE_25G_KRS1_SELf_SET(AN_X4_LD_BASE_ABIL4r_reg, 9);
  }
  if ((cl73_adv->an_base_speed  >> TEFMOD16_CL73_25GBASE_CRS1 ) & 1) {
      AN_X4_LD_BASE_ABIL4r_BASE_25G_CRS1_ENf_SET(AN_X4_LD_BASE_ABIL4r_reg, 1);
      /* IEEE Technology Ability Field A9 25GBASE-CR-S */
      AN_X4_LD_BASE_ABIL4r_BASE_25G_CRS1_SELf_SET(AN_X4_LD_BASE_ABIL4r_reg, 9);
  }
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_BASE_ABIL3r(pc, AN_X4_LD_BASE_ABIL3r_reg));
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_BASE_ABIL4r(pc, AN_X4_LD_BASE_ABIL4r_reg));

  AN_X4_LD_FEC_BASEPAGE_ABILr_BASE_R_FEC_REQ_SELf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, TEFMOD16_CL73_25GBASE_FEC_POS);
  AN_X4_LD_FEC_BASEPAGE_ABILr_BASE_R_FEC_REQ_ENf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, 0);
  AN_X4_LD_FEC_BASEPAGE_ABILr_RS_FEC_REQ_SELf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, TEFMOD16_CL73_25G_RS_FEC_POS);
  AN_X4_LD_FEC_BASEPAGE_ABILr_RS_FEC_REQ_ENf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, 0);

  /* check which fec to enable 0xC1CA */
  if (cl73_adv->an_fec & TEFMOD16_FEC_CL74_SUPRTD_REQSTD) {
      AN_X4_LD_FEC_BASEPAGE_ABILr_BASE_R_FEC_REQ_ENf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, 1);
  }
  if (cl73_adv->an_fec & TEFMOD16_FEC_CL91_SUPRTD_REQSTD) {
      AN_X4_LD_FEC_BASEPAGE_ABILr_RS_FEC_REQ_ENf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, 1);
  }

  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_FEC_BASEPAGE_ABILr(pc, AN_X4_LD_FEC_BASEPAGE_ABILr_reg));

  /****** Base selector 0xC1C3[4:0] *****/
  AN_X4_LD_BASE_ABIL0r_CLR(AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg);
  AN_X4_LD_BASE_ABIL0r_CL73_BASE_SELECTORf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_BASE_ABIL0r(pc, AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg));

  /******* User page0 abilities 0xC1C1[9:6] and 0xC1C1[3:1] *********/
  AN_X4_LD_UP1_ABIL0r_CLR(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg);
  AN_X4_LD_UP1_ABIL0r_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, cl73_adv->an_bam_speed & 0x3cf);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_UP1_ABIL0r(pc, AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg));
  /******* User page0 abilities 0xC1C1[15]*********/
  AN_X4_LD_UP1_ABIL0r_BAM_HG2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, cl73_adv->an_hg2);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_UP1_ABIL0r(pc, AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg));

  /******* User page1 abilities 0xC1C2[4:1] *********/
  AN_X4_LD_UP1_ABIL1r_CLR(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg);
  AN_X4_LD_UP1_ABIL1r_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, cl73_adv->an_bam_speed1 & 0x1e);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_UP1_ABIL1r(pc, AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg));

  if ((cl73_adv->an_bam_speed) || (cl73_adv->an_bam_speed1)) {
      AN_X4_LD_UP1_ABIL1r_CLR(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg);
      PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_UP1_ABIL1r(pc, &AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg));
      /******* User page abilities 0xC1C2[15:14] *********/
      AN_X4_LD_UP1_ABIL1r_CL74_REQf_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, 0x1);
      /******* User page abilities 0xC1C2[13:12] *********/
      AN_X4_LD_UP1_ABIL1r_CL91_REQf_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, 0x1);
      if (cl73_adv->an_fec & TEFMOD16_FEC_CL74_SUPRTD_REQSTD) {
          AN_X4_LD_UP1_ABIL1r_CL74_REQf_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, 0x3);
      }
      if (cl73_adv->an_fec & TEFMOD16_FEC_CL91_SUPRTD_REQSTD) {
          AN_X4_LD_UP1_ABIL1r_CL91_REQf_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, 0x3);
      }
      PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_UP1_ABIL1r(pc, AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg));
  }

  /*
  * if cl72_en = 1, use default setting, no overrides
  * if cl72_en = 0, then override to disable cl72
  */
  if ((cl73_adv->an_cl72 & 0x1) == 1) {
      override_v = ((OVERRIDE_CL72_EN_DIS<<16) | 1);
      tefmod16_set_override_1(pc, 0, override_v);
   } else {
      override_v = ((OVERRIDE_CL72_EN<<16) | 0);
      tefmod16_set_override_1(pc, 0, override_v);
   }

   return PHYMOD_E_NONE;
}

#endif /* _SDK_TEFMOD16_ */


/*!
@brief To get autoneg advertisement registers.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details
*/
int tefmod16_autoneg_get(PHYMOD_ST* pc)               /* AUTONEG_GET */
{
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  /* TBD */
  return PHYMOD_E_NONE;
}

/*!
@brief Controls the setting/resetting of autoneg ability and enabling/disabling
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.

@details
This function programs auto-negotiation (AN) modes for the TEF. It can
enable/disable clause37/clause73/BAM autonegotiation capabilities. Call this
function once for combo mode and once per lane in independent lane mode.

The autonegotiation mode is indicated by setting an_control as required.
*/

#ifdef _DV_TB_
int tefmod16_autoneg_control(PHYMOD_ST* pc)
{
  uint16_t cl73_bam_enable, cl73_hpam_enable, cl73_enable;
  uint16_t cl73_nonce_match_over, cl73_nonce_match_val ;
  uint16_t bam_to_hpam_ad_en;
  uint16_t num_advertised_lanes;
  AN_X4_CL73_CFGr_t AN_X4_ABILITIES_CL73_CFGr_reg;
  AN_X4_WAIT_ACK_COMPLETEr_t AN_X4_WAIT_ACK_COMPLETEr_reg;
 
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  cl73_enable                   = pc->an_en;
  cl73_nonce_match_over         = pc->nonce_match_override;
  cl73_nonce_match_val          = pc->nonce_match_val;
  cl73_bam_enable               = pc->cl73_bam_enable;
  cl73_hpam_enable              = pc->cl73_hpam_enable;
  bam_to_hpam_ad_en             = pc->bam_to_hpam_ad_en;
  num_advertised_lanes          = pc->num_advertised_lanes;

/****Added for cl73_disable test case ****/
#if defined (_DV_TB_) 
  if(pc->override_an_en_as_disable==1) { 
    cl73_enable =0;
  }
#endif

  AN_X4_WAIT_ACK_COMPLETEr_CLR(AN_X4_WAIT_ACK_COMPLETEr_reg);
  AN_X4_WAIT_ACK_COMPLETEr_WAIT_FOR_ACK_ENf_SET(AN_X4_WAIT_ACK_COMPLETEr_reg, pc->sw_an_wait_for_ack);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_WAIT_ACK_COMPLETEr(pc, AN_X4_WAIT_ACK_COMPLETEr_reg));
 
  AN_X4_CL73_CFGr_CLR(AN_X4_ABILITIES_CL73_CFGr_reg);

  AN_X4_CL73_CFGr_NUM_ADVERTISED_LANESf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, num_advertised_lanes);
 
  AN_X4_CL73_CFGr_CL73_NONCE_MATCH_VALf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, cl73_nonce_match_val);
 
  AN_X4_CL73_CFGr_CL73_NONCE_MATCH_OVERf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, cl73_nonce_match_over);

  if(pc->sw_an) {
    cl73_enable = 0;
  } 
  AN_X4_CL73_CFGr_CL73_ENABLEf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, cl73_enable);
  
 
  AN_X4_CL73_CFGr_CL73_HPAM_ENABLEf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, cl73_hpam_enable);
 
  AN_X4_CL73_CFGr_CL73_BAM_ENABLEf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, cl73_bam_enable);
 
  AN_X4_CL73_CFGr_BAM_TO_HPAM_AD_ENf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, bam_to_hpam_ad_en);
 
  AN_X4_CL73_CFGr_AD_TO_CL73_ENf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, pc->hpam_to_cl73_ad_en);
  /*********Setting AN_X4_ABILITIES_cl73_cfg 0xC180 ******/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_CL73_CFGr(pc, AN_X4_ABILITIES_CL73_CFGr_reg));

  return PHYMOD_E_NONE;
} /* tefmod16_autoneg_control(PHYMOD_ST* pc) */
#endif

#ifdef _SDK_TEFMOD16_
int tefmod16_autoneg_control(PHYMOD_ST* pc, tefmod16_an_control_t *an_control)
{
  phymod_access_t pa_copy;
  uint16_t num_advertised_lanes, cl73_bam_enable  ;
  int start_lane, num_of_lane, i;
  uint16_t cl73_hpam_enable, cl73_enable;
  uint16_t cl73_next_page;
  uint16_t cl73_restart ;
  uint16_t cl73_bam_code;
  uint16_t msa_overrides;
  uint32_t plldiv_r_val = 0;
  MAIN0_SETUPr_t         reg_setup;
  AN_X4_CL73_CFGr_t      AN_X4_CL73_CFGr_reg;
  AN_X1_CL73_ERRr_t      AN_X1_CL73_ERRr_reg;
  AN_X4_CL73_CTLSr_t     AN_X4_CL73_CTLSr_reg;
  AN_X4_LD_BASE_ABIL1r_t AN_X4_LD_BASE_ABIL1r_reg;
  AN_X4_LD_BAM_ABILr_t   AN_X4_ABILITIES_LD_BAM_ABILITIESr_reg;

  PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_of_lane));
  PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  num_advertised_lanes          = an_control->num_lane_adv;
  cl73_bam_code                 = 0x0;
  cl73_bam_enable               = 0x0;
  cl73_hpam_enable              = 0x0;
  cl73_enable                   = 0x0;
  cl73_next_page                = 0x0;
  cl73_restart                  = 0x0;
  msa_overrides                 = 0x0;

  switch (an_control->an_type) {
    case TEFMOD16_AN_MODE_CL73:
      cl73_restart                = an_control->enable;
      cl73_enable                 = an_control->enable;
      break;
    case TEFMOD16_AN_MODE_CL73_BAM:
      cl73_restart                = an_control->enable;
      cl73_enable                 = an_control->enable;
      cl73_bam_enable             = an_control->enable;
      cl73_bam_code               = 0x3;
      cl73_next_page              = 0x1;
      break;
    case TEFMOD16_AN_MODE_CL73_MSA:
      cl73_restart                = an_control->enable;
      cl73_enable                 = an_control->enable;
      cl73_bam_enable             = an_control->enable;
      cl73_bam_code               = 0x3;
      cl73_next_page              = 0x1;
      msa_overrides               = 0x1;
      break;
    case TEFMOD16_AN_MODE_MSA:
      cl73_restart                = an_control->enable;
      cl73_enable                 = an_control->enable;
      cl73_bam_enable             = an_control->enable;
      cl73_bam_code               = 0x3;
      cl73_next_page              = 0x1;
      msa_overrides               = 0x1;
      break;      
    case TEFMOD16_AN_MODE_HPAM:
      cl73_restart                = an_control->enable;
      cl73_enable                 = an_control->enable;
      cl73_hpam_enable            = an_control->enable;
      cl73_next_page              = 0x1;
      break;
    default:
      return PHYMOD_E_FAIL;
      break;
  }
    if(msa_overrides == 0x1) {
        tefmod16_an_oui_t oui;
        oui.oui = 0x6a737d;
        oui.oui_override_bam73_adv = 0x1;
        oui.oui_override_bam73_det = 0x1;
        oui.oui_override_hpam_adv  = 0x0;
        oui.oui_override_hpam_det  = 0x0;
        PHYMOD_IF_ERR_RETURN(tefmod16_an_oui_set(pc, oui));
        PHYMOD_IF_ERR_RETURN(tefmod16_an_msa_mode_set(pc, msa_overrides));
    }
  /* RESET Speed Change bit */
  if(an_control->enable){
    tefmod16_disable_set(pc);
  }

  MAIN0_SETUPr_CLR(reg_setup);
  if(an_control->enable){
      /*next we decide which cl73_vco to set based on the current VCO */
      PHYMOD_IF_ERR_RETURN(tefmod16_get_plldiv(pc, &plldiv_r_val));

      if(plldiv_r_val<=4) {  /* pll_div <= 132; for 156MHz only */
          MAIN0_SETUPr_CL73_VCOf_SET(reg_setup, 0);
      } else {
          MAIN0_SETUPr_CL73_VCOf_SET(reg_setup, 1);
      }
      PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc, reg_setup));
  }


  /*0x9251 AN_X1_TIMERS_cl73_error*/
  AN_X1_CL73_ERRr_CLR(AN_X1_CL73_ERRr_reg);
  if(an_control->an_type == TEFMOD16_AN_MODE_CL73) {
    AN_X1_CL73_ERRr_SET(AN_X1_CL73_ERRr_reg, 0);
  } else if(an_control->an_type == TEFMOD16_AN_MODE_HPAM){
    AN_X1_CL73_ERRr_SET(AN_X1_CL73_ERRr_reg, 0xfff0);
  } else if (an_control->an_type == TEFMOD16_AN_MODE_CL73_BAM) {
    AN_X1_CL73_ERRr_SET(AN_X1_CL73_ERRr_reg, 0x1a10);
  }
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_ERRr(pc, AN_X1_CL73_ERRr_reg));


  /*need to set cl73 BAM next page 0xc1c4 probably*/
  AN_X4_LD_BASE_ABIL1r_CLR(AN_X4_LD_BASE_ABIL1r_reg);
  AN_X4_LD_BASE_ABIL1r_NEXT_PAGEf_SET(AN_X4_LD_BASE_ABIL1r_reg, cl73_next_page & 1);
  PHYMOD_IF_ERR_RETURN ( MODIFY_AN_X4_LD_BASE_ABIL1r(pc, AN_X4_LD_BASE_ABIL1r_reg));

  /* Writing bam_code 0xc1c5*/
  AN_X4_LD_BAM_ABILr_CLR(AN_X4_ABILITIES_LD_BAM_ABILITIESr_reg);
  AN_X4_LD_BAM_ABILr_CL73_BAM_CODEf_SET(AN_X4_ABILITIES_LD_BAM_ABILITIESr_reg, cl73_bam_code);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_BAM_ABILr(pc, AN_X4_ABILITIES_LD_BAM_ABILITIESr_reg));

  /* 0xc1c6 */
  AN_X4_CL73_CTLSr_CLR(AN_X4_CL73_CTLSr_reg);
  AN_X4_CL73_CTLSr_PD_KX_ENf_SET(AN_X4_CL73_CTLSr_reg, an_control->pd_kx_en & 0x1);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_CL73_CTLSr(pc, AN_X4_CL73_CTLSr_reg));

  /* 0xc1c0 */
  AN_X4_CL73_CFGr_CLR(AN_X4_CL73_CFGr_reg);
  AN_X4_CL73_CFGr_CL73_ENABLEf_SET(AN_X4_CL73_CFGr_reg,0);
  AN_X4_CL73_CFGr_CL73_AN_RESTARTf_SET(AN_X4_CL73_CFGr_reg,0);
  PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_CL73_CFGr(pc, AN_X4_CL73_CFGr_reg));

  /*Setting X4 abilities*/
  AN_X4_CL73_CFGr_CLR(AN_X4_CL73_CFGr_reg);
  AN_X4_CL73_CFGr_CL73_BAM_ENABLEf_SET(AN_X4_CL73_CFGr_reg,cl73_bam_enable);
  AN_X4_CL73_CFGr_CL73_HPAM_ENABLEf_SET(AN_X4_CL73_CFGr_reg,cl73_hpam_enable);
  AN_X4_CL73_CFGr_CL73_ENABLEf_SET(AN_X4_CL73_CFGr_reg,cl73_enable);
  AN_X4_CL73_CFGr_CL73_AN_RESTARTf_SET(AN_X4_CL73_CFGr_reg,cl73_restart);
  if(an_control->an_property_type & TEFMOD16_AN_PROPERTY_ENABLE_HPAM_TO_CL73_AUTO )
    AN_X4_CL73_CFGr_AD_TO_CL73_ENf_SET(AN_X4_CL73_CFGr_reg,0x1);
  else
    AN_X4_CL73_CFGr_AD_TO_CL73_ENf_SET(AN_X4_CL73_CFGr_reg,0x0);
    /* AN_X4_CL73_CFGr_HPAM_TO_CL73_AUTO_ENABLEf_SET(AN_X4_CL73_CFGr_reg,0x0); */

  if(an_control->an_property_type & TEFMOD16_AN_PROPERTY_ENABLE_CL73_BAM_TO_HPAM_AUTO )
    AN_X4_CL73_CFGr_BAM_TO_HPAM_AD_ENf_SET(AN_X4_CL73_CFGr_reg,0x1);
  else
    AN_X4_CL73_CFGr_BAM_TO_HPAM_AD_ENf_SET(AN_X4_CL73_CFGr_reg,0x0);

  AN_X4_CL73_CFGr_NUM_ADVERTISED_LANESf_SET(AN_X4_CL73_CFGr_reg,num_advertised_lanes);

  PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_CL73_CFGr(pc, AN_X4_CL73_CFGr_reg));

 /* if an is enabled, the restart bit needs to be cleared */

 if (an_control->enable) {
      AN_X4_CL73_CFGr_CLR(AN_X4_CL73_CFGr_reg);
      AN_X4_CL73_CFGr_CL73_AN_RESTARTf_SET(AN_X4_CL73_CFGr_reg,0);
      PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_CL73_CFGr(pc, AN_X4_CL73_CFGr_reg));
 }
  
  /* Disable the cl72 , when AN port is disabled. */
  if (an_control->cl72_config_allowed) {
      if(an_control->enable == 0) {
        if(an_control->num_lane_adv == 1) {
            num_of_lane = 2;
        } else if (an_control->num_lane_adv == 2) {
            num_of_lane = 4;
        } else {
            num_of_lane = 1;
        }
        for (i = (num_of_lane-1); i >= 0; i--) {
            pa_copy.lane_mask = 0x1 << (i + start_lane);
            tefmod16_clause72_control(&pa_copy, 0);
        }
      }
  }

  return PHYMOD_E_NONE;
} /* temod_autoneg_control */
#endif

/* internal supporting function */
int tefmod16_get_plldiv(PHYMOD_ST* pc, uint32_t *plldiv_r_val)
{
  PLL_CAL_CTL7r_t PLL_CAL_CTL7r_reg;
  /* read back plldiv */

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  PHYMOD_IF_ERR_RETURN (READ_PLL_CAL_CTL7r(pc, &PLL_CAL_CTL7r_reg));

  *plldiv_r_val = PLL_CAL_CTL7r_PLL_MODEf_GET(PLL_CAL_CTL7r_reg);

  return PHYMOD_E_NONE;
}

/* to update port_mode_select value.  If the update warrants a pll reset,
   then return accData=1, otherwise return accData=0.  The update shall 
   support flex port technology. Called forced speed modes */

int tefmod16_update_port_mode_select(PHYMOD_ST* pc, tefmod16_port_type_t port_type, int master_port, int tsc_clk_freq_pll_by_48, int pll_reset_en)
{   
  MAIN0_SETUPr_t MAIN0_SETUPr_reg;
  MAIN0_SPD_CTLr_t MAIN0_SPEED_CTRLr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

#ifdef _DV_TB_
  pc->accData   = 0;
  if(pc->tsc_touched == 1) {
    return PHYMOD_E_NONE;
  }  
#endif
 
  PHYMOD_IF_ERR_RETURN (READ_MAIN0_SETUPr(pc, &MAIN0_SETUPr_reg));
  
  if((port_type ==TEFMOD16_MULTI_PORT) ||(port_type ==TEFMOD16_DXGXS)||
     (port_type ==TEFMOD16_SINGLE_PORT)||(port_type ==TEFMOD16_TRI1_PORT)||
     (port_type ==TEFMOD16_TRI2_PORT)){
  } else {
    PHYMOD_DEBUG_ERROR(("%-22s: ERROR port_type=%0d undefined\n", __func__, port_type));
    return PHYMOD_E_FAIL;
  }

  MAIN0_SETUPr_CLR(MAIN0_SETUPr_reg);
  MAIN0_SETUPr_PORT_MODE_SELf_SET(MAIN0_SETUPr_reg, port_type);
    /* Leave port mode to quad in an_en */
       
  MAIN0_SETUPr_MASTER_PORT_NUMf_SET(MAIN0_SETUPr_reg, master_port);

  PHYMOD_IF_ERR_RETURN (MODIFY_MAIN0_SETUPr(pc, MAIN0_SETUPr_reg));

  PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUPr(pc, &MAIN0_SETUPr_reg));
    

  MAIN0_SETUPr_CLR(MAIN0_SETUPr_reg);

  MAIN0_SETUPr_TSC_CLK_CTRLf_SET(MAIN0_SETUPr_reg, tsc_clk_freq_pll_by_48);

  PHYMOD_IF_ERR_RETURN (MODIFY_MAIN0_SETUPr(pc, MAIN0_SETUPr_reg));

  PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUPr(pc, &MAIN0_SETUPr_reg));

  MAIN0_SPD_CTLr_CLR(MAIN0_SPEED_CTRLr_reg);
  MAIN0_SPD_CTLr_PLL_RESET_ENf_SET(MAIN0_SPEED_CTRLr_reg, pll_reset_en);
  PHYMOD_IF_ERR_RETURN (MODIFY_MAIN0_SPD_CTLr(pc, MAIN0_SPEED_CTRLr_reg));
   
  return PHYMOD_E_NONE; 
}

/*!
@brief Init routine sets various operating modes of TEF.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  refclk  Reference clock
@param  plldiv  PLL Divider value
@param  port_type Port type as in enum #tefmod16_port_type_t
@param  master_port master port (which controls the PLL)
@param  master_port master port (which controls the PLL)
@param  tsc_clk_freq_pll_by_48  TBD
@param  pll_reset_en  TBD
@returns PHYMOD_E_NONE if successful. PHYMOD_E_ERROR else.
@details
This function is called once per TEF. It cannot be called multiple times
and should be called immediately after reset. Elements of #PHYMOD_ST should be
initialized to required values prior to calling this function. The following
sub-configurations are performed here.

\li Read the revision Id.
\li Set reference clock (0x9000[15:13])
\li Set pll divider for VCO setting (0x9000[12, 11:8]). This is a function of max_speed. 
\li Port mode select single/dual/quad combo lane mode
\li PMA/PMD mux/demux (lane swap) (0x9004[15:0])
\li Load Firmware. (In MDK/SDK this is done externally. The platform provides a
method to load firmware. TEFMod cannot load firmware via MDIO.)
*/

int tefmod16_set_port_mode(PHYMOD_ST* pc, int refclk, int plldiv, tefmod16_port_type_t port_type, int master_port, int tsc_clk_freq_pll_by_48, int pll_reset_en)
{
  uint16_t dataref;
  MAIN0_SETUPr_t MAIN0_SETUPr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  tefmod16_update_port_mode_select(pc, port_type, master_port, tsc_clk_freq_pll_by_48, pll_reset_en);

  switch(refclk) {
    case 25 : dataref=main0_refClkSelect_clk_25MHz;     break;
    case 50 : dataref=main0_refClkSelect_clk_50Mhz;     break;
    case 100: dataref=main0_refClkSelect_clk_100MHz;    break;
    case 106: dataref=main0_refClkSelect_clk_106p25Mhz; break;
    case 125: dataref=main0_refClkSelect_clk_125MHz;    break;
    case 156: dataref=main0_refClkSelect_clk_156p25MHz; break;
    case 161: dataref=main0_refClkSelect_clk_161p25Mhz; break;
    case 187: dataref=main0_refClkSelect_clk_187p5MHz;  break;
    default : dataref=main0_refClkSelect_clk_156p25MHz;
  }

  MAIN0_SETUPr_CLR(MAIN0_SETUPr_reg);

  MAIN0_SETUPr_REFCLK_SELf_SET(MAIN0_SETUPr_reg, dataref);

  switch(plldiv) {
    /*
    case 32 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div32;  break;
    case 36 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div36;  break;
    case 40 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div40;  break;
    case 42 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div42;  break;
    case 48 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div48;  break;
    case 50 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div50;  break;
    case 52 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div52;  break;
    case 54 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div54;  break;
    case 60 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div60;  break;
    case 64 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div64;  break;
    case 66 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div66;  break;
    case 68 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div68;  break;
    case 70 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div70;  break;
    case 80 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div80;  break;
    case 92 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div92;  break;
    case 100: datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div100; break;
    default : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div40;
    */
  }
  PHYMOD_IF_ERR_RETURN (MODIFY_MAIN0_SETUPr(pc, MAIN0_SETUPr_reg));
  return PHYMOD_E_NONE;
} /* tefmod16_set_port_mode(PHYMOD_ST* pc) */

#ifdef _DV_TB_
/*!
@brief Sets the core for autoneg
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
This function sets the tsc core for autoneg generic settings.
If per_lane_control=TEFMOD16_CTL_AN_CHIP_INIT(0x1) PLL and RX path are reset.
If per_lane_control=TEFMOD16_CTL_AN_MODE_INIT(0x2) PLL and RX path are not reset.
*/
int tefmod16_set_an_port_mode(PHYMOD_ST* pc)    /* SET_AN_PORT_MODE */
{
#ifdef _DV_TB_ 
  uint16_t ignore_link_timer_period;
  uint16_t tx_disable_timer_period;
  uint16_t link_fail_inhibit_no_cl72_period ;
  uint16_t link_fail_inhibit_cl72_period ;
#endif
#ifdef TSCF_GEN1  
  uint16_t mld_swap_count;
#endif
  uint16_t single_port_mode;
  uint16_t ref_clk_sel;
  uint16_t new_port_mode_sel;
  uint16_t modify_port_mode;
  uint16_t port_mode_sel_reg; 

  MAIN0_SETUPr_t MAIN0_SETUPr_reg;
  AN_X1_IGNORE_LNK_TMRr_t AN_X1_TIMERS_IGNORE_LINK_TIMERr_reg;
  MAIN0_TICK_CTL0r_t MAIN0_TICK_CONTROL_0r_reg;
  MAIN0_TICK_CTL1r_t MAIN0_TICK_CONTROL_1r_reg;
  AN_X1_CL73_DME_LOCKr_t AN_X1_TIMERS_CL73_DME_LOCKr_reg;
  AN_X1_PD_SD_TMRr_t     AN_X1_TIMERS_PD_SD_TIMERr_reg;
  AN_X1_CL73_BRK_LNKr_t AN_X1_TIMERS_CL73_BREAK_LINKr_reg;
  AN_X1_CL73_ERRr_t AN_X1_TIMERS_CL73_ERRORr_reg;
  AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_t AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r_reg;
  AN_X1_LNK_FAIL_INHBT_TMR_CL72r_t AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r_reg;
  TX_X4_CRED0r_t TX_X4_CREDIT0_CREDIT0r_reg;
  #ifdef TSCF_GEN1
  TX_X2_MLD_SWP_CNTr_t TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg;
/*  CL82_RX_AM_TMRr_t CL82_SHARED_CL82_RX_AM_TIMERr_reg; 
  #else
  CL82_AM_TMRr_t CL82_SHARED_CL82_AM_TIMERr_reg; */
  #endif

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  modify_port_mode              = 0;

#if defined (_DV_TB_) 
  ignore_link_timer_period      = 0xf;
/*  tick_denominator              = 0x1;
  tick_numerator_lower          = 0x0;
  tick_override                 = 0x1;
  tick_numerator_upper          = 0x8;  
  tx_disable_timer_period       = 0x5;
  cl73_error_timer_period       = 0x64; */
  link_fail_inhibit_no_cl72_period = 0x64;
  link_fail_inhibit_cl72_period    = 0xfff;

#endif
/*  cl48_dswin64b66b              = 0x1;
  cl48_dswin8b10b               = 0x7; */
#ifdef TSCF_GEN1  
  mld_swap_count                = 0xfffc;
#endif
/*  am_timer_init_rx_val          = 0x3fff; */
  single_port_mode              = 0x0;  
  ref_clk_sel                   = 0x0;  



  PHYMOD_IF_ERR_RETURN (READ_MAIN0_SETUPr(pc, &MAIN0_SETUPr_reg));
  port_mode_sel_reg = MAIN0_SETUPr_PORT_MODE_SELf_GET(MAIN0_SETUPr_reg);

  MAIN0_SETUPr_CLR(MAIN0_SETUPr_reg);

/*  if ((pc-> an_type == TEFMOD16_HPAM) | (pc-> an_type == TEFMOD_CL73_BAM)) {
#if defined (_DV_TB_) 
    cl73_error_timer_period       = 0x64;
#endif
  } */

  if ((pc-> an_type == TEFMOD16_CL73) | (pc-> an_type == TEFMOD_HPAM) | 
      (pc-> an_type == TEFMOD16_CL73_BAM)) {

    ref_clk_sel = main0_refClkSelect_clk_156p25MHz;
    if(pc->tsc_touched != 1)
      single_port_mode = 1;
     
  /*  plldiv = 66; */
  }

  if(pc->per_lane_control == TEFMOD16_CTL_AN_CHIP_INIT) {
    MAIN0_SETUPr_REFCLK_SELf_SET(MAIN0_SETUPr_reg, ref_clk_sel);
  }
  MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(MAIN0_SETUPr_reg, single_port_mode);


  if(port_mode_sel_reg == TEFMOD16_DXGXS) {
    modify_port_mode = 1;
    if(pc->this_lane == 0) {
      new_port_mode_sel = TEFMOD16_TRI1_PORT;
    }
    else if(pc->this_lane == 2) {
      new_port_mode_sel = TEFMOD16_TRI2_PORT;
    }
  }
  if(port_mode_sel_reg == TEFMOD16_TRI1_PORT) {
    if(pc->this_lane == 2) {
      modify_port_mode = 1;
      new_port_mode_sel = TEFMOD16_MULTI_PORT;
    }
  }
  if(port_mode_sel_reg == TEFMOD16_TRI2_PORT) {
    if(pc->this_lane == 0) {
      modify_port_mode = 1;
      new_port_mode_sel = TEFMOD16_MULTI_PORT;
    }
  }
  if(port_mode_sel_reg == TEFMOD16_SINGLE_PORT) {
    modify_port_mode = 1;
    new_port_mode_sel = TEFMOD16_MULTI_PORT;
  }
  if(modify_port_mode == 1) {
    MAIN0_SETUPr_PORT_MODE_SELf_SET(MAIN0_SETUPr_reg, new_port_mode_sel);
  }
   
  /*mask |= MAIN0_SETUP_PORT_MODE_SEL_MASK; */  
  MAIN0_SETUPr_CL72_ENf_SET(MAIN0_SETUPr_reg, pc->cl72_en);
 
  MAIN0_SETUPr_CL73_VCOf_SET(MAIN0_SETUPr_reg, pc->cl73_vco);
 
  /* 0x9000 */
  PHYMOD_IF_ERR_RETURN (MODIFY_MAIN0_SETUPr(pc, MAIN0_SETUPr_reg));
  
  pc->port_type = TEFMOD16_MULTI_PORT; 

  /* XXXX: need to do sleep sal_usleep(1000);  */

#if defined (_DV_TB_)
  /* ignore_link_timer_period 0x925c  */
  if(pc->ignore_link_timer_period !=0) ignore_link_timer_period = pc->ignore_link_timer_period;
  AN_X1_IGNORE_LNK_TMRr_CLR(AN_X1_TIMERS_IGNORE_LINK_TIMERr_reg);
  AN_X1_IGNORE_LNK_TMRr_IGNORE_LINK_TIMER_PERIODf_SET(AN_X1_TIMERS_IGNORE_LINK_TIMERr_reg, ignore_link_timer_period);

  PHYMOD_IF_ERR_RETURN (MODIFY_AN_X1_IGNORE_LNK_TMRr(pc, AN_X1_TIMERS_IGNORE_LINK_TIMERr_reg));
#endif    

  
#if defined (_DV_TB_) 
  /* set tick values 0x9008 x1 (only in sim) */
  MAIN0_TICK_CTL0r_CLR(MAIN0_TICK_CONTROL_0r_reg);
  MAIN0_TICK_CTL0r_TICK_DENOMINATORf_SET(MAIN0_TICK_CONTROL_0r_reg, pc->tick_denominator);
 
  MAIN0_TICK_CTL0r_TICK_NUMERATOR_LOWERf_SET(MAIN0_TICK_CONTROL_0r_reg, pc->tick_numerator_lower);

  PHYMOD_IF_ERR_RETURN (MODIFY_MAIN0_TICK_CTL0r(pc, MAIN0_TICK_CONTROL_0r_reg));
    
  /* set tick values 0x9007 (only in sim) */
  MAIN0_TICK_CTL1r_CLR(MAIN0_TICK_CONTROL_1r_reg);
  MAIN0_TICK_CTL1r_TICK_OVERRIDEf_SET(MAIN0_TICK_CONTROL_1r_reg, pc->tick_override);
  MAIN0_TICK_CTL1r_TICK_NUMERATOR_UPPERf_SET(MAIN0_TICK_CONTROL_1r_reg, pc->tick_numerator_upper);

  PHYMOD_IF_ERR_RETURN (MODIFY_MAIN0_TICK_CTL1r(pc, MAIN0_TICK_CONTROL_1r_reg));

 /*TBD::0x9255 AN_X1_TIMERS_cl73_dme_lock*/ 
  AN_X1_CL73_DME_LOCKr_CLR(AN_X1_TIMERS_CL73_DME_LOCKr_reg);
  AN_X1_CL73_DME_LOCKr_SET(AN_X1_TIMERS_CL73_DME_LOCKr_reg, 100);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_DME_LOCKr(pc, AN_X1_TIMERS_CL73_DME_LOCKr_reg));
 
 /*TBD::0x9253 AN_X1_TIMERS_pd_sd_timer*/ 
  AN_X1_PD_SD_TMRr_CLR(AN_X1_TIMERS_PD_SD_TIMERr_reg);
  AN_X1_PD_SD_TMRr_SET(AN_X1_TIMERS_PD_SD_TIMERr_reg, 50);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_PD_SD_TMRr(pc, AN_X1_TIMERS_PD_SD_TIMERr_reg));
/*   set cl73 break link timer values 0x9253 (only in sim) need a production value */
  if(pc->tx_disable_timer_period !=0) tx_disable_timer_period = pc->tx_disable_timer_period;
  if(pc->tsc_touched ==0) {
    AN_X1_CL73_BRK_LNKr_CLR(AN_X1_TIMERS_CL73_BREAK_LINKr_reg);
    AN_X1_CL73_BRK_LNKr_CL73_BREAK_TIMER_PERIODf_SET(AN_X1_TIMERS_CL73_BREAK_LINKr_reg, tx_disable_timer_period);
    PHYMOD_IF_ERR_RETURN (MODIFY_AN_X1_CL73_BRK_LNKr(pc, AN_X1_TIMERS_CL73_BREAK_LINKr_reg));
  }


  
  /* set cl73 error timer period values 0x9254 write once per core (timeout timer) */
  /*if(pc->cl73_error_timer_period !=0) cl73_error_timer_period = pc->cl73_error_timer_period;*/
  AN_X1_CL73_ERRr_CLR(AN_X1_TIMERS_CL73_ERRORr_reg);
  AN_X1_CL73_ERRr_CL73_ERROR_TIMER_PERIODf_SET(AN_X1_TIMERS_CL73_ERRORr_reg, pc->cl73_error_timer_period);
  PHYMOD_IF_ERR_RETURN (MODIFY_AN_X1_CL73_ERRr(pc, AN_X1_TIMERS_CL73_ERRORr_reg));

  /**** 0x9256*******/
  if(pc->link_fail_inhibit_no_cl72_period !=0) link_fail_inhibit_no_cl72_period = pc->link_fail_inhibit_no_cl72_period;

  AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_CLR(AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r_reg);
  AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_LINK_FAIL_INHIBIT_TIMER_NCL72_PERIODf_SET(AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r_reg, link_fail_inhibit_no_cl72_period);
  
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r(pc, AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r_reg));

  /**** 0x9255*******/
  if(pc->link_fail_inhibit_cl72_period !=0) link_fail_inhibit_cl72_period = pc->link_fail_inhibit_cl72_period;
  AN_X1_LNK_FAIL_INHBT_TMR_CL72r_CLR(AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r_reg);
  AN_X1_LNK_FAIL_INHBT_TMR_CL72r_LINK_FAIL_INHIBIT_TIMER_CL72_PERIODf_SET(AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r_reg, link_fail_inhibit_cl72_period);
  
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_FAIL_INHBT_TMR_CL72r(pc, AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r_reg));
#endif
  
  
   /* MLD swap_count 0xa000 */
#ifdef TSCF_GEN1  
  
#if defined (_DV_TB_) 
  mld_swap_count = 0xffc;      
#else      
  mld_swap_count = 0xfffc;      
#endif

  TX_X2_MLD_SWP_CNTr_CLR(TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg);
  TX_X2_MLD_SWP_CNTr_MLD_SWAP_COUNTf_SET(TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg, mld_swap_count);
  
  PHYMOD_IF_ERR_RETURN(MODIFY_TX_X2_MLD_SWP_CNTr(pc, TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg));
#endif

  /* cl82 shared rc_am_timer  0x9123 */
  
/*#if defined (_DV_TB_) 
  am_timer_init_rx_val = 0x3ff;      
#else      
  am_timer_init_rx_val = 0x3fff;      
#endif */


  
  /* clear credtis 0xc100, 0xc104  */
  TX_X4_CRED0r_CLR(TX_X4_CREDIT0_CREDIT0r_reg);
  TX_X4_CRED0r_SET(TX_X4_CREDIT0_CREDIT0r_reg, 0); 
  PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_CRED0r(pc, TX_X4_CREDIT0_CREDIT0r_reg));

  return PHYMOD_E_NONE;
} /* tefmod16_set_an_port_mode(PHYMOD_ST* pc) */
#endif


/*!
@brief Sets loopback mode from Tx to Rx at PCS/PMS parallel interface. (gloop).
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  pcs_gloop_en  Enable or disable PCS loopback.
@param  starting_lane first of multiple lanes if in  multi lane ports
@param  num_lanes Number of lanes to configure if in multi lane ports
@returns The value PHYMOD_E_NONE upon successful completion
@details
This function sets the TX-to-RX digital loopback mode, which is set
independently for each lane at the PCS/PMS parallel interface. If the port in
question contains multiple lanes we specify the starting and the lane count and
all those lanes are configured to loopback mode

The 1st, 2nd, 3rd and 4th byte of 'lane' input arg. is associated with lanes 0,
1, 2, and 3 respectively. The bits of each byte control their lanes as follows.

\li 0:1 : Enable  TX->RX loopback
\li 0:0 : Disable TX-RX loopback

Note that this function can program <B>multiple lanes simultaneously</B>.
As an example, to enable gloop on all lanes and enable TX on lane 0 only,
set lane to 0x01010103
*/

int tefmod16_tx_loopback_control(PHYMOD_ST* pc, int pcs_gloop_en, int starting_lane, int num_lanes)           /* TX_LOOPBACK_CONTROL  */
{
  MAIN0_LPBK_CTLr_t MAIN0_LOOPBACK_CONTROLr_reg;
  PMD_X4_OVRRr_t PMD_X4_OVERRIDEr_reg;
  PMD_X4_CTLr_t PMD_X4_PMD_X4_CONTROLr_reg;
  SC_X4_CTLr_t  SC_X4_CTLr_reg;
  uint16_t lane_mask, i, data, tmp_data;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  MAIN0_LPBK_CTLr_CLR(MAIN0_LOOPBACK_CONTROLr_reg);
  READ_MAIN0_LPBK_CTLr(pc, &MAIN0_LOOPBACK_CONTROLr_reg); 
  tmp_data = MAIN0_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_GET(MAIN0_LOOPBACK_CONTROLr_reg);

   /* write 0 to the speed change */
  SC_X4_CTLr_CLR(SC_X4_CTLr_reg);
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc, &SC_X4_CTLr_reg));
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CTLr_reg, 0);
  PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_CTLr(pc, SC_X4_CTLr_reg));

  lane_mask = 0;
  data = 0;

  for (i = 0; i < num_lanes; i++) {
     if (!PHYMOD_LANEPBMP_MEMBER(pc->lane_mask, starting_lane + i)) {
            continue;
     }
     lane_mask |= 1 << (starting_lane + i);
     data |= pcs_gloop_en << (starting_lane + i);
   }

  tmp_data &= ~lane_mask;
  tmp_data |= data;

  MAIN0_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_SET(MAIN0_LOOPBACK_CONTROLr_reg,  tmp_data);
  PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_LPBK_CTLr(pc, MAIN0_LOOPBACK_CONTROLr_reg));

  /* signal_detect and rx_lock */
  PMD_X4_OVRRr_CLR(PMD_X4_OVERRIDEr_reg);
  if (pcs_gloop_en) {
    PMD_X4_OVRRr_RX_LOCK_OVRDf_SET(PMD_X4_OVERRIDEr_reg, 1);
    PMD_X4_OVRRr_SIGNAL_DETECT_OVRDf_SET(PMD_X4_OVERRIDEr_reg, 1);
    PMD_X4_OVRRr_TX_DISABLE_OENf_SET(PMD_X4_OVERRIDEr_reg, 1);
  } else {
    PMD_X4_OVRRr_RX_LOCK_OVRDf_SET(PMD_X4_OVERRIDEr_reg, 0);
    PMD_X4_OVRRr_SIGNAL_DETECT_OVRDf_SET(PMD_X4_OVERRIDEr_reg, 0);
    PMD_X4_OVRRr_TX_DISABLE_OENf_SET(PMD_X4_OVERRIDEr_reg, 0);
  }
  PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_OVRRr(pc, PMD_X4_OVERRIDEr_reg));

  /* set tx_disable */
  PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);
  PMD_X4_CTLr_TX_DISABLEf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));

  /* write 1 to the speed change */
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CTLr_reg, 1);
  PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_CTLr(pc, SC_X4_CTLr_reg));

  return PHYMOD_E_NONE;
}  /* tefmod16_tx_loopback_control */


/*!
@brief Set remote loopback mode for GMII, cl49, aggregate(XAUI) and R2 modes.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  pcs_rloop_en  controls rloop  enable or disable
@returns The value PHYMOD_E_NONE upon successful completion
@details
This function sets the remote loopback (RX-to-TX) mode for one lane at a time,
where the lane is indicated by the  field. To enable remote loopback, set
pcs_rloop_en to 1; to disable remote loopback, set the it to 0.
*/

int tefmod16_rx_loopback_control(PHYMOD_ST* pc, int pcs_rloop_en)
{
  MAIN0_LPBK_CTLr_t MAIN0_LOOPBACK_CONTROLr_reg;
  DSC_CDR_CTL2r_t DSC_C_CDR_CONTROL_2r_reg;
  TX_PI_CTL0r_t TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg; 

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
/* remote device is set in rloop */
#ifdef _DV_TB_
  /* remote device is set in rloop */
  if ((pc->unit) == 2) {
#endif
    MAIN0_LPBK_CTLr_CLR(MAIN0_LOOPBACK_CONTROLr_reg);
    MAIN0_LPBK_CTLr_REMOTE_PCS_LOOPBACK_ENABLEf_SET(MAIN0_LOOPBACK_CONTROLr_reg, pcs_rloop_en);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_LPBK_CTLr(pc, MAIN0_LOOPBACK_CONTROLr_reg));

    /* set Tx_PI */
    DSC_CDR_CTL2r_CLR(DSC_C_CDR_CONTROL_2r_reg);
    DSC_CDR_CTL2r_TX_PI_LOOP_TIMING_SRC_SELf_SET(DSC_C_CDR_CONTROL_2r_reg, 1);

    PHYMOD_IF_ERR_RETURN(MODIFY_DSC_CDR_CTL2r(pc, DSC_C_CDR_CONTROL_2r_reg));

    /* NEW 16NM */
    TX_PI_CTL0r_CLR(TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg);
    TX_PI_CTL0r_TX_PI_ENf_SET(TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg,1);

    PHYMOD_IF_ERR_RETURN(MODIFY_TX_PI_CTL0r(pc, TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg));

#ifdef _DV_TB_
  }
#endif
  return PHYMOD_E_NONE;
} /* tefmod16_rx_loopback_control */

int tefmod16_tx_loopback_get(PHYMOD_ST* pc, uint32_t *enable)           /* TX_LOOPBACK_get  */
{
  MAIN0_LPBK_CTLr_t loopReg;
  READ_MAIN0_LPBK_CTLr(pc, &loopReg);
  *enable = MAIN0_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_GET(loopReg);

  return PHYMOD_E_NONE;
}

/**
@brief   PMD per lane reset
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per lane PMD ln_rst and ln_dp_rst by writing to PMD_X4_CONTROL in pcs space
*/
int tefmod16_pmd_x4_reset(PHYMOD_ST* pc)              /* PMD_X4_RESET */
{
  PMD_X4_CTLr_t reg_pmd_x4_ctrl;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  PMD_X4_CTLr_CLR(reg_pmd_x4_ctrl);
  PMD_X4_CTLr_LN_H_RSTBf_SET(reg_pmd_x4_ctrl,0);
  PMD_X4_CTLr_LN_DP_H_RSTBf_SET(reg_pmd_x4_ctrl,0);
  PHYMOD_IF_ERR_RETURN (MODIFY_PMD_X4_CTLr(pc, reg_pmd_x4_ctrl));

  PMD_X4_CTLr_CLR(reg_pmd_x4_ctrl);
  PMD_X4_CTLr_LN_H_RSTBf_SET(reg_pmd_x4_ctrl,1);
  PMD_X4_CTLr_LN_DP_H_RSTBf_SET(reg_pmd_x4_ctrl,1);
  PHYMOD_IF_ERR_RETURN (MODIFY_PMD_X4_CTLr(pc, reg_pmd_x4_ctrl));

  return  PHYMOD_E_NONE;
}

/**
@brief   PMD per lane reset get
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per lane PMD ln_rst and ln_dp_rst by writing to PMD_X4_CONTROL in pcs space
*/
int tefmod16_pmd_x4_reset_get(PHYMOD_ST* pc, int *is_in_reset)              /* PMD_X4_RESET */
{
  PMD_X4_CTLr_t reg_pmd_x4_ctrl;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
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
@brief   Set Per lane OS mode set in PMD
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   spd_intf speed type #tefmod16_spd_intfc_type_t
@param   os_mode over sample rate.
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PMD Init

*/
int tefmod16_pmd_osmode_set(PHYMOD_ST* pc, tefmod16_spd_intfc_type_t spd_intf, phymod_ref_clk_t refclk, int os_mode)   /* INIT_PMD */
{
  RXTXCOM_OSR_MODE_CTLr_t reg_osr_mode;
  int speed;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  RXTXCOM_OSR_MODE_CTLr_CLR(reg_osr_mode);
  tefmod16_get_mapped_speed(spd_intf, &speed);

  /* 0=OS_MODE_1;  1=OS_MODE_2;  
     2=OS_MODE_4;  8=OS MODE_16p5; 
     12=OS_MODE_20p625;  */

  if(os_mode & 0x80000000) {
    os_mode =  (os_mode) & 0x0000ffff;
   
  } else {
    if (refclk == phymodRefClk125Mhz) {
        os_mode =  sc_pmd16_entry_125M_ref[speed].t_pma_os_mode;
    } else {
        os_mode =  sc_pmd16_entry[speed].t_pma_os_mode;
    }
  }

  RXTXCOM_OSR_MODE_CTLr_OSR_MODE_FRCf_SET(reg_osr_mode, 1);
  RXTXCOM_OSR_MODE_CTLr_OSR_MODE_FRC_VALf_SET(reg_osr_mode, os_mode);

  PHYMOD_IF_ERR_RETURN
     (MODIFY_RXTXCOM_OSR_MODE_CTLr(pc, reg_osr_mode));

  return PHYMOD_E_NONE;
}

/**
@brief   Checks config valid status for the port
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details This register bit indicates that PCS is now programmed as required
by the HT entry for that speed
*/
STATIC
int _tefmod16_wait_sc_stats_set(PHYMOD_ST* pc)
{
#if defined(_DV_TB_) || defined(_SDK_TEFMOD16_)
  uint16_t data;
  uint16_t i;
  SC_X4_STSr_t reg_sc_ctrl_sts;

  SC_X4_STSr_CLR(reg_sc_ctrl_sts);
  i = 0;
  data = 0;
#endif /* _DV_TB_ || _SDK_TEFMOD16_ */

#ifdef _DV_TB_
  while(1){
   i=i+1; /* added only to eliminate compile warning */
   PHYMOD_IF_ERR_RETURN(READ_SC_X4_STSr(pc, &reg_sc_ctrl_sts));
   data = SC_X4_STSr_SW_SPEED_CONFIG_VLDf_GET(reg_sc_ctrl_sts);
   if(data == 1)
     break;
  }
#endif /* _DV_TB_ */
#ifdef _SDK_TEFMOD16_
  for (i= 0; i < 10; i++) {
    PHYMOD_USLEEP(1);
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_STSr(pc, &reg_sc_ctrl_sts));
    data = SC_X4_STSr_SW_SPEED_CONFIG_VLDf_GET(reg_sc_ctrl_sts);
    if(data == 1) {
      return PHYMOD_E_NONE;
    }
  }
  if (data != 1) {
    return PHYMOD_E_TIMEOUT;
  }
#endif /* _SDK_TEFMOD16_ */
  /* Intentional check .if none of the above MACROs are defined,return PHYMOD_E_NONE */
  /* coverity[dead_error_line] */
  return PHYMOD_E_NONE;
}

/*Enable 10G clock when VCO is 25G*/
int tefmod16_10g_clock_enable(PHYMOD_ST *pc, int enable)
{
    RX_CTL8r_t RX_CTL8r_t_reg;
    TX_CTL3r_t TX_CTL3r_t_reg;

    RX_CTL8r_CLR(RX_CTL8r_t_reg);
    TX_CTL3r_CLR(TX_CTL3r_t_reg);

    RX_CTL8r_AMS_RX_SPARE_134f_SET(RX_CTL8r_t_reg, enable);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_CTL8r(pc, RX_CTL8r_t_reg));

    PHYMOD_IF_ERR_RETURN(READ_RX_CTL8r(pc, &RX_CTL8r_t_reg));
    RX_CTL8r_AMS_RX_RXCLK_DIV2P5f_SET(RX_CTL8r_t_reg, enable);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_CTL8r(pc, RX_CTL8r_t_reg));

    TX_CTL3r_AMS_TX_SPARE_58f_SET(TX_CTL3r_t_reg, enable);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_CTL3r(pc, TX_CTL3r_t_reg));

    PHYMOD_IF_ERR_RETURN(READ_TX_CTL3r(pc, &TX_CTL3r_t_reg));
    TX_CTL3r_AMS_TX_TXCLK_DIV2P5f_SET(TX_CTL3r_t_reg, enable);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_CTL3r(pc, TX_CTL3r_t_reg));

    return PHYMOD_E_NONE;
}

/*override the speed id 0x1(S_10G_KR1)*/
int tefmod16_override_10g_kr1_spd_intf(PHYMOD_ST *pc, int override)
{
    phymod_access_t pa_copy;
    int start_lane = 0, num_lane = 0;
    /*0x9280*/
    SC_X1_SPD_OVRR1_SPDr_t SC_X1_SPD_OVRR1_SPDr_t_reg;
    /*0x9282*/
    SC_X1_SPD_OVRR1_0r_t SC_X1_SPD_OVRR1_0r_t_reg;
    /*0x9283*/
    SC_X1_SPD_OVRR1_1r_t SC_X1_SPD_OVRR1_1r_t_reg;
    /*0x9284*/
    SC_X1_SPD_OVRR1_2r_t SC_X1_SPD_OVRR1_2r_t_reg;
    /*0x9285*/
    SC_X1_SPD_OVRR1_3r_t SC_X1_SPD_OVRR1_3r_t_reg;
    /*0x9286*/
    SC_X1_SPD_OVRR1_4r_t SC_X1_SPD_OVRR1_4r_t_reg;
    /*0x9287*/
    SC_X1_SPD_OVRR1_5r_t SC_X1_SPD_OVRR1_5r_t_reg;
    /*0x9288*/
    SC_X1_SPD_OVRR1_6r_t SC_X1_SPD_OVRR1_6r_t_reg;

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
    pa_copy.lane_mask = 0x1 << start_lane;
    if (override) {
        /*0x9280*/
        SC_X1_SPD_OVRR1_SPDr_CLR(SC_X1_SPD_OVRR1_SPDr_t_reg);
        SC_X1_SPD_OVRR1_SPDr_SPEEDf_SET(SC_X1_SPD_OVRR1_SPDr_t_reg, 0x1);
        SC_X1_SPD_OVRR1_SPDr_T_PMA_40B_MODEf_SET(SC_X1_SPD_OVRR1_SPDr_t_reg, 0x0);
        SC_X1_SPD_OVRR1_SPDr_NUM_LANESf_SET(SC_X1_SPD_OVRR1_SPDr_t_reg, 0x0);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_SPDr(&pa_copy, SC_X1_SPD_OVRR1_SPDr_t_reg));

        /*0x9282*/
        SC_X1_SPD_OVRR1_0r_CLR(SC_X1_SPD_OVRR1_0r_t_reg);
        SC_X1_SPD_OVRR1_0r_SCR_MODEf_SET(SC_X1_SPD_OVRR1_0r_t_reg, 0x1);
        SC_X1_SPD_OVRR1_0r_T_PMA_BTMX_MODEf_SET(SC_X1_SPD_OVRR1_0r_t_reg, 0x0);
        SC_X1_SPD_OVRR1_0r_T_HG2_ENABLEf_SET(SC_X1_SPD_OVRR1_0r_t_reg, 0x0);
        SC_X1_SPD_OVRR1_0r_T_ENC_MODEf_SET(SC_X1_SPD_OVRR1_0r_t_reg, 0x01);
        SC_X1_SPD_OVRR1_0r_T_FIFO_MODEf_SET(SC_X1_SPD_OVRR1_0r_t_reg, 0x0);
        SC_X1_SPD_OVRR1_0r_OS_MODEf_SET(SC_X1_SPD_OVRR1_0r_t_reg, 0x0);
        SC_X1_SPD_OVRR1_0r_CL72_ENf_SET(SC_X1_SPD_OVRR1_0r_t_reg, 0x01);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_0r(&pa_copy, SC_X1_SPD_OVRR1_0r_t_reg));

        /**0x9283*/
        SC_X1_SPD_OVRR1_1r_CLR(SC_X1_SPD_OVRR1_1r_t_reg);
        SC_X1_SPD_OVRR1_1r_SET(SC_X1_SPD_OVRR1_1r_t_reg, 0x5570);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR1_1r(&pa_copy, SC_X1_SPD_OVRR1_1r_t_reg));

        /*0x9284*/
        SC_X1_SPD_OVRR1_2r_CLR(SC_X1_SPD_OVRR1_2r_t_reg);
        SC_X1_SPD_OVRR1_2r_CLOCKCNT0f_SET(SC_X1_SPD_OVRR1_2r_t_reg, 0x25);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_2r(&pa_copy, SC_X1_SPD_OVRR1_2r_t_reg));

        /*0x9285*/
        SC_X1_SPD_OVRR1_3r_CLR(SC_X1_SPD_OVRR1_3r_t_reg);
        SC_X1_SPD_OVRR1_3r_CLOCKCNT1f_SET(SC_X1_SPD_OVRR1_3r_t_reg, 0x19);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_3r(&pa_copy, SC_X1_SPD_OVRR1_3r_t_reg));

        /*0x9286*/
        SC_X1_SPD_OVRR1_4r_CLR(SC_X1_SPD_OVRR1_4r_t_reg);
        SC_X1_SPD_OVRR1_4r_LOOPCNT0f_SET(SC_X1_SPD_OVRR1_4r_t_reg, 0x2);
        SC_X1_SPD_OVRR1_4r_LOOPCNT1f_SET(SC_X1_SPD_OVRR1_4r_t_reg, 0x1);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_4r(&pa_copy, SC_X1_SPD_OVRR1_4r_t_reg));

        /*0x9287*/
        SC_X1_SPD_OVRR1_5r_CLR(SC_X1_SPD_OVRR1_5r_t_reg);
        SC_X1_SPD_OVRR1_5r_MAC_CREDITGENCNTf_SET(SC_X1_SPD_OVRR1_5r_t_reg, 0xc);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_5r(&pa_copy, SC_X1_SPD_OVRR1_5r_t_reg));

        /*0x9288*/
        SC_X1_SPD_OVRR1_6r_CLR(SC_X1_SPD_OVRR1_6r_t_reg);
        SC_X1_SPD_OVRR1_6r_T_CL91_CW_SCRAMBLEf_SET(SC_X1_SPD_OVRR1_6r_t_reg, 0);
        SC_X1_SPD_OVRR1_6r_T_FIVE_BIT_XOR_ENf_SET(SC_X1_SPD_OVRR1_6r_t_reg, 0);
        SC_X1_SPD_OVRR1_6r_T_PMA_CL91_MUX_SELf_SET(SC_X1_SPD_OVRR1_6r_t_reg, 0);
        SC_X1_SPD_OVRR1_6r_T_PMA_WATERMARKf_SET(SC_X1_SPD_OVRR1_6r_t_reg, 0x4);
        SC_X1_SPD_OVRR1_6r_T_PMA_BITMUX_DELAYf_SET(SC_X1_SPD_OVRR1_6r_t_reg, 0x1);
        SC_X1_SPD_OVRR1_6r_AM_SPACING_MULf_SET(SC_X1_SPD_OVRR1_6r_t_reg, 0x0);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_6r(&pa_copy, SC_X1_SPD_OVRR1_6r_t_reg));
    } else {
        /*clear the override*/

        /*0x9280*/
        SC_X1_SPD_OVRR1_SPDr_CLR(SC_X1_SPD_OVRR1_SPDr_t_reg);
        SC_X1_SPD_OVRR1_SPDr_SPEEDf_SET(SC_X1_SPD_OVRR1_SPDr_t_reg, 0xff);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR1_SPDr(&pa_copy, SC_X1_SPD_OVRR1_SPDr_t_reg));

        /*0x9282*/
        SC_X1_SPD_OVRR1_0r_CLR(SC_X1_SPD_OVRR1_0r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR1_0r(&pa_copy, SC_X1_SPD_OVRR1_0r_t_reg));

        /**0x9283*/
        SC_X1_SPD_OVRR1_1r_CLR(SC_X1_SPD_OVRR1_1r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR1_1r(&pa_copy, SC_X1_SPD_OVRR1_1r_t_reg));

        /*0x9284*/
        SC_X1_SPD_OVRR1_2r_CLR(SC_X1_SPD_OVRR1_2r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR1_2r(&pa_copy, SC_X1_SPD_OVRR1_2r_t_reg));

        /*0x9285*/
        SC_X1_SPD_OVRR1_3r_CLR(SC_X1_SPD_OVRR1_3r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR1_3r(&pa_copy, SC_X1_SPD_OVRR1_3r_t_reg));

        /*0x9286*/
        SC_X1_SPD_OVRR1_4r_CLR(SC_X1_SPD_OVRR1_4r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR1_4r(&pa_copy, SC_X1_SPD_OVRR1_4r_t_reg));

        /*0x9287*/
        SC_X1_SPD_OVRR1_5r_CLR(SC_X1_SPD_OVRR1_5r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR1_5r(&pa_copy, SC_X1_SPD_OVRR1_5r_t_reg));

        /*0x9288*/
        SC_X1_SPD_OVRR1_6r_CLR(SC_X1_SPD_OVRR1_6r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR1_6r(&pa_copy, SC_X1_SPD_OVRR1_6r_t_reg));
    }

    return PHYMOD_E_NONE;
}

/*override the speed id 0x2*/
int tefmod16_override_10g_x1_spd_intf(PHYMOD_ST *pc, int override)
{
    phymod_access_t pa_copy;
    int start_lane = 0, num_lane = 0;
    /*0x9290*/
    SC_X1_SPD_OVRR2_SPDr_t SC_X1_SPD_OVRR2_SPDr_t_reg;
    /*0x9292*/
    SC_X1_SPD_OVRR2_0r_t SC_X1_SPD_OVRR2_0r_t_reg;
    /*0x9293*/
    SC_X1_SPD_OVRR2_1r_t SC_X1_SPD_OVRR2_1r_t_reg;
    /*0x9294*/
    SC_X1_SPD_OVRR2_2r_t SC_X1_SPD_OVRR2_2r_t_reg;
    /*0x9295*/
    SC_X1_SPD_OVRR2_3r_t SC_X1_SPD_OVRR2_3r_t_reg;
    /*0x9296*/
    SC_X1_SPD_OVRR2_4r_t SC_X1_SPD_OVRR2_4r_t_reg;
    /*0x9297*/
    SC_X1_SPD_OVRR2_5r_t SC_X1_SPD_OVRR2_5r_t_reg;
    /*0x9298*/
    SC_X1_SPD_OVRR2_6r_t SC_X1_SPD_OVRR2_6r_t_reg;

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
    pa_copy.lane_mask = 0x1 << start_lane;
    if (override) {
        /*0x9290*/
        SC_X1_SPD_OVRR2_SPDr_CLR(SC_X1_SPD_OVRR2_SPDr_t_reg);
        SC_X1_SPD_OVRR2_SPDr_SPEEDf_SET(SC_X1_SPD_OVRR2_SPDr_t_reg, 0x2);
        SC_X1_SPD_OVRR2_SPDr_T_PMA_40B_MODEf_SET(SC_X1_SPD_OVRR2_SPDr_t_reg, 0x0);
        SC_X1_SPD_OVRR2_SPDr_NUM_LANESf_SET(SC_X1_SPD_OVRR2_SPDr_t_reg, 0x0);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_SPDr(&pa_copy, SC_X1_SPD_OVRR2_SPDr_t_reg));

        /*0x9292*/
        SC_X1_SPD_OVRR2_0r_CLR(SC_X1_SPD_OVRR2_0r_t_reg);
        SC_X1_SPD_OVRR2_0r_SCR_MODEf_SET(SC_X1_SPD_OVRR2_0r_t_reg, 0x1);
        SC_X1_SPD_OVRR2_0r_T_PMA_BTMX_MODEf_SET(SC_X1_SPD_OVRR2_0r_t_reg, 0x0);
        SC_X1_SPD_OVRR2_0r_T_HG2_ENABLEf_SET(SC_X1_SPD_OVRR2_0r_t_reg, 0x0);
        SC_X1_SPD_OVRR2_0r_T_ENC_MODEf_SET(SC_X1_SPD_OVRR2_0r_t_reg, 0x01);
        SC_X1_SPD_OVRR2_0r_T_FIFO_MODEf_SET(SC_X1_SPD_OVRR2_0r_t_reg, 0x0);
        SC_X1_SPD_OVRR2_0r_OS_MODEf_SET(SC_X1_SPD_OVRR2_0r_t_reg, 0x0);
        SC_X1_SPD_OVRR2_0r_CL72_ENf_SET(SC_X1_SPD_OVRR2_0r_t_reg, 0x01);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_0r(&pa_copy, SC_X1_SPD_OVRR2_0r_t_reg));

        /**0x9293*/
        SC_X1_SPD_OVRR2_1r_CLR(SC_X1_SPD_OVRR2_1r_t_reg);
        SC_X1_SPD_OVRR2_1r_SET(SC_X1_SPD_OVRR2_1r_t_reg, 0x5570);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR2_1r(&pa_copy, SC_X1_SPD_OVRR2_1r_t_reg));

        /*0x9294*/
        SC_X1_SPD_OVRR2_2r_CLR(SC_X1_SPD_OVRR2_2r_t_reg);
        SC_X1_SPD_OVRR2_2r_CLOCKCNT0f_SET(SC_X1_SPD_OVRR2_2r_t_reg, 0x25);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_2r(&pa_copy, SC_X1_SPD_OVRR2_2r_t_reg));

        /*0x9295*/
        SC_X1_SPD_OVRR2_3r_CLR(SC_X1_SPD_OVRR2_3r_t_reg);
        SC_X1_SPD_OVRR2_3r_CLOCKCNT1f_SET(SC_X1_SPD_OVRR2_3r_t_reg, 0x19);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_3r(&pa_copy, SC_X1_SPD_OVRR2_3r_t_reg));

        /*0x9296*/
        SC_X1_SPD_OVRR2_4r_CLR(SC_X1_SPD_OVRR2_4r_t_reg);
        SC_X1_SPD_OVRR2_4r_LOOPCNT0f_SET(SC_X1_SPD_OVRR2_4r_t_reg, 0x2);
        SC_X1_SPD_OVRR2_4r_LOOPCNT1f_SET(SC_X1_SPD_OVRR2_4r_t_reg, 0x1);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_4r(&pa_copy, SC_X1_SPD_OVRR2_4r_t_reg));

        /*0x9297*/
        SC_X1_SPD_OVRR2_5r_CLR(SC_X1_SPD_OVRR2_5r_t_reg);
        SC_X1_SPD_OVRR2_5r_MAC_CREDITGENCNTf_SET(SC_X1_SPD_OVRR2_5r_t_reg, 0xc);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_5r(&pa_copy, SC_X1_SPD_OVRR2_5r_t_reg));

        /*0x9298*/
        SC_X1_SPD_OVRR2_6r_CLR(SC_X1_SPD_OVRR2_6r_t_reg);
        SC_X1_SPD_OVRR2_6r_T_CL91_CW_SCRAMBLEf_SET(SC_X1_SPD_OVRR2_6r_t_reg, 0);
        SC_X1_SPD_OVRR2_6r_T_FIVE_BIT_XOR_ENf_SET(SC_X1_SPD_OVRR2_6r_t_reg, 0);
        SC_X1_SPD_OVRR2_6r_T_PMA_CL91_MUX_SELf_SET(SC_X1_SPD_OVRR2_6r_t_reg, 0);
        SC_X1_SPD_OVRR2_6r_T_PMA_WATERMARKf_SET(SC_X1_SPD_OVRR2_6r_t_reg, 0x4);
        SC_X1_SPD_OVRR2_6r_T_PMA_BITMUX_DELAYf_SET(SC_X1_SPD_OVRR2_6r_t_reg, 0x1);
        SC_X1_SPD_OVRR2_6r_AM_SPACING_MULf_SET(SC_X1_SPD_OVRR2_6r_t_reg, 0x0);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_6r(&pa_copy, SC_X1_SPD_OVRR2_6r_t_reg));
    } else {
        /*clear the override*/

        /*0x9290*/
        SC_X1_SPD_OVRR2_SPDr_CLR(SC_X1_SPD_OVRR2_SPDr_t_reg);
        SC_X1_SPD_OVRR2_SPDr_SPEEDf_SET(SC_X1_SPD_OVRR2_SPDr_t_reg, 0xff);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR2_SPDr(&pa_copy, SC_X1_SPD_OVRR2_SPDr_t_reg));

        /*0x9292*/
        SC_X1_SPD_OVRR2_0r_CLR(SC_X1_SPD_OVRR2_0r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR2_0r(&pa_copy, SC_X1_SPD_OVRR2_0r_t_reg));

        /**0x9293*/
        SC_X1_SPD_OVRR2_1r_CLR(SC_X1_SPD_OVRR2_1r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR2_1r(&pa_copy, SC_X1_SPD_OVRR2_1r_t_reg));

        /*0x9294*/
        SC_X1_SPD_OVRR2_2r_CLR(SC_X1_SPD_OVRR2_2r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR2_2r(&pa_copy, SC_X1_SPD_OVRR2_2r_t_reg));

        /*0x9295*/
        SC_X1_SPD_OVRR2_3r_CLR(SC_X1_SPD_OVRR2_3r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR2_3r(&pa_copy, SC_X1_SPD_OVRR2_3r_t_reg));

        /*0x9296*/
        SC_X1_SPD_OVRR2_4r_CLR(SC_X1_SPD_OVRR2_4r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR2_4r(&pa_copy, SC_X1_SPD_OVRR2_4r_t_reg));

        /*0x9297*/
        SC_X1_SPD_OVRR2_5r_CLR(SC_X1_SPD_OVRR2_5r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR2_5r(&pa_copy, SC_X1_SPD_OVRR2_5r_t_reg));

        /*0x9298*/
        SC_X1_SPD_OVRR2_6r_CLR(SC_X1_SPD_OVRR2_6r_t_reg);
        PHYMOD_IF_ERR_RETURN(WRITE_SC_X1_SPD_OVRR2_6r(&pa_copy, SC_X1_SPD_OVRR2_6r_t_reg));
    }

    return PHYMOD_E_NONE;
}
/**
@brief   Set the port speed and enable the port
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   spd_intf the speed to set the port enum #tefmod16_set_spd_intf
@returns The value PHYMOD_E_NONE upon successful completion
@details Sets the port to the specified speed and triggers the port, and waits
         for the port to be configured
*/
int tefmod16_set_spd_intf(PHYMOD_ST *pc, tefmod16_spd_intfc_type_t spd_intf)
{
  SC_X4_CTLr_t xgxs_x4_ctrl;
  phymod_access_t pa_copy;
  int speed_id = 0, start_lane = 0, num_lane = 0;
  TX_X4_TX_CTL0r_t TX_X4_TX_CTL0r_reg;
  RX_X4_PCS_CTL0r_t RX_X4_PCS_CTL0r_reg;
  int tx_fec_mode;
  int rx_fec_mode;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  TEFMOD16_DBG_IN_FUNC_VIN_INFO(pc,("spd_intf: %d", spd_intf));

  /* need to figure out what's the starting lane */
  PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
  PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
  pa_copy.lane_mask = 0x1 << start_lane;

  PHYMOD_IF_ERR_RETURN (tefmod16_get_mapped_speed(spd_intf, &speed_id));

  if (speed_id == sc_x4_control_sc_S_25G_X1) {
      TX_X4_TX_CTL0r_CLR(TX_X4_TX_CTL0r_reg);
      RX_X4_PCS_CTL0r_CLR(RX_X4_PCS_CTL0r_reg);

      READ_TX_X4_TX_CTL0r(pc, &TX_X4_TX_CTL0r_reg);
      READ_RX_X4_PCS_CTL0r(pc, &RX_X4_PCS_CTL0r_reg);

      tx_fec_mode = TX_X4_TX_CTL0r_CL91_FEC_MODEf_GET(TX_X4_TX_CTL0r_reg);
      rx_fec_mode = RX_X4_PCS_CTL0r_CL91_FEC_MODEf_GET(RX_X4_PCS_CTL0r_reg);

      /*In IEEE 25G FEC, speed id is S_25G_KR_IEEE (0x72)*/
      if ((tx_fec_mode == TEFMOD16_IEEE_25G_CL91_SINGLE_LANE)
        && (rx_fec_mode == TEFMOD16_IEEE_25G_CL91_SINGLE_LANE)) {
          speed_id = sc_x4_control_sc_S_25G_KR_IEEE;
      }
  }

  /* write the speed_id into the speed_change register */
  SC_X4_CTLr_CLR(xgxs_x4_ctrl);
  SC_X4_CTLr_SPEEDf_SET(xgxs_x4_ctrl, speed_id);
  MODIFY_SC_X4_CTLr(pc, xgxs_x4_ctrl);
  /*next call the speed_change routine */
  PHYMOD_IF_ERR_RETURN (tefmod16_trigger_speed_change(&pa_copy));
  /*check the speed_change_done bit*/
  PHYMOD_IF_ERR_RETURN (_tefmod16_wait_sc_stats_set(pc));

  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/*!
@brief Set remote loopback mode for GMII, cl49, aggregate(XAUI) and R2 modes.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details
This function sets the remote loopback (RX-to-TX) mode for one lane at a time,
where the lane is indicated by the lane field. To enable remote loopback, set
the field #PHYMOD_ST::en_dis to 1; to disable remote loopback, set pmd_rloop_en
to 0.

Note that the speed field PHYMOD_ST::spd_intf must be set prior to calling this
function because the loopback register and path are speed/interface dependent.

The first two bits of #PHYMOD_ST::per_lane_control decide for PCS pr PMD.  After 
right shifting the first two bits of #PHYMOD_ST::per_lane_control  the 1st, 2nd, 
3rd and 4th byte is associated with lanes 0, 1, 2, and 3 respectively. 

The bits of each byte control their lanes as follows.  !! Also 1st byte decides
it is for PCS or PMD remote loopback.

\li 0:0 : Enable PMD Loopback (rloop)
\li 0:1 : Enable PCS Loopback (rloop)
\li 1:1 : Enable loopback
\li 1:0 : Disable loopback

The function modifies the following registers:
\li 0x9009 (MAIN0_LPBK_CTLr)
\li   bit 15:12 for REMOTE_PCS_LOOPBACK_ENABLE
\li   bit 11:08 for REMOTE_PMD_LOOPBACK_ENABLE
*/
int tefmod16_rx_pmd_loopback_control(PHYMOD_ST* pc, int pcs_rloop_en, int pmd_rloop_en, int lane)
{
  uint16_t data;
  int    cntl;
  TLB_TX_RMT_LPBK_CFGr_t TLB_TX_RMT_LPBK_CONFIGr_reg;
  DSC_CDR_CTL2r_t DSC_C_CDR_CONTROL_2r_reg;
  TX_PI_PMD_CTL0r_t TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  data = 0;

/* remote pmd device is set in rloop */

#ifdef _DV_TB_
  if ((pc->unit) == 2) {
#endif
    cntl = (((pmd_rloop_en) >> lane) & 0x1); 
    TLB_TX_RMT_LPBK_CFGr_CLR(TLB_TX_RMT_LPBK_CONFIGr_reg);
    if (cntl) {
      TLB_TX_RMT_LPBK_CFGr_RMT_LPBK_ENf_SET(TLB_TX_RMT_LPBK_CONFIGr_reg, 1);
    } else {
      TLB_TX_RMT_LPBK_CFGr_RMT_LPBK_ENf_SET(TLB_TX_RMT_LPBK_CONFIGr_reg, 0);
    } 

    PHYMOD_IF_ERR_RETURN (MODIFY_TLB_TX_RMT_LPBK_CFGr(pc, TLB_TX_RMT_LPBK_CONFIGr_reg));

    /* set Tx_PI */
  
    DSC_CDR_CTL2r_CLR(DSC_C_CDR_CONTROL_2r_reg);
    DSC_CDR_CTL2r_TX_PI_LOOP_TIMING_SRC_SELf_SET(DSC_C_CDR_CONTROL_2r_reg, 1);

    PHYMOD_IF_ERR_RETURN(MODIFY_DSC_CDR_CTL2r(pc, DSC_C_CDR_CONTROL_2r_reg));

    TX_PI_CTL0r_CLR(TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg);
    TX_PI_CTL0r_TX_PI_ENf_SET(TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg, 1);

    PHYMOD_IF_ERR_RETURN(MODIFY_TX_PI_CTL0r(pc, TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg));
#ifdef _DV_TB_
  } else {

  }
#endif
  return PHYMOD_E_NONE;
}  /* tefmod16_rx_pmd_loopback_control */
#endif

#ifdef _DV_TB_
/*!
@brief Control resetting/setting of credits based on speed interface selection
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details

This function sets/resets all credits for PCS and MAC as per speed interface
selected.  Set #PHYMOD_ST::per_lane_control to 1 to set the credits and set it
to 0 to clear the credits. 

This function is controlled by the following register bit:

\li set credits: clockcnt0 (0xC100[13:0]) 
\li clockcnt1 (0xC101[7:0]) 
\li loopcnt0 (0xC102[13:6]), loopcnt1 (0xC102[5:0])
\li mac_creditgencnt (0xC103[12:0])
\li pcs_clockcnt0 (0xC104[13:0])
\li pcs_creditgencnt (0xC105[12:0])

*/

int tefmod16_credit_set(PHYMOD_ST* pc)   /* CREDIT_SET */
{
  uint16_t data_loopcnt;
  uint16_t data_mac_credit;
  uint16_t clockcnt0, clockcnt1, loopcnt0, loopcnt1, mac_credit;

  TX_X4_CRED0r_t TX_X4_CREDIT0_CREDIT0r_reg;
  TX_X4_CRED1r_t TX_X4_CREDIT0_CREDIT1r_reg;
  TX_X4_LOOPCNTr_t TX_X4_CREDIT0_LOOPCNTr_reg;
  TX_X4_MAC_CREDGENCNTr_t TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg;
  /* uint16_t data_sgmii_spd_switch;  */
  int cntl;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  /*dv_tfmod_update_st(pc);*/
  cntl = pc->per_lane_control;
  /* data_sgmii_spd_switch = 0; */
  data_loopcnt     = 0;
  data_mac_credit  = 0;
  clockcnt0        = 0;
  clockcnt1        = 0;
  loopcnt0         = 1;
  loopcnt1         = 0;
  mac_credit       = 0;
   
  if (cntl) {
    if (pc->spd_intf == TEFMOD16_SPD_1000_SGMII) {

    } else if ((pc->spd_intf == TEFMOD16_SPD_10000_XFI)||
       (pc->spd_intf == TEFMOD16_SPD_10600_XFI_HG)) {
      clockcnt0  = 9;
      clockcnt1  = 10;
      loopcnt0   = 1;
      loopcnt1   = 9;
      mac_credit = 6;

    } else if ((pc->spd_intf == TEFMOD16_SPD_20000_XFI) ||
       (pc->spd_intf == TEFMOD16_SPD_21200_XFI_HG)     ||
       (pc->spd_intf == TEFMOD16_SPD_25000_XFI)        ||
       (pc->spd_intf == TEFMOD16_SPD_26500_XFI_HG)) {
      clockcnt0  = 9;
      clockcnt1  = 5;
      loopcnt0   = 1;
      loopcnt1   = 18;
      mac_credit = 4;

    } else if ((pc->spd_intf == TEFMOD16_SPD_20G_MLD_X2) ||
       (pc->spd_intf == TEFMOD16_SPD_21G_MLD_HG_X2)) {
      clockcnt0  = 9;
      clockcnt1  = 5;
      loopcnt0   = 1;
      loopcnt1   = 18;
      mac_credit = 4;

    } else if ((pc->spd_intf == TEFMOD16_SPD_40G_MLD_X2) ||
       (pc->spd_intf == TEFMOD16_SPD_42G_MLD_HG_X2)      ||
       (pc->spd_intf == TEFMOD16_SPD_50G_MLD_X2)         ||
       (pc->spd_intf == TEFMOD16_SPD_53G_MLD_HG_X2)      ||
       (pc->spd_intf == TEFMOD16_SPD_40G_MLD_X4)         ||
       (pc->spd_intf == TEFMOD16_SPD_42G_MLD_HG_X4)      ||
       (pc->spd_intf == TEFMOD16_SPD_50G_MLD_X4)         ||
       (pc->spd_intf == TEFMOD16_SPD_53G_MLD_HG_X4) ) {
      clockcnt0  = 9;
      clockcnt1  = 5;
      loopcnt0   = 1;
      loopcnt1   = 18;
      mac_credit = 2;

    } else if ((pc->spd_intf == TEFMOD16_SPD_100G_MLD_X4) ||
       (pc->spd_intf == TEFMOD16_SPD_106G_MLD_HG_X4)) {
      clockcnt0  = 9;
      clockcnt1  = 5;
      loopcnt0   = 1;
      loopcnt1   = 18;
      mac_credit = 1;

    } else {
      PHYMOD_DEBUG_ERROR(("%-22s ERROR: p=%0d spd_intf=%d undefined %s cntl=%0x verb=%x\n",
                        __func__, pc->port, pc->spd_intf,
              e2s_tefmod16_spd_intfc_type_t[pc->spd_intf], cntl, pc->verbosity));
      return PHYMOD_E_FAIL;
    }
    /* set credtis 0xc100 */
    TX_X4_CRED0r_CLR(TX_X4_CREDIT0_CREDIT0r_reg);
    TX_X4_CRED0r_CLOCKCNT0f_SET(TX_X4_CREDIT0_CREDIT0r_reg, clockcnt0);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED0r(pc, TX_X4_CREDIT0_CREDIT0r_reg));
    
    /* 0xc101 */
    TX_X4_CRED1r_CLR(TX_X4_CREDIT0_CREDIT1r_reg);
    TX_X4_CRED1r_CLOCKCNT1f_SET(TX_X4_CREDIT0_CREDIT1r_reg, clockcnt1);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED1r(pc, TX_X4_CREDIT0_CREDIT1r_reg));

    /* 0xc102 */
    TX_X4_LOOPCNTr_CLR(TX_X4_CREDIT0_LOOPCNTr_reg);
    TX_X4_LOOPCNTr_LOOPCNT0f_SET(TX_X4_CREDIT0_LOOPCNTr_reg, loopcnt0);
    TX_X4_LOOPCNTr_LOOPCNT1f_SET(TX_X4_CREDIT0_LOOPCNTr_reg, loopcnt1);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_LOOPCNTr(pc, TX_X4_CREDIT0_LOOPCNTr_reg));

    /* 0xc103 */
    TX_X4_MAC_CREDGENCNTr_CLR(TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg);
    TX_X4_MAC_CREDGENCNTr_MAC_CREDITGENCNTf_SET(TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg, mac_credit);
    PHYMOD_IF_ERR_RETURN 
     (MODIFY_TX_X4_MAC_CREDGENCNTr(pc, TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg));
  } else {
    /* clear credits */
    TX_X4_CRED0r_CLR(TX_X4_CREDIT0_CREDIT0r_reg);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED0r(pc, TX_X4_CREDIT0_CREDIT0r_reg));
  }
  return PHYMOD_E_NONE;
}  /* CREDIT_SET  */
#endif

#ifdef _DV_TB_
/*!
@brief set tx encoder of any particular lane
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This function is used to select encoder of any transmit lane.

\li select encodeMode

This function reads/modifies the following registers:
\li Address (Name)
\li 0xc111 (TX_X4_ENC0)
\i     field               bit position     variable defined in this function
\li    ENCODEMODE         [4:2]             encode_mode
\li    CL49_BYPASS_TXSM   [9]               -
\li
\li 0xc113 (TX_X4_MISC)
\li    SCR_MODE           [15:14]           scr_mode
\li    RESERVED0          [13:11]           block_sync_mode
\li    CL49_TX_LI_ENABLE  [8]               -
\li    CL49_TX_LF_ENABLE  [7]               -
\li    CL49_TX_RF_ENABLE  [6]               -
\li 
\li 0xa000 (TX_X2_MLD_SWP_CNT)
\li    MLD_SWAP_COUNT     [15:0]            -

<B> ENCODE_SET Progamming values 
\li 10G_XFI
\li   encodeMode	   :   5
\li   scr_mode	       :   3
\li   
\li 10G_XAUI  
\li   encode_mode	   : 0x1     cl48 8b10b 
\li   CL49_BYPASS_TXSM : 0 
\li   scr_mode   	   : 0x0     bypass scramble 
\li
\li 40G_MLD
*/

/* for DV only ? CHECK Lpti */
int tefmod16_encode_set(PHYMOD_ST* pc, int per_lane_control, tefmod_spd_intfc_type_t spd_intf, int tx_am_timer_init_val)         /* ENCODE_SET */
{
  uint16_t t_pma_btmx_mode, scr_mode, os_mode, hg2_en, t_fifo_mode, t_enc_mode;
  uint16_t cl82_bypass_txsm, cl49_tx_tl_mode, cl49_bypass_txsm;
  int cntl;
  #ifdef TSCF_GEN1
  uint16_t mld_swap_count;
  TX_X1_T_PMA_WTRMKr_t TX_X1_CONTROL0_T_PMA_WATERMARKr_reg;
  TX_X2_MLD_SWP_CNTr_t TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg;
  #endif
  TX_X2_CL82_0r_t TX_X2_CONTROL0_CL82_0r_reg;
  TX_X4_ENC0r_t TX_X4_CONTROL0_ENCODE_0r_reg;
  TX_X4_MISCr_t TX_X4_CONTROL0_MISCr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  cntl = per_lane_control;

  cl49_bypass_txsm  = 0;
  hg2_en            = 0;    /* 001 cl48 8b10b
                                010 cl48 8b10b rxaui
                                011 cl36 8b10b
                                100 cl82 64b66b
                                101 cl49 64b66b
                                110 brcm 64b66b */
  scr_mode          = 0;   /* 00 bypass scrambler
                               01 64b66b scrambler (all 66 bits)
                               10 8b10b scrambler (all 80 bits)
                               11 64b66b scrambler (sync bits not scrambled)*/
  t_pma_btmx_mode   = 0;
  t_fifo_mode       = 0;
  t_enc_mode        = 0;
  cl82_bypass_txsm  = 0;
  cl49_tx_tl_mode   = 0;
  cl49_bypass_txsm  = 0;

  #ifdef TSCF_GEN1
  mld_swap_count    = 0;
  #endif

  if((spd_intf == TEFMOD16_SPD_10_SGMII) |
     (spd_intf == TEFMOD16_SPD_100_SGMII) | 
     (spd_intf == TEFMOD16_SPD_1000_SGMII) ) { 
    /* ------------------------------------- 0xc114 */
  } else if((spd_intf == TEFMOD16_SPD_10000_XFI) ||
           (spd_intf == TEFMOD16_SPD_10600_XFI_HG)) {
    t_fifo_mode      = 0;
    t_enc_mode       = 1;
    cl49_tx_tl_mode  = 0;
    cl49_bypass_txsm = 0;
    hg2_en           = 0;
    scr_mode         = 1;
    os_mode          = 1;
    t_pma_btmx_mode  = 0;
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 0;
    /* ------------------------------------ 0xc113 */
    /* scr_mode          = 0x1; */
    /* ------------------------------------ 0xa000 */
  } else if ((spd_intf == TEFMOD16_SPD_20000_XFI)    ||
             (spd_intf == TEFMOD16_SPD_25000_XFI)    ||         
             (spd_intf == TEFMOD16_SPD_21200_XFI_HG) ||
             (spd_intf == TEFMOD16_SPD_26500_XFI_HG) ||
             (spd_intf == TEFMOD16_SPD_25G_CR_IEEE)  ||
             (spd_intf == TEFMOD16_SPD_25G_CRS_IEEE) ||
             (spd_intf == TEFMOD16_SPD_25G_KR_IEEE)  ||
             (spd_intf == TEFMOD16_SPD_25G_KRS_IEEE) ||
             (spd_intf == TEFMOD16_SPD_25G_CR1)      ||
             (spd_intf == TEFMOD16_SPD_25G_KR1)      ||
             (spd_intf == TEFMOD16_SPD_25G_CR1_HG2)  ||
             (spd_intf == TEFMOD16_SPD_25G_KR1_HG2)) {
    t_fifo_mode      = 0;
    t_enc_mode       = 1;
    cl49_tx_tl_mode  = 0;
    cl49_bypass_txsm = 0;
    hg2_en           = 0;
    scr_mode         = 1;
    os_mode          = 0;
    t_pma_btmx_mode  = 0;
  } else if ((spd_intf == TEFMOD16_SPD_20G_MLD_X2) ||
             (spd_intf == TEFMOD16_SPD_21G_MLD_HG_X2)) {
    t_fifo_mode      = 3;
    t_enc_mode       = 2;
    cl82_bypass_txsm = 0;
    hg2_en           = 0;
    os_mode          = 1;
    scr_mode         = 4;
    t_pma_btmx_mode  = 0;
  #ifdef TSCF_GEN1
    mld_swap_count = tx_am_timer_init_val; /* shorten from 0xfffc.  For sim ? */      
  #endif
  } else if ((spd_intf == TEFMOD16_SPD_40G_MLD_X2)      ||
             (spd_intf == TEFMOD16_SPD_42G_MLD_HG_X2)   ||
             (spd_intf == TEFMOD16_SPD_50G_CR2)         ||
             (spd_intf == TEFMOD16_SPD_50G_KR2)         ||
             (spd_intf == TEFMOD16_SPD_50G_CR2_HG2)     ||
             (spd_intf == TEFMOD16_SPD_50G_KR2_HG2)     ||
             (spd_intf == TEFMOD16_SPD_53G_MLD_HG_X2)) {
    t_fifo_mode      = 1;
    t_enc_mode       = 2;
    cl82_bypass_txsm = 0;
    hg2_en           = 0;
    scr_mode         = 2;
    t_pma_btmx_mode  = 1;
    os_mode = 0;
  #ifdef TSCF_GEN1
    mld_swap_count = tx_am_timer_init_val;        /* shorten from 0xfffc.  For sim ? */      
  #endif
  } else if ((spd_intf == TEFMOD16_SPD_40G_MLD_X4) ||
             (spd_intf == TEFMOD16_SPD_50G_MLD_X4) ||
             (spd_intf == TEFMOD16_SPD_42G_MLD_HG_X4) ||
             (spd_intf == TEFMOD16_SPD_53G_MLD_HG_X4)) {
    t_fifo_mode      = 1;
    t_enc_mode       = 2;
    cl82_bypass_txsm = 0;
    hg2_en           = 0;
    scr_mode         = 5;
    t_pma_btmx_mode  = 0;
    os_mode = 1;
  #ifdef TSCF_GEN1
    mld_swap_count = tx_am_timer_init_val;        /* shorten from 0xfffc.  For sim ? */      
  #endif
  } else if ((spd_intf == TEFMOD16_SPD_100G_MLD_X4) ||
             (spd_intf == TEFMOD16_SPD_106G_MLD_HG_X4)) {
    t_fifo_mode      = 2;
    t_enc_mode       = 2;
    cl82_bypass_txsm = 0;
    hg2_en           = 0;
    scr_mode         = 3;
    t_pma_btmx_mode  = 2;
    os_mode          = 0;
  #ifdef TSCF_GEN1
    mld_swap_count = tx_am_timer_init_val;
  #endif
  } else if (spd_intf == TEFMOD16_SPD_10000 ) {   /* 10G XAUI */
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;   
    /* ------------------------------------ 0xc113 */
    scr_mode          = 0x0;         /* bypass scramble */
    /* ------------------------------------ 0xa000 */
  } else if (spd_intf == TEFMOD16_SPD_10000_HI ) {   /* 10.6G HI XAUI */
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;   
    /* ------------------------------------ 0xc113 */
    scr_mode          = 0x0;         /* bypass scramble */
    /* ------------------------------------ 0xa000 */
  } else if (spd_intf == TEFMOD16_SPD_10000_DXGXS) {
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;   
    /* ------------------------------------ 0xc113 */
    scr_mode          = 0x0;         /* bypass scramble */
    /* ------------------------------------ 0xa000 */
  } else if (spd_intf == TEFMOD16_SPD_20000) {  /* 20G XAIU */
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;   
    /* ------------------------------------ 0xc113 */
    scr_mode          = 0x0;                 
    /* ------------------------------------ 0xa000 */
  } else if (spd_intf == TEFMOD16_SPD_21000) {  /* 20G XAIU */
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;   
    /* ------------------------------------ 0xc113 */
    scr_mode          = 0x2;                 
    /* ------------------------------------ 0xa000 */
  } else if (spd_intf == TEFMOD16_SPD_20G_MLD_DXGXS ) {
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 0;
    /* --------------------- ---------------0xc113 */
    scr_mode          = 0x3;         /* 64b66b scrambled */
    /* ------------------------------------ 0xa000 */
  #ifdef TSCF_GEN1
    mld_swap_count = tx_am_timer_init_val;        /* shorten from 0xfffc.  For sim ? */      
  #endif

     /* 0xa000 default value in silicon is for dual port */
  } else if ((spd_intf ==TEFMOD16_SPD_21G_HI_MLD_DXGXS)||
             (spd_intf ==TEFMOD16_SPD_12773_HI_DXGXS)) {
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 0;
    hg2_en            = 1;
    /*hg2_codec         = 1; */
    /* --------------------- ---------------0xc113 */
    scr_mode          = 0x3;         /* 64b66b scrambled */
    /* ------------------------------------ 0xa000 */
  #ifdef TSCF_GEN1
    mld_swap_count = tx_am_timer_init_val;        /* shorten from 0xfffc.  For sim ? */      
  #endif

  } else if ((spd_intf == TEFMOD16_SPD_20G_DXGXS)||
             (spd_intf == TEFMOD16_SPD_21G_HI_DXGXS)) {
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;
    /* ------------------------------------ 0xc113 */
    scr_mode          = 0x1;        /* 64b66b scrambled */
    /* ------------------------------------ 0xa000 */
  } else if ((spd_intf == TEFMOD16_SPD_42G_X4)||
             (spd_intf == TEFMOD16_SPD_40G_X4)||
             (spd_intf == TEFMOD16_SPD_25455) ||
             (spd_intf == TEFMOD16_SPD_12773_DXGXS)) {
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;
    /* ------------------------------------ 0xc113 */
    scr_mode     = 0x1;              /* 64b66b scrambled */
    /* ------------------------------------ 0xa000 */
  } else if (spd_intf == TEFMOD16_SPD_40G_XLAUI ) {
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 0;
    /* ------------------------------------ 0xc113 */
    scr_mode     = 0x3;              /* 64b66b scrambled */
    /* ------------------------------------ 0xa000 */
/*
#if defined (_DV_TRIDENT2)     
    mld_swap_count = tx_am_timer_init_val;
#else
    mld_swap_count = 0xfffc;
#endif
     */
  } else if (spd_intf == TEFMOD16_SPD_42G_XLAUI ) {
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 0;
    hg2_en            = 1;
    /*hg2_codec         = 1;*/
    /* ------------------------------------ 0xc113 */
    scr_mode     = 0x3;              /* 64b66b scrambled */
    /* ------------------------------------ 0xa000 */
/*
#if defined (_DV_TRIDENT2) 
    mld_swap_count = tx_am_timer_init_val;
#else
    mld_swap_count = 0xfffc;
#endif      
     */
  }
  if((spd_intf == TEFMOD16_SPD_10600_XFI_HG)   ||
     (spd_intf == TEFMOD16_SPD_21200_XFI_HG)   ||
     (spd_intf == TEFMOD16_SPD_26500_XFI_HG)   ||
     (spd_intf == TEFMOD16_SPD_21G_MLD_HG_X2)  ||
     (spd_intf == TEFMOD16_SPD_42G_MLD_HG_X4)  ||
     (spd_intf == TEFMOD16_SPD_53G_MLD_HG_X4)  ||
     (spd_intf == TEFMOD16_SPD_42G_MLD_HG_X2)  ||
     (spd_intf == TEFMOD16_SPD_53G_MLD_HG_X2)  ||
     (spd_intf == TEFMOD16_SPD_106G_MLD_HG_X4)) {
    /* not CL48 */
    hg2_en          = 1;
  }
  if (cntl) {
    TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
    TX_X4_MISCr_OS_MODEf_SET(TX_X4_CONTROL0_MISCr_reg, os_mode);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));

    #ifdef TSCF_GEN1
    TX_X1_T_PMA_WTRMKr_CLR(TX_X1_CONTROL0_T_PMA_WATERMARKr_reg);
    TX_X1_T_PMA_WTRMKr_SINGLE_LANE_BITMUX_WATERMARKf_SET(TX_X1_CONTROL0_T_PMA_WATERMARKr_reg, 4);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X1_T_PMA_WTRMKr(pc, TX_X1_CONTROL0_T_PMA_WATERMARKr_reg));
    #endif

    TX_X2_CL82_0r_CLR(TX_X2_CONTROL0_CL82_0r_reg);
    #ifdef TSCF_GEN1
    TX_X2_CL82_0r_T_FIFO_MODEf_SET(TX_X2_CONTROL0_CL82_0r_reg, t_fifo_mode);
    #else
    TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);
    TX_X4_ENC0r_T_FIFO_MODEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, t_fifo_mode);
    #endif
    TX_X2_CL82_0r_CL82_BYPASS_TXSMf_SET(TX_X2_CONTROL0_CL82_0r_reg, cl82_bypass_txsm);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X2_CL82_0r(pc, TX_X2_CONTROL0_CL82_0r_reg));

    TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);
    TX_X4_ENC0r_T_ENC_MODEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, t_enc_mode);
    TX_X4_ENC0r_CL49_TX_TL_MODEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, cl49_tx_tl_mode);
    TX_X4_ENC0r_CL49_BYPASS_TXSMf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, cl49_bypass_txsm);
    TX_X4_ENC0r_HG2_ENABLEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, hg2_en);
    PHYMOD_IF_ERR_RETURN 
         (MODIFY_TX_X4_ENC0r(pc, TX_X4_CONTROL0_ENCODE_0r_reg));

    TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
    TX_X4_MISCr_SCR_MODEf_SET(TX_X4_CONTROL0_MISCr_reg, scr_mode);
    TX_X4_MISCr_T_PMA_BTMX_MODEf_SET(TX_X4_CONTROL0_MISCr_reg, t_pma_btmx_mode);
    PHYMOD_IF_ERR_RETURN 
         (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
 
    #ifdef TSCF_GEN1   
    /* 0xa000 */
    TX_X2_MLD_SWP_CNTr_CLR(TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg);
    TX_X2_MLD_SWP_CNTr_MLD_SWAP_COUNTf_SET(TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg, mld_swap_count);
    /*if(mask) */
    PHYMOD_IF_ERR_RETURN 
    (MODIFY_TX_X2_MLD_SWP_CNTr(pc, TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg));
   #endif 
  } else {
    /* no op */
    return PHYMOD_E_NONE;
  }
  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/*!
@brief Local Fault/Remote Fault signalling enable for Tx and RX
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This function is used to enable/disable Link Interrupt, Local Fault and
Remote Fault signalling of any lane .

\li #PHYMOD_ST::per_lane_control[3:0] represents the mode (CL49, CL48, CL82).
\li #PHYMOD_ST::per_lane_control[6]   is for li;
\li #PHYMOD_ST::per_lane_control[5]   is for lf;
\li #PHYMOD_ST::per_lane_control[4]   is for rf;

This function reads/modifies the following registers:

\li cl49_tx            :   0xc113
\li cl49_rx            :   0xc134
\li cl48_tx            :   0xa001
\li cl82_tx            :   0xa002
\li cl48_cl82_rx       :   0xa024
*/

int tefmod16_lf_rf_control(PHYMOD_ST* pc)         /* LF_RF_CONTROL */
{
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/*!
@brief set rx decoder of any particular lane

@param  pc handle to current TSCF context (#PHYMOD_ST)

@returns The value PHYMOD_E_NONE upon successful completion.

This function is used to select decoder of any receive lane.

\li select encodeMode

This function reads/modifies the following registers:

\li descr_mode     :   0xc130[15:14]
\li dec_tl_mode	   :   0xc130[13:11]
\li deskew_mode	   :   0xc130[10:8]
\li dec_fsm_mode   :  0xc130[7:5]
\li cl36_en	       :   0xc134[0]

*/

/* for DV only ? CHECK Lpti */
int tefmod16_decode_set(PHYMOD_ST* pc)         /* DECODE_SET */
{
  uint16_t descr_mode, dec_tl_mode, deskew_mode, dec_fsm_mode;
  uint16_t bypass_cl49rxsm, hg2_message_ivalid_code_enable, hg2_en;
  uint16_t bs_sm_sync_mode;
  uint16_t bs_sync_en;
  uint16_t bs_dist_mode;
  uint16_t bs_btmx_mode;
/*  uint16_t sync_code_sm_en; */
  uint16_t rx_control0_misc0_en;
  int cntl;
  RX_X4_BLKSYNC_CFGr_t   RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg;
  RX_X4_DEC_CTL0r_t RX_X4_CONTROL0_DECODE_CONTROL_0r_reg;
  RX_X4_PCS_CTL0r_t    RX_X4_CONTROL0_PCS_CONTROL_0r_reg;
  CL82_LANES_1_0_AM_BYTE2r_t     CL82_AMS_LANES_1_0_AM_BYTE2r_reg;
  CL82_LN_1_AM_BYTE10r_t       CL82_AMS_LANE_1_AM_BYTE10r_reg;
  CL82_LN_0_AM_BYTE10r_t       CL82_AMS_LANE_0_AM_BYTE10r_reg;
  #ifdef TSCF_GEN1
  CL82_RX_AM_TMRr_t    CL82_SHARED_CL82_RX_AM_TIMERr_reg;
  #else
  CL82_AM_TMRr_t       CL82_SHARED_CL82_AM_TIMERr_reg;
  #endif

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  cntl = pc->per_lane_control;
  descr_mode       = 0x0; /* 00=bypass descrambler; 01=64b66b descrambler; 
                            10=8b10b descrambler */
  dec_tl_mode      = 0x0; /* 3'b000 - None; 3'b001 - cl49 64b66b mode; 
                            3'b010 - BRCM 64b66b mode; 
                            3'b011 - ATL - cl49/BRCM 64b66b mode; 
                            3'b100 - 8b10b mode - cl48 mode 
                            3'b101 - 8b10b mode - cl36 mode */
  deskew_mode 	   = 0x0; 
  dec_fsm_mode     = 0x0; /* {001:CL49}, {010:BRCM}, {011:ATL}, 
                            {100:CL48}, {101:CL36}, {110:CL82}, {111:NONE} */
  bypass_cl49rxsm = 0x0;
  hg2_message_ivalid_code_enable = 0x1;
  hg2_en          = 0;

/*  cl48_dswin64b66b     = 0;
  cl48_dswin8b10b      = 0;
  sync_code_sm_en      = 0; */

  rx_control0_misc0_en = 0;

  if (cntl) { /* set decoder */
    if(pc->spd_intf == TEFMOD16_SPD_1000_SGMII) {
      dec_tl_mode     = 0x5;
      deskew_mode     = 0x4;
      dec_fsm_mode    = 0x5;
      hg2_message_ivalid_code_enable = 0x0;
      if (pc->plldiv == 40) {
      } else if (pc->plldiv == 66){
      }
    } else if(pc->spd_intf == TEFMOD16_SPD_1G_20G) {
    } else if(pc->spd_intf == TEFMOD16_SPD_1G_25G) {
    } else if(pc->spd_intf == TEFMOD16_SPD_10000_XFI) {
      /* ------------------------------------  0xc130 */
      deskew_mode 	  = 0x5;
      descr_mode      = 0x1;
      dec_tl_mode     = 0x1;
      dec_fsm_mode    = 0x1;
      /* ------------------------------------  0xc136 */
      /* ------------------------------------  0xc134 */
      bypass_cl49rxsm = 0x0;

      bs_sm_sync_mode = 1;
      bs_sync_en      = 1;
      bs_dist_mode    = 0;
      bs_btmx_mode    = 0;
    } else if(pc->spd_intf == TEFMOD16_SPD_10600_XFI_HG) {
      /* ------------------------------------  0xc130 */
      deskew_mode 	  = 0x5;
      descr_mode      = 0x1;
      dec_tl_mode     = 0x1;
      dec_fsm_mode    = 0x1;
      /* ------------------------------------  0xc136 */
      /* ------------------------------------  0xc134 */
      bypass_cl49rxsm = 0x0;

      bs_sm_sync_mode = 1;
      bs_sync_en      = 1;
      bs_dist_mode    = 0;
      bs_btmx_mode    = 0;
      hg2_en          = 1;
    } else if ((pc->spd_intf == TEFMOD16_SPD_20000_XFI) ||
               (pc->spd_intf == TEFMOD16_SPD_21200_XFI_HG)      ||
               (pc->spd_intf == TEFMOD16_SPD_25000_XFI)         ||
               (pc->spd_intf == TEFMOD16_SPD_26500_XFI_HG)) {
      bs_sm_sync_mode = 1;
      bs_sync_en      = 1;
      bs_dist_mode    = 0;
      bs_btmx_mode    = 0;
      deskew_mode     = 5;
      descr_mode      = 1;
      dec_tl_mode     = 1;
      dec_fsm_mode    = 1;
    } else if ((pc->spd_intf == TEFMOD16_SPD_20G_MLD_X2) ||
               (pc->spd_intf == TEFMOD16_SPD_21G_MLD_HG_X2)) {
      bs_sm_sync_mode = 0;
      bs_sync_en      = 1;
      bs_dist_mode    = 2;
      bs_btmx_mode    = 0;
      deskew_mode     = 1;
      descr_mode      = 2;
      dec_tl_mode     = 2;
      dec_fsm_mode    = 2;
    } else if ((pc->spd_intf == TEFMOD16_SPD_40G_MLD_X4) ||
               (pc->spd_intf == TEFMOD16_SPD_50G_MLD_X4) ||
               (pc->spd_intf == TEFMOD16_SPD_42G_MLD_HG_X4) ||
               (pc->spd_intf == TEFMOD16_SPD_53G_MLD_HG_X4)) {
      bs_sm_sync_mode = 0;
      bs_sync_en      = 1;
      bs_dist_mode    = 2;
      bs_btmx_mode    = 0;
      deskew_mode     = 2;
      descr_mode      = 2;
      dec_tl_mode     = 2;
      dec_fsm_mode    = 2;
    } else if ((pc->spd_intf == TEFMOD16_SPD_40G_MLD_X2) ||
               (pc->spd_intf == TEFMOD16_SPD_42G_MLD_HG_X2)      ||
               (pc->spd_intf == TEFMOD16_SPD_50G_MLD_X2)         ||
               (pc->spd_intf == TEFMOD16_SPD_53G_MLD_HG_X2)) {
      bs_sm_sync_mode = 0;
      bs_sync_en      = 1;
      bs_dist_mode    = 1;
      bs_btmx_mode    = 1;
      deskew_mode     = 3;
      descr_mode      = 2;
      dec_tl_mode     = 2;
      dec_fsm_mode    = 2;
    } else if ((pc->spd_intf == TEFMOD16_SPD_100G_MLD_X4) ||
               (pc->spd_intf == TEFMOD16_SPD_106G_MLD_HG_X4)) {
      bs_sm_sync_mode = 0;
      bs_sync_en      = 1;
      bs_dist_mode    = 3;
      bs_btmx_mode    = 2;
      deskew_mode     = 4;
      descr_mode      = 2;
      dec_tl_mode     = 2;
      dec_fsm_mode    = 2;
    } else {
      PHYMOD_DEBUG_ERROR(("%-22s ERROR: p=%0d undefined spd_intf=%0d(%s)\n", __func__, pc->port, pc->spd_intf,
              e2s_tefmod16_spd_intfc_type_t[pc->spd_intf])); 
      return PHYMOD_E_FAIL;
    }
    /* HG setting */
    /* if((pc->ctrl_type & TEFMOD16_CTRL_TYPE_HG)) {} */
    if((pc->spd_intf == TEFMOD16_SPD_10600_XFI_HG)   ||
       (pc->spd_intf == TEFMOD16_SPD_21200_XFI_HG)   ||
       (pc->spd_intf == TEFMOD16_SPD_26500_XFI_HG)   ||
       (pc->spd_intf == TEFMOD16_SPD_21G_MLD_HG_X2)  ||
       (pc->spd_intf == TEFMOD16_SPD_42G_MLD_HG_X4)  ||
       (pc->spd_intf == TEFMOD16_SPD_53G_MLD_HG_X4)  ||
       (pc->spd_intf == TEFMOD16_SPD_42G_MLD_HG_X2)  ||
       (pc->spd_intf == TEFMOD16_SPD_53G_MLD_HG_X2)  ||
       (pc->spd_intf == TEFMOD16_SPD_106G_MLD_HG_X4)) {
      /* not CL48 */
      hg2_en          = 1;
    }

    /* 0xc130 */
    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
    RX_X4_PCS_CTL0r_DESCR_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, descr_mode);
    RX_X4_PCS_CTL0r_DEC_TL_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, dec_tl_mode);
    RX_X4_PCS_CTL0r_DESKEW_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, deskew_mode);
    RX_X4_PCS_CTL0r_DEC_FSM_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, dec_fsm_mode);

    PHYMOD_IF_ERR_RETURN 
        (MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));

    /* 0xc134 */
    RX_X4_DEC_CTL0r_CLR(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg);
    RX_X4_DEC_CTL0r_BYPASS_CL49RXSMf_SET(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg, bypass_cl49rxsm);
    RX_X4_DEC_CTL0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg, hg2_message_ivalid_code_enable);
    RX_X4_DEC_CTL0r_HG2_ENABLEf_SET(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg, hg2_en);

    PHYMOD_IF_ERR_RETURN 
        (MODIFY_RX_X4_DEC_CTL0r(pc, RX_X4_CONTROL0_DECODE_CONTROL_0r_reg));

    RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);
    RX_X4_BLKSYNC_CFGr_BS_SM_SYNC_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, bs_sm_sync_mode);
    RX_X4_BLKSYNC_CFGr_BS_SYNC_ENf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, bs_sync_en);
    RX_X4_BLKSYNC_CFGr_BS_DIST_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, bs_dist_mode);
    RX_X4_BLKSYNC_CFGr_BS_BTMX_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, bs_btmx_mode);

    PHYMOD_IF_ERR_RETURN 
        (MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));
    /* --Addr=0x9220 ------------------------- */
	  /*
    if((pc->spd_intf ==TEFMOD16_SPD_10000)||
       (pc->spd_intf ==TEFMOD16_SPD_10000_HI)||
       (pc->spd_intf ==TEFMOD16_SPD_10000_DXGXS)||
       (pc->spd_intf ==TEFMOD16_SPD_20000)){
      data = 0x0444;
      mask = 0xffff;
      sync_code_sm_en = 1;
    } else if ((pc->spd_intf ==TEFMOD16_SPD_42G_X4)||
               (pc->spd_intf ==TEFMOD16_SPD_40G_X4)||
               (pc->spd_intf ==TEFMOD16_SPD_25455)||
               (pc->spd_intf ==TEFMOD16_SPD_21G_HI_DXGXS)||
               (pc->spd_intf ==TEFMOD16_SPD_20G_DXGXS)||
               (pc->spd_intf == TEFMOD16_SPD_12773_DXGXS) ) {
      data = 0x0323;
      mask = 0xffff;
      sync_code_sm_en = 1;
    }

    if(sync_code_sm_en) {
        PHYMOD_IF_ERR_RETURN 
        (MODIFY_RX_X1_CONTROL0_DECODE_CONTROL_0r(pc->unit, pc, data, mask));
    }
    */
    if(rx_control0_misc0_en) {
    /* rx_control0_misc0_en is made 0
        RX_X2_MISC0r_t RX_X2_CONTROL0_MISC_0r_reg;
        RX_X2_MISC0r_CLR(RX_X2_CONTROL0_MISC_0r_reg);
        RX_X2_MISC0r_SET(RX_X2_CONTROL0_MISC_0r_reg, );
        PHYMOD_IF_ERR_RETURN(MODIFY_RX_X2_MISC0r(pc, RX_X2_CONTROL0_MISC_0r_reg));
    */
    }
    /* --Addr=0xc131 ------------------------- */
    if((pc->spd_intf == TEFMOD16_SPD_20G_MLD_DXGXS) ||
       (pc->spd_intf == TEFMOD16_SPD_21G_HI_MLD_DXGXS) ||
       (pc->spd_intf == TEFMOD16_SPD_40G_XLAUI) || 
       (pc->spd_intf == TEFMOD16_SPD_42G_XLAUI) ||
       (pc->spd_intf == TEFMOD16_SPD_12773_HI_DXGXS) ) {
      /*data = 0; */
      /*PHYMOD_IF_ERR_RETURN (MODIFY_RX_X4_CONTROL0_FEC_0r(pc->unit, pc, data, mask));*/ 
    }
    /* --Addr=0x9123 ------------------------- */
    /* for simualtion                          */
/* #if defined (_DV_TRIDENT2) */

    #ifdef TSCF_GEN1  
    CL82_RX_AM_TMRr_CLR(CL82_SHARED_CL82_RX_AM_TIMERr_reg);
    #else
    CL82_AM_TMRr_CLR(CL82_SHARED_CL82_AM_TIMERr_reg);
    #endif 

    /*ABHIif((pc->spd_intf == TEFMOD16_SPD_40G_XLAUI)||(pc->spd_intf == TEFMOD_SPD_42G_XLAUI)) {
      data = pc->rx_am_timer_init_val;
      mask = 0xffff;
      CL82_RX_AM_TMRr_SET(CL82_SHARED_CL82_RX_AM_TIMERr_reg, pc->rx_am_timer_init_val);
      PHYMOD_IF_ERR_RETURN 
            (MODIFY_CL82_RX_AM_TMRr(pc, CL82_SHARED_CL82_RX_AM_TIMERr_reg));
    }*/
    if((pc->spd_intf == TEFMOD16_SPD_20G_MLD_DXGXS) ||
       (pc->spd_intf == TEFMOD16_SPD_20G_MLD_X2)    ||
       (pc->spd_intf == TEFMOD16_SPD_21G_MLD_HG_X2) ||
       (pc->spd_intf == TEFMOD16_SPD_40G_MLD_X4)    ||
       (pc->spd_intf == TEFMOD16_SPD_50G_MLD_X4)    ||
       (pc->spd_intf == TEFMOD16_SPD_40G_MLD_X2)    ||
       (pc->spd_intf == TEFMOD16_SPD_42G_MLD_HG_X2) ||
       (pc->spd_intf == TEFMOD16_SPD_50G_MLD_X2)    ||
       (pc->spd_intf == TEFMOD16_SPD_100G_MLD_X4)   ||
       (pc->spd_intf == TEFMOD16_SPD_53G_MLD_HG_X2) ||
       (pc->spd_intf == TEFMOD16_SPD_42G_MLD_HG_X4) ||
       (pc->spd_intf == TEFMOD16_SPD_53G_MLD_HG_X4) ||
       (pc->spd_intf == TEFMOD16_SPD_106G_MLD_HG_X4)||
       (pc->spd_intf == TEFMOD16_SPD_40G_XLAUI)     ||
       (pc->spd_intf == TEFMOD16_SPD_42G_XLAUI))  {
      #ifdef TSCF_GEN1 
      CL82_RX_AM_TMRr_SET(CL82_SHARED_CL82_RX_AM_TIMERr_reg, pc->rx_am_timer_init_val);
      PHYMOD_IF_ERR_RETURN 
            (MODIFY_CL82_RX_AM_TMRr(pc, CL82_SHARED_CL82_RX_AM_TIMERr_reg));
      #else
      CL82_AM_TMRr_SET(CL82_SHARED_CL82_AM_TIMERr_reg, pc->rx_am_timer_init_val);
      PHYMOD_IF_ERR_RETURN 
            (MODIFY_CL82_AM_TMRr(pc, CL82_SHARED_CL82_AM_TIMERr_reg));
      #endif
    }
/* #endif */
    /* --Addr=0x9130 ------------------------- */
    if((pc->spd_intf == TEFMOD16_SPD_20G_MLD_DXGXS)) {
      CL82_LN_0_AM_BYTE10r_CLR(CL82_AMS_LANE_0_AM_BYTE10r_reg);
      CL82_LN_0_AM_BYTE10r_SET(CL82_AMS_LANE_0_AM_BYTE10r_reg, 0x7690);
      PHYMOD_IF_ERR_RETURN 
        (MODIFY_CL82_LN_0_AM_BYTE10r(pc, CL82_AMS_LANE_0_AM_BYTE10r_reg));
    }
    /* --Addr=0x9131 ------------------------- */
    if((pc->spd_intf == TEFMOD16_SPD_20G_MLD_DXGXS)) {
      CL82_LN_1_AM_BYTE10r_CLR(CL82_AMS_LANE_1_AM_BYTE10r_reg);
      CL82_LN_1_AM_BYTE10r_SET(CL82_AMS_LANE_1_AM_BYTE10r_reg, 0xc4f0);
      PHYMOD_IF_ERR_RETURN 
        (MODIFY_CL82_LN_1_AM_BYTE10r(pc, CL82_AMS_LANE_1_AM_BYTE10r_reg));
    }
    /* --Addr=0x9132 ------------------------- */
    if((pc->spd_intf == TEFMOD16_SPD_20G_MLD_DXGXS)) {
      CL82_LANES_1_0_AM_BYTE2r_CLR(CL82_AMS_LANES_1_0_AM_BYTE2r_reg);
      CL82_LANES_1_0_AM_BYTE2r_SET(CL82_AMS_LANES_1_0_AM_BYTE2r_reg, 0xe647);
      PHYMOD_IF_ERR_RETURN 
        (MODIFY_CL82_LANES_1_0_AM_BYTE2r(pc, CL82_AMS_LANES_1_0_AM_BYTE2r_reg));
    }
    
    if(pc->ctrl_type & TEFMOD16_CTRL_TYPE_FAULT_DIS) {
      pc->per_lane_control = TEFMOD16_FAULT_CL49 | TEFMOD_FAULT_CL48 | TEFMOD_FAULT_CL82;
      /* li, lf, rf =0 */
      tefmod16_lf_rf_control(pc); 
    } else {
      pc->per_lane_control = 
         (TEFMOD16_FAULT_RF | TEFMOD_FAULT_LF | TEFMOD_FAULT_LI)<<4 |
         (TEFMOD16_FAULT_CL49 | TEFMOD_FAULT_CL48 | TEFMOD_FAULT_CL82);
       tefmod16_lf_rf_control(pc); 
    }
  } else {  /* if ~cntl */
    /* no op */
    return PHYMOD_E_NONE;
  }
  return PHYMOD_E_NONE;
}
#endif

#ifdef _SDK_TEFMOD16_

/**
@brief   Set the AN Portmode and Single Port for AN of TSCF.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   num_of_lanes Number of lanes in this port
@param   starting_lane first lane in the port
@param   single_port Single port or not
@returns PHYMOD_E_NONE if successful. PHYMOD_E_FAIL else.
@details

This function can be called multiple times.Elements of #PHYMOD_ST should be
initialized to required values prior to calling this function. The following
sub-configurations are performed here.

\li Set pll divider for VCO setting (0x9000[12, 11:8]). pll divider is calculated from max_speed.

*/

int tefmod16_set_an_single_port_mode(PHYMOD_ST* pc, int an_enable)    /* SET_AN_PORT_MODE */
{
  MAIN0_SETUPr_t  reg_setup;
  SC_X4_CTLr_t xgxs_x4_ctrl;
  phymod_access_t phy_copy;


  PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));
  SC_X4_CTLr_CLR(xgxs_x4_ctrl);

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  TEFMOD16_DBG_IN_FUNC_VIN_INFO(pc,("an_enable %d \n", an_enable));

  MAIN0_SETUPr_CLR(reg_setup);
  PHYMOD_IF_ERR_RETURN (READ_MAIN0_SETUPr(pc, &reg_setup));

  /*Ref clock selection tied to 156.25MHz */
  MAIN0_SETUPr_REFCLK_SELf_SET(reg_setup, 0x3);

  /*Setting single port mode*/
  MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(reg_setup, an_enable);
  /*if signle port mode is set and not single port mode, we need
  to disable the sister port */
    if (an_enable) {
        SC_X4_CTLr_SW_SPEED_CHANGEf_SET(xgxs_x4_ctrl, 0);
        phy_copy.lane_mask= 0xf;
        MODIFY_SC_X4_CTLr(&phy_copy, xgxs_x4_ctrl);
    }  else {
        SC_X4_CTLr_SW_SPEED_CHANGEf_SET(xgxs_x4_ctrl, 1);
        phy_copy.lane_mask= 0xf;
        MODIFY_SC_X4_CTLr(pc, xgxs_x4_ctrl);
    }

  PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc,reg_setup));

  return PHYMOD_E_NONE;

}

/**
@brief   Set the AN Portmode and Single Port for AN of TSCF.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   num_of_lanes Number of lanes in this port
@param   starting_lane first lane in the port
@param   single_port Single port or not
@returns PHYMOD_E_NONE if successful. PHYMOD_E_FAIL else.
@details

This function can be called multiple times.Elements of #PHYMOD_ST should be
initialized to required values prior to calling this function. The following
sub-configurations are performed here.

\li Set pll divider for VCO setting (0x9000[12, 11:8]). pll divider is calculated from max_speed. 

*/
int tefmod16_set_an_port_mode(PHYMOD_ST* pc, int num_of_lanes, int starting_lane, int single_port)    /* SET_AN_PORT_MODE */
{
  MAIN0_SETUPr_t  reg_setup;
  SC_X4_CTLr_t xgxs_x4_ctrl;
  phymod_access_t phy_copy;
  
  PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));
  SC_X4_CTLr_CLR(xgxs_x4_ctrl);

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  TEFMOD16_DBG_IN_FUNC_VIN_INFO(pc,("num_of_lanes: %d, starting_lane: %d, single_port: %d", num_of_lanes, starting_lane, single_port));

  MAIN0_SETUPr_CLR(reg_setup);
  PHYMOD_IF_ERR_RETURN (READ_MAIN0_SETUPr(pc, &reg_setup));

  /*Ref clock selection tied to 156.25MHz */
  MAIN0_SETUPr_REFCLK_SELf_SET(reg_setup, 0x3);

  /*Setting single port mode*/
  MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(reg_setup, single_port);
  /*if signle port mode is set and not single port mode, we need
  to disable the sister port */
  if (pc->lane_mask != 0xf)  {
    if (single_port) {
        SC_X4_CTLr_SW_SPEED_CHANGEf_SET(xgxs_x4_ctrl, 0);
        phy_copy.lane_mask= 0xf;
        MODIFY_SC_X4_CTLr(&phy_copy, xgxs_x4_ctrl);
    }  else {
        SC_X4_CTLr_SW_SPEED_CHANGEf_SET(xgxs_x4_ctrl, 1);
        MODIFY_SC_X4_CTLr(pc, xgxs_x4_ctrl);
    }
    /*phy_copy.lane_mask= 0xf;
    MODIFY_SC_X4_CTLr(&phy_copy, xgxs_x4_ctrl); */
  } else {
    if (single_port) {
        SC_X4_CTLr_SW_SPEED_CHANGEf_SET(xgxs_x4_ctrl, 0);
        MODIFY_SC_X4_CTLr(pc, xgxs_x4_ctrl);
    }  else {
        SC_X4_CTLr_SW_SPEED_CHANGEf_SET(xgxs_x4_ctrl, 1);
        MODIFY_SC_X4_CTLr(pc, xgxs_x4_ctrl);
    }
  }

  PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc,reg_setup));

  return PHYMOD_E_NONE;

} /* tefmod16_set_an_port_mode(PHYMOD_ST* pc, int num_of_lanes, int starting_lane, int single_port) */
#endif /* _SDK_TEFMOD16_ */


#ifdef _DV_TB_
/*!
@brief Programs the speed mode for a particular lane
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
         pc->accData nozero if PLL is needed to restarted with different value;
@details
This function sets the TEF into a particular speed mode if per_lane_control=
(1|(aspd<<4)).  If per_lane_control=0, the function tells us PLLdiv is required
to be changed or not by return the new pll value in pc->accData.  If no change
is required, then return 0 in accData.  
If per_lane_control = 2; then pc->accData returns plldiv in the register.
If per_lane_control = 1; set to the desired speed.
 
To set the desired speed, set the following fields of #PHYMOD_ST

\li #PHYMOD_ST::lane_select to the lane number(s)
\li #PHYMOD_ST::spd_intf to a value from enumerated type #tefmod16_spd_intfc_type_t
\li #PHYMOD_ST::port_type to a type from the enumerated type #tefmod16_port_type_t
\li #PHYMOD_ST::os_type to a type from the enumerated type #tefmod16_os_type

Note that legitimate combinations for these fields must be used; otherwise
ill-defined behavior may result.

This function must be called once for combo mode and once per lane for
indepedent lane mode. Only if all lanes are being set to the same
speed-interface can this function be called once for all lanes. Before calling
this function, the PLL sequencer must be disabled via the function
#tefmod16_pll_sequencer_control().  After calling this function, the PLL sequencer
must be enabled via the same function.

To program multiple lanes in independent lane mode, it is recommended to
disable the PLL sequencer, call this function once for every lane being
programmed, and then enable the PLL sequencer.

This function modifies the following registers:

\li force_ln_mode (0xC001[8]), force_speed (0XC001[7:0]), force_oscdr_mode_val (0xC001[14:11])
\li set pcs mode (encodeMode) (0xC110[4:2]) 

The following speeds are not implemented.
\li TEFMOD16_SPD_10000_DXGXS_SCR
\li TEFMOD16_SPD_15000
\li TEFMOD16_SPD_2000
\li TEFMOD16_SPD_6364_SINGLE

When accData return 1, upper layer needs to adjust the plldiv of other
lanes/tefmod to prevent unnecessary PLL restart when other lanes/tefmod is
called.
*/

int tefmod16_set_spd_intf(PHYMOD_ST* pc)                  /* SET_SPD_INTF */
{
  int    plldiv, rv; 
  uint32_t plldiv_r_val;
 
  tefmod16_spd_intfc_type_t spd_intf;
  int    port_mode_sel_restart;
  /* 
     0x0 = DEFAULT
     0x1 = SFP_OPT
     0x2 = SFP_DAC
     0x3 = XLAUI
     0x4 = FORCE_OSDFE
     0x5 = FORCE_BRDFE
     0x6 = SW_CL72
     0x7 = CL72_woAN
     0x8 = SFP_OPT_PF_TEMP_COMP
  */
  /* #define SR4                     0x1
     #define SFI_DAC                 0x2
     #define XLAUI                   0x3
     #define FORCE_OSDFE             0x4
     #define FORCE_BRDFE             0x5
     #define SW_CL72                 0x6
     #define FORCED_MODE_CL72        0x7
     #define FORCE_OS                0xF
  */
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  
#ifdef _DV_TB_
  spd_intf    = pc->spd_intf;
  pc->accAddr = ((pc->unit) << 16) | (pc->port); 
#endif

  if (pc->spd_intf > TEFMOD16_SPD_ILLEGAL) {
    PHYMOD_DEBUG_ERROR(("%-22s ERROR: Bad spd-intf: %d > TEFMOD16_SPD_ILLEGAL\n",
                                                       __func__, pc->spd_intf));
    return PHYMOD_E_PARAM;
  } else {
    if (pc->verbosity ) 
      PHYMOD_DEBUG_ERROR(("%-22s: %s[%d]\n",__func__,e2s_tefmod16_spd_intfc_type_t[pc->spd_intf],
                                       e2n_tefmod16_spd_intfc_type_t[pc->spd_intf]));
  }
  /* pll div calculation; assume refclk=156 */
  plldiv = 66; /* default */
  if (spd_intf == TEFMOD16_SPD_1000_SGMII) {
    if((pc->plldiv==40)||(pc->plldiv==66)) { 
      plldiv = pc->plldiv;
    } else {
      plldiv = 40;  
    }
    plldiv = 40;
    /* speed_mii = IEEE0BLK_MIICNTL_MANUAL_SPEED1_BITS <<
                   IEEE0BLK_MIICNTL_MANUAL_SPEED1_SHIFT; MII_CTRL_SS_1000; */
    /* remove TEFMOD16_SPD_1000_FIBER  */
    /* speed_mii = PMD_IEEE0BLK_PMD_IEEECONTROL1_SPEEDSELECTION1_MASK; */
  } else if (spd_intf == TEFMOD16_SPD_10000_XFI) {
    plldiv = 66;
  } else if (spd_intf == TEFMOD16_SPD_10600_XFI_HG) {
    plldiv = 70;
  } else if (spd_intf == TEFMOD16_SPD_20000_XFI) {
    plldiv = 132;
  } else if (spd_intf == TEFMOD16_SPD_21200_XFI_HG) {
    plldiv = 140;
  } else if (spd_intf == TEFMOD16_SPD_25000_XFI) {
    plldiv = 165;
  } else if (spd_intf == TEFMOD16_SPD_26500_XFI_HG) {
    plldiv = 175;
  } else if (spd_intf == TEFMOD16_SPD_20G_MLD_X2) {
    plldiv = 66;
  } else if (spd_intf == TEFMOD16_SPD_21G_MLD_HG_X2) {
    plldiv = 70;
  } else if (spd_intf == TEFMOD16_SPD_40G_MLD_X2) {
    plldiv = 132;
  } else if (spd_intf == TEFMOD16_SPD_42G_MLD_HG_X2) {
    plldiv = 140;
  } else if (spd_intf == TEFMOD16_SPD_50G_MLD_X2) {
    plldiv = 165;
  } else if (spd_intf == TEFMOD16_SPD_53G_MLD_HG_X2) {
    plldiv = 175;
  } else if (spd_intf == TEFMOD16_SPD_40G_MLD_X4) {
    plldiv = 66;
  } else if (spd_intf == TEFMOD16_SPD_42G_MLD_HG_X4) {
    plldiv = 70;
  } else if (spd_intf == TEFMOD16_SPD_50G_MLD_X4) {
    plldiv = 165;
  } else if (spd_intf == TEFMOD16_SPD_53G_MLD_HG_X4) {
    plldiv = 175;
  } else if (spd_intf == TEFMOD16_SPD_100G_MLD_X4) {
    plldiv = 165;
  } else if (spd_intf == TEFMOD16_SPD_106G_MLD_HG_X4) {
    plldiv = 175;
  }
  rv = PHYMOD_E_NONE;
  
  port_mode_sel_restart = 0;

  if(pc->per_lane_control !=0x2) {
    tefmod16_update_port_mode_select(pc, pc->port_type, pc->this_lane, pc->tsc_clk_freq_pll_by_48, pc->pll_reset_en);
    port_mode_sel_restart = pc->accData;
  }
  pc->accData = 0;
  /* read back plldiv  */
  tefmod16_get_plldiv(pc, &plldiv_r_val);

  if( (pc->plldiv != plldiv_r_val)|| port_mode_sel_restart) {
    if( pc->verbosity )
      PHYMOD_DEBUG_ERROR(("%-22s: p=%0d pc->plldiv:%d mismatched reg plldiv=%0d exp \
      plldiv=%0d mode_restart=%0d\n", __func__, pc->port, pc->plldiv,
      plldiv_r_val, plldiv, port_mode_sel_restart));
    pc->plldiv = 0 ;
  }
  rv = PHYMOD_E_NONE;

  if(pc->per_lane_control == 0 ) {
    PHYMOD_DEBUG_ERROR(("%-22s: p=%0d set_spd_intf probed\n", __func__, pc->port));
    return rv; 
  } else if(pc->per_lane_control == 2) {
    pc->accData = plldiv_r_val;
    PHYMOD_DEBUG_ERROR(("%-22s: p=%0d set_spd_intf get reg plldiv=%0d\n", 
               __func__, pc->port, plldiv_r_val));
    return rv; 
  }
  pc->per_lane_control=1; 
  rv |= tefmod16_credit_set(pc);
  pc->per_lane_control=1; 
  rv |= tefmod16_encode_set(pc, pc->per_lane_control, pc->spd_intf, pc->tx_am_timer_init_val);
  pc->per_lane_control=1; 
  rv |= tefmod16_decode_set(pc);

  return rv;

} /* tefmod16_set_spd_intf(PHYMOD_ST* pc) */
#endif /* _DV_TB_ */

/*!
@brief supports per field override 
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  per_field_override_en Controls which field to override
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details
  TBD
*/
int tefmod16_set_override_0(PHYMOD_ST* pc, int per_field_override_en)   /* OVERRIDE_SET */
{
  int or_value;
  override_type_t or_en;
  RX_X4_PCS_CTL0r_t    RX_X4_CONTROL0_PCS_CONTROL_0r_reg;
  SC_X4_SC_X4_OVRRr_t  SC_X4_CONTROL_LANE_OVERRIDEr_reg; 
  SC_X1_SPD_OVRR0_0r_t SC_X1_SPEED_OVERRIDE0r_reg;
  TX_X4_ENC0r_t        TX_X4_CONTROL0_ENCODE_0r_reg;
  TX_X4_MISCr_t        TX_X4_CONTROL0_MISCr_reg;
  SC_X4_FLD_OVRR_EN0_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg;
  SC_X4_FLD_OVRR_EN1_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  or_en = (override_type_t) (per_field_override_en >> 16);

  or_value = per_field_override_en & 0x0000ffff;
  
  switch(or_en) {
  case OVERRIDE_CLEAR:
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, or_value);
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_NUM_LANES:
    SC_X4_SC_X4_OVRRr_CLR(SC_X4_CONTROL_LANE_OVERRIDEr_reg);
    SC_X4_SC_X4_OVRRr_NUM_LANES_OVERRIDE_VALUEf_SET(SC_X4_CONTROL_LANE_OVERRIDEr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_SC_X4_OVRRr(pc, SC_X4_CONTROL_LANE_OVERRIDEr_reg));
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_NUM_LANES_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_NUM_LANES_DIS:
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_NUM_LANES_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_OS_MODE:
    TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
    TX_X4_MISCr_OS_MODEf_SET(TX_X4_CONTROL0_MISCr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_OS_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_OS_MODE_DIS:
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_OS_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_T_FIFO_MODE:
    SC_X1_SPD_OVRR0_0r_CLR(SC_X1_SPEED_OVERRIDE0r_reg);
    SC_X1_SPD_OVRR0_0r_T_FIFO_MODEf_SET(SC_X1_SPEED_OVERRIDE0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR0_0r(pc, SC_X1_SPEED_OVERRIDE0r_reg));
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_T_FIFO_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_T_FIFO_MODE_DIS:
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_T_FIFO_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_T_ENC_MODE:
    TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);
    TX_X4_ENC0r_T_ENC_MODEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc, TX_X4_CONTROL0_ENCODE_0r_reg));
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_T_ENC_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_T_ENC_MODE_DIS:
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_T_ENC_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_T_HG2_ENABLE:
    TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);
    TX_X4_ENC0r_HG2_ENABLEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc, TX_X4_CONTROL0_ENCODE_0r_reg));
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_T_HG2_ENABLE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_T_HG2_ENABLE_DIS:
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_T_HG2_ENABLE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_T_PMA_BTMX_MODE_OEN:
    TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
    TX_X4_MISCr_T_PMA_BTMX_MODEf_SET(TX_X4_CONTROL0_MISCr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_T_PMA_BTMX_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_T_PMA_BTMX_MODE_OEN_DIS:
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_T_PMA_BTMX_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    /* No missing break. Intentional fall through case */
    /* coverity[unterminated_case] */
    break;
  case OVERRIDE_SCR_MODE:
    TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
    TX_X4_MISCr_SCR_MODEf_SET(TX_X4_CONTROL0_MISCr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_SCR_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_SCR_MODE_DIS:
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_SCR_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_DESCR_MODE_OEN:
    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
    RX_X4_PCS_CTL0r_DESCR_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));
     /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_DESCR_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_DESCR_MODE_OEN_DIS:
     /*Set the override disable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_DESCR_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_DEC_TL_MODE:
    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
    RX_X4_PCS_CTL0r_DEC_TL_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));
     /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_DEC_TL_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_DEC_TL_MODE_DIS:
     /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_DEC_TL_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_DESKEW_MODE:
    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
    RX_X4_PCS_CTL0r_DESKEW_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));
     /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_DESKEW_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_DESKEW_MODE_DIS:
     /*Set the override disable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_DESKEW_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_DEC_FSM_MODE:
    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
    RX_X4_PCS_CTL0r_DEC_FSM_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));
     /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_DEC_FSM_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_DEC_FSM_MODE_DIS:
     /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_DEC_FSM_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  default:
    break;
  }

  return PHYMOD_E_NONE;
}  /* OVERRIDE_SET  */

/*!
@brief TBD
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  per_lane_control
@param  per_field_override_en
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details
  TBD
*/
int tefmod16_set_override_1(PHYMOD_ST* pc, int per_lane_control, uint32_t per_field_override_en)   /* OVERRIDE_SET */
{
  int or_value;
  override_type_t or_en;
  MAIN0_SETUPr_t                     MAIN0_SETUPr_reg;
  RX_X4_BLKSYNC_CFGr_t   RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg;
  TX_X4_MISCr_t             TX_X4_CONTROL0_MISCr_reg;
  TX_X4_CRED0r_t           TX_X4_CREDIT0_CREDIT0r_reg;
  TX_X4_CRED1r_t           TX_X4_CREDIT0_CREDIT1r_reg;
  TX_X4_LOOPCNTr_t           TX_X4_CREDIT0_LOOPCNTr_reg;
  TX_X4_MAC_CREDGENCNTr_t  TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg;
  SC_X4_FLD_OVRR_EN0_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg;
  SC_X4_FLD_OVRR_EN1_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  or_en = (override_type_t) (per_field_override_en >> 16);

  or_value = per_field_override_en & 0x0000ffff;

  
  switch(or_en) {
  case OVERRIDE_R_HG2_ENABLE:
    TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
    TX_X4_MISCr_SET(TX_X4_CONTROL0_MISCr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_R_HG2_ENABLE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_R_HG2_ENABLE_DIS:
    /*Set the override disable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_R_HG2_ENABLE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_BS_SM_SYNC_MODE_OEN:
    RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);
    RX_X4_BLKSYNC_CFGr_BS_SM_SYNC_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_BS_SM_SYNC_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_BS_SM_SYNC_MODE_OEN_DIS:
    /*Set the override disable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_BS_SM_SYNC_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_BS_SYNC_EN_OEN:
    RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);
    RX_X4_BLKSYNC_CFGr_BS_SYNC_ENf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_BS_SYNC_EN_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_BS_SYNC_EN_OEN_DIS:
    /*Set the override disable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_BS_SYNC_EN_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_BS_DIST_MODE_OEN:
    RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);
    RX_X4_BLKSYNC_CFGr_BS_DIST_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_BS_DIST_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_BS_DIST_MODE_OEN_DIS:
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_BS_DIST_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_BS_BTMX_MODE_OEN:
    RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);
    RX_X4_BLKSYNC_CFGr_BS_BTMX_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_BS_BTMX_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_BS_BTMX_MODE_OEN_DIS:
    /*Set the override disable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_BS_BTMX_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_CL72_EN:
    MAIN0_SETUPr_CLR(MAIN0_SETUPr_reg);
    MAIN0_SETUPr_CL72_ENf_SET(MAIN0_SETUPr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc, MAIN0_SETUPr_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_CL72_EN_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_CL72_EN_DIS:
    /*Set the override disable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_CL72_EN_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
    break;
  case OVERRIDE_CLOCKCNT0:
    TX_X4_CRED0r_CLR(TX_X4_CREDIT0_CREDIT0r_reg);
    TX_X4_CRED0r_CLOCKCNT0f_SET(TX_X4_CREDIT0_CREDIT0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED0r(pc, TX_X4_CREDIT0_CREDIT0r_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT0_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_CLOCKCNT0_DIS:
    /*Set the override disable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT0_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    
    break;
  case OVERRIDE_CLOCKCNT1:
    TX_X4_CRED1r_CLR(TX_X4_CREDIT0_CREDIT1r_reg);
    TX_X4_CRED1r_CLOCKCNT1f_SET(TX_X4_CREDIT0_CREDIT1r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED1r(pc, TX_X4_CREDIT0_CREDIT1r_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT1_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_CLOCKCNT1_DIS:
    /*Set the override disable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT1_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));

    break;
  case OVERRIDE_LOOPCNT0:
    TX_X4_LOOPCNTr_CLR(TX_X4_CREDIT0_LOOPCNTr_reg);
    TX_X4_LOOPCNTr_LOOPCNT0f_SET(TX_X4_CREDIT0_LOOPCNTr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_LOOPCNTr(pc, TX_X4_CREDIT0_LOOPCNTr_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT0_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_LOOPCNT0_DIS:
    /*Set the override disable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT0_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));

    break;
  case OVERRIDE_LOOPCNT1:
    TX_X4_LOOPCNTr_CLR(TX_X4_CREDIT0_LOOPCNTr_reg);
    TX_X4_LOOPCNTr_LOOPCNT1f_SET(TX_X4_CREDIT0_LOOPCNTr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_LOOPCNTr(pc, TX_X4_CREDIT0_LOOPCNTr_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT1_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break; 
  case OVERRIDE_LOOPCNT1_DIS:
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT1_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));

    break;
  case OVERRIDE_MAC_CREDITGENCNT: 
    TX_X4_MAC_CREDGENCNTr_CLR(TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg);
    TX_X4_MAC_CREDGENCNTr_MAC_CREDITGENCNTf_SET(TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MAC_CREDGENCNTr(pc, TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_MAC_CREDITGENCNT_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  case OVERRIDE_MAC_CREDITGENCNT_DIS:
    /*Set the override disable*/
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_MAC_CREDITGENCNT_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
    break;
  default:
    break;
  }
  return PHYMOD_E_NONE;
}  /* OVERRIDE_SET  */

/*!
@brief Enable / Disable credits for any particular lane
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  per_lane_control Control to enable or disable credits
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This function is used to enable or disable the credit generation.

\li set to 0x1 to enable credit
\li Set to 0x0 disable credit for selected lane.

This function reads/modifies the following registers:

\li enable credits: clockcnt0 (0xC100[14])
*/
int tefmod16_credit_control(PHYMOD_ST* pc, int per_lane_control)         /* CREDIT_CONTROL */
{
  int cntl;
  TX_X4_CRED0r_t TX_X4_CREDIT0_CREDIT0r_reg;
  cntl = per_lane_control;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  TX_X4_CRED0r_CLR(TX_X4_CREDIT0_CREDIT0r_reg);
  if (cntl) { /* enable credits */
    TX_X4_CRED0r_CREDITENABLEf_SET(TX_X4_CREDIT0_CREDIT0r_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED0r(pc, TX_X4_CREDIT0_CREDIT0r_reg));
   } else { /* disable credits */
    TX_X4_CRED0r_CREDITENABLEf_SET(TX_X4_CREDIT0_CREDIT0r_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED0r(pc, TX_X4_CREDIT0_CREDIT0r_reg));
  }
  return PHYMOD_E_NONE;
}

/*!
@brief tx lane reset and enable of any particular lane
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  en controls enabling properties of  lane  control
@param  dis controls what property to control enum #tefmod16_tx_disable_enum_t
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This function is used to reset tx lane and enable/disable tx lane of any
transmit lane.  Set bit 0 of per_lane_control to enable the TX_LANE
(1) or disable the TX lane (0).
When diable TX lane, two types of operations are inovked independently.  
If per_lane_control bit [7:4] = 1, only traffic is disabled. 
      (TEFMOD16_TX_LANE_TRAFFIC =0x10)
If bit [7:4] = 2, only reset function is invoked.
      (TEFMOD16_TX_LANE_RESET = 0x20)

This function reads/modifies the following registers:

\li rstb_tx_lane  :   0xc113[1]
\li enable_tx_lane:   0xc113[0]
*/


/*!
@brief tx lane reset and enable of any particular lane
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  en controls enabling properties of  lane  control
@param  dis controls what property to control enum #tefmod16_tx_disable_enum_t
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This function is used to reset tx lane and enable/disable tx lane of any
transmit lane.  Set bit 0 of per_lane_control to enable the TX_LANE
(1) or disable the TX lane (0).
When diable TX lane, two types of operations are inovked independently.
If per_lane_control bit [7:4] = 1, only traffic is disabled.
      (TEFMOD16_TX_LANE_TRAFFIC =0x10)
If bit [7:4] = 2, only reset function is invoked.
      (TEFMOD16_TX_LANE_RESET = 0x20)

This function reads/modifies the following registers:

\li rstb_tx_lane  :   0xc113[1]
\li enable_tx_lane:   0xc113[0]
*/
/* TBD: Use this tier1 for DV, instead of  tefmod16_tx_lane_control */
int tefmod16_tx_lane_control_set(PHYMOD_ST* pc,  tefmod16_tx_disable_enum_t tx_dis_type)         /* TX_LANE_CONTROL */
{
  TX_X4_MISCr_t TX_X4_CONTROL0_MISCr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);

  switch(tx_dis_type) {
    case TEFMOD16_TX_LANE_RESET:
      TX_X4_MISCr_RSTB_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 0);
      PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
      TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
      TX_X4_MISCr_RSTB_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 1);
      PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
    break;
    case TEFMOD16_TX_LANE_TRAFFIC_ENABLE:
      TX_X4_MISCr_ENABLE_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 1);
      PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
    break;
    case TEFMOD16_TX_LANE_TRAFFIC_DISABLE:
      TX_X4_MISCr_ENABLE_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 0);
      PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
    break;
    case TEFMOD16_TX_LANE_RESET_TRAFFIC_ENABLE:
      TX_X4_MISCr_RSTB_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 1);
      PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
      TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
      TX_X4_MISCr_ENABLE_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 1);
      PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
    break;
    case TEFMOD16_TX_LANE_RESET_TRAFFIC_DISABLE:
      TX_X4_MISCr_RSTB_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 0);
      PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
      TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
      TX_X4_MISCr_ENABLE_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 0);
      PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
    break;
    default:
    break;
  }

  return PHYMOD_E_NONE;
}


/*!
@brief tx lane reset and enable of any particular lane
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  reset gets the configured tx rstb
@param  enable gets the configured tx enable
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This function reads the following registers:

\li rstb_tx_lane  :   0xc113[1]
\li enable_tx_lane:   0xc113[0]
*/
int tefmod16_tx_lane_control_get(PHYMOD_ST* pc,  int *reset, int *enable)         /* TX_LANE_CONTROL */
{
  TX_X4_MISCr_t TX_X4_CONTROL0_MISCr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);

  PHYMOD_IF_ERR_RETURN (READ_TX_X4_MISCr(pc, &TX_X4_CONTROL0_MISCr_reg));

  *reset = TX_X4_MISCr_RSTB_TX_LANEf_GET(TX_X4_CONTROL0_MISCr_reg);
  *enable = TX_X4_MISCr_ENABLE_TX_LANEf_GET(TX_X4_CONTROL0_MISCr_reg);

   return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/*!
@brief rx lane reset and enable of any particular lane
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  accData TBD
@param  per_lane_control TBD
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This function enables/disables rx lane (RSTB_LANE) or read back control bit for
that based on per_lane_control being 1 or 0. If per_lane_control is 0xa, only
read back RSTB_LANE.
\li select encodeMode
This function reads/modifies the following registers:
\li rstb_rx_lane  :   0xc137[0]
*/

int tefmod16_rx_lane_control(PHYMOD_ST* pc, int accData, int per_lane_control)         /* RX_LANE_CONTROL */
{
  int cntl;
  RX_X4_PMA_CTL0r_t RX_X4_CONTROL0_PMA_CONTROL_0r_reg;
  /* coverity[param_set_but_not_used]*/ 
  accData = accData; 

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  cntl = per_lane_control;

    PHYMOD_DEBUG_ERROR(("%-22s u=%0d p=%0d sel=%x ln=%0d dxgxs=%0d cntl=%x\n", __func__,
           pc->unit, pc->port, pc->lane_select, pc->this_lane, pc->dxgxs, cntl));

  RX_X4_PMA_CTL0r_CLR(RX_X4_CONTROL0_PMA_CONTROL_0r_reg);
  if (cntl==0xa) {
    /* set encoder */
    PHYMOD_IF_ERR_RETURN(READ_RX_X4_PMA_CTL0r(pc, &RX_X4_CONTROL0_PMA_CONTROL_0r_reg));
    if(RX_X4_PMA_CTL0r_RSTB_LANEf_GET(RX_X4_CONTROL0_PMA_CONTROL_0r_reg))
      accData =1;
    else
      accData =0;
  } else if (cntl) {
    /* set encoder */
    RX_X4_PMA_CTL0r_RSTB_LANEf_SET(RX_X4_CONTROL0_PMA_CONTROL_0r_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PMA_CTL0r(pc, RX_X4_CONTROL0_PMA_CONTROL_0r_reg));
  } else {
     /* bit set to 0 for disabling RXP */
    RX_X4_PMA_CTL0r_RSTB_LANEf_SET(RX_X4_CONTROL0_PMA_CONTROL_0r_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PMA_CTL0r(pc, RX_X4_CONTROL0_PMA_CONTROL_0r_reg));
  }
  return PHYMOD_E_NONE;
}
#endif /* _DV_TB_ */

#ifdef _DV_TB_
/*!
@brief set broadcast enable and broadcast address

@param  pc handle to current TSCF context (#PHYMOD_ST)

@returns The value PHYMOD_E_NONE upon successful completion.
*/

int tefmod16_bypass_sc(PHYMOD_ST* pc)         /* BYPASS_SC */
{
  uint16_t bypass_en, fild_override_enable0, fild_override_enable1;
  SC_X4_BYPASSr_t SC_X4_CONTROL_BYPASSr_reg;
  SC_X4_FLD_OVRR_EN0_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg;
  SC_X4_FLD_OVRR_EN1_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  bypass_en = 0xf;
  fild_override_enable0 = 0xffff;
  fild_override_enable1 = 0xfff0;

  SC_X4_BYPASSr_CLR(SC_X4_CONTROL_BYPASSr_reg);
  SC_X4_BYPASSr_SC_BYPASSf_SET(SC_X4_CONTROL_BYPASSr_reg, bypass_en);

  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_BYPASSr(pc, SC_X4_CONTROL_BYPASSr_reg));

  SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
  SC_X4_FLD_OVRR_EN0_TYPEr_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, fild_override_enable0);
  PHYMOD_IF_ERR_RETURN 
  (MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));

  SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
  SC_X4_FLD_OVRR_EN1_TYPEr_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, fild_override_enable1);
  PHYMOD_IF_ERR_RETURN 
  (MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
  return PHYMOD_E_NONE;
}
#endif /* _DV_TB_ */

#ifdef _DV_TB_
/*!
@brief Control per lane clause 72 auto tuning function  (training patterns)
@param  pc handle to current TSCF context (#PHYMOD_ST)
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

This function modifies the following registers:

\li DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150 (0x0800_0096)
\li CL72_MISC1_CONTROL                                 (0xc253)

*/
int tefmod16_clause72_control(PHYMOD_ST* pc, int cl72en)                /* CLAUSE_72_CONTROL */
{
  int cntl;
#ifdef _DV_TB_
  int tmp_port_type;
#endif
  int cl72_restart, cl72_enable;
  CL93N72_IT_BASE_R_PMD_CTLr_t CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg;
  SC_X4_CTLr_t SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg;
  CL93N72_UR_CTL0r_t CL93N72_USER_RX_CL93N72UR_CONTROL0r_reg;

  TEFMOD_DBG_IN_FUNC_INFO(pc);
  cntl         = cl72en;
  cl72_restart = cntl & 0x1;
  cl72_enable  = (cntl & 0x2) ? 1 : 0;

#ifdef _DV_TB_
  if(pc->verbosity)
    PHYMOD_DEBUG_ERROR(("%-22s: port %0d cl72_restart %0d cl72_en %0d\n", __func__, pc->this_lane, cl72_restart, cl72_enable));
#endif
  CL93N72_IT_BASE_R_PMD_CTLr_CLR(CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg);

  CL93N72_IT_BASE_R_PMD_CTLr_CL93N72_IEEE_RESTART_TRAININGf_SET(CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg, cl72_restart);
  CL93N72_IT_BASE_R_PMD_CTLr_CL93N72_IEEE_TRAINING_ENABLEf_SET(CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg, cl72_enable);

  PHYMOD_IF_ERR_RETURN(MODIFY_CL93N72_IT_BASE_R_PMD_CTLr(pc, CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg));


  CL93N72_UR_CTL0r_CLR(CL93N72_USER_RX_CL93N72UR_CONTROL0r_reg);
  PHYMOD_IF_ERR_RETURN(READ_CL93N72_UR_CTL0r(pc, &CL93N72_USER_RX_CL93N72UR_CONTROL0r_reg));

  CL93N72_UR_CTL0r_CLR(CL93N72_USER_RX_CL93N72UR_CONTROL0r_reg);
  PHYMOD_IF_ERR_RETURN(READ_CL93N72_UR_CTL0r(pc, &CL93N72_USER_RX_CL93N72UR_CONTROL0r_reg));


  CL93N72_UR_CTL0r_CL93N72_RX_TRAINING_ENf_SET(CL93N72_USER_RX_CL93N72UR_CONTROL0r_reg, 1);
  PHYMOD_IF_ERR_RETURN(WRITE_CL93N72_UR_CTL0r(pc, CL93N72_USER_RX_CL93N72UR_CONTROL0r_reg));

  if(cl72_restart) {
#ifdef _DV_TB_
    tmp_port_type = port_type;
    port_type = TEFMOD_MULTI_PORT;
#endif

    SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);
    SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

    SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);
    SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

#ifdef _DV_TB_
    port_type = tmp_port_type;
#endif
  }
  return PHYMOD_E_NONE;
} /* CLAUSE_72_CONTROL */
#endif /* _DV_TB_ */

#ifdef _SDK_TEFMOD16_
int tefmod16_clause72_control(PHYMOD_ST* pc, int cl72en)                /* CLAUSE_72_CONTROL */
{
  int cl72_enable;
  SC_X4_CTLr_t                      SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg;
  CL93N72_IT_BASE_R_PMD_CTLr_t      CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg;
  int start_lane = 0, num_lane = 0 , i=0;
  uint32_t enable = 0;
  phymod_access_t pa_copy;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  cl72_enable  = cl72en & 0x1;

/* need to figure out what's the starting lane */
  PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
  PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
  pa_copy.lane_mask = 0x1 << start_lane;


  CL93N72_IT_BASE_R_PMD_CTLr_CLR(CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg);
  PHYMOD_IF_ERR_RETURN(READ_CL93N72_IT_BASE_R_PMD_CTLr(&pa_copy, &CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg));

  if(CL93N72_IT_BASE_R_PMD_CTLr_CL93N72_IEEE_TRAINING_ENABLEf_GET(CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg) != (uint32_t)cl72_enable) {

    for (i = (num_lane-1); i >= 0; i--) {
      pa_copy.lane_mask = 0x1 << (i + start_lane);
      CL93N72_IT_BASE_R_PMD_CTLr_CLR(CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg);
      PHYMOD_IF_ERR_RETURN(READ_CL93N72_IT_BASE_R_PMD_CTLr(&pa_copy, &CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg));
      CL93N72_IT_BASE_R_PMD_CTLr_CL93N72_IEEE_TRAINING_ENABLEf_SET(CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg, cl72_enable);

      PHYMOD_IF_ERR_RETURN(MODIFY_CL93N72_IT_BASE_R_PMD_CTLr(&pa_copy, CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg));
    }

    pa_copy.lane_mask = 0x1 << start_lane;
    /* release pcs reset */
    tefmod16_enable_get(&pa_copy, &enable);
    if(enable) {
      SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);
      SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, 0);
      PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

      SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);
      SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, 1);
      PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(&pa_copy, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));
    }
}

  return PHYMOD_E_NONE;
} /* CLAUSE_72_CONTROL */
#endif /* _SDK_TEFMOD16_ */


/*!
@brief Sets CJPAT/CRPAT parameters for a particular port
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details
This function enables either a CJPAT or CRPAT for a particular port. 
*/

int tefmod16_cjpat_crpat_control(PHYMOD_ST* pc)     /* CJPAT_CRPAT_CONTROL  */
{
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  /* needed only for TSCF-B0 */
  return PHYMOD_E_NONE;
}

/*!
@brief Checks CJPAT/CRPAT parameters for a particular port
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details
This function checks for CJPAT or CRPAT for a particular port. 
*/

int tefmod16_cjpat_crpat_check(PHYMOD_ST* pc)     /* CJPAT_CRPAT_CHECK */
{
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  /* needed only for TSCF-B0 */
  return PHYMOD_E_NONE;  
}
/*!
@brief  Enables or disables the bit error rate testing for a particular lane.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details

This function enables or disables bit-error-rate testing (BERT)

TBD
*/
int tefmod16_tx_bert_control(PHYMOD_ST* pc)         /* TX_BERT_CONTROL  */
{
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  /* needed only for TSCF-B0 */
  return PHYMOD_E_NONE;
}

/* !
@brief Allows user to set fec_override bit for disabling FEC in 100G AN
@param  pc        handle to current TSCF context (#PHYMOD_ST)
@param enable     bit to enable (1) or disable (0) fec override bit

@returns PHYMOD_E_NONE if no errors. PHYMOD_E_ERROR else

@details
CL91 is ON in  100G AN mode by default.To disable  cl91 in 100G the fec 
override bit need to be set to 1 by user before turning on AN

@Note
This bit must be set to 0 for all the cases except the above mentioned.
 */

int tefmod16_fec_override_set(PHYMOD_ST* pc, uint32_t enable) 
{
    SC_X4_SC_X4_OVRRr_t sc_x4_sc_x4_overr_reg;

    SC_X4_SC_X4_OVRRr_CLR(sc_x4_sc_x4_overr_reg);
    SC_X4_SC_X4_OVRRr_AN_FEC_SEL_OVERRIDEf_SET(sc_x4_sc_x4_overr_reg, enable);
    PHYMOD_IF_ERR_RETURN
        (MODIFY_SC_X4_SC_X4_OVRRr(pc, sc_x4_sc_x4_overr_reg));

    return PHYMOD_E_NONE;  
}


int tefmod16_fec_override_get(PHYMOD_ST* pc, uint32_t *enable) 
{
    SC_X4_SC_X4_OVRRr_t sc_x4_sc_x4_overr_reg;
    
    PHYMOD_IF_ERR_RETURN
        (READ_SC_X4_SC_X4_OVRRr(pc, &sc_x4_sc_x4_overr_reg));

    *enable = SC_X4_SC_X4_OVRRr_AN_FEC_SEL_OVERRIDEf_GET(sc_x4_sc_x4_overr_reg);
    
    return PHYMOD_E_NONE;  

}

/*!
@brief enables/disables FEC function.
@brief Controls PMD reset pins irrespective of PCS is in Speed Control mode or not
@param  pc        handle to current TSCF context (#PHYMOD_ST)
@param  fec_en    Bit encoded enable for various fec feature
                     TEFMOD16_CL91_TX_EN_DIS       Enable CL91 TX
                     TEFMOD16_CL91_RX_EN_DIS       Enable CL91 RX
                     TEFMOD16_CL91_IND_ONLY_EN_DIS Enable CL91 indication only
                     TEFMOD16_CL91_COR_ONLY_EN_DIS Enable CL91 correction only
                     TEFMOD16_CL74_TX_EN_DIS       Enable CL74 TX
                     TEFMOD16_CL74_RX_EN_DIS       Enable CL74 RX

@param  fec_dis   Bit encoded disable for various fec feature
                     TEFMOD16_CL91_TX_EN_DIS       Disable CL91 TX
                     TEFMOD16_CL91_RX_EN_DIS       Disable CL91 RX
                     TEFMOD16_CL91_IND_ONLY_EN_DIS Disable CL91 indication only
                     TEFMOD16_CL91_COR_ONLY_EN_DIS Disable CL91 correction only
                     TEFMOD16_CL74_TX_EN_DIS       Disable CL74 TX
                     TEFMOD16_CL74_RX_EN_DIS       Disable CL74 RX

@param  cl74or91  make 100G speed use cl74 after autoneg
                              bit 1-0 cl74or91 value

@returns PHYMOD_E_NONE if no errors. PHYMOD_E_ERROR else.

@details
To use cl91 in forced speed mode (in 100G only) Tx only fec_en=TEFMOD16_CL91_TX_EN_DIS
To use cl91 in forced speed mode (in 100G only) Rx only fec_en=TEFMOD16_CL91_RX_EN_DIS
To use cl91 in forced speed mode (in 100G only) Tx Rx   fec_en=(TEFMOD16_CL91_TX_EN_DIS|TEFMOD16_CL91_RX_EN_DIS)

To enable cl74 Tx only fec_en = TEFMOD16_CL74_TX_EN_DIS
To disable cl74 Tx Rx fec_dis =(TEFMOD16_CL74_TX_EN_DIS|TEFMOD16_CL74_RX_EN_DIS)

Note: cl74 will be enabled per port.
      cl91 is used only in 100G (so all 4 lanes make a port)

      cl74or91 is only used in autoneg. And other parm used in forced speed.
*/
int tefmod16_FEC_control(PHYMOD_ST* pc, tefmod16_fec_type_t fec_type, int enable, int cl74or91)
{
  phymod_access_t pa_copy;
  int start_lane = 0, num_lane = 0 ;
  uint32_t local_enable = 0;
  tefmod16_cl91_fec_mode_t fec_mode = TEFMOD16_CL91_COUNT;
  tefmod16_cl91_fec_mode_t current_fec_mode = TEFMOD16_CL91_COUNT;
  TX_X4_TX_CTL0r_t TX_X4_TX_CTL0r_reg;
  RX_X4_PCS_CTL0r_t RX_X4_PCS_CTL0r_reg;
  TX_X4_MISCr_t TX_X4_MISCr_reg;
  SC_X4_CTLr_t SC_X4_CTL_reg;
  int speed_id;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
  PHYMOD_IF_ERR_RETURN
     (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
  pa_copy.lane_mask = 0x1 << start_lane;

  TX_X4_TX_CTL0r_CLR(TX_X4_TX_CTL0r_reg);
  RX_X4_PCS_CTL0r_CLR(RX_X4_PCS_CTL0r_reg);
  TX_X4_MISCr_CLR(TX_X4_MISCr_reg);
  SC_X4_CTLr_CLR(SC_X4_CTL_reg);

  PHYMOD_IF_ERR_RETURN
      (READ_TX_X4_TX_CTL0r(pc, &TX_X4_TX_CTL0r_reg));
  PHYMOD_IF_ERR_RETURN
      (READ_RX_X4_PCS_CTL0r(pc, &RX_X4_PCS_CTL0r_reg));
  PHYMOD_IF_ERR_RETURN
      (READ_TX_X4_MISCr(pc, &TX_X4_MISCr_reg));
  PHYMOD_IF_ERR_RETURN
      (READ_SC_X4_CTLr(pc, &SC_X4_CTL_reg));

  current_fec_mode = TX_X4_TX_CTL0r_CL91_FEC_MODEf_GET(TX_X4_TX_CTL0r_reg) ;

  if(enable < TEFMOD16_PHY_CONTROL_FEC_AUTO) {
    if (fec_type == TEFMOD16_CL91) {
        if (enable) {
            switch (num_lane) {
                case 1:
                    fec_mode = TEFMOD16_CL91_SINGLE_LANE_BRCM_PROP;
                    break;
                case 2:
                    fec_mode = TEFMOD16_CL91_DUAL_LANE_BRCM_PROP;
                    break;
                case 4:
                    fec_mode = TEFMOD16_CL91_QUAD_LANE;
                    break;
                default:
                    fec_mode = TEFMOD16_CL91_SINGLE_LANE_BRCM_PROP;
                    break;
            }
        } else {
            if (current_fec_mode == TEFMOD16_IEEE_25G_CL91_SINGLE_LANE) {
                return PHYMOD_E_INTERNAL;
            } else {
                fec_mode = TEFMOD16_NO_CL91_FEC;
            }
        }

        /*In Broadcom 25G FEC or in no FEC mode, speed id is S_25G_X1 (0x12)*/
        if (num_lane == 1) {
            if((fec_mode == TEFMOD16_CL91_SINGLE_LANE_BRCM_PROP)
                || (fec_mode == TEFMOD16_NO_CL91_FEC))
            {
                tefmod16_speed_id_get(pc, &speed_id);
                /*If current speed id is IEEE 25G, it need to be recoverd to Broadcom 25G*/
                if (speed_id == sc_x4_control_sc_S_25G_KR_IEEE) {
                    speed_id = sc_x4_control_sc_S_25G_X1;
                    SC_X4_CTLr_SPEEDf_SET(SC_X4_CTL_reg, speed_id);
                    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CTL_reg));
                }
            }
        }

        TX_X4_TX_CTL0r_CL91_FEC_MODEf_SET(TX_X4_TX_CTL0r_reg, fec_mode);
        RX_X4_PCS_CTL0r_CL91_FEC_MODEf_SET(RX_X4_PCS_CTL0r_reg, fec_mode);
        PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_TX_CTL0r(pc, TX_X4_TX_CTL0r_reg));
        PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_PCS_CTL0r_reg));

    } else if (fec_type == TEFMOD16_CL108) {
        int current_speed_id;

        if (num_lane != 1) {
            return PHYMOD_E_INTERNAL;
        }

        if (enable) {
            fec_mode = TEFMOD16_IEEE_25G_CL91_SINGLE_LANE;
        } else {
            if (current_fec_mode != TEFMOD16_IEEE_25G_CL91_SINGLE_LANE) {
                return PHYMOD_E_INTERNAL;
            } else {
                fec_mode = TEFMOD16_NO_CL91_FEC;
            }
        }

        tefmod16_speed_id_get(pc, &current_speed_id);
        speed_id = current_speed_id;

        if ((current_speed_id == sc_x4_control_sc_S_25G_KR_IEEE)
            || (current_speed_id == sc_x4_control_sc_S_25G_X1)) {
            if (fec_mode == TEFMOD16_NO_CL91_FEC) {
                speed_id = sc_x4_control_sc_S_25G_X1;
            } else if (fec_mode == TEFMOD16_IEEE_25G_CL91_SINGLE_LANE) {
                speed_id = sc_x4_control_sc_S_25G_KR_IEEE;
            }

            if (speed_id != current_speed_id) {
                SC_X4_CTLr_SPEEDf_SET(SC_X4_CTL_reg, speed_id);
                PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CTL_reg));
            }
        }

        TX_X4_TX_CTL0r_CL91_FEC_MODEf_SET(TX_X4_TX_CTL0r_reg, fec_mode);
        RX_X4_PCS_CTL0r_CL91_FEC_MODEf_SET(RX_X4_PCS_CTL0r_reg, fec_mode);
        PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_TX_CTL0r(pc, TX_X4_TX_CTL0r_reg));
        PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_PCS_CTL0r_reg));
    } else {/* cl74 */
     if (enable) {
        TX_X4_MISCr_FEC_ENABLEf_SET(TX_X4_MISCr_reg, 1);
        RX_X4_PCS_CTL0r_FEC_ENABLEf_SET(RX_X4_PCS_CTL0r_reg, 1);
     } else {
        TX_X4_MISCr_FEC_ENABLEf_SET(TX_X4_MISCr_reg, 0);
        RX_X4_PCS_CTL0r_FEC_ENABLEf_SET(RX_X4_PCS_CTL0r_reg, 0);
     }
     PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MISCr(pc, TX_X4_MISCr_reg));
     PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_PCS_CTL0r_reg));
    }
  }

  tefmod16_enable_get(&pa_copy, &local_enable);
  if (local_enable) {
    SC_X4_CTLr_t SC_X4_CTLr_reg;
    SC_X4_CTLr_CLR(SC_X4_CTLr_reg);
    SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CTLr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(&pa_copy, SC_X4_CTLr_reg));

    SC_X4_CTLr_CLR(SC_X4_CTLr_reg);
    SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CTLr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(&pa_copy, SC_X4_CTLr_reg));
  }

  return PHYMOD_E_NONE;
}

int tefmod16_FEC_get(PHYMOD_ST* pc,  tefmod16_fec_type_t fec_type, int *fec_en)
{
  uint32_t fec_mode ;
  int speed_id;
  int rev_letter;
  SC_X4_FEC_STSr_t SC_X4_FINAL_CONFIG_STATUS_FEC_STATUSr_reg;
  MAIN0_SERDESIDr_t serdesid;

  *fec_en = 0;

  SC_X4_FEC_STSr_CLR(SC_X4_FINAL_CONFIG_STATUS_FEC_STATUSr_reg);
  READ_SC_X4_FEC_STSr(pc, &SC_X4_FINAL_CONFIG_STATUS_FEC_STATUSr_reg);

  if ((fec_type == TEFMOD16_CL91) || (fec_type == TEFMOD16_CL108)) {
      fec_mode = SC_X4_FEC_STSr_T_CL91_FEC_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_FEC_STATUSr_reg);

      switch (fec_mode) {
      case TEFMOD16_NO_CL91_FEC:
          *fec_en = 0;
          break;
      case TEFMOD16_CL91_SINGLE_LANE_BRCM_PROP:
          READ_MAIN0_SERDESIDr(pc, &serdesid);
          rev_letter = MAIN0_SERDESIDr_REV_LETTERf_GET(serdesid);

          /* revision A can't distinguish CL91 and CL108 FEC, so check speed ID */
          if (rev_letter == REV_A) {
              PHYMOD_IF_ERR_RETURN(tefmod16_speed_id_get(pc, &speed_id));
              if (speed_id == sc_x4_control_sc_S_25G_KR_IEEE || speed_id == sc_x4_control_sc_S_25G_CR_IEEE) {
                  *fec_en = (fec_type == TEFMOD16_CL108 ? 1 : 0);
              } else {
                 *fec_en = (fec_type == TEFMOD16_CL91 ? 1 : 0);
              }
          } else {
              *fec_en = (fec_type == TEFMOD16_CL91 ? 1 : 0);
          }
          break;
      case TEFMOD16_CL91_SINGLE_LANE_FC:
      case TEFMOD16_CL91_DUAL_LANE_BRCM_PROP:
      case TEFMOD16_CL91_QUAD_LANE:
          *fec_en = (fec_type == TEFMOD16_CL91 ? 1 : 0);
          break;
      case TEFMOD16_IEEE_25G_CL91_SINGLE_LANE:
          *fec_en = (fec_type == TEFMOD16_CL108 ? 1 : 0);
          break;
      default:
          break;
      }
  } else {
      *fec_en = SC_X4_FEC_STSr_T_FEC_ENABLEf_GET(SC_X4_FINAL_CONFIG_STATUS_FEC_STATUSr_reg);
  }

  return PHYMOD_E_NONE;
}

int tefmod16_cl74_fec_control(PHYMOD_ST *pc, int cl74_en) /*CL74_FEC_CONTROL */
{
  TX_X4_MISCr_t     TX_X4_MISCr_reg;
  RX_X4_PCS_CTL0r_t RX_X4_PCS_CTL0r_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  TX_X4_MISCr_CLR(TX_X4_MISCr_reg);
  TX_X4_MISCr_FEC_ENABLEf_SET(TX_X4_MISCr_reg, cl74_en);
  PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MISCr(pc, TX_X4_MISCr_reg));

  RX_X4_PCS_CTL0r_CLR(RX_X4_PCS_CTL0r_reg);
  RX_X4_PCS_CTL0r_FEC_ENABLEf_SET(RX_X4_PCS_CTL0r_reg, cl74_en);
  PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_PCS_CTL0r_reg));
 
  return PHYMOD_E_NONE;

}

/*!
@brief Power down transmitter or receiver per lane basis.
@param  pc handle to current TSCF context (a.k.a #PHYMOD_ST)
@param tx control for power of TX path
@param rx control for power of RX path
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details
Indi
*/
int tefmod16_power_control(PHYMOD_ST* pc, int tx, int rx)
{
  PMD_X4_CTLr_t PMD_X4_PMD_X4_CONTROLr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);

  PMD_X4_CTLr_LN_RX_H_PWRDNf_SET(PMD_X4_PMD_X4_CONTROLr_reg, rx);
  PMD_X4_CTLr_LN_TX_H_PWRDNf_SET(PMD_X4_PMD_X4_CONTROLr_reg, tx);
  PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));
  return PHYMOD_E_NONE;
}

/*!
@brief sets the lanes in Full Duplex/ half duplex mode.
@param  pc handle to current TSCF context (a.k.a tefmod struct)
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details
This is bit encoded function. Bit 3 to 0 is for lane 3 to 0. High sets full
duplex. Low is for half duplex.

<B>Currently, this function is not implemented</B>
*/
int tefmod16_duplex_control(PHYMOD_ST* pc)
{
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  /* needed for B0 */
  return PHYMOD_E_NONE;
}

/*!
@brief Set reference clock
@param  pc handle to current TSCF context (a.k.a tefmod struct)
@param  ref_clk The reference clock to set the PHY to.
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details
The reference clock is set to inform the micro. The value written into the
register is interpreted by the micro as thus.

    refclk = regVal * 2 + 100;

Since the refclk field is 7 bits, the range is 100 - 228

*/

int tefmod16_refclk_get(PHYMOD_ST* pc, phymod_ref_clk_t* ref_clk)
{
  MAIN0_SETUPr_t MAIN0_SETUPr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  MAIN0_SETUPr_CLR(MAIN0_SETUPr_reg);
  READ_MAIN0_SETUPr(pc, &MAIN0_SETUPr_reg);
  *ref_clk = MAIN0_SETUPr_REFCLK_SELf_GET(MAIN0_SETUPr_reg);

  return PHYMOD_E_NONE;
}

/*!
@brief disables the PCS by clearing the writing to Speed control reg
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details
TBD
*/

#ifdef _DV_TB_
int tefmod16_disable_pcs_falcon(PHYMOD_ST* pc)
{
  SC_X4_CTLr_t SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, 0);

  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

  return PHYMOD_E_NONE;
}
#endif /* _DV_TB_*/

/*!
@brief sets PMD TX lane swap values for all lanes simultaneously.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  per_lane_control controls the lane feature
@returns The value PHYMOD_E_NONE upon successful completion
@details
This function sets the TX lane swap values for all lanes simultaneously.

The TSCF has several two sets of lane swap registers per lane.  This function
uses the lane swap registers closest to the pads.  For TX, the lane swap occurs
after all other analog/digital functionality.  For RX, the lane swap occurs
prior to all other analog/digital functionality.

Swap must be the first thing to do after reset/PLL configuration.

How the swap is done is indicated in the per_lane_control argument.
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

This function modifies the following registers:

\li MAIN0_LN_SWP (0x9003)
*/
int tefmod16_pmd_addr_lane_swap(PHYMOD_ST *pc, int per_lane_control)
{

  return PHYMOD_E_NONE;
}

/*!
@brief sets PMD TX lane swap values for all lanes simultaneously.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  per_lane_control controls the lane swap 
@returns The value PHYMOD_E_NONE upon successful completion
@details
This function sets the TX lane swap values for all lanes simultaneously.

The TSCF has several two sets of lane swap registers per lane.  This function
uses the lane swap registers closest to the pads.  For TX, the lane swap occurs
after all other analog/digital functionality.  For RX, the lane swap occurs
prior to all other analog/digital functionality.

It is not allowed to swap lanes without going through reset.

How the swap is done is indicated in the per_lane_control field;
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

This function modifies the following registers:

\li MAIN0_LN_SWP (0x9003)
*/
#if 0 
int tefmod16_pmd_lane_swap_tx(PHYMOD_ST *pc, int per_lane_control)
{
  unsigned int map;
  unsigned int tx_lane_map_0, tx_lane_map_1, tx_lane_map_2, tx_lane_map_3;
  DIG_TX_LN_MAP_0_1_2r_t DIG_COM_TX_LANE_MAP_0_1_2r_reg;
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_t DIG_COM_TX_LANE_MAP_3_N_LANE_ADDR_0_1r_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  map = (unsigned int)per_lane_control;
  tx_lane_map_0 = (map >> 0) & 0xf;
  tx_lane_map_1 = (map >> 4) & 0xf;
  tx_lane_map_2 = (map >> 8) & 0xf;
  tx_lane_map_3 = (map >> 12)& 0xf;

#ifdef _DV_TB_
  pc->adjust_port_mode = 1;
#endif
  DIG_TX_LN_MAP_0_1_2r_CLR(DIG_COM_TX_LANE_MAP_0_1_2r_reg);

  DIG_TX_LN_MAP_0_1_2r_TX_LANE_MAP_2f_SET(DIG_COM_TX_LANE_MAP_0_1_2r_reg, tx_lane_map_2);
  DIG_TX_LN_MAP_0_1_2r_TX_LANE_MAP_1f_SET(DIG_COM_TX_LANE_MAP_0_1_2r_reg, tx_lane_map_1);
  DIG_TX_LN_MAP_0_1_2r_TX_LANE_MAP_0f_SET(DIG_COM_TX_LANE_MAP_0_1_2r_reg, tx_lane_map_0);
  PHYMOD_IF_ERR_RETURN(MODIFY_DIG_TX_LN_MAP_0_1_2r(pc, DIG_COM_TX_LANE_MAP_0_1_2r_reg));

  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_CLR(DIG_COM_TX_LANE_MAP_3_N_LANE_ADDR_0_1r_reg);
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_TX_LANE_MAP_3f_SET(DIG_COM_TX_LANE_MAP_3_N_LANE_ADDR_0_1r_reg, tx_lane_map_3);
  PHYMOD_IF_ERR_RETURN(MODIFY_DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r(pc, DIG_COM_TX_LANE_MAP_3_N_LANE_ADDR_0_1r_reg));

#ifdef _DV_TB_
  pc->adjust_port_mode = 0;
#endif

  return PHYMOD_E_NONE;
}
#endif /* end if 0*/

int tefmod16_pmd_lane_swap(PHYMOD_ST *pc, int tx_lane_swap, int rx_lane_swap)
{

  unsigned int tx_map, rx_map;
  LN_ADDR0r_t LN_ADDR0r_reg;
  LN_ADDR1r_t LN_ADDR1r_reg;
  LN_ADDR2r_t LN_ADDR2r_reg;
  LN_ADDR3r_t LN_ADDR3r_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  tx_map = (unsigned int)tx_lane_swap;
  rx_map = (unsigned int)rx_lane_swap;

  LN_ADDR0r_CLR(LN_ADDR0r_reg);
  LN_ADDR1r_CLR(LN_ADDR1r_reg);
  LN_ADDR2r_CLR(LN_ADDR2r_reg);
  LN_ADDR3r_CLR(LN_ADDR3r_reg);

  LN_ADDR0r_TX_LANE_ADDR_0f_SET(LN_ADDR0r_reg, ((tx_map >> 0)  & 0xf));
  LN_ADDR1r_TX_LANE_ADDR_1f_SET(LN_ADDR1r_reg, ((tx_map >> 4)  & 0xf));
  LN_ADDR2r_TX_LANE_ADDR_2f_SET(LN_ADDR2r_reg, ((tx_map >> 8)  & 0xf));
  LN_ADDR3r_TX_LANE_ADDR_3f_SET(LN_ADDR3r_reg, ((tx_map >> 12) & 0xf));

  LN_ADDR0r_RX_LANE_ADDR_0f_SET(LN_ADDR0r_reg, ((rx_map >> 0)  & 0xf));
  LN_ADDR1r_RX_LANE_ADDR_1f_SET(LN_ADDR1r_reg, ((rx_map >> 4)  & 0xf));
  LN_ADDR2r_RX_LANE_ADDR_2f_SET(LN_ADDR2r_reg, ((rx_map >> 8)  & 0xf));
  LN_ADDR3r_RX_LANE_ADDR_3f_SET(LN_ADDR3r_reg, ((rx_map >> 12) & 0xf));

  PHYMOD_IF_ERR_RETURN(MODIFY_LN_ADDR0r(pc, LN_ADDR0r_reg));
  PHYMOD_IF_ERR_RETURN(MODIFY_LN_ADDR1r(pc, LN_ADDR1r_reg));
  PHYMOD_IF_ERR_RETURN(MODIFY_LN_ADDR2r(pc, LN_ADDR2r_reg));
  PHYMOD_IF_ERR_RETURN(MODIFY_LN_ADDR3r(pc, LN_ADDR3r_reg));

  return PHYMOD_E_NONE;
}



#ifdef _DV_TB_
/*!
@brief removes resets from all the PMD lanes
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details
Removes resets
*/

int tefmod16_pmd_reset_remove(PHYMOD_ST *pc, int pmd_touched)
{
  uint16_t data;
  PMD_X1_CTLr_t PMD_X1_PMD_X1_CONTROLr_reg;
  PMD_X4_CTLr_t PMD_X4_PMD_X4_CONTROLr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  data = 0;
  
  if(pmd_touched == 0) {
    PMD_X1_CTLr_CLR(PMD_X1_PMD_X1_CONTROLr_reg);
    /* setup the PMD reset */
    PMD_X1_CTLr_POR_H_RSTBf_SET(PMD_X1_PMD_X1_CONTROLr_reg, 1);
    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(PMD_X1_PMD_X1_CONTROLr_reg, 1);

    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_CTLr(pc, PMD_X1_PMD_X1_CONTROLr_reg));
  }

  data = 0;
  
  PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);
  PMD_X4_CTLr_LN_RX_H_PWRDNf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 0);
  PMD_X4_CTLr_LN_TX_H_PWRDNf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 0);
  
  PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));

  PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);
  PMD_X4_CTLr_LN_H_RSTBf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 1);
  PMD_X4_CTLr_LN_DP_H_RSTBf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 1);
  
  PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));
  
  return PHYMOD_E_NONE;
}
#endif

/*!
@brief sets both the RX and TX lane swap values for all lanes simultaneously.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  per_lane_control TBD
@returns The value PHYMOD_E_NONE upon successful completion
@details
This function sets the TX lane swap values for all lanes simultaneously.

The TSCF has several two sets of lane swap registers per lane.  This function
uses the lane swap registers closest to the pads.  For TX, the lane swap occurs
after all other analog/digital functionality.  For RX, the lane swap occurs
prior to all other analog/digital functionality.

It is not advisable to swap lanes without going through reset.

How the swap is done is indicated in the per_lane_control field; bits 0 through
15 represent the swap settings for RX while bits 16 through 31 represent the
swap settings for TX.

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

This function modifies the following registers:
*/
/* TSCF_GEN2 : Rx lane swap */
int tefmod16_pcs_rx_lane_swap(PHYMOD_ST *pc, int rx_lane_swap)
{
  unsigned int map;
  RX_X1_RX_LN_SWPr_t RX_X1_RX_LN_SWPr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  map = (unsigned int)rx_lane_swap;

  RX_X1_RX_LN_SWPr_CLR(RX_X1_RX_LN_SWPr_reg);
  RX_X1_RX_LN_SWPr_LOGICAL3_TO_PHY_SELf_SET(RX_X1_RX_LN_SWPr_reg, ((map >> 12) & 0x3));
  RX_X1_RX_LN_SWPr_LOGICAL2_TO_PHY_SELf_SET(RX_X1_RX_LN_SWPr_reg, ((map >> 8)  & 0x3));
  RX_X1_RX_LN_SWPr_LOGICAL1_TO_PHY_SELf_SET(RX_X1_RX_LN_SWPr_reg, ((map >> 4)  & 0x3));
  RX_X1_RX_LN_SWPr_LOGICAL0_TO_PHY_SELf_SET(RX_X1_RX_LN_SWPr_reg, ((map >> 0)  & 0x3));

  PHYMOD_IF_ERR_RETURN(MODIFY_RX_X1_RX_LN_SWPr(pc, RX_X1_RX_LN_SWPr_reg));

  return PHYMOD_E_NONE;
}

/* TSCF_GEN2 : Tx lane swap */
int tefmod16_pcs_tx_lane_swap(PHYMOD_ST *pc, int tx_lane_swap)
{
  unsigned int map;
  TX_X1_TX_LN_SWPr_t TX_X1_TX_LN_SWPr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  map = (unsigned int)tx_lane_swap;

  TX_X1_TX_LN_SWPr_CLR(TX_X1_TX_LN_SWPr_reg);
  TX_X1_TX_LN_SWPr_LOGICAL2_TO_PHY_SELf_SET(TX_X1_TX_LN_SWPr_reg, ((map >> 8)  & 0x3));
  TX_X1_TX_LN_SWPr_LOGICAL3_TO_PHY_SELf_SET(TX_X1_TX_LN_SWPr_reg, ((map >> 12) & 0x3));
  TX_X1_TX_LN_SWPr_LOGICAL1_TO_PHY_SELf_SET(TX_X1_TX_LN_SWPr_reg, ((map >> 4)  & 0x3));
  TX_X1_TX_LN_SWPr_LOGICAL0_TO_PHY_SELf_SET(TX_X1_TX_LN_SWPr_reg, ((map >> 0)  & 0x3));

  PHYMOD_IF_ERR_RETURN(MODIFY_TX_X1_TX_LN_SWPr(pc, TX_X1_TX_LN_SWPr_reg));

  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/*!
@brief configure soft table entry 
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details

This is a tier2 function, whenever something from Hard table needs to be overwritten
using soft table, this function is called.
There are 4 sets of regs to override.  These regs are global sc_x1_speed_override0 .. override3
*/
int configure_st_entry(int st_entry_num, int st_entry_speed, PHYMOD_ST* pc)
{
  SC_X1_SPD_OVRR0_SPDr_t SC_X1_SPEED_OVERRIDE0_OVERRIDE_SPEEDr_reg;
  SC_X1_SPD_OVRR1_SPDr_t SC_X1_SPEED_OVERRIDE1_OVERRIDE_SPEEDr_reg;
  SC_X1_SPD_OVRR2_SPDr_t SC_X1_SPEED_OVERRIDE2_OVERRIDE_SPEEDr_reg;
  SC_X1_SPD_OVRR3_SPDr_t SC_X1_SPEED_OVERRIDE3_OVERRIDE_SPEEDr_reg;

  SC_X1_SPD_OVRR0_0r_t SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg;
  SC_X1_SPD_OVRR1_0r_t SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg;
  SC_X1_SPD_OVRR2_0r_t SC_X1_SPEED_OVERRIDE2_OVERRIDE_0r_reg;
  SC_X1_SPD_OVRR3_0r_t SC_X1_SPEED_OVERRIDE3_OVERRIDE_0r_reg;

  SC_X1_SPD_OVRR0_1r_t SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg;
  SC_X1_SPD_OVRR1_1r_t SC_X1_SPEED_OVERRIDE1_OVERRIDE_1r_reg;
  SC_X1_SPD_OVRR2_1r_t SC_X1_SPEED_OVERRIDE2_OVERRIDE_1r_reg;
  SC_X1_SPD_OVRR3_1r_t SC_X1_SPEED_OVERRIDE3_OVERRIDE_1r_reg;

  SC_X1_SPD_OVRR0_2r_t SC_X1_SPEED_OVERRIDE0_OVERRIDE_2r_reg;
  SC_X1_SPD_OVRR1_2r_t SC_X1_SPEED_OVERRIDE1_OVERRIDE_2r_reg;
  SC_X1_SPD_OVRR2_2r_t SC_X1_SPEED_OVERRIDE2_OVERRIDE_2r_reg;
  SC_X1_SPD_OVRR3_2r_t SC_X1_SPEED_OVERRIDE3_OVERRIDE_2r_reg;

  SC_X1_SPD_OVRR0_3r_t SC_X1_SPEED_OVERRIDE0_OVERRIDE_3r_reg;
  SC_X1_SPD_OVRR1_3r_t SC_X1_SPEED_OVERRIDE1_OVERRIDE_3r_reg;
  SC_X1_SPD_OVRR2_3r_t SC_X1_SPEED_OVERRIDE2_OVERRIDE_3r_reg;
  SC_X1_SPD_OVRR3_3r_t SC_X1_SPEED_OVERRIDE3_OVERRIDE_3r_reg;

  SC_X1_SPD_OVRR0_4r_t SC_X1_SPEED_OVERRIDE0_OVERRIDE_4r_reg;
  SC_X1_SPD_OVRR1_4r_t SC_X1_SPEED_OVERRIDE1_OVERRIDE_4r_reg;
  SC_X1_SPD_OVRR2_4r_t SC_X1_SPEED_OVERRIDE2_OVERRIDE_4r_reg;
  SC_X1_SPD_OVRR3_4r_t SC_X1_SPEED_OVERRIDE3_OVERRIDE_4r_reg;

  SC_X1_SPD_OVRR0_5r_t SC_X1_SPEED_OVERRIDE0_OVERRIDE_5r_reg;
  SC_X1_SPD_OVRR1_5r_t SC_X1_SPEED_OVERRIDE1_OVERRIDE_5r_reg;
  SC_X1_SPD_OVRR2_5r_t SC_X1_SPEED_OVERRIDE2_OVERRIDE_5r_reg;
  SC_X1_SPD_OVRR3_5r_t SC_X1_SPEED_OVERRIDE3_OVERRIDE_5r_reg;

  SC_X1_SPD_OVRR0_6r_t SC_X1_SPD_OVRR0_6r_reg;
  SC_X1_SPD_OVRR1_6r_t SC_X1_SPD_OVRR1_6r_reg;
  SC_X1_SPD_OVRR2_6r_t SC_X1_SPD_OVRR2_6r_reg;
  SC_X1_SPD_OVRR3_6r_t SC_X1_SPD_OVRR3_6r_reg;
 
  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  pc->speed = st_entry_speed;
  /* st_entry_num can be 0, 1, 2, or 3 */
  if (pc->verbosity)
    PHYMOD_DEBUG_ERROR(("Using speed_id: %x and st_entry_num: %x", st_entry_speed, st_entry_num));

  if (st_entry_num == 0) {
    SC_X1_SPD_OVRR0_SPDr_CLR(SC_X1_SPEED_OVERRIDE0_OVERRIDE_SPEEDr_reg);
    SC_X1_SPD_OVRR0_SPDr_SPEEDf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_SPEEDr_reg, st_entry_speed);

    SC_X1_SPD_OVRR0_SPDr_NUM_LANESf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_SPEEDr_reg, (pc->num_lanes));

    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR0_SPDr(pc, SC_X1_SPEED_OVERRIDE0_OVERRIDE_SPEEDr_reg));


    SC_X1_SPD_OVRR0_0r_CLR(SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg);
    SC_X1_SPD_OVRR0_0r_OS_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg, (pc->os_mode));

    SC_X1_SPD_OVRR0_0r_SCR_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg, (pc->scr_mode));

    SC_X1_SPD_OVRR0_0r_T_ENC_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg, (pc->t_enc_mode));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR0_0r(pc, SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg));

    SC_X1_SPD_OVRR0_1r_CLR(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg);
    SC_X1_SPD_OVRR0_1r_DESCR_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->descr_mode));

    SC_X1_SPD_OVRR0_1r_DEC_TL_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->dec_tl_mode));

    SC_X1_SPD_OVRR0_1r_DESKEW_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->deskew_mode));

    SC_X1_SPD_OVRR0_1r_DEC_FSM_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->dec_fsm_mode));

    SC_X1_SPD_OVRR0_1r_BS_SM_SYNC_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->bs_sm_sync_mode));

    SC_X1_SPD_OVRR0_1r_BS_SYNC_ENf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->bs_sync_en));

    SC_X1_SPD_OVRR0_1r_BS_DIST_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->bs_dist_mode));

    SC_X1_SPD_OVRR0_1r_BS_BTMX_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->bs_btmx_mode));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR0_1r(pc, SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg));

    SC_X1_SPD_OVRR0_2r_CLR(SC_X1_SPEED_OVERRIDE0_OVERRIDE_2r_reg);
    SC_X1_SPD_OVRR0_2r_CLOCKCNT0f_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_2r_reg, (pc->clkcnt0));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR0_2r(pc, SC_X1_SPEED_OVERRIDE0_OVERRIDE_2r_reg));

    SC_X1_SPD_OVRR0_3r_CLR(SC_X1_SPEED_OVERRIDE0_OVERRIDE_3r_reg);
    SC_X1_SPD_OVRR0_3r_CLOCKCNT1f_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_3r_reg, (pc->clkcnt1));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR0_3r(pc, SC_X1_SPEED_OVERRIDE0_OVERRIDE_3r_reg));

    SC_X1_SPD_OVRR0_4r_CLR(SC_X1_SPEED_OVERRIDE0_OVERRIDE_4r_reg);
    SC_X1_SPD_OVRR0_4r_LOOPCNT0f_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_4r_reg, (pc->lpcnt0));

    SC_X1_SPD_OVRR0_4r_LOOPCNT1f_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_4r_reg, (pc->lpcnt1));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR0_4r(pc, SC_X1_SPEED_OVERRIDE0_OVERRIDE_4r_reg));

    SC_X1_SPD_OVRR0_5r_CLR(SC_X1_SPEED_OVERRIDE0_OVERRIDE_5r_reg);
    SC_X1_SPD_OVRR0_5r_MAC_CREDITGENCNTf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_5r_reg, (pc->cgc));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR0_5r(pc, SC_X1_SPEED_OVERRIDE0_OVERRIDE_5r_reg));

    SC_X1_SPD_OVRR0_6r_CLR(SC_X1_SPD_OVRR0_6r_reg);
    SC_X1_SPD_OVRR0_6r_AM_SPACING_MULf_SET(SC_X1_SPD_OVRR0_6r_reg, pc->am_spacing_mul);
    SC_X1_SPD_OVRR0_6r_T_PMA_BITMUX_DELAYf_SET(SC_X1_SPD_OVRR0_6r_reg, pc->t_pma_btmx_delay);
    SC_X1_SPD_OVRR0_6r_T_PMA_WATERMARKf_SET(SC_X1_SPD_OVRR0_6r_reg, pc->t_pma_watermark);
    SC_X1_SPD_OVRR0_6r_T_PMA_CL91_MUX_SELf_SET(SC_X1_SPD_OVRR0_6r_reg, pc->t_pma_cl91_mux_sel);
    SC_X1_SPD_OVRR0_6r_T_FIVE_BIT_XOR_ENf_SET(SC_X1_SPD_OVRR0_6r_reg, pc->t_5bit_xor_en);
    SC_X1_SPD_OVRR0_6r_T_CL91_CW_SCRAMBLEf_SET(SC_X1_SPD_OVRR0_6r_reg, pc->t_cl91_cw_scr);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR0_6r(pc, SC_X1_SPD_OVRR0_6r_reg));
  
  }
  if (st_entry_num == 1) {
    SC_X1_SPD_OVRR1_SPDr_CLR(SC_X1_SPEED_OVERRIDE1_OVERRIDE_SPEEDr_reg);
    SC_X1_SPD_OVRR1_SPDr_SPEEDf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_SPEEDr_reg, st_entry_speed);

    SC_X1_SPD_OVRR1_SPDr_NUM_LANESf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_SPEEDr_reg, (pc->num_lanes));

    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_SPDr(pc, SC_X1_SPEED_OVERRIDE1_OVERRIDE_SPEEDr_reg));


    SC_X1_SPD_OVRR1_0r_CLR(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg);
    SC_X1_SPD_OVRR1_0r_OS_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg, (pc->os_mode));

    SC_X1_SPD_OVRR1_0r_SCR_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg, (pc->scr_mode));

    SC_X1_SPD_OVRR1_0r_T_ENC_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg, (pc->t_enc_mode));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_0r(pc, SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg));

    SC_X1_SPD_OVRR1_0r_SCR_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg, (pc->scr_mode));

    SC_X1_SPD_OVRR1_0r_T_ENC_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg, (pc->t_enc_mode));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_0r(pc, SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg));

    SC_X1_SPD_OVRR1_1r_CLR(SC_X1_SPEED_OVERRIDE1_OVERRIDE_1r_reg);
    SC_X1_SPD_OVRR1_1r_DESCR_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_1r_reg, (pc->descr_mode));

    SC_X1_SPD_OVRR1_1r_DEC_TL_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_1r_reg, (pc->dec_tl_mode));

    SC_X1_SPD_OVRR1_1r_DESKEW_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_1r_reg, (pc->deskew_mode));

    SC_X1_SPD_OVRR1_1r_DEC_FSM_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_1r_reg, (pc->dec_fsm_mode));

    #ifdef TSCF_GEN1
    SC_X1_SPD_OVRR1_1r_R_HG2_ENABLEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_1r_reg, (pc->r_HG2_ENABLE));
    #endif

    SC_X1_SPD_OVRR1_1r_BS_SM_SYNC_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_1r_reg, (pc->bs_sm_sync_mode));

    SC_X1_SPD_OVRR1_1r_BS_SYNC_ENf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_1r_reg, (pc->bs_sync_en));

    SC_X1_SPD_OVRR1_1r_BS_DIST_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_1r_reg, (pc->bs_dist_mode));

    SC_X1_SPD_OVRR1_1r_BS_BTMX_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_1r_reg, (pc->bs_btmx_mode));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_1r(pc, SC_X1_SPEED_OVERRIDE1_OVERRIDE_1r_reg));

    SC_X1_SPD_OVRR1_2r_CLR(SC_X1_SPEED_OVERRIDE1_OVERRIDE_2r_reg);
    SC_X1_SPD_OVRR1_2r_CLOCKCNT0f_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_2r_reg, (pc->clkcnt0));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_2r(pc, SC_X1_SPEED_OVERRIDE1_OVERRIDE_2r_reg));

    SC_X1_SPD_OVRR1_3r_CLR(SC_X1_SPEED_OVERRIDE1_OVERRIDE_3r_reg);
    SC_X1_SPD_OVRR1_3r_CLOCKCNT1f_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_3r_reg, (pc->clkcnt1));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_3r(pc, SC_X1_SPEED_OVERRIDE1_OVERRIDE_3r_reg));

    SC_X1_SPD_OVRR1_4r_CLR(SC_X1_SPEED_OVERRIDE1_OVERRIDE_4r_reg);
    SC_X1_SPD_OVRR1_4r_LOOPCNT0f_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_4r_reg, (pc->lpcnt0));

    SC_X1_SPD_OVRR1_4r_LOOPCNT1f_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_4r_reg, (pc->lpcnt1));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_4r(pc, SC_X1_SPEED_OVERRIDE1_OVERRIDE_4r_reg));

    SC_X1_SPD_OVRR1_5r_CLR(SC_X1_SPEED_OVERRIDE1_OVERRIDE_5r_reg);
    SC_X1_SPD_OVRR1_5r_MAC_CREDITGENCNTf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_5r_reg, (pc->cgc));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_5r(pc, SC_X1_SPEED_OVERRIDE1_OVERRIDE_5r_reg));

    SC_X1_SPD_OVRR1_6r_CLR(SC_X1_SPD_OVRR1_6r_reg);
    SC_X1_SPD_OVRR1_6r_AM_SPACING_MULf_SET(SC_X1_SPD_OVRR1_6r_reg, pc->am_spacing_mul);
    SC_X1_SPD_OVRR1_6r_T_PMA_BITMUX_DELAYf_SET(SC_X1_SPD_OVRR1_6r_reg, pc->t_pma_btmx_delay);
    SC_X1_SPD_OVRR1_6r_T_PMA_WATERMARKf_SET(SC_X1_SPD_OVRR1_6r_reg, pc->t_pma_watermark);
    SC_X1_SPD_OVRR1_6r_T_PMA_CL91_MUX_SELf_SET(SC_X1_SPD_OVRR1_6r_reg, pc->t_pma_cl91_mux_sel);
    SC_X1_SPD_OVRR1_6r_T_FIVE_BIT_XOR_ENf_SET(SC_X1_SPD_OVRR1_6r_reg, pc->t_5bit_xor_en);
    SC_X1_SPD_OVRR1_6r_T_CL91_CW_SCRAMBLEf_SET(SC_X1_SPD_OVRR1_6r_reg, pc->t_cl91_cw_scr);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR1_6r(pc, SC_X1_SPD_OVRR1_6r_reg));

  }
  if (st_entry_num == 2) {
    SC_X1_SPD_OVRR2_SPDr_CLR(SC_X1_SPEED_OVERRIDE2_OVERRIDE_SPEEDr_reg);
    SC_X1_SPD_OVRR2_SPDr_SPEEDf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_SPEEDr_reg, st_entry_speed);

    SC_X1_SPD_OVRR2_SPDr_NUM_LANESf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_SPEEDr_reg, (pc->num_lanes));

    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_SPDr(pc, SC_X1_SPEED_OVERRIDE2_OVERRIDE_SPEEDr_reg));

    SC_X1_SPD_OVRR2_0r_CLR(SC_X1_SPEED_OVERRIDE2_OVERRIDE_0r_reg);
    SC_X1_SPD_OVRR2_0r_CL72_ENf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_0r_reg, pc->cl72_en); 
    SC_X1_SPD_OVRR2_0r_OS_MODEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_0r_reg, pc->os_mode);
    SC_X1_SPD_OVRR2_0r_T_FIFO_MODEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_0r_reg, pc->t_fifo_mode); 
    SC_X1_SPD_OVRR2_0r_T_ENC_MODEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_0r_reg, pc->t_enc_mode); 
    SC_X1_SPD_OVRR2_0r_T_HG2_ENABLEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_0r_reg, pc->t_HG2_ENABLE); 
    SC_X1_SPD_OVRR2_0r_T_PMA_BTMX_MODEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_0r_reg, pc->t_pma_btmx_mode); 
    SC_X1_SPD_OVRR2_0r_SCR_MODEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_0r_reg, pc->scr_mode);

    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_0r(pc, SC_X1_SPEED_OVERRIDE2_OVERRIDE_0r_reg));


    SC_X1_SPD_OVRR2_1r_CLR(SC_X1_SPEED_OVERRIDE2_OVERRIDE_1r_reg);
    SC_X1_SPD_OVRR2_1r_DESCR_MODEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_1r_reg, (pc->descr_mode));

    SC_X1_SPD_OVRR2_1r_DEC_TL_MODEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_1r_reg, (pc->dec_tl_mode));

    SC_X1_SPD_OVRR2_1r_DESKEW_MODEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_1r_reg, (pc->deskew_mode));

    SC_X1_SPD_OVRR2_1r_DEC_FSM_MODEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_1r_reg, (pc->dec_fsm_mode));

    #ifdef TSCF_GEN1
    SC_X1_SPD_OVRR2_1r_R_HG2_ENABLEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_1r_reg, (pc->r_HG2_ENABLE));
    #endif

    SC_X1_SPD_OVRR2_1r_BS_SM_SYNC_MODEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_1r_reg, (pc->bs_sm_sync_mode));

    SC_X1_SPD_OVRR2_1r_BS_SYNC_ENf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_1r_reg, (pc->bs_sync_en));

    SC_X1_SPD_OVRR2_1r_BS_DIST_MODEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_1r_reg, (pc->bs_dist_mode));

    SC_X1_SPD_OVRR2_1r_BS_BTMX_MODEf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_1r_reg, (pc->bs_btmx_mode));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_1r(pc, SC_X1_SPEED_OVERRIDE2_OVERRIDE_1r_reg));

    SC_X1_SPD_OVRR2_2r_CLR(SC_X1_SPEED_OVERRIDE2_OVERRIDE_2r_reg);
    SC_X1_SPD_OVRR2_2r_CLOCKCNT0f_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_2r_reg, (pc->clkcnt0));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_2r(pc, SC_X1_SPEED_OVERRIDE2_OVERRIDE_2r_reg));

    SC_X1_SPD_OVRR2_3r_CLR(SC_X1_SPEED_OVERRIDE2_OVERRIDE_3r_reg);
    SC_X1_SPD_OVRR2_3r_CLOCKCNT1f_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_3r_reg, (pc->clkcnt1));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_3r(pc, SC_X1_SPEED_OVERRIDE2_OVERRIDE_3r_reg));

    SC_X1_SPD_OVRR2_4r_CLR(SC_X1_SPEED_OVERRIDE2_OVERRIDE_4r_reg);
    SC_X1_SPD_OVRR2_4r_LOOPCNT0f_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_4r_reg, (pc->lpcnt0));

    SC_X1_SPD_OVRR2_4r_LOOPCNT1f_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_4r_reg, (pc->lpcnt1));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_4r(pc, SC_X1_SPEED_OVERRIDE2_OVERRIDE_4r_reg));

    SC_X1_SPD_OVRR2_5r_CLR(SC_X1_SPEED_OVERRIDE2_OVERRIDE_5r_reg);
    SC_X1_SPD_OVRR2_5r_MAC_CREDITGENCNTf_SET(SC_X1_SPEED_OVERRIDE2_OVERRIDE_5r_reg, (pc->cgc));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_5r(pc, SC_X1_SPEED_OVERRIDE2_OVERRIDE_5r_reg));

    SC_X1_SPD_OVRR2_6r_CLR(SC_X1_SPD_OVRR2_6r_reg);
    SC_X1_SPD_OVRR2_6r_AM_SPACING_MULf_SET(SC_X1_SPD_OVRR2_6r_reg, pc->am_spacing_mul);
    SC_X1_SPD_OVRR2_6r_T_PMA_BITMUX_DELAYf_SET(SC_X1_SPD_OVRR2_6r_reg, pc->t_pma_btmx_delay);
    SC_X1_SPD_OVRR2_6r_T_PMA_WATERMARKf_SET(SC_X1_SPD_OVRR2_6r_reg, pc->t_pma_watermark);
    SC_X1_SPD_OVRR2_6r_T_PMA_CL91_MUX_SELf_SET(SC_X1_SPD_OVRR2_6r_reg, pc->t_pma_cl91_mux_sel);
    SC_X1_SPD_OVRR2_6r_T_FIVE_BIT_XOR_ENf_SET(SC_X1_SPD_OVRR2_6r_reg, pc->t_5bit_xor_en);
    SC_X1_SPD_OVRR2_6r_T_CL91_CW_SCRAMBLEf_SET(SC_X1_SPD_OVRR2_6r_reg, pc->t_cl91_cw_scr);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR2_6r(pc, SC_X1_SPD_OVRR2_6r_reg));

  }
  if (st_entry_num == 3) {
    SC_X1_SPD_OVRR3_SPDr_CLR(SC_X1_SPEED_OVERRIDE3_OVERRIDE_SPEEDr_reg);
    SC_X1_SPD_OVRR3_SPDr_SPEEDf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_SPEEDr_reg, st_entry_speed);

    SC_X1_SPD_OVRR3_SPDr_NUM_LANESf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_SPEEDr_reg, (pc->num_lanes));

    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR3_SPDr(pc, SC_X1_SPEED_OVERRIDE3_OVERRIDE_SPEEDr_reg));

    SC_X1_SPD_OVRR3_0r_CLR(SC_X1_SPEED_OVERRIDE3_OVERRIDE_0r_reg);
    SC_X1_SPD_OVRR3_0r_CL72_ENf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_0r_reg, pc->cl72_en); 
    SC_X1_SPD_OVRR3_0r_OS_MODEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_0r_reg, pc->os_mode);
    SC_X1_SPD_OVRR3_0r_T_FIFO_MODEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_0r_reg, pc->t_fifo_mode); 
    SC_X1_SPD_OVRR3_0r_T_ENC_MODEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_0r_reg, pc->t_enc_mode); 
    SC_X1_SPD_OVRR3_0r_T_HG2_ENABLEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_0r_reg, pc->t_HG2_ENABLE); 
    SC_X1_SPD_OVRR3_0r_T_PMA_BTMX_MODEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_0r_reg, pc->t_pma_btmx_mode); 
    SC_X1_SPD_OVRR3_0r_SCR_MODEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_0r_reg, pc->scr_mode);

    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR3_0r(pc, SC_X1_SPEED_OVERRIDE3_OVERRIDE_0r_reg));


    SC_X1_SPD_OVRR3_1r_CLR(SC_X1_SPEED_OVERRIDE3_OVERRIDE_1r_reg);
    SC_X1_SPD_OVRR3_1r_DESCR_MODEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_1r_reg, (pc->descr_mode));

    SC_X1_SPD_OVRR3_1r_DEC_TL_MODEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_1r_reg, (pc->dec_tl_mode));

    SC_X1_SPD_OVRR3_1r_DESKEW_MODEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_1r_reg, (pc->deskew_mode));

    SC_X1_SPD_OVRR3_1r_DEC_FSM_MODEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_1r_reg, (pc->dec_fsm_mode));

    #ifdef TSCF_GEN1
    SC_X1_SPD_OVRR3_1r_R_HG2_ENABLEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_1r_reg, (pc->r_HG2_ENABLE)); 
    #endif

    SC_X1_SPD_OVRR3_1r_BS_SM_SYNC_MODEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_1r_reg, (pc->bs_sm_sync_mode));

    SC_X1_SPD_OVRR3_1r_BS_SYNC_ENf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_1r_reg, (pc->bs_sync_en));

    SC_X1_SPD_OVRR3_1r_BS_DIST_MODEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_1r_reg, (pc->bs_dist_mode));

    SC_X1_SPD_OVRR3_1r_BS_BTMX_MODEf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_1r_reg, (pc->bs_btmx_mode));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR3_1r(pc, SC_X1_SPEED_OVERRIDE3_OVERRIDE_1r_reg));

    SC_X1_SPD_OVRR3_2r_CLR(SC_X1_SPEED_OVERRIDE3_OVERRIDE_2r_reg);
    SC_X1_SPD_OVRR3_2r_CLOCKCNT0f_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_2r_reg, (pc->clkcnt0));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR3_2r(pc, SC_X1_SPEED_OVERRIDE3_OVERRIDE_2r_reg));

    SC_X1_SPD_OVRR3_3r_CLR(SC_X1_SPEED_OVERRIDE3_OVERRIDE_3r_reg);
    SC_X1_SPD_OVRR3_3r_CLOCKCNT1f_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_3r_reg, (pc->clkcnt1));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR3_3r(pc, SC_X1_SPEED_OVERRIDE3_OVERRIDE_3r_reg));

    SC_X1_SPD_OVRR3_4r_CLR(SC_X1_SPEED_OVERRIDE3_OVERRIDE_4r_reg);
    SC_X1_SPD_OVRR3_4r_LOOPCNT0f_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_4r_reg, (pc->lpcnt0));

    SC_X1_SPD_OVRR3_4r_LOOPCNT1f_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_4r_reg, (pc->lpcnt1));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR3_4r(pc, SC_X1_SPEED_OVERRIDE3_OVERRIDE_4r_reg));

    SC_X1_SPD_OVRR3_5r_CLR(SC_X1_SPEED_OVERRIDE3_OVERRIDE_5r_reg);
    SC_X1_SPD_OVRR3_5r_MAC_CREDITGENCNTf_SET(SC_X1_SPEED_OVERRIDE3_OVERRIDE_5r_reg, (pc->cgc));
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR3_5r(pc, SC_X1_SPEED_OVERRIDE3_OVERRIDE_5r_reg));

    SC_X1_SPD_OVRR3_6r_CLR(SC_X1_SPD_OVRR3_6r_reg);
    SC_X1_SPD_OVRR3_6r_AM_SPACING_MULf_SET(SC_X1_SPD_OVRR3_6r_reg, pc->am_spacing_mul);
    SC_X1_SPD_OVRR3_6r_T_PMA_BITMUX_DELAYf_SET(SC_X1_SPD_OVRR3_6r_reg, pc->t_pma_btmx_delay);
    SC_X1_SPD_OVRR3_6r_T_PMA_WATERMARKf_SET(SC_X1_SPD_OVRR3_6r_reg, pc->t_pma_watermark);
    SC_X1_SPD_OVRR3_6r_T_PMA_CL91_MUX_SELf_SET(SC_X1_SPD_OVRR3_6r_reg, pc->t_pma_cl91_mux_sel);
    SC_X1_SPD_OVRR3_6r_T_FIVE_BIT_XOR_ENf_SET(SC_X1_SPD_OVRR3_6r_reg, pc->t_5bit_xor_en);
    SC_X1_SPD_OVRR3_6r_T_CL91_CW_SCRAMBLEf_SET(SC_X1_SPD_OVRR3_6r_reg, pc->t_cl91_cw_scr);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR3_6r(pc, SC_X1_SPD_OVRR3_6r_reg));

  }
  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/*!
@brief override soft table entry speed in AN mode 
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This is a tier2 function, it is called from init_pcs_an
For instance, if a speed is resolved to 10G, but using Softtable entry
if user likes to override to 20G XFI, the individual fields below are 
updated in pc for comparison and the configure_st_entry is called 
configure_st_entry func does the actual overriding
returns The value PHYMOD_E_NONE upon successful completion.
*/

int tefmod16_set_an_override(PHYMOD_ST* pc)
{
  TEFMOD16_DBG_IN_FUNC_INFO(pc);



  /* override_speed */
  pc->speed= 64;         /*100G CR4*/ 
  pc->num_lanes = NUM_LANES_04;
  pc->cl36rx_en = 0;
  pc->cl36tx_en = 0;
  pc->t_pma_40b_mode = 0;

  /* override_0 */
  pc->cl72_en=1;
  pc->os_mode   = OS_MODE_1;
  pc->t_fifo_mode=T_FIFO_MODE_100G;
  pc->t_enc_mode=T_ENC_MODE_CL82;
  pc->t_HG2_ENABLE=0;
  pc->t_pma_btmx_mode=T_PMA_BITMUX_MODE_5to1;
  pc->scr_mode=T_SCR_MODE_100G;

  /* override_1 */
  pc->descr_mode=R_DESCR_MODE_CL82;
  pc->dec_tl_mode=R_DEC_TL_MODE_CL82;
  pc->deskew_mode=R_DESKEW_MODE_100G;
  pc->dec_fsm_mode=R_DEC_FSM_MODE_CL82;
  pc->bs_sm_sync_mode=BS_CL82_SYNC_MODE;
  pc->bs_sync_en=1;
  pc->bs_dist_mode=BS_DIST_MODE_NO_TDM;
  pc->bs_btmx_mode=BS_BTMX_MODE_5to1;



  /* override_2 */
  pc->clkcnt0 = 9;

  /* override_3 */
  pc->clkcnt1 = 5;

  /* override_4 */
  pc->lpcnt0 = 1;
  pc->lpcnt1 = 18;

  /* override_5 */
  pc->cgc = 1;

  /* override_6 */
  pc->am_spacing_mul = 3;
  pc->t_pma_btmx_delay = 1;
  pc->t_pma_watermark = 7;
  pc->t_pma_cl91_mux_sel = 0;
  pc->t_5bit_xor_en = 0;
  pc->t_cl91_cw_scr = 0;

  
  /* override_7 */
  pc->r_HG2_ENABLE=0;
  pc->r_tc_out_mode = 0;
  pc->r_5bit_xor_en = 0;
  pc->r_tc_mode = 0;
  pc->r_tc_in_mode = 0;
  pc->r_cl91_cw_scr = 0;
  pc->r_merge_mode = 0;
  pc->cl91_blksync_mode = 0;
  configure_st_entry(0, pc->speed,  pc);

  /*100G KR4 */ 
  pc->speed= 65;         
  configure_st_entry(2, pc->speed,  pc);

  /*100G CR4 HG2*/
  pc->speed=68;    
  pc->t_HG2_ENABLE=1;
  pc->r_HG2_ENABLE=1;
  configure_st_entry(1, pc->speed,  pc);

  /*100G KR4 HG2*/
  pc->speed= 69;          
  configure_st_entry(3, pc->speed,  pc); 


  return PHYMOD_E_NONE;
}
#endif


#ifdef _DV_TB_
/*!
@brief tefmod16_init_pcs_an , program PCS for AN
@returns The value PHYMOD_E_NONE upon successful completion.
@details
If any override option switch is selected, it will call tier2 tefmod16_set_an_override
This is a tier2 function, whenever something from Hard table needs to be overwritten
calls the other autoneg routines to perform AN.
Sets AN port mode 
Controls the setting/resetting of autoneg advertisement registers.
tefmod16_autoneg_control starts the AN
*/
int tefmod16_init_pcs_an(PHYMOD_ST* pc)
{
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  if(pc->sc_mode == TEFMOD16_SC_MODE_AN_CL73) {
    tefmod16_set_an_override(pc);
  }
  tefmod16_set_an_port_mode(pc);
  tefmod16_autoneg_set(pc);
  tefmod16_autoneg_control(pc);
  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/*!
@brief tefmod16_set_sc_speed , sets the required speed ,  
@returns The value PHYMOD_E_NONE upon successful completion.
@details
After the resets, speed_ctrl regs are programmed(as per Ankits new init seq), fills up the lookup table by
Updating the expected value
*/
int tefmod16_set_sc_speed(PHYMOD_ST* pc)  /* SET_SC_SPEED */
{
  int mapped_speed;
  SC_X4_CTLr_t SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg;
  TX_X4_ENC0r_t TX_X4_ENC0r_reg;
  RX_X4_PCS_CTL0r_t RX_X4_PCS_CTL0r_reg; 


  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  /* write 0 to the speed change */
  SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc, &SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, 0);
  PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

   /* Set speed and write 1 */
  SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);

  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, 1);
  tefmod16_get_mapped_speed(pc->spd_intf, &mapped_speed);
  if(pc->sc_mode == TEFMOD16_SC_MODE_HT_OVERRIDE || pc->sc_mode == TEFMOD_SC_MODE_ST_OVERRIDE) { mapped_speed = 2;
  }
  SC_X4_CTLr_SPEEDf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, mapped_speed);
  PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

  /*  Update the speed */
  pc->speed= mapped_speed;

  /*  Do lookup here */
  tefmod16_fill_lkup_exp(pc);

  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/*!
@brief tefmod16_init_pcs_fs , program PCS for Forced speed mode
@returns The value PHYMOD_E_NONE upon successful completion.
@details
If any SW override option switch is selected, it will call tier2 which for eg will convert from 10G to 20G XFI
Using HT, we do overrides of HT entries for eg will convert from 10G to 40G MLD
If none of the override options are selected, we just program the SC table
*/

int tefmod16_init_pcs_fs(PHYMOD_ST* pc)
{
  int or_value;
  int st_entry;
  int speed_entry;
  SC_X4_FLD_OVRR_EN0_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg;
  SC_X4_FLD_OVRR_EN1_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg;
  SC_X4_SC_X4_OVRRr_t  SC_X4_SC_X4_OVRRr_reg;
  TX_X2_CL82_0r_t TX_X2_CONTROL0_CL82_0r_reg;
  TX_X4_ENC0r_t TX_X4_CONTROL0_ENCODE_0r_reg;
  TX_X4_MISCr_t TX_X4_CONTROL0_MISCr_reg;
  RX_X4_PCS_CTL0r_t RX_X4_CONTROL0_PCS_CONTROL_0r_reg;
  RX_X4_BLKSYNC_CFGr_t RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg;
  TX_X4_LOOPCNTr_t TX_X4_CREDIT0_LOOPCNTr_reg;
  TX_X4_CRED1r_t TX_X4_CREDIT0_CREDIT1r_reg;
  TX_X4_MAC_CREDGENCNTr_t TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg;
  SC_X4_FLD_OVRR_EN2_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  if(pc->sc_mode == TEFMOD16_SC_MODE_ST_OVERRIDE) {

    /*Set the override enable*/
    /*override from 10G XFI to 20G XFI */
    configure_st_entry(pc->sw_table_entry, pc->speed,  pc);
  }
  
  if(pc->fc_mode) {
    /*write to address c062 and c133 Temp fix for override*/

    printf("PER_FIELD_OR is %x\n", pc->per_field_override_en);
    /*Set the override enable*/

    if(pc->per_field_override_en == 0x88000001) {
      SC_X4_FLD_OVRR_EN2_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg);
      SC_X4_FLD_OVRR_EN2_TYPEr_R_CL91_CW_SCRAMBLE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg, 1);
      PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN2_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg));

      RX_X4_CL91_RX_CTL0r_CLR(RX_X4_CL91_RX_CTL0r_reg);
      RX_X4_CL91_RX_CTL0r_CL91_CW_SCRAMBLEf_SET(RX_X4_CL91_RX_CTL0r_reg, 0);
      PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_CL91_RX_CTL0r(pc, RX_X4_CL91_RX_CTL0r_reg));

      SC_X4_FLD_OVRR_EN2_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg);
      SC_X4_FLD_OVRR_EN2_TYPEr_R_FIVE_BIT_XOR_EN_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg, 1);
      PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN2_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg));

      RX_X4_CL91_RX_CTL0r_CLR(RX_X4_CL91_RX_CTL0r_reg);
      RX_X4_CL91_RX_CTL0r_FIVE_BIT_XOR_ENf_SET(RX_X4_CL91_RX_CTL0r_reg, 1);
      PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_CL91_RX_CTL0r(pc, RX_X4_CL91_RX_CTL0r_reg));
    }
  }

  if(pc->sc_mode == TEFMOD16_SC_MODE_HT_OVERRIDE) {

    /*override from 10G XFI to 40G MLD */

    SC_X4_SC_X4_OVRRr_CLR(SC_X4_SC_X4_OVRRr_reg); 
    SC_X4_SC_X4_OVRRr_NUM_LANES_OVERRIDE_VALUEf_SET( SC_X4_SC_X4_OVRRr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_SC_X4_OVRRr(pc, SC_X4_SC_X4_OVRRr_reg));
      
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_NUM_LANES_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));

    /*write t_fifo_mode */
    or_value = 1;

    TX_X2_CL82_0r_CLR(TX_X2_CONTROL0_CL82_0r_reg);
    TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);
    TX_X4_ENC0r_T_FIFO_MODEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, or_value);

    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X2_CL82_0r(pc, TX_X2_CONTROL0_CL82_0r_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_T_FIFO_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));

     /*write t_enc_mode */
    or_value = 2;
    TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);
    TX_X4_ENC0r_T_ENC_MODEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc, TX_X4_CONTROL0_ENCODE_0r_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_T_ENC_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));


    /* scrambler mode*/
    or_value = 5;
    TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
    TX_X4_MISCr_SCR_MODEf_SET(TX_X4_CONTROL0_MISCr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_SCR_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));

    /*descrambler mode*/
    or_value = 2;
    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
    RX_X4_PCS_CTL0r_DESCR_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_DESCR_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));

    /*dec_tl_mode*/
    or_value = 2;
    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
    RX_X4_PCS_CTL0r_DEC_TL_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));

    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_DEC_TL_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));

    /*desjkew_mode*/
    or_value = 2;
    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
    RX_X4_PCS_CTL0r_DESKEW_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));
    /*Set the override enable*/
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_DESKEW_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));

    /*dec_fsm_mode*/
    /*Set the override enable*/
    or_value = 2;
    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
    RX_X4_PCS_CTL0r_DEC_FSM_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));
    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
    SC_X4_FLD_OVRR_EN0_TYPEr_DEC_FSM_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));

    /*bs_sm_sync_mode*/
    /*Set the override enable*/
    or_value = 0;
    RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);
    RX_X4_BLKSYNC_CFGr_BS_SM_SYNC_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_BS_SM_SYNC_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));

    /*bs_dist_mode*/
    /*Set the override enable*/
    or_value = 2;
    RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);
    RX_X4_BLKSYNC_CFGr_BS_DIST_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_BS_DIST_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));

      /*  Program resets and credits */
    /*Set the override enable*/
    or_value = 18;
    TX_X4_LOOPCNTr_CLR(TX_X4_CREDIT0_LOOPCNTr_reg);
    TX_X4_LOOPCNTr_LOOPCNT1f_SET(TX_X4_CREDIT0_LOOPCNTr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_LOOPCNTr(pc, TX_X4_CREDIT0_LOOPCNTr_reg));
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT1_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));

    or_value = 5;
    TX_X4_CRED1r_CLR(TX_X4_CREDIT0_CREDIT1r_reg);
    TX_X4_CRED1r_CLOCKCNT1f_SET(TX_X4_CREDIT0_CREDIT1r_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED1r(pc, TX_X4_CREDIT0_CREDIT1r_reg));
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT1_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));

    or_value = 2;
    TX_X4_MAC_CREDGENCNTr_CLR(TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg);
    TX_X4_MAC_CREDGENCNTr_MAC_CREDITGENCNTf_SET(TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg, or_value);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MAC_CREDGENCNTr(pc, TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg));
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
    SC_X4_FLD_OVRR_EN1_TYPEr_MAC_CREDITGENCNT_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
      
  }

  if(pc->sc_mode == TEFMOD16_SC_MODE_ST) {
    st_entry = pc->sw_table_entry & 0xff;
    speed_entry = (pc->sw_table_entry & 0xff00) >> 8;
    configure_st_entry(st_entry, speed_entry, pc);
  }
#ifdef _DV_TB_
  pc->adjust_port_mode = 1;
#endif
  tefmod16_set_sc_speed(pc);
#ifdef _DV_TB_
  pc->adjust_port_mode = 0;
#endif
  return PHYMOD_E_NONE;
}
#endif


/*!
@brief tefmod16_init_pcs_falcon , initializes PCS to the required speed ,  
@returns The value PHYMOD_E_NONE upon successful completion.
@details
if gloop and rloops are set, calls the corresponding routines
Sets watermark regs, AM timer regs, Higig en/dis controls
calls init_pcs FS or AN, 
*/
#ifdef _DV_TB_
int tefmod16_init_pcs_falcon(PHYMOD_ST* pc)
{
  /* Set_port_mode is common to both Autoneg and Forced_speed mode */
  int    bypass_int_cor;
  int    num_lane;

  PMD_X4_CTLr_t PMD_X4_PMD_X4_CONTROLr_reg;
  #ifdef TSCF_GEN1
  TX_X1_T_PMA_WTRMKr_t TX_X1_CONTROL0_T_PMA_WATERMARKr_reg;
  CL82_RX_AM_TMRr_t CL82_SHARED_CL82_RX_AM_TIMERr_reg;
  TX_X2_MLD_SWP_CNTr_t TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg;
  #else
  CL82_AM_TMRr_t CL82_SHARED_CL82_AM_TIMERr_reg;
  #endif
  RX_X1_CL91_CFGr_t RX_X1_CONTROL0_CL91_CONFIGr_reg;
  RX_X4_DEC_CTL0r_t RX_X4_CONTROL0_DECODE_CONTROL_0r_reg;
  TX_X4_ENC0r_t TX_X4_CONTROL0_ENCODE_0r_reg;
  SC_X4_FLD_OVRR_EN0_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg;
  SC_X4_FLD_OVRR_EN1_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg;
  AN_X4_INT_ENr_t AN_X4_INT_ENr_reg;
  MAIN0_SPD_CTLr_t MAIN0_SPD_CTLr_reg;
  TX_X4_TX_CTL0r_t TX_X4_TX_CTL0r_reg; 

  SC_X4_MSA_25G_50G_CTLr_t              SC_X4_MSA_25G_50G_CTLr_reg;
  SC_X4_IEEE_25G_CTLr_t                 SC_X4_IEEE_25G_CTLr_reg;
  RX_X4_IEEE_25G_PARLLEL_DET_CTRr_t     RX_X4_IEEE_25G_PARLLEL_DET_CTRr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  if(pc->tsc_touched == 0 && pc->pcs_lane_swap_en) {
    tefmod16_pcs_rx_lane_swap(pc, pc->pcs_log_to_phy_rx_lane_swap);
    tefmod16_pcs_tx_lane_swap(pc, pc->pcs_log_to_phy_tx_lane_swap);
  }
 
  if(pc->tsc_touched == 0) {
    MAIN0_SPD_CTLr_CLR(MAIN0_SPD_CTLr_reg);
    MAIN0_SPD_CTLr_TSC_CREDIT_SELf_SET(MAIN0_SPD_CTLr_reg, pc->credit_sel);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SPD_CTLr(pc, MAIN0_SPD_CTLr_reg));
    #ifdef _DV_TB_
    PHYMOD_DEBUG_ERROR(("FCSN_Info : tefmod16_init_pcs_falcon : fc_mode %0d : tsc_credit_sel %0d\n", pc->fc_mode, pc->credit_sel));
    #endif
  }
  if ((pc->spd_intf == TEFMOD16_SPD_100G_MLD_X4) ||
      (pc->spd_intf == TEFMOD16_SPD_106G_MLD_HG_X4)) {
    TX_X4_TX_CTL0r_CLR(TX_X4_TX_CTL0r_reg);
    TX_X4_TX_CTL0r_T_PMA_WATERMARKf_SET( TX_X4_TX_CTL0r_reg , 7);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_TX_CTL0r(pc,TX_X4_TX_CTL0r_reg)); 

    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg); 
    SC_X4_FLD_OVRR_EN1_TYPEr_T_PMA_WATERMARK_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc,SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
   
  }
  else {
    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg); 
    SC_X4_FLD_OVRR_EN1_TYPEr_T_PMA_WATERMARK_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc,SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
  } 

  switch(pc->port_type) {
    case TEFMOD16_MULTI_PORT: num_lane = 1; break;
    case TEFMOD16_TRI1_PORT: if(pc->this_lane == 2) num_lane = 2;
                           else num_lane = 1;
                           break;
    case TEFMOD16_TRI2_PORT: if(pc->this_lane == 0) num_lane = 2;
                           else num_lane = 1;
                           break;
    case TEFMOD16_DXGXS: num_lane = 2; break;
    case TEFMOD16_SINGLE_PORT: num_lane = 4; break;
    default : num_lane = 0;
  }

  if(pc->pcs_gloop_en != 0) {
    tefmod16_tx_loopback_control(pc, pc->pcs_gloop_en, pc->this_lane, num_lane);
  }

  if(pc->pcs_rloop_en != 0) {
    tefmod16_rx_loopback_control(pc, pc->pcs_rloop_en, pc->this_lane, num_lane);
  }

  /* FCSN */
  if(pc->fc_mode) {
    #ifdef _DV_TB_
    PHYMOD_DEBUG_ERROR(("FCSN_Info : tefmod16_init_pcs_falcon : port %0d : fc_mode %0d : tx_fcsn_mode %02x : rx_fcsn_mode %02x\n", pc->this_lane, pc->fc_mode, pc->tx_fcsn_mode, pc->rx_fcsn_mode));
    #endif

    if(pc->pmd_touched == 0) {
      pc->pmd_reset_control = 0x1;
      tefmod16_pmd_core_reset_bypass(pc, pc->pmd_reset_control);
    }
    pc->pmd_reset_control = 0x10;
    tefmod16_pmd_lane_reset_bypass(pc, pc->pmd_reset_control);
  } 

  if(pc->pmd_rloop_en != 0) {
    tefmod16_rx_pmd_loopback_control(pc, pc->pcs_rloop_en, pc->pmd_rloop_en, pc->this_lane);
  }
  tefmod16_update_port_mode_select(pc, pc->port_type, pc->this_lane, pc->tsc_clk_freq_pll_by_48, pc->pll_reset_en);
  pc->per_lane_control = 3;
 
  PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);

  PMD_X4_CTLr_LN_RX_H_PWRDNf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 0);
  PMD_X4_CTLr_LN_TX_H_PWRDNf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 0);
  
  PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));
  
  PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);
  PMD_X4_CTLr_LN_H_RSTBf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 1);
  PMD_X4_CTLr_LN_DP_H_RSTBf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 1);

  PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));

  #ifdef TSCF_GEN1
  TX_X1_T_PMA_WTRMKr_CLR(TX_X1_CONTROL0_T_PMA_WATERMARKr_reg);
  TX_X1_T_PMA_WTRMKr_FIVE_LANE_BITMUX_WATERMARKf_SET(TX_X1_CONTROL0_T_PMA_WATERMARKr_reg, 7);
  TX_X1_T_PMA_WTRMKr_TWO_LANE_BITMUX_WATERMARKf_SET(TX_X1_CONTROL0_T_PMA_WATERMARKr_reg, 5);
  TX_X1_T_PMA_WTRMKr_SINGLE_LANE_BITMUX_WATERMARKf_SET(TX_X1_CONTROL0_T_PMA_WATERMARKr_reg, 4);

  PHYMOD_IF_ERR_RETURN(MODIFY_TX_X1_T_PMA_WTRMKr(pc, TX_X1_CONTROL0_T_PMA_WATERMARKr_reg));
  #endif

  if(pc->cl91_en != 0 && pc->an_en != 1) {
    bypass_int_cor = 0;
    if(pc->cl91_bypass_indication) {
      bypass_int_cor |= TEFMOD16_CL91_IND_ONLY_EN_DIS;
    }
    if(pc->cl91_bypass_correction) {
      bypass_int_cor |= TEFMOD16_CL91_COR_ONLY_EN_DIS;
    }
    tefmod16_FEC_control(pc, (TEFMOD16_CL91_TX_EN_DIS|TEFMOD_CL91_RX_EN_DIS|bypass_int_cor), 0, 0);
  } else if(pc->cl91_en == 0 && pc->an_en != 1) {
    tefmod16_FEC_control(pc, 0, (TEFMOD16_CL91_TX_EN_DIS|TEFMOD_CL91_RX_EN_DIS), 0);
  }

  if(pc->cl91_nofec == 1) {
    tefmod16_FEC_control(pc, TEFMOD16_CL74_CL91_EN_DIS, 0, 0);
  }
  if(pc->cl91_cl74 == 1) {
    tefmod16_FEC_control(pc, TEFMOD16_CL74_CL91_EN_DIS, 0, 1);
  }
  
  tefmod16_tx_lane_control(pc, pc->per_lane_control&1, 0);
  tefmod16_rx_lane_control(pc, pc->accData, pc->per_lane_control);


  RX_X1_CL91_CFGr_CLR(RX_X1_CONTROL0_CL91_CONFIGr_reg);
  if(pc->rx_am_timer_init_val == 0x400) {
    printf("CL91_AM_SPACING_1024 set.\n");
    RX_X1_CL91_CFGr_CL91_AM_SPACING_1024f_SET(RX_X1_CONTROL0_CL91_CONFIGr_reg, 1);
  }

  #ifdef TSC_MT2
  printf("CL91_FC_LOCK_CORR_CW set.\n");
  RX_X1_CL91_CFGr_CL91_FC_LOCK_CORR_CWf_SET(RX_X1_CONTROL0_CL91_CONFIGr_reg, 1);
  #else
  printf("CL91_FC_LOCK_CORR_CW clr.\n");
  RX_X1_CL91_CFGr_CL91_FC_LOCK_CORR_CWf_SET(RX_X1_CONTROL0_CL91_CONFIGr_reg, 0);
  #endif
  PHYMOD_IF_ERR_RETURN(MODIFY_RX_X1_CL91_CFGr(pc, RX_X1_CONTROL0_CL91_CONFIGr_reg));
  
  tefmod16_cl74_control_fec_0(pc);
  tefmod16_cl74_control_fec_1(pc);
  tefmod16_cl74_control_fec_2(pc);
  tefmod16_cl74_control_fec_3(pc);

  /* cfg_rx_am_timer_init_val=3ff cfg_tx_am_timer_init_val=ffc */
  #ifdef TSCF_GEN1
  CL82_RX_AM_TMRr_CLR(CL82_SHARED_CL82_RX_AM_TIMERr_reg);
  TX_X2_MLD_SWP_CNTr_CLR(TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg);

  
  if(pc->rx_am_timer_init_val != 0x3fff) {
    CL82_RX_AM_TMRr_SET(CL82_SHARED_CL82_RX_AM_TIMERr_reg, pc->rx_am_timer_init_val);
    PHYMOD_IF_ERR_RETURN(WRITE_CL82_RX_AM_TMRr(pc, CL82_SHARED_CL82_RX_AM_TIMERr_reg));
  }
  if(pc->tx_am_timer_init_val != 0xffff) {
    TX_X2_MLD_SWP_CNTr_SET(TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg, pc->tx_am_timer_init_val);
    PHYMOD_IF_ERR_RETURN(WRITE_TX_X2_MLD_SWP_CNTr(pc, TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg));
  }
  #else
  CL82_AM_TMRr_CLR(CL82_SHARED_CL82_AM_TIMERr_reg);
  if(pc->rx_am_timer_init_val != 0x3fff) {
    CL82_AM_TMRr_SET(CL82_SHARED_CL82_AM_TIMERr_reg, pc->rx_am_timer_init_val);
    PHYMOD_IF_ERR_RETURN(WRITE_CL82_AM_TMRr(pc, CL82_SHARED_CL82_AM_TIMERr_reg));
  }

  #endif

  /* +cfg_rx_higig2_oen +cfg_tx_higig2_oen */
    /* Set t_HG2_ENABLE_oen bit 10 in 0xc060 */
   SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
   SC_X4_FLD_OVRR_EN0_TYPEr_T_HG2_ENABLE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, pc->tx_higig2_oen & 0x1);
   PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));

   /* Clr/Set HG2_ENABLE bit 10 in 0xc111 */
   TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);
   TX_X4_ENC0r_HG2_ENABLEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, pc->tx_higig2_en);
   PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc,TX_X4_CONTROL0_ENCODE_0r_reg));

  /* +cfg_tefmod16_use_higig2_knobs +cfg_tx_higig2_codec */
  if(pc->tefmod16_use_higig2_knobs == 0x1) {
    TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);
    TX_X4_ENC0r_HG2_CODECf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, pc->tx_higig2_codec);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc,TX_X4_CONTROL0_ENCODE_0r_reg));
  }

   /*CHAK setting tx scramble idle test mode*/
   
   if(pc->scramble_idle_tx_test_mode==0x1) {
     printf("CHAK:eabling tx testmode cfg");
     TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);
     TX_X4_ENC0r_TX_TEST_MODE_CFGf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, pc->scramble_idle_tx_test_mode);
     PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc,TX_X4_CONTROL0_ENCODE_0r_reg));

   }
    /* Set r_HG2_ENABLE_oen bit 15 in 0xc061 */
   SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
   SC_X4_FLD_OVRR_EN1_TYPEr_R_HG2_ENABLE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, pc->rx_higig2_oen & 0x1);
   PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));

   /* Clr/Set HG2_ENABLE bit 12 in 0xc134 */
   RX_X4_DEC_CTL0r_CLR(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg);
   RX_X4_DEC_CTL0r_HG2_ENABLEf_SET(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg, pc->rx_higig2_en);
   PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_DEC_CTL0r(pc, RX_X4_CONTROL0_DECODE_CONTROL_0r_reg));

    /* CHAK:set rx_test_mode to 1 */
   if(pc->scramble_idle_rx_test_mode == 0x1) {
     printf("CHAK:enabling testmode in rx");
     RX_X4_DEC_CTL0r_CLR(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg);
     RX_X4_DEC_CTL0r_R_TEST_MODE_CFGf_SET(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg, pc->scramble_idle_rx_test_mode);
     PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_DEC_CTL0r(pc, RX_X4_CONTROL0_DECODE_CONTROL_0r_reg));
   }

  /* +cfg_tefmod16_use_higig2_knobs +cfg_rx_higig2_codec */
  if(pc->tefmod16_use_higig2_knobs == 0x1) {
    RX_X4_DEC_CTL0r_CLR(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg);
    RX_X4_DEC_CTL0r_HG2_CODECf_SET(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg, pc->rx_higig2_codec);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_DEC_CTL0r(pc, RX_X4_CONTROL0_DECODE_CONTROL_0r_reg));
  }
  
#ifdef _DV_TB_
  RX_X4_DEC_CTL0r_CLR(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg);
  RX_X4_DEC_CTL0r_DISABLE_CL49_BERMONf_SET(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg, pc->disable_cl49_bermon);
  PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_DEC_CTL0r(pc, RX_X4_CONTROL0_DECODE_CONTROL_0r_reg));
#endif 



   /* MSA Related configurations */
  printf("In tefmod16_init_pcs_falcon and configuring the MSA registers msa_25G_am0_mode:%0d msa_25G_am123_mode:%0d msa_50G_am0_mode:%0d msa_50G_am123_mode:%0d cl74_shcorrupt_50msa: %0d cl74_shcorrupt_25msa :%0d \n",
                                                                  pc->msa_25G_am0_mode,pc->msa_25G_am123_mode,pc->msa_50G_am0_mode,pc->msa_50G_am123_mode,pc->cl74_shcorrupt_50msa,pc->cl74_shcorrupt_25msa);
  SC_X4_MSA_25G_50G_CTLr_CLR(SC_X4_MSA_25G_50G_CTLr_reg);
  SC_X4_MSA_25G_50G_CTLr_MSA_25G_CWSCR_ENf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_25G_cw_scr_en);
  SC_X4_MSA_25G_50G_CTLr_MSA_25G_5BIT_XORf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_25G_5bit_XOR); 
  SC_X4_MSA_25G_50G_CTLr_MSA_25G_AM_ENf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_25G_am_en);
  SC_X4_MSA_25G_50G_CTLr_MSA_25G_AM0_FORMATf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_25G_am0_mode);
  SC_X4_MSA_25G_50G_CTLr_MSA_25G_AM123_FORMATf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_25G_am123_mode);
  SC_X4_MSA_25G_50G_CTLr_MSA_50G_AM0_FORMATf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_50G_am0_mode);
  SC_X4_MSA_25G_50G_CTLr_MSA_50G_AM123_FORMATf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_50G_am123_mode);
  SC_X4_MSA_25G_50G_CTLr_MSA_WINDOW_SELf_SET(SC_X4_MSA_25G_50G_CTLr_reg,pc->msa_window_sel);
  SC_X4_MSA_25G_50G_CTLr_MSA_COUNT_SELf_SET(SC_X4_MSA_25G_50G_CTLr_reg,pc->msa_count_sel);
  SC_X4_MSA_25G_50G_CTLr_CL74_SHCORRUPT_50GMSAf_SET(SC_X4_MSA_25G_50G_CTLr_reg,pc->cl74_shcorrupt_50msa);
  SC_X4_MSA_25G_50G_CTLr_CL74_SHCORRUPT_25GMSAf_SET(SC_X4_MSA_25G_50G_CTLr_reg,pc->cl74_shcorrupt_25msa);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_MSA_25G_50G_CTLr(pc, SC_X4_MSA_25G_50G_CTLr_reg));
  

 
  /* IEEE Related configurations */
  printf("In tefmod16_init_pcs_falcon and configuring the IEEE registers ieee_25G_am0_mode:%0d ieee_25G_am123_mode:%0d 25G shcorrupt :%0d \n",pc->ieee_25G_am0_mode,pc->ieee_25G_am123_mode,pc->cl74_shcorrupt_25ieee);
  SC_X4_IEEE_25G_CTLr_CLR(SC_X4_IEEE_25G_CTLr_reg);
  SC_X4_IEEE_25G_CTLr_IEEE_25G_CWSCR_ENf_SET(SC_X4_IEEE_25G_CTLr_reg, pc->ieee_25G_cw_scr_en);
  SC_X4_IEEE_25G_CTLr_IEEE_25G_5BIT_XORf_SET(SC_X4_IEEE_25G_CTLr_reg, pc->ieee_25G_5bit_XOR);
  SC_X4_IEEE_25G_CTLr_IEEE_25G_AM_ENf_SET(SC_X4_IEEE_25G_CTLr_reg, pc->ieee_25G_am_en);
  SC_X4_IEEE_25G_CTLr_IEEE_25G_AM0_FORMATf_SET(SC_X4_IEEE_25G_CTLr_reg, pc->ieee_25G_am0_mode);
  SC_X4_IEEE_25G_CTLr_IEEE_25G_AM123_FORMATf_SET(SC_X4_IEEE_25G_CTLr_reg, pc->ieee_25G_am123_mode);
  SC_X4_IEEE_25G_CTLr_IEEE_WINDOW_SELf_SET(SC_X4_IEEE_25G_CTLr_reg,pc->ieee_window_sel);
  SC_X4_IEEE_25G_CTLr_IEEE_COUNT_SELf_SET(SC_X4_IEEE_25G_CTLr_reg,pc->ieee_count_sel);
  SC_X4_IEEE_25G_CTLr_CL74_SHCORRUPT_25IEEEf_SET(SC_X4_IEEE_25G_CTLr_reg,pc->cl74_shcorrupt_25ieee);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_IEEE_25G_CTLr(pc, SC_X4_IEEE_25G_CTLr_reg));

  /*
  SC_X4_IEEE_25G_CTLr_IEEE_WINDOW_SELf_SET(SC_X4_IEEE_25G_CTLr_reg,);
  SC_X4_IEEE_25G_CTLr_IEEE_COUNT_SELf_SET(SC_X4_IEEE_25G_CTLr_reg,);
  SC_X4_IEEE_25G_CTLr_CL74_SHCORRUPT_25IEEEf_SET(SC_X4_IEEE_25G_CTLr_reg,);
  */

  /* Auto detect settings */
  printf("In tefmod16_init_pcs_falcon and configuring the parallel detect registers msa_ieee_det_en:%0x \n",pc->msa_ieee_det_en);
  RX_X4_IEEE_25G_PARLLEL_DET_CTRr_CLR(RX_X4_IEEE_25G_PARLLEL_DET_CTRr_reg);
  RX_X4_IEEE_25G_PARLLEL_DET_CTRr_MSA_IEEE_DET_ENf_SET(RX_X4_IEEE_25G_PARLLEL_DET_CTRr_reg, pc->msa_ieee_det_en);
  PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_IEEE_25G_PARLLEL_DET_CTRr(pc, RX_X4_IEEE_25G_PARLLEL_DET_CTRr_reg));




  /*if FEC enabled */
  if(pc->t_fec_enable) {
    tefmod16_FEC_control(pc, (TEFMOD16_CL74_TX_EN_DIS|TEFMOD_CL74_RX_EN_DIS), 0, 0);
  }

  /* Programming the an_good_check_entry, an_completed_sw, ld_page_req, lp_page_rdy interrupt enable    */
  AN_X4_INT_ENr_CLR(AN_X4_INT_ENr_reg);
  AN_X4_INT_ENr_AN_GOOD_CHK_ENf_SET(AN_X4_INT_ENr_reg, pc->an_gc_entry_int_en);
  AN_X4_INT_ENr_AN_COMPLETED_SW_ENf_SET(AN_X4_INT_ENr_reg, pc->sw_an_completed_en);
  AN_X4_INT_ENr_LD_PAGE_REQ_ENf_SET(AN_X4_INT_ENr_reg, pc->ld_page_req_en);
  AN_X4_INT_ENr_LP_PAGE_RDY_ENf_SET(AN_X4_INT_ENr_reg, pc->lp_page_rdy_en);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_INT_ENr(pc, AN_X4_INT_ENr_reg));


  /* call FS or AN mode */
  if(pc->an_en) {
    tefmod16_init_pcs_an(pc);
  } else {
    tefmod16_init_pcs_fs(pc);
  }
  if(pc->pmd_gloop_en != 0) {
    tefmod16_tx_pmd_loopback_control(pc, pc->pmd_gloop_en);
  }


  if(pc->cl72_training_restart || pc->cl72_training_enabled) {
    pc->per_lane_control = pc->cl72_training_restart | (pc->cl72_training_enabled<<1); 
    tefmod16_clause72_control(pc, pc->per_lane_control, pc->port_type);
  }
  return PHYMOD_E_NONE;
}
#endif

/*!
@brief tefmod16_poll_for_sc_done , polls for the sc_done bit ,  
@returns The value PHYMOD_E_NONE upon successful completion.
@details
Reads the SC_X4_COntrol_status reg and polls it to see if sc_done bit is set
Sets the sc_done flag=1/0, if speed control is done, after resolving to the correct speed
*/
int tefmod16_poll_for_sc_done(PHYMOD_ST* pc, int mapped_speed)
{
  int done;
  int spd_match, sc_done;
  int cnt;
  uint16_t data;

  SC_X4_STSr_t SC_X4_CONTROL_SC_X4_CONTROL_STATUSr_reg;
  SC_X4_RSLVD_SPDr_t SC_X4_FINAL_CONFIG_STATUS_RESOLVED_SPEEDr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  cnt=0;
  sc_done = 0;
  spd_match = 0;

  SC_X4_STSr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_STATUSr_reg);
  while (cnt <=2000) {
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_STSr(pc, &SC_X4_CONTROL_SC_X4_CONTROL_STATUSr_reg));
    cnt = cnt + 1;
    if(SC_X4_STSr_SW_SPEED_CHANGE_DONEf_GET(SC_X4_CONTROL_SC_X4_CONTROL_STATUSr_reg)) {
      sc_done = 1;
      break;
    } else {
      if(cnt == 2000) {
        sc_done = 0;
        break;
      }
    }
  }

  cnt=0;
  while (cnt <=400) {
    SC_X4_RSLVD_SPDr_CLR(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_SPEEDr_reg);
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD_SPDr(pc, &SC_X4_FINAL_CONFIG_STATUS_RESOLVED_SPEEDr_reg));
    data = SC_X4_RSLVD_SPDr_SPEEDf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_SPEEDr_reg);
#ifdef _DV_TB_
    if(pc->verbosity)
     PHYMOD_DEBUG_ERROR(("Resolved speed is %x :: need %x :: uint :: %d :: lane :: %d\n", data, mapped_speed, pc->unit, pc->this_lane));
#endif
    cnt = cnt + 1;
    if(data == mapped_speed) {
      spd_match = 1;
      break;
    } else {
      if(cnt == 400) {
        spd_match = 0;
        break;
      }
    }
  } 
  if(sc_done && spd_match) {
    done = 1;
  } else {
    done = 0;
  }
  return done;
}

/*!
@brief tefmod16_check_status , checks the statistics port_mode,  
@returns The value PHYMOD_E_NONE upon successful completion.
@details
Populates the HT entries, based on the speed 
In this routine, the following regs are read:RESOLVED_0r_STATUS to RESOLVED_5r_STATUS
The read value from these are what is the actual (RTL sends this)and whatever was stored in lookup is what DV expects
Sets the sc_done flag=1, if there are no mismatches
*/

#ifdef _DV_TB_
int tefmod16_check_status(PHYMOD_ST* pc)
{
  uint16_t fail;
  int  actual_speed;
  int  mapped_speed;
  int spd_change_done;
  PHYMOD_ST  sc_stats_actual;
  PHYMOD_ST*  sc_stats_print_actual;
  SC_X4_RSLVD_SPDr_t SC_X4_FINAL_CONFIG_STATUS_RESOLVED_SPEEDr_reg;
  SC_X4_RSLVD0r_t SC_X4_FINAL_CONFIG_STATUS_RESOLVED_0r_reg;
  SC_X4_RSLVD1r_t SC_X4_FINAL_CONFIG_STATUS_RESOLVED_1r_reg;
  SC_X4_RSLVD2r_t SC_X4_FINAL_CONFIG_STATUS_RESOLVED_2r_reg;
  SC_X4_RSLVD3r_t SC_X4_FINAL_CONFIG_STATUS_RESOLVED_3r_reg;
  SC_X4_RSLVD4r_t SC_X4_FINAL_CONFIG_STATUS_RESOLVED_4r_reg;
  SC_X4_RSLVD5r_t SC_X4_FINAL_CONFIG_STATUS_RESOLVED_5r_reg;
  /*SC_X4_RSLVD6r_t SC_X4_FINAL_CONFIG_STATUS_RESOLVED_6r_reg;*/
  SC_X4_RSLVD7r_t SC_X4_FINAL_CONFIG_STATUS_RESOLVED_7r_reg;
  SC_X4_RSLVD8r_t SC_X4_FINAL_CONFIG_STATUS_RESOLVED_8r_reg; 
  SC_X4_FEC_STSr_t SC_X4_FINAL_CONFIG_STATUS_FEC_STATUSr_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  spd_change_done = 0;


  if(!pc->an_en) {
    tefmod16_get_mapped_speed(pc->spd_intf, &mapped_speed);
    pc->speed = mapped_speed;
    spd_change_done = tefmod16_poll_for_sc_done(pc, pc->speed);
    if(spd_change_done) {
     printf("Speed change done for :: unit %0d port %0d \n", pc->unit, pc->this_lane);
    }
  }

  printf("\nSTATS_Info : tefmod16_check_status (1a) : pc->an_en %01x : pc->speed %02x : pc->an_fec_sel_override %01x : pc->cl91_en %01x : pc->an_fec %01x : pc->bam_cl74_fec %01x : pc->t_fec_enable %1x : pc->cl91_nofec %1x : pc->cl91_cl74 %1x\n", pc->an_en, pc->speed, pc->an_fec_sel_override, pc->cl91_en, pc->an_fec, pc->bam_cl74_fec, pc->t_fec_enable, pc->t_fec_enable, pc->cl91_nofec, pc->cl91_cl74) ;

  printf("STATS_Info : tefmod16_check_status (1b) : pc->sc_mode %2d : pc->fc_mode %1x : pc->fc_rx_spd_intf %02d : pc->fc_tx_spd_intf %02d\n", pc->sc_mode, pc->fc_mode, pc->fc_rx_spd_intf, pc->fc_tx_spd_intf) ;

  printf("STATS_Info : tefmod16_check_status (1c) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;

  if(pc->an_en) {
   
     if(pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_CR4     || 
        pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_KR4     ||
        pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_HG2_CR4 ||  
        pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_HG2_KR4 
       ) {
          pc->cl91_en = 1;
         }
  
    if(cl91_valid_speed(pc)) {
       printf("Resolved speed is %0d an_fec_sel_override %0d \n", pc->speed, pc->an_fec_sel_override); 
       if(pc->an_fec_sel_override == 1) {
         pc->cl91_en      = 0;
         pc->an_hcd_fec       = 0;
       }
       else if(pc->an_fec_sel_override == 2) {
         pc->cl91_en      = 0;
         pc->an_hcd_fec       = 1;
       }
       else if(pc->an_fec_sel_override == 3) {
         pc->cl91_en      = 1;
         pc->an_hcd_fec       = 0;
       }
    }
  }

  printf("STATS_Info : tefmod16_check_status (2a) : pc->an_en %01x : pc->speed %02x : pc->an_fec_sel_override %01x : pc->cl91_en %01x : pc->an_fec %01x : pc->bam_cl74_fec %01x : pc->t_fec_enable %1x : pc->cl91_nofec %1x : pc->cl91_cl74 %1x\n", pc->an_en, pc->speed, pc->an_fec_sel_override, pc->cl91_en, pc->an_fec, pc->bam_cl74_fec, pc->t_fec_enable, pc->t_fec_enable, pc->cl91_nofec, pc->cl91_cl74) ;

  printf("STATS_Info : tefmod16_check_status (2b) : pc->sc_mode %2d : pc->fc_mode %1x : pc->fc_rx_spd_intf %02d : pc->fc_tx_spd_intf %02d\n", pc->sc_mode, pc->fc_mode, pc->fc_rx_spd_intf, pc->fc_tx_spd_intf) ;

  printf("STATS_Info : tefmod16_check_status (2c) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;

  tefmod16_get_ht_entries(pc); /* Must be after an_fec_sel_override code */

  printf("\nSTATS_Info : tefmod16_check_status (3a) : pc->an_en %01x : pc->speed %02x : pc->an_fec_sel_override %01x : pc->cl91_en %01x : pc->an_fec %01x : pc->bam_cl74_fec %01x : pc->t_fec_enable %1x : pc->cl91_nofec %1x : pc->cl91_cl74 %1x\n", pc->an_en, pc->speed, pc->an_fec_sel_override, pc->cl91_en, pc->an_fec, pc->bam_cl74_fec, pc->t_fec_enable, pc->t_fec_enable, pc->cl91_nofec, pc->cl91_cl74) ;

  printf("STATS_Info : tefmod16_check_status (3b) : pc->sc_mode %2d : pc->fc_mode %1x : pc->fc_rx_spd_intf %02d : pc->fc_tx_spd_intf %02d\n", pc->sc_mode, pc->fc_mode, pc->fc_rx_spd_intf, pc->fc_tx_spd_intf) ;

  printf("STATS_Info : tefmod16_check_status (3c) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;

  if(pc->sc_mode == TEFMOD16_SC_MODE_ST_OVERRIDE) {
     pc->os_mode=0;
     pc->clkcnt1 = 0x5;
     pc->lpcnt0 = 0x1;
     pc->lpcnt1 = 0x12;
     pc->cgc = 0x4;
     pc->cl72_en = 0;
  }

  if(pc->sc_mode == TEFMOD16_SC_MODE_AN_CL73) {
    pc->speed = 0x01;
  }


    printf("STATS_Info : tefmod16_check_status (4a) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;


  /* This code below is used only in the case where AN resolves to 100G, and you want to
  * override the default 100G FEC */
  if(pc->cl91_nofec != 0 || pc->cl91_cl74) {
    pc->t_pma_btmx_mode = 2;
    pc->deskew_mode = 4;
    pc->bs_sync_en = 1;
    pc->bs_dist_mode = 3;
    pc->bs_btmx_mode = 2;

    printf("STATS_Info : tefmod16_check_status (4d) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;
  }

  if(pc->cl91_cl74 != 0) {
    pc->bs_sync_en = 0;

    printf("STATS_Info : tefmod16_check_status (4e) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;
  }

  if(pc->cl91_en != 0) {
    if(pc->speed == 0x30 || pc->speed == 0x31 || pc->speed == 0x32 ||
       pc->speed == 0x34 || pc->speed == 0x35 || pc->speed == 0x36) {
      pc->deskew_mode = 8;
      pc->bs_dist_mode = 0;
      pc->bs_btmx_mode = 0;

      printf("STATS_Info : tefmod16_check_status (4f) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;
    }

    if(pc->speed == 0x10 || pc->speed == 0x11 || pc->speed == 0x12 ||
       pc->speed == 0x14 || pc->speed == 0x15 || pc->speed == 0x16) {
      if(pc->msa_25G_am_en == 1) {
        pc->t_fifo_mode = 1;
        pc->deskew_mode = 7;
        pc->bs_sm_sync_mode = 0;
      }
      else {
        pc->t_fifo_mode = 0;
        pc->deskew_mode = 9;
        pc->bs_sm_sync_mode = 0;
      }

      printf("STATS_Info : tefmod16_check_status (4g) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;
    }

    if(pc->speed == 0x70 || pc->speed == 0x71 || pc->speed == 0x72 || pc->speed == 0x73) {
      if(pc->ieee_25G_am_en == 1) {
        pc->t_fifo_mode = 1;
        pc->deskew_mode = 7;
        pc->bs_sm_sync_mode = 0;
      }
      else {
        pc->t_fifo_mode = 0;
        pc->deskew_mode = 9;
        pc->bs_sm_sync_mode = 0;
      }

      printf("STATS_Info : tefmod16_check_status (4h) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;
    }

    if(pc->fc_rx_spd_intf == TEFMOD16_SPD_32G_FC || pc->fc_tx_spd_intf == TEFMOD_SPD_32G_FC) {
      if(pc->fc_mode == 1) {
        pc->t_fifo_mode = 0;
        pc->deskew_mode = 9;
        pc->bs_sm_sync_mode = 0;

        printf("STATS_Info : tefmod16_check_status (4i) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;
      }
    }
  }

  printf("\nSTATS_Info : tefmod16_check_status (5a) : pc->an_en %01x : pc->speed %02x : pc->an_fec_sel_override %01x : pc->cl91_en %01x : pc->an_fec %01x : pc->bam_cl74_fec %01x : pc->t_fec_enable %1x : pc->cl91_nofec %1x : pc->cl91_cl74 %1x\n", pc->an_en, pc->speed, pc->an_fec_sel_override, pc->cl91_en, pc->an_fec, pc->bam_cl74_fec, pc->t_fec_enable, pc->t_fec_enable, pc->cl91_nofec, pc->cl91_cl74) ;

  printf("STATS_Info : tefmod16_check_status (5b) : pc->sc_mode %2d : pc->fc_mode %1x : pc->fc_rx_spd_intf %02d : pc->fc_tx_spd_intf %02d\n", pc->sc_mode, pc->fc_mode, pc->fc_rx_spd_intf, pc->fc_tx_spd_intf) ;

  printf("STATS_Info : tefmod16_check_status (5c) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;

  /* Read resolved speed */

  SC_X4_RSLVD_SPDr_CLR(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_SPEEDr_reg);
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD_SPDr(pc, &SC_X4_FINAL_CONFIG_STATUS_RESOLVED_SPEEDr_reg));

  sc_stats_actual.num_lanes = SC_X4_RSLVD_SPDr_NUM_LANESf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_SPEEDr_reg);

  actual_speed = SC_X4_RSLVD_SPDr_SPEEDf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_SPEEDr_reg); 


  /*   RESOLVED 0    */
  /* cl72_en, scr_mode, os_mode, enc_mode, fifo_mode, HG2_en, t_pma_btmx_mode */

  PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD0r(pc, &SC_X4_FINAL_CONFIG_STATUS_RESOLVED_0r_reg));

  sc_stats_actual.cl72_en = SC_X4_RSLVD0r_CL72_ENf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_0r_reg);
  sc_stats_actual.os_mode = SC_X4_RSLVD0r_OS_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_0r_reg);
  sc_stats_actual.t_fifo_mode = SC_X4_RSLVD0r_T_FIFO_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_0r_reg);
  sc_stats_actual.t_enc_mode = SC_X4_RSLVD0r_T_ENC_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_0r_reg);
  sc_stats_actual.t_HG2_ENABLE = SC_X4_RSLVD0r_T_HG2_ENABLEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_0r_reg);
  sc_stats_actual.t_pma_btmx_mode = SC_X4_RSLVD0r_T_PMA_BTMX_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_0r_reg);
  sc_stats_actual.scr_mode = SC_X4_RSLVD0r_SCR_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_0r_reg);

  /*   RESOLVED 1    */
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD1r(pc, &SC_X4_FINAL_CONFIG_STATUS_RESOLVED_1r_reg));
  sc_stats_actual.descr_mode = SC_X4_RSLVD1r_DESCR_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_1r_reg);
  sc_stats_actual.dec_tl_mode = SC_X4_RSLVD1r_DEC_TL_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_1r_reg);
  sc_stats_actual.deskew_mode = SC_X4_RSLVD1r_DESKEW_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_1r_reg);
  sc_stats_actual.dec_fsm_mode = SC_X4_RSLVD1r_DEC_FSM_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_1r_reg);
  #ifdef TSCF_GEN1
  sc_stats_actual.r_HG2_ENABLE = SC_X4_RSLVD1r_R_HG2_ENABLEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_1r_reg);
  #endif
  sc_stats_actual.bs_sm_sync_mode = SC_X4_RSLVD1r_BS_SM_SYNC_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_1r_reg);
  sc_stats_actual.bs_sync_en   = SC_X4_RSLVD1r_BS_SYNC_ENf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_1r_reg);
  sc_stats_actual.bs_dist_mode   = SC_X4_RSLVD1r_BS_DIST_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_1r_reg);
  sc_stats_actual.bs_btmx_mode   = SC_X4_RSLVD1r_BS_BTMX_MODEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_1r_reg);


             /*RESOLVED 2   */ 
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD2r(pc, &SC_X4_FINAL_CONFIG_STATUS_RESOLVED_2r_reg));
  sc_stats_actual.clkcnt0   = SC_X4_RSLVD2r_CLOCKCNT0f_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_2r_reg);
             /*RESOLVED 3   */ 
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD3r(pc, &SC_X4_FINAL_CONFIG_STATUS_RESOLVED_3r_reg));
  sc_stats_actual.clkcnt1   = SC_X4_RSLVD3r_CLOCKCNT1f_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_3r_reg);
             /*RESOLVED 4   */ 
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD4r(pc, &SC_X4_FINAL_CONFIG_STATUS_RESOLVED_4r_reg));
  sc_stats_actual.lpcnt0   = SC_X4_RSLVD4r_LOOPCNT0f_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_4r_reg);
  sc_stats_actual.lpcnt1   = SC_X4_RSLVD4r_LOOPCNT1f_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_4r_reg);
             /*RESOLVED 5   */ 
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD5r(pc, &SC_X4_FINAL_CONFIG_STATUS_RESOLVED_5r_reg));
  sc_stats_actual.cgc   = SC_X4_RSLVD5r_MAC_CREDITGENCNTf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_5r_reg);

  /*RESOVLED 7 */
  #ifndef TSCF_GEN1
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD7r(pc, &SC_X4_FINAL_CONFIG_STATUS_RESOLVED_7r_reg));
  sc_stats_actual.r_HG2_ENABLE = SC_X4_RSLVD7r_R_HG2_ENABLEf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_7r_reg);
  #endif

  /*RESOVLED 8 */ 
  #ifndef TSCF_GEN1
  PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD8r(pc, &SC_X4_FINAL_CONFIG_STATUS_RESOLVED_8r_reg));
  sc_stats_actual.USE_100G_AM0   = SC_X4_RSLVD8r_USE_100G_AM0f_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_8r_reg);
  sc_stats_actual.USE_100G_AM123 = SC_X4_RSLVD8r_USE_100G_AM123f_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_8r_reg);
  sc_stats_actual.ber_window_sel    = SC_X4_RSLVD8r_BER_WINDOW_SELf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_8r_reg);   
  sc_stats_actual.ber_count_sel     = SC_X4_RSLVD8r_BER_COUNT_SELf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_8r_reg);    
  sc_stats_actual.cl74_shcorrupt    = SC_X4_RSLVD8r_CL74_SHCORRUPTf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_8r_reg);   
  sc_stats_actual.corrupt_6th_group = SC_X4_RSLVD8r_CORRUPT_6TH_GROUPf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_8r_reg);
  sc_stats_actual.corrupt_2nd_group = SC_X4_RSLVD8r_CORRUPT_2ND_GROUPf_GET(SC_X4_FINAL_CONFIG_STATUS_RESOLVED_8r_reg);
  #endif
  

  PHYMOD_IF_ERR_RETURN(READ_SC_X4_FEC_STSr(pc, &SC_X4_FINAL_CONFIG_STATUS_FEC_STATUSr_reg));
  if(pc->an_hcd_fec || pc->t_fec_enable) {
    /* if(data != 3) {} */
    if((SC_X4_FEC_STSr_T_FEC_ENABLEf_GET(SC_X4_FINAL_CONFIG_STATUS_FEC_STATUSr_reg) &
        SC_X4_FEC_STSr_R_FEC_ENABLEf_GET(SC_X4_FINAL_CONFIG_STATUS_FEC_STATUSr_reg)) != 1) {
     fail=1;
    }
  }
  
  if(pc->cl91_en) {
    /* if(data != 0xc) {} */
     #ifdef TSCF_GEN1
    if((SC_X4_FEC_STSr_T_CL91_FEC_ENABLEf_GET(SC_X4_FINAL_CONFIG_STATUS_FEC_STATUSr_reg) &
        SC_X4_FEC_STSr_R_CL91_FEC_ENABLEf_GET(SC_X4_FINAL_CONFIG_STATUS_FEC_STATUSr_reg)) != 1) {
     fail=1;
    }
    #endif
  }

  if(pc->tx_higig2_oen == 0x1) { pc->t_HG2_ENABLE = pc->tx_higig2_en; }
  if(pc->rx_higig2_oen == 0x1) { pc->r_HG2_ENABLE = pc->rx_higig2_en; }

  /* Compare actual vs expected stats */

  fail = 0;

#ifdef _DV_TB_
  printf("SC_Info : tefmod16_cfg_seq.c : tefmod_check_status() : pc->spd_intf %02x :    mapped_speed %02x : pc->speed %02x : actual_speed %02x\n",
          pc->spd_intf, mapped_speed, pc->speed, actual_speed);
#endif

  if(actual_speed != pc->speed) {
     printf("ACTUAL is %x and EXP is %x and SPDI is %x\n", actual_speed, pc->speed, pc->spd_intf);
     if(pc->verbosity)
     #ifdef _DV_TB_
       PHYMOD_DEBUG_ERROR(("Act speed is %d and exp_speed is %d\n", actual_speed, pc->speed));
     #endif
     fail=1;
  } else {
#ifdef _DV_TB_
     if(pc->verbosity)
       PHYMOD_DEBUG_ERROR(("speed %d executed correctly \n", actual_speed));
#endif
  }

  /*Resolved 0 comparison */
  if(sc_stats_actual.cl72_en != pc->cl72_en) fail=1;
  if(sc_stats_actual.os_mode != pc->os_mode) fail=1;
  if(sc_stats_actual.t_fifo_mode != pc->t_fifo_mode) fail=1;
  if(sc_stats_actual.t_enc_mode != pc->t_enc_mode) fail=1;
  if(sc_stats_actual.t_HG2_ENABLE != pc->t_HG2_ENABLE) fail=1;
  if(sc_stats_actual.t_pma_btmx_mode != pc->t_pma_btmx_mode) fail=1;
  if(sc_stats_actual.scr_mode != pc->scr_mode) fail=1;

  /*Resolved 1 comparison */
  if(sc_stats_actual.descr_mode != pc->descr_mode) fail=1;
  if(sc_stats_actual.dec_tl_mode != pc->dec_tl_mode) fail=1;
  if(sc_stats_actual.deskew_mode != pc->deskew_mode) fail=1;
  if(sc_stats_actual.dec_fsm_mode != pc->dec_fsm_mode) fail=1;
  if(sc_stats_actual.r_HG2_ENABLE != pc->r_HG2_ENABLE) fail=1;
  if(sc_stats_actual.bs_sm_sync_mode != pc->bs_sm_sync_mode) fail=1;
  if(sc_stats_actual.bs_sync_en != pc->bs_sync_en) fail=1;
  if(sc_stats_actual.bs_dist_mode != pc->bs_dist_mode) fail=1;
  if(sc_stats_actual.bs_btmx_mode != pc->bs_btmx_mode) fail=1;

  
  /*Resolved 2 comparison */
  if(sc_stats_actual.clkcnt0 != pc->clkcnt0) fail=1;

  /*Resolved 3 comparison */
  if(sc_stats_actual.clkcnt1 != pc->clkcnt1) fail=1;

  /*Resolved 4 comparison */
  if(sc_stats_actual.lpcnt0 != pc->lpcnt0) fail=1;
  if(sc_stats_actual.lpcnt1 != pc->lpcnt1) fail=1;

  /*Resolved 5 comparison */
  if(sc_stats_actual.cgc != pc->cgc) fail=1;
 

  /*Resolved 8 comparison */
  if(pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_CR1      || pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_KR1      ||
      pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_X1      || pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_HG2_CR1  ||
      pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_HG2_KR1 || pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_HG2_X1   ){

         pc->USE_100G_AM0      = pc->msa_25G_am0_mode ;
         pc->USE_100G_AM123    = pc->msa_25G_am123_mode ;
         pc->cl74_shcorrupt    = pc->cl74_shcorrupt_25msa;   
         pc->corrupt_6th_group = 0 ;
         pc->corrupt_2nd_group = 1 ;
         pc->ber_window_sel    =  pc->msa_window_sel ;   
         pc->ber_count_sel     =  pc->msa_count_sel ;    
         
         if(sc_stats_actual.USE_100G_AM0      != pc->USE_100G_AM0) fail=1;
         if(sc_stats_actual.USE_100G_AM123    != pc->USE_100G_AM123) fail=1;
         if(sc_stats_actual.corrupt_6th_group != pc->corrupt_6th_group) fail=1;
         if(sc_stats_actual.corrupt_2nd_group != pc->corrupt_2nd_group) fail=1;
         if(sc_stats_actual.ber_window_sel    != pc->ber_window_sel) fail=1;
         if(sc_stats_actual.ber_count_sel     != pc->ber_count_sel) fail=1;
  }
  if(pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_CR_IEEE || pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_CRS_IEEE ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_KR_IEEE || pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_KRS_IEEE) {

         pc->USE_100G_AM0      = pc->ieee_25G_am0_mode ;
         pc->USE_100G_AM123    = pc->ieee_25G_am123_mode ;
         pc->cl74_shcorrupt    = pc->cl74_shcorrupt_25ieee ;   
         pc->corrupt_6th_group = 0 ;
         pc->corrupt_2nd_group = 1 ;
         pc->ber_window_sel    =  pc->ieee_window_sel ;   
         pc->ber_count_sel     =  pc->ieee_count_sel ;    
         

         if(sc_stats_actual.USE_100G_AM0      != pc->USE_100G_AM0) fail=1;
         if(sc_stats_actual.USE_100G_AM123    != pc->USE_100G_AM123) fail=1;
         if(sc_stats_actual.corrupt_6th_group != pc->corrupt_6th_group) fail=1;
         if(sc_stats_actual.corrupt_2nd_group != pc->corrupt_2nd_group) fail=1;
         if(sc_stats_actual.ber_window_sel    != pc->ber_window_sel) fail=1;
         if(sc_stats_actual.ber_count_sel     != pc->ber_count_sel) fail=1;
  }    
  if(pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_50G_CR2     || pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_50G_KR2      ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_50G_HG2_X2  || pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_50G_HG2_KR2  ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_50G_X2      || pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_50G_HG2_CR2) {
         pc->USE_100G_AM0      = pc->msa_50G_am0_mode;
         pc->USE_100G_AM123    = pc->msa_50G_am123_mode;
         pc->cl74_shcorrupt    = pc->cl74_shcorrupt_50msa;   
         pc->corrupt_6th_group = 0 ;
         pc->corrupt_2nd_group = 1 ;
         pc->ber_window_sel    =  pc->msa_window_sel ;   
         pc->ber_count_sel     =  pc->msa_count_sel ;    
         

         if(sc_stats_actual.USE_100G_AM0      != pc->USE_100G_AM0) fail=1;
         if(sc_stats_actual.USE_100G_AM123    != pc->USE_100G_AM123) fail=1;
         if(sc_stats_actual.corrupt_6th_group != pc->corrupt_6th_group) fail=1;
         if(sc_stats_actual.corrupt_2nd_group != pc->corrupt_2nd_group) fail=1;
         if(sc_stats_actual.ber_window_sel    != pc->ber_window_sel) fail=1;
         if(sc_stats_actual.ber_count_sel     != pc->ber_count_sel) fail=1;
  }
  if(pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_CR4     || pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_KR4     ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_X4      || pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_HG2_CR4 ||  
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_HG2_KR4 || pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_HG2_X4) {
         pc->corrupt_6th_group = 1 ;
         pc->corrupt_2nd_group = 0 ;
         if(sc_stats_actual.corrupt_6th_group != pc->corrupt_6th_group) fail=1;
         if(sc_stats_actual.corrupt_2nd_group != pc->corrupt_2nd_group) fail=1;
     
   }

  
  /* pc->   = fail; */
  if(fail == 1) {
    #ifdef _DV_TB_
    PHYMOD_DEBUG_ERROR(("Actual stats\n"));
    sc_stats_print_actual = &sc_stats_actual;
    print_tefmod16_sc_lkup_table(sc_stats_print_actual);
    PHYMOD_DEBUG_ERROR(("Expected stats\n"));
    print_tefmod16_sc_lkup_table(pc);
    return PHYMOD_E_FAIL;
    #endif
    pc->sc_done = 0;
  } else {
    pc->sc_done=1;
    #ifdef _DV_TB_
      PHYMOD_DEBUG_ERROR(("SC_DONE set\n"));
  if(pc->tefmod16_st_print==1) {
    print_tefmod16_st(pc);
  }
     return PHYMOD_E_NONE;
    #endif
  }
}
#endif

/*!
@brief tefmod16_set_port_mode_sel , selects the port_mode,  
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This is used when we are not using any speed control logic, but instead when bypassing this entire SC logic, this has to be programmed
*/
int tefmod16_set_port_mode_sel(PHYMOD_ST* pc, int tsc_touched, tefmod16_port_type_t port_type)
{
  int port_mode_sel;
  uint16_t single_port_mode;
  MAIN0_SETUPr_t main0_reg;
 
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  port_mode_sel = 0;
  if(tsc_touched == 1)
    single_port_mode = 0;
  else
    single_port_mode = 1;

  switch(port_type) {
    case TEFMOD16_MULTI_PORT:   port_mode_sel = 0;  break;
    case TEFMOD16_TRI1_PORT:    port_mode_sel = 2;  break;
    case TEFMOD16_TRI2_PORT:    port_mode_sel = 1;  break;
    case TEFMOD16_DXGXS:        port_mode_sel = 3;  break;
    case TEFMOD16_SINGLE_PORT:  port_mode_sel = 4;  break;
    default: break;
  }
  MAIN0_SETUPr_CLR(main0_reg);
  MAIN0_SETUPr_PORT_MODE_SELf_SET(main0_reg, port_mode_sel);
  MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(main0_reg, single_port_mode);
  PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc, main0_reg));

  return PHYMOD_E_NONE;
}

/*!
@brief firmware load request 
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion.
@details
*/
int tefmod16_firmware_set(PHYMOD_ST* pc)
{
  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  return PHYMOD_E_NONE;
}

#ifdef _SDK_TEFMOD16_
/*!
@brief Controls the init setting/resetting of autoneg  registers.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  an_init_st structure tefmod16_an_init_t with all the one time autoneg init cfg.
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Get aneg features via #tefmod16_an_init_t.
  This does not start the autoneg. That is done in tefmod16_autoneg_control
*/
int tefmod16_autoneg_set_init(PHYMOD_ST* pc, tefmod16_an_init_t *an_init_st) /* AUTONEG_SET */
{
  AN_X4_CL73_CTLSr_t AN_X4_CL73_CTLSr_t_reg;
  AN_X4_LD_BASE_ABIL1r_t AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg;
  AN_X4_LD_BASE_ABIL0r_t AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg;
  AN_X4_CL73_CFGr_t AN_X4_CL73_CFGr_t_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  AN_X4_CL73_CTLSr_CLR(AN_X4_CL73_CTLSr_t_reg);
  AN_X4_CL73_CTLSr_AN_GOOD_TRAPf_SET(AN_X4_CL73_CTLSr_t_reg, an_init_st->an_good_trap);
  AN_X4_CL73_CTLSr_AN_GOOD_CHECK_TRAPf_SET(AN_X4_CL73_CTLSr_t_reg, an_init_st->an_good_check_trap);
  AN_X4_CL73_CTLSr_LINKFAILTIMER_DISf_SET(AN_X4_CL73_CTLSr_t_reg, an_init_st->linkfailtimer_dis);
  AN_X4_CL73_CTLSr_LINKFAILTIMERQUAL_ENf_SET(AN_X4_CL73_CTLSr_t_reg, an_init_st->linkfailtimerqua_en);
  AN_X4_CL73_CTLSr_AN_FAIL_COUNT_LIMITf_SET(AN_X4_CL73_CTLSr_t_reg, an_init_st->an_fail_cnt);
  AN_X4_CL73_CTLSr_AN_OUI_OVERRIDE_BAM73_ADVf_SET(AN_X4_CL73_CTLSr_t_reg, an_init_st->bam73_adv_oui);
  AN_X4_CL73_CTLSr_AN_OUI_OVERRIDE_BAM73_DETf_SET(AN_X4_CL73_CTLSr_t_reg, an_init_st->bam73_det_oui);
  AN_X4_CL73_CTLSr_AN_OUI_OVERRIDE_HPAM_ADVf_SET(AN_X4_CL73_CTLSr_t_reg, an_init_st->hpam_adv_oui);
  AN_X4_CL73_CTLSr_AN_OUI_OVERRIDE_HPAM_DETf_SET(AN_X4_CL73_CTLSr_t_reg, an_init_st->hpam_det_oui);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_CL73_CTLSr(pc, AN_X4_CL73_CTLSr_t_reg));

  AN_X4_LD_BASE_ABIL1r_CLR(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg);
  if(an_init_st->disable_rf_report == 1) {
    AN_X4_LD_BASE_ABIL1r_CL73_REMOTE_FAULTf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_BASE_ABIL1r(pc, AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg));

  AN_X4_LD_BASE_ABIL0r_CLR(AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg);
  AN_X4_LD_BASE_ABIL0r_TX_NONCEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg, (an_init_st->cl73_transmit_nonce) & 0x1f);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_BASE_ABIL0r(pc, AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg));

  AN_X4_CL73_CFGr_CLR(AN_X4_CL73_CFGr_t_reg);
  AN_X4_CL73_CFGr_CL73_NONCE_MATCH_OVERf_SET(AN_X4_CL73_CFGr_t_reg, an_init_st->cl73_nonce_match_over);
  AN_X4_CL73_CFGr_CL73_NONCE_MATCH_VALf_SET(AN_X4_CL73_CFGr_t_reg,  an_init_st->cl73_nonce_match_val);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_CL73_CFGr(pc, AN_X4_CL73_CFGr_t_reg));

  return PHYMOD_E_NONE;
}
#endif /* _SDK_TEFMOD16_ */

#ifdef _SDK_TEFMOD16_
/*!
@brief Controls the setting/resetting of autoneg timers.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details

*/
int tefmod16_autoneg_timer_init(PHYMOD_ST* pc)               /* AUTONEG timer set*/
{

  AN_X1_IGNORE_LNK_TMRr_t AN_X1_IGNORE_LNK_TMRr_reg;
  AN_X1_CL73_DME_LOCKr_t AN_X1_TIMERS_CL73_DME_LOCKr_reg;
  AN_X1_PD_SD_TMRr_t AN_X1_TIMERS_PD_SD_TIMERr_reg;
  AN_X1_CL73_BRK_LNKr_t AN_X1_TIMERS_CL73_BREAK_LINKr_reg;
  AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_t AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r_reg;
  AN_X1_LNK_FAIL_INHBT_TMR_CL72r_t AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r_reg;
  
  AN_X1_IGNORE_LNK_TMRr_SET(AN_X1_IGNORE_LNK_TMRr_reg, 0x29a );
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_IGNORE_LNK_TMRr(pc,AN_X1_IGNORE_LNK_TMRr_reg));

  AN_X1_CL73_DME_LOCKr_SET(AN_X1_TIMERS_CL73_DME_LOCKr_reg, 0x14d4);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_DME_LOCKr(pc, AN_X1_TIMERS_CL73_DME_LOCKr_reg));

  AN_X1_PD_SD_TMRr_SET(AN_X1_TIMERS_PD_SD_TIMERr_reg, 0xa6a);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_PD_SD_TMRr(pc, AN_X1_TIMERS_PD_SD_TIMERr_reg));

  AN_X1_CL73_BRK_LNKr_SET(AN_X1_TIMERS_CL73_BREAK_LINKr_reg, 0x10ed);
  PHYMOD_IF_ERR_RETURN (WRITE_AN_X1_CL73_BRK_LNKr(pc, AN_X1_TIMERS_CL73_BREAK_LINKr_reg));

  /* AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_SET(AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r_reg, 0xbb8); */
  /* Change based on finding in eagle */
  AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_SET(AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r_reg, 0x3080);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r(pc, AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r_reg));

  AN_X1_LNK_FAIL_INHBT_TMR_CL72r_SET(AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r_reg, 0x8382);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_FAIL_INHBT_TMR_CL72r(pc, AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r_reg));

  return PHYMOD_E_NONE;
}
#endif /* _SDK_TEFMOD16_ */

int tefmod16_toggle_sw_speed_change(PHYMOD_ST* pc)
{
  int cnt;
  int sw_sp_cfg_vld;
  SC_X4_CTLr_t SC_X4_CTLr_reg;
  SC_X4_STSr_t SC_X4_STSr_reg;
  #ifdef TSCF_GEN1
  TX_X1_T_CL91_FECr_t TX_X1_T_CL91_FECr_reg;
  RX_X1_CL91_CFGr_t RX_X1_CL91_CFGr_reg; 
  #endif

  cnt=0;
  sw_sp_cfg_vld = 0;

#ifdef _DV_TB_
  printf("Inside toggle_sw_speed_change\n"); 
#endif

  #ifdef TSCF_GEN1
  TX_X1_T_CL91_FECr_CLR(TX_X1_T_CL91_FECr_reg);
  TX_X1_T_CL91_FECr_SET(TX_X1_T_CL91_FECr_reg, 0);
  PHYMOD_IF_ERR_RETURN(MODIFY_TX_X1_T_CL91_FECr(pc, TX_X1_T_CL91_FECr_reg));

  RX_X1_CL91_CFGr_CLR(RX_X1_CL91_CFGr_reg);
  RX_X1_CL91_CFGr_RX_CL91_FEC_ENf_SET (RX_X1_CL91_CFGr_reg, 0); 
  PHYMOD_IF_ERR_RETURN(MODIFY_RX_X1_CL91_CFGr(pc, RX_X1_CL91_CFGr_reg));
  #endif

  SC_X4_CTLr_CLR(SC_X4_CTLr_reg);
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CTLr_reg, 0);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CTLr_reg));

  SC_X4_CTLr_CLR(SC_X4_CTLr_reg);
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CTLr_reg, 1);
  SC_X4_CTLr_SPEEDf_SET(SC_X4_CTLr_reg, 0);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CTLr_reg));

  while (cnt <=2000) {
    SC_X4_STSr_CLR(SC_X4_STSr_reg);
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_STSr(pc, &SC_X4_STSr_reg));
    cnt = cnt + 1;
    if(SC_X4_STSr_SW_SPEED_CONFIG_VLDf_GET(SC_X4_STSr_reg)) {
      sw_sp_cfg_vld = 1;
#ifdef _DV_TB_
      printf("sw_speed_cfg_vld set  uint :: %d :: lane :: %d\n",pc->unit, pc->this_lane);
#endif
      break;
    }
  }

  SC_X4_CTLr_CLR(SC_X4_CTLr_reg);
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CTLr_reg, 0);
  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CTLr_reg));
  
  if(!sw_sp_cfg_vld) {
    return PHYMOD_E_FAIL;
  }
  return PHYMOD_E_NONE;
}

#ifdef _SDK_TEFMOD16_
/**
@brief   To get autoneg control registers.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   an_control reference to struct with autoneg parms #tefmod16_an_control_t
@param   an_complete status of AN completion
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Upper layer software calls this function to get autoneg
         info.
  
<B>How to call:</B><br>
<p>Call directly </p>
*/
int tefmod16_autoneg_control_get(PHYMOD_ST* pc, tefmod16_an_control_t *an_control, int *an_complete)
{
  AN_X4_CL73_CFGr_t      AN_X4_CL73_CFGr_reg;
  AN_X4_CL73_CTLSr_t     AN_X4_CL73_CTLSr_reg;
  AN_X4_AN_MISC_STSr_t   AN_X4_AN_MISC_STSr_reg;
  uint32_t    msa_mode, base_ability,base_ability1;
  AN_X4_LD_BASE_ABIL3r_t AN_X4_LD_BASE_ABIL3r_reg;
  AN_X4_LD_BASE_ABIL4r_t AN_X4_LD_BASE_ABIL4r_reg;
  
  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  /* CL73 AN MISC CONTROL 0xC1C6 */
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_CL73_CTLSr(pc, &AN_X4_CL73_CTLSr_reg));
  an_control->pd_kx_en = AN_X4_CL73_CTLSr_PD_KX_ENf_GET(AN_X4_CL73_CTLSr_reg);

  /* CL73 AN CONFIG 0xC1C0 */
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_CL73_CFGr(pc, &AN_X4_CL73_CFGr_reg));
  if (AN_X4_CL73_CFGr_CL73_BAM_ENABLEf_GET(AN_X4_CL73_CFGr_reg) == 1) {
    PHYMOD_IF_ERR_RETURN(tefmod16_an_msa_mode_get(pc, &msa_mode)); 
      
    if (msa_mode) {
              /* 
               * Read the msa_mode value.
               * If it is 1, it is a MSA/IEEE_MSA AN mode.
               * Read the base page values (abil_3 & abil4).
               * If it is set, then set AN mode to IEEE_MSA, else set it to MSA mode. 
 */
               
               PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_BASE_ABIL3r(pc, &AN_X4_LD_BASE_ABIL3r_reg));
               PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_BASE_ABIL4r(pc, &AN_X4_LD_BASE_ABIL4r_reg));
               base_ability =  AN_X4_LD_BASE_ABIL3r_GET(AN_X4_LD_BASE_ABIL3r_reg) & 0x0fff;
               base_ability1 = AN_X4_LD_BASE_ABIL4r_GET(AN_X4_LD_BASE_ABIL4r_reg) & 0x0fff;

               if (!(base_ability) && !(base_ability1)) {
                   an_control->an_type = TEFMOD16_AN_MODE_MSA;
                   an_control->enable = 1;
                } else {
                      an_control->an_type = TEFMOD16_AN_MODE_CL73_MSA;
                      an_control->enable = 1;
                }
          } 
    else { 
        an_control->an_type = TEFMOD16_AN_MODE_CL73_BAM;
        an_control->enable = 1;
      }
  } else if (AN_X4_CL73_CFGr_CL73_HPAM_ENABLEf_GET(AN_X4_CL73_CFGr_reg) == 1) {
     an_control->an_type = TEFMOD16_AN_MODE_HPAM;
     an_control->enable = 1;
  } else if (AN_X4_CL73_CFGr_CL73_ENABLEf_GET(AN_X4_CL73_CFGr_reg) == 1) {
     an_control->an_type = TEFMOD16_AN_MODE_CL73;
     an_control->enable = 1;
  } else {
     an_control->an_type = TEFMOD16_AN_MODE_NONE;
     an_control->enable = 0;
  }

  if(AN_X4_CL73_CFGr_AD_TO_CL73_ENf_GET(AN_X4_CL73_CFGr_reg) == 1) {
    an_control->an_property_type = TEFMOD16_AN_PROPERTY_ENABLE_HPAM_TO_CL73_AUTO;
  } else if(AN_X4_CL73_CFGr_BAM_TO_HPAM_AD_ENf_GET(AN_X4_CL73_CFGr_reg) == 1) {
    an_control->an_property_type = TEFMOD16_AN_PROPERTY_ENABLE_CL73_BAM_TO_HPAM_AUTO;
  }

  an_control->num_lane_adv = AN_X4_CL73_CFGr_NUM_ADVERTISED_LANESf_GET(AN_X4_CL73_CFGr_reg);

  /* an_complete status 0xC1E9 */
  AN_X4_AN_MISC_STSr_CLR(AN_X4_AN_MISC_STSr_reg);
  PHYMOD_IF_ERR_RETURN (READ_AN_X4_AN_MISC_STSr(pc, &AN_X4_AN_MISC_STSr_reg));
  *an_complete = AN_X4_AN_MISC_STSr_AN_COMPLETEf_GET(AN_X4_AN_MISC_STSr_reg);

  return PHYMOD_E_NONE;

}
#endif /* _SDK_TEFMOD16_ */

#ifdef _SDK_TEFMOD16_
/**
@brief   To get local autoneg advertisement registers.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   an_ability_st receives autoneg info. #tefmod16_an_adv_ability_t)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Upper layer software calls this function to get local autoneg
         info. This function is currently not implemented
*/

int tefmod16_autoneg_local_ability_get(PHYMOD_ST* pc, tefmod16_an_adv_ability_t *an_ability_st)
{
  AN_X4_LD_BASE_ABIL1r_t            AN_X4_LD_BASE_ABIL1r_reg;
  AN_X4_LD_UP1_ABIL0r_t             AN_X4_LD_UP1_ABIL0r_reg;
  AN_X4_LD_UP1_ABIL1r_t             AN_X4_LD_UP1_ABIL1r_reg;
  AN_X4_LD_BASE_ABIL3r_t            AN_X4_LD_BASE_ABIL3r_reg;
  AN_X4_LD_BASE_ABIL4r_t            AN_X4_LD_BASE_ABIL4r_reg;
  AN_X4_LD_FEC_BASEPAGE_ABILr_t     AN_X4_LD_FEC_BASEPAGE_ABILr_reg;
  int an_fec = 0;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  /***** AN_X4_ABILITIES_ld_up0_abilities_1 0xC1C1 *******/
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_UP1_ABIL0r(pc, &AN_X4_LD_UP1_ABIL0r_reg));
  an_ability_st->an_bam_speed = AN_X4_LD_UP1_ABIL0r_GET(AN_X4_LD_UP1_ABIL0r_reg) & 0x3ff;
  an_ability_st->an_hg2 = AN_X4_LD_UP1_ABIL0r_BAM_HG2f_GET(AN_X4_LD_UP1_ABIL0r_reg);

  /***** AN_X4_ABILITIES_ld_up1_abilities_1 0xC1C2 *******/
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_UP1_ABIL1r(pc, &AN_X4_LD_UP1_ABIL1r_reg));
  an_ability_st->an_bam_speed1 = AN_X4_LD_UP1_ABIL1r_GET(AN_X4_LD_UP1_ABIL1r_reg) & 0x1f;

  /***** AN_X4_ABILITIES_ld_base_abilities_1 0xC1C4 *******/
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_BASE_ABIL1r(pc, &AN_X4_LD_BASE_ABIL1r_reg));
  an_ability_st->an_base_speed = AN_X4_LD_BASE_ABIL1r_GET(AN_X4_LD_BASE_ABIL1r_reg) & 0x3f;
  an_ability_st->an_pause = AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_GET(AN_X4_LD_BASE_ABIL1r_reg);
  an_fec = AN_X4_LD_BASE_ABIL1r_FEC_REQf_GET(AN_X4_LD_BASE_ABIL1r_reg);

  /***** AN_X4_ABILITIES_ld_base_abilities_3 0xC1C8 *******/
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_BASE_ABIL3r(pc, &AN_X4_LD_BASE_ABIL3r_reg));
  if (AN_X4_LD_BASE_ABIL3r_BASE_25G_CR1_ENf_GET(AN_X4_LD_BASE_ABIL3r_reg)) {
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_25GBASE_CR1);
  }
  if (AN_X4_LD_BASE_ABIL3r_BASE_25G_KR1_ENf_GET(AN_X4_LD_BASE_ABIL3r_reg)) {
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_25GBASE_KR1);
  }

  /***** AN_X4_ABILITIES_ld_base_abilities_4 0xC1C9 *******/
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_BASE_ABIL4r(pc, &AN_X4_LD_BASE_ABIL4r_reg));
  if (AN_X4_LD_BASE_ABIL4r_BASE_25G_CRS1_ENf_GET(AN_X4_LD_BASE_ABIL4r_reg)) {
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_25GBASE_CRS1);
  }
  if (AN_X4_LD_BASE_ABIL4r_BASE_25G_KRS1_ENf_GET(AN_X4_LD_BASE_ABIL4r_reg)) {
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_25GBASE_KRS1);
  }

  /***** AN_X4_ABILITIES_ld_fec_base_abilities 0xC1CA *******/
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_FEC_BASEPAGE_ABILr(pc, &AN_X4_LD_FEC_BASEPAGE_ABILr_reg));

  if (an_fec == 0x3) {
      if (AN_X4_LD_FEC_BASEPAGE_ABILr_RS_FEC_REQ_ENf_GET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg)) {
          an_ability_st->an_fec = TEFMOD16_FEC_CL91_SUPRTD_REQSTD;
      }
      if (AN_X4_LD_FEC_BASEPAGE_ABILr_BASE_R_FEC_REQ_ENf_GET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg)) {
          an_ability_st->an_fec = TEFMOD16_FEC_CL74_SUPRTD_REQSTD;
      }
  } else {
      an_ability_st->an_fec = (an_fec == 0x1) ? TEFMOD16_FEC_SUPRTD_NOT_REQSTD : TEFMOD16_FEC_NOT_SUPRTD;
  }

  return PHYMOD_E_NONE;

}
#endif /* _SDK_TEFMOD16_ */

/**
@brief   Controls port RX squelch
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable is the control to  RX  squelch
@returns The value PHYMOD_E_NONE upon successful completion.
@details Controls port RX squelch
*/
int tefmod16_rx_squelch_set(PHYMOD_ST *pc, int enable)
{
  SIGDET_CTL1r_t sigdet_ctl;

  SIGDET_CTL1r_CLR(sigdet_ctl);
  READ_SIGDET_CTL1r(pc, &sigdet_ctl);

  if(enable){
     SIGDET_CTL1r_SIGNAL_DETECT_FRCf_SET(sigdet_ctl, 1);
     SIGDET_CTL1r_SIGNAL_DETECT_FRC_VALf_SET(sigdet_ctl, 0);
  } else {
     SIGDET_CTL1r_SIGNAL_DETECT_FRCf_SET(sigdet_ctl, 0);
     SIGDET_CTL1r_SIGNAL_DETECT_FRC_VALf_SET(sigdet_ctl, 0);
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
int tefmod16_tx_squelch_set(PHYMOD_ST *pc, int enable)
{
  TXFIR_MISC_CTL0r_t tx_misc_ctl;

  TXFIR_MISC_CTL0r_CLR(tx_misc_ctl);
  READ_TXFIR_MISC_CTL0r(pc, &tx_misc_ctl);

  TXFIR_MISC_CTL0r_SDK_TX_DISABLEf_SET(tx_misc_ctl, enable);
  PHYMOD_IF_ERR_RETURN(MODIFY_TXFIR_MISC_CTL0r(pc, tx_misc_ctl));

  return PHYMOD_E_NONE;
}


/**
@brief   Controls port TX/RX squelch
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable is the control to  TX/RX  squelch. Enable=1 means enable the port,no squelch
@returns The value PHYMOD_E_NONE upon successful completion.
@details Controls port TX/RX squelch
*/
int tefmod16_port_enable_set(PHYMOD_ST *pc, int enable)
{
  if (enable)  {
      tefmod16_rx_squelch_set(pc, 0);
      tefmod16_tx_squelch_set(pc, 0);
  } else {
      tefmod16_rx_squelch_set(pc, 1);
      tefmod16_tx_squelch_set(pc, 1);
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
int tefmod16_tx_squelch_get(PHYMOD_ST *pc, int *val)
{
  TXFIR_MISC_CTL0r_t tx_misc_ctl;

  TXFIR_MISC_CTL0r_CLR(tx_misc_ctl);

  PHYMOD_IF_ERR_RETURN(READ_TXFIR_MISC_CTL0r(pc, &tx_misc_ctl));
  *val = TXFIR_MISC_CTL0r_SDK_TX_DISABLEf_GET(tx_misc_ctl);

  return PHYMOD_E_NONE;
}

/**
@brief   Gets port RX squelch settings
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   val Receiver for status of  RX  squelch
@returns The value PHYMOD_E_NONE upon successful completion.
@details Gets port RX squelch settings
*/
int tefmod16_rx_squelch_get(PHYMOD_ST *pc, int *val)
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
int tefmod16_port_enable_get(PHYMOD_ST *pc, int *tx_enable, int *rx_enable)
{

  tefmod16_rx_squelch_get(pc, rx_enable);
  tefmod16_tx_squelch_get(pc, tx_enable);

  return PHYMOD_E_NONE;
}

#ifdef _SDK_TEFMOD16_
/**
@brief   To get remote autoneg advertisement registers.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   an_ability_st receives autoneg info. #tefmod16_an_adv_ability_t)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Upper layer software calls this function to get autoneg
         info. This function is not currently implemented.
*/
int tefmod16_autoneg_remote_ability_get(PHYMOD_ST* pc, tefmod16_an_adv_ability_t *an_ability_st)
{ 
  uint32_t data;
  uint32_t user_code;
  AN_X4_LP_BASE1r_t   AN_X4_LP_BASE1r_reg;
  AN_X4_LP_BASE3r_t   AN_X4_LP_BASE3r_reg;

  AN_X4_LP_OUI_UP3r_t AN_X4_LP_OUI_UP3r_reg;
  AN_X4_LP_OUI_UP4r_t AN_X4_LP_OUI_UP4r_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE1r(pc, &AN_X4_LP_BASE1r_reg));
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE3r(pc, &AN_X4_LP_BASE3r_reg));
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_OUI_UP3r(pc, &AN_X4_LP_OUI_UP3r_reg));
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_OUI_UP4r(pc, &AN_X4_LP_OUI_UP4r_reg));

  
  /*an_ability_st->cl73_adv.an_bam_speed = 0xdead;*/
  an_ability_st->an_bam_speed = 0x0;

  user_code = (AN_X4_LP_OUI_UP3r_reg.v[0] & 0x1ff) << 11 ;

  data = AN_X4_LP_OUI_UP4r_reg.v[0];
  /* 0xc193 */
  if(AN_X4_LP_OUI_UP4r_reg.v[0]&(1<<1)) {
     an_ability_st->an_bam_speed |=(1<<0) ;
  } else {
     user_code |= (AN_X4_LP_OUI_UP4r_reg.v[0] & 0x7ff) ;
     if(user_code == 0xabe20)
        an_ability_st->an_bam_speed  |=(1<<0) ;
  }
  if(data&(1<<0)) an_ability_st->an_bam_speed |=(1<<1) ;

  an_ability_st->an_pause = (AN_X4_LP_BASE1r_reg.v[0] >> 10 ) & 0x3;;
  an_ability_st->an_fec = (AN_X4_LP_BASE3r_reg.v[0] >> 14) & 0x3;

  return PHYMOD_E_NONE;

}


/**
@brief   Sets the field of the Soft Table
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   st_entry_no: The ST to write to(0..3)
@param   st_control_field,: The ST field to write to
@param   st_field_value: The ST value to write to the field
returns  The value PHYMOD_E_NONE upon successful completion.
@details Sets the field of the Soft Table
*/
int tefmod16_st_control_field_set (PHYMOD_ST* pc,uint16_t st_entry_no, override_type_t  st_control_field, uint16_t st_field_value)
{
  SC_X1_SPD_OVRR0_SPDr_t SC_X1_SPD_OVRR0_SPDr_reg;
  SC_X1_SPD_OVRR1_SPDr_t SC_X1_SPD_OVRR1_SPDr_reg;
  SC_X1_SPD_OVRR2_SPDr_t SC_X1_SPD_OVRR2_SPDr_reg;
  SC_X1_SPD_OVRR3_SPDr_t SC_X1_SPD_OVRR3_SPDr_reg;

  SC_X1_SPD_OVRR0_0r_t   SC_X1_SPD_OVRR0_0r_reg;
  SC_X1_SPD_OVRR1_0r_t   SC_X1_SPD_OVRR1_0r_reg;
  SC_X1_SPD_OVRR2_0r_t   SC_X1_SPD_OVRR2_0r_reg;
  SC_X1_SPD_OVRR3_0r_t   SC_X1_SPD_OVRR3_0r_reg;

  SC_X1_SPD_OVRR0_1r_t   SC_X1_SPD_OVRR0_1r_reg;
  SC_X1_SPD_OVRR1_1r_t   SC_X1_SPD_OVRR1_1r_reg;
  SC_X1_SPD_OVRR2_1r_t   SC_X1_SPD_OVRR2_1r_reg;
  SC_X1_SPD_OVRR3_1r_t   SC_X1_SPD_OVRR3_1r_reg;

  SC_X1_SPD_OVRR0_2r_t   SC_X1_SPD_OVRR0_2r_reg;
  SC_X1_SPD_OVRR1_2r_t   SC_X1_SPD_OVRR1_2r_reg;
  SC_X1_SPD_OVRR2_2r_t   SC_X1_SPD_OVRR2_2r_reg;
  SC_X1_SPD_OVRR3_2r_t   SC_X1_SPD_OVRR3_2r_reg;

  SC_X1_SPD_OVRR0_3r_t   SC_X1_SPD_OVRR0_3r_reg;
  SC_X1_SPD_OVRR1_3r_t   SC_X1_SPD_OVRR1_3r_reg;
  SC_X1_SPD_OVRR2_3r_t   SC_X1_SPD_OVRR2_3r_reg;
  SC_X1_SPD_OVRR3_3r_t   SC_X1_SPD_OVRR3_3r_reg;

  SC_X1_SPD_OVRR0_4r_t   SC_X1_SPD_OVRR0_4r_reg;
  SC_X1_SPD_OVRR1_4r_t   SC_X1_SPD_OVRR1_4r_reg;
  SC_X1_SPD_OVRR2_4r_t   SC_X1_SPD_OVRR2_4r_reg;
  SC_X1_SPD_OVRR3_4r_t   SC_X1_SPD_OVRR3_4r_reg;

  SC_X1_SPD_OVRR0_5r_t   SC_X1_SPD_OVRR0_5r_reg;
  SC_X1_SPD_OVRR1_5r_t   SC_X1_SPD_OVRR1_5r_reg;
  SC_X1_SPD_OVRR2_5r_t   SC_X1_SPD_OVRR2_5r_reg;
  SC_X1_SPD_OVRR3_5r_t   SC_X1_SPD_OVRR3_5r_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  switch (st_entry_no){
  case 0:
    SC_X1_SPD_OVRR0_SPDr_CLR(SC_X1_SPD_OVRR0_SPDr_reg);
    SC_X1_SPD_OVRR0_0r_CLR(SC_X1_SPD_OVRR0_0r_reg);
    SC_X1_SPD_OVRR0_1r_CLR(SC_X1_SPD_OVRR0_1r_reg);
    SC_X1_SPD_OVRR0_2r_CLR(SC_X1_SPD_OVRR0_2r_reg);
    SC_X1_SPD_OVRR0_3r_CLR(SC_X1_SPD_OVRR0_3r_reg);
    SC_X1_SPD_OVRR0_4r_CLR(SC_X1_SPD_OVRR0_4r_reg);
    SC_X1_SPD_OVRR0_5r_CLR(SC_X1_SPD_OVRR0_5r_reg);
  break;
  case 1:
    SC_X1_SPD_OVRR1_SPDr_CLR(SC_X1_SPD_OVRR1_SPDr_reg);
    SC_X1_SPD_OVRR1_0r_CLR(SC_X1_SPD_OVRR1_0r_reg);
    SC_X1_SPD_OVRR1_1r_CLR(SC_X1_SPD_OVRR1_1r_reg);
    SC_X1_SPD_OVRR1_2r_CLR(SC_X1_SPD_OVRR1_2r_reg);
    SC_X1_SPD_OVRR1_3r_CLR(SC_X1_SPD_OVRR1_3r_reg);
    SC_X1_SPD_OVRR1_4r_CLR(SC_X1_SPD_OVRR1_4r_reg);
    SC_X1_SPD_OVRR1_5r_CLR(SC_X1_SPD_OVRR1_5r_reg);
  break;
  case 2:
    SC_X1_SPD_OVRR2_SPDr_CLR(SC_X1_SPD_OVRR2_SPDr_reg);
    SC_X1_SPD_OVRR2_0r_CLR(SC_X1_SPD_OVRR2_0r_reg);
    SC_X1_SPD_OVRR2_1r_CLR(SC_X1_SPD_OVRR2_1r_reg);
    SC_X1_SPD_OVRR2_2r_CLR(SC_X1_SPD_OVRR2_2r_reg);
    SC_X1_SPD_OVRR2_3r_CLR(SC_X1_SPD_OVRR2_3r_reg);
    SC_X1_SPD_OVRR2_4r_CLR(SC_X1_SPD_OVRR2_4r_reg);
    SC_X1_SPD_OVRR2_5r_CLR(SC_X1_SPD_OVRR2_5r_reg);
  break;
  case 3:
    SC_X1_SPD_OVRR3_SPDr_CLR(SC_X1_SPD_OVRR3_SPDr_reg);
    SC_X1_SPD_OVRR3_0r_CLR(SC_X1_SPD_OVRR3_0r_reg);
    SC_X1_SPD_OVRR3_1r_CLR(SC_X1_SPD_OVRR3_1r_reg);
    SC_X1_SPD_OVRR3_2r_CLR(SC_X1_SPD_OVRR3_2r_reg);
    SC_X1_SPD_OVRR3_3r_CLR(SC_X1_SPD_OVRR3_3r_reg);
    SC_X1_SPD_OVRR3_4r_CLR(SC_X1_SPD_OVRR3_4r_reg);
    SC_X1_SPD_OVRR3_5r_CLR(SC_X1_SPD_OVRR3_5r_reg);
  break;
  }
  switch (st_entry_no){
  case 0:
    switch (st_control_field){
    case OVERRIDE_NUM_LANES:
      SC_X1_SPD_OVRR0_SPDr_NUM_LANESf_SET(SC_X1_SPD_OVRR0_SPDr_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_SPDr(pc, SC_X1_SPD_OVRR0_SPDr_reg));
      break;
    case OVERRIDE_OS_MODE:
      SC_X1_SPD_OVRR0_0r_OS_MODEf_SET(SC_X1_SPD_OVRR0_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_0r(pc, SC_X1_SPD_OVRR0_0r_reg));
      break;
    case OVERRIDE_T_FIFO_MODE:
      SC_X1_SPD_OVRR0_0r_T_FIFO_MODEf_SET(SC_X1_SPD_OVRR0_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_0r(pc, SC_X1_SPD_OVRR0_0r_reg));
      break;
    case OVERRIDE_T_ENC_MODE:
      SC_X1_SPD_OVRR0_0r_T_ENC_MODEf_SET(SC_X1_SPD_OVRR0_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_0r(pc, SC_X1_SPD_OVRR0_0r_reg));
      break;
    case OVERRIDE_T_HG2_ENABLE:
      SC_X1_SPD_OVRR0_0r_T_HG2_ENABLEf_SET(SC_X1_SPD_OVRR0_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_0r(pc, SC_X1_SPD_OVRR0_0r_reg));
      break;
    case OVERRIDE_SCR_MODE:
      SC_X1_SPD_OVRR0_0r_SCR_MODEf_SET(SC_X1_SPD_OVRR0_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_0r(pc, SC_X1_SPD_OVRR0_0r_reg));
      break;
    case OVERRIDE_DESCR_MODE_OEN:
      SC_X1_SPD_OVRR0_1r_DESCR_MODEf_SET(SC_X1_SPD_OVRR0_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_1r(pc, SC_X1_SPD_OVRR0_1r_reg));
      break;
    case OVERRIDE_DEC_TL_MODE:
      SC_X1_SPD_OVRR0_1r_DEC_TL_MODEf_SET(SC_X1_SPD_OVRR0_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_1r(pc, SC_X1_SPD_OVRR0_1r_reg));
      break;
    case OVERRIDE_DESKEW_MODE:
      SC_X1_SPD_OVRR0_1r_DESKEW_MODEf_SET(SC_X1_SPD_OVRR0_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_1r(pc, SC_X1_SPD_OVRR0_1r_reg));
      break;
    case OVERRIDE_DEC_FSM_MODE:
      SC_X1_SPD_OVRR0_1r_DEC_FSM_MODEf_SET(SC_X1_SPD_OVRR0_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_1r(pc, SC_X1_SPD_OVRR0_1r_reg));
      break;
    case OVERRIDE_CL72_EN:
      SC_X1_SPD_OVRR0_0r_CL72_ENf_SET(SC_X1_SPD_OVRR0_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_0r(pc, SC_X1_SPD_OVRR0_0r_reg));
      break;
    case OVERRIDE_CLOCKCNT0:
      SC_X1_SPD_OVRR0_2r_CLOCKCNT0f_SET(SC_X1_SPD_OVRR0_2r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_2r(pc, SC_X1_SPD_OVRR0_2r_reg));
      break;
    case OVERRIDE_CLOCKCNT1:
      SC_X1_SPD_OVRR0_3r_CLOCKCNT1f_SET(SC_X1_SPD_OVRR0_3r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_3r(pc, SC_X1_SPD_OVRR0_3r_reg));
      break;
    case OVERRIDE_LOOPCNT0:
      SC_X1_SPD_OVRR0_4r_LOOPCNT0f_SET(SC_X1_SPD_OVRR0_4r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_4r(pc, SC_X1_SPD_OVRR0_4r_reg));
      break;
    case OVERRIDE_LOOPCNT1:
      SC_X1_SPD_OVRR0_4r_LOOPCNT1f_SET(SC_X1_SPD_OVRR0_4r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_4r(pc, SC_X1_SPD_OVRR0_4r_reg));
      break;
    case OVERRIDE_MAC_CREDITGENCNT:
      SC_X1_SPD_OVRR0_5r_MAC_CREDITGENCNTf_SET(SC_X1_SPD_OVRR0_5r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR0_5r(pc, SC_X1_SPD_OVRR0_5r_reg));
      break;
    default:
      return PHYMOD_E_FAIL;
      break;
    }
  break;
  case 1:
    switch (st_control_field){
    case OVERRIDE_NUM_LANES:
      SC_X1_SPD_OVRR1_SPDr_NUM_LANESf_SET(SC_X1_SPD_OVRR1_SPDr_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_SPDr(pc, SC_X1_SPD_OVRR1_SPDr_reg));
      break;
    case OVERRIDE_OS_MODE:
      SC_X1_SPD_OVRR1_0r_OS_MODEf_SET(SC_X1_SPD_OVRR1_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_0r(pc, SC_X1_SPD_OVRR1_0r_reg));
      break;
    case OVERRIDE_T_FIFO_MODE:
      SC_X1_SPD_OVRR1_0r_T_FIFO_MODEf_SET(SC_X1_SPD_OVRR1_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_0r(pc, SC_X1_SPD_OVRR1_0r_reg));
      break;
    case OVERRIDE_T_ENC_MODE:
      SC_X1_SPD_OVRR1_0r_T_ENC_MODEf_SET(SC_X1_SPD_OVRR1_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_0r(pc, SC_X1_SPD_OVRR1_0r_reg));
      break;
    case OVERRIDE_T_HG2_ENABLE:
      SC_X1_SPD_OVRR1_0r_T_HG2_ENABLEf_SET(SC_X1_SPD_OVRR1_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_0r(pc, SC_X1_SPD_OVRR1_0r_reg));
      break;
    case OVERRIDE_SCR_MODE:
      SC_X1_SPD_OVRR1_0r_SCR_MODEf_SET(SC_X1_SPD_OVRR1_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_0r(pc, SC_X1_SPD_OVRR1_0r_reg));
      break;
    case OVERRIDE_DESCR_MODE_OEN:
      SC_X1_SPD_OVRR1_1r_DESCR_MODEf_SET(SC_X1_SPD_OVRR1_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_1r(pc, SC_X1_SPD_OVRR1_1r_reg));
      break;
    case OVERRIDE_DEC_TL_MODE:
      SC_X1_SPD_OVRR1_1r_DEC_TL_MODEf_SET(SC_X1_SPD_OVRR1_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_1r(pc, SC_X1_SPD_OVRR1_1r_reg));
      break;
    case OVERRIDE_DESKEW_MODE:
      SC_X1_SPD_OVRR1_1r_DESKEW_MODEf_SET(SC_X1_SPD_OVRR1_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_1r(pc, SC_X1_SPD_OVRR1_1r_reg));
      break;
    case OVERRIDE_DEC_FSM_MODE:
      SC_X1_SPD_OVRR1_1r_DEC_FSM_MODEf_SET(SC_X1_SPD_OVRR1_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_1r(pc, SC_X1_SPD_OVRR1_1r_reg));
      break;
    case OVERRIDE_CL72_EN:
      SC_X1_SPD_OVRR1_0r_CL72_ENf_SET(SC_X1_SPD_OVRR1_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_0r(pc, SC_X1_SPD_OVRR1_0r_reg));
      break;
    case OVERRIDE_CLOCKCNT0:
      SC_X1_SPD_OVRR1_2r_CLOCKCNT0f_SET(SC_X1_SPD_OVRR1_2r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_2r(pc, SC_X1_SPD_OVRR1_2r_reg));
      break;
    case OVERRIDE_CLOCKCNT1:
      SC_X1_SPD_OVRR1_3r_CLOCKCNT1f_SET(SC_X1_SPD_OVRR1_3r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_3r(pc, SC_X1_SPD_OVRR1_3r_reg));
      break;
    case OVERRIDE_LOOPCNT0:
      SC_X1_SPD_OVRR1_4r_LOOPCNT0f_SET(SC_X1_SPD_OVRR1_4r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_4r(pc, SC_X1_SPD_OVRR1_4r_reg));
      break;
    case OVERRIDE_LOOPCNT1:
      SC_X1_SPD_OVRR1_4r_LOOPCNT1f_SET(SC_X1_SPD_OVRR1_4r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_4r(pc, SC_X1_SPD_OVRR1_4r_reg));
      break;
    case OVERRIDE_MAC_CREDITGENCNT:
      SC_X1_SPD_OVRR1_5r_MAC_CREDITGENCNTf_SET(SC_X1_SPD_OVRR1_5r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR1_5r(pc, SC_X1_SPD_OVRR1_5r_reg));
      break;
    default:
      return PHYMOD_E_FAIL;
      break;
    }
  break;
  case 2:
    switch (st_control_field){
    case OVERRIDE_NUM_LANES:
      SC_X1_SPD_OVRR2_SPDr_NUM_LANESf_SET(SC_X1_SPD_OVRR2_SPDr_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_SPDr(pc, SC_X1_SPD_OVRR2_SPDr_reg));
      break;
    case OVERRIDE_OS_MODE:
      SC_X1_SPD_OVRR2_0r_OS_MODEf_SET(SC_X1_SPD_OVRR2_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_0r(pc, SC_X1_SPD_OVRR2_0r_reg));
      break;
    case OVERRIDE_T_FIFO_MODE:
      SC_X1_SPD_OVRR2_0r_T_FIFO_MODEf_SET(SC_X1_SPD_OVRR2_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_0r(pc, SC_X1_SPD_OVRR2_0r_reg));
      break;
    case OVERRIDE_T_ENC_MODE:
      SC_X1_SPD_OVRR2_0r_T_ENC_MODEf_SET(SC_X1_SPD_OVRR2_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_0r(pc, SC_X1_SPD_OVRR2_0r_reg));
      break;
    case OVERRIDE_T_HG2_ENABLE:
      SC_X1_SPD_OVRR2_0r_T_HG2_ENABLEf_SET(SC_X1_SPD_OVRR2_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_0r(pc, SC_X1_SPD_OVRR2_0r_reg));
      break;
    case OVERRIDE_SCR_MODE:
      SC_X1_SPD_OVRR2_0r_SCR_MODEf_SET(SC_X1_SPD_OVRR2_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_0r(pc, SC_X1_SPD_OVRR2_0r_reg));
      break;
    case OVERRIDE_DESCR_MODE_OEN:
      SC_X1_SPD_OVRR2_1r_DESCR_MODEf_SET(SC_X1_SPD_OVRR2_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_1r(pc, SC_X1_SPD_OVRR2_1r_reg));
      break;
    case OVERRIDE_DEC_TL_MODE:
      SC_X1_SPD_OVRR2_1r_DEC_TL_MODEf_SET(SC_X1_SPD_OVRR2_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_1r(pc, SC_X1_SPD_OVRR2_1r_reg));
      break;
    case OVERRIDE_DESKEW_MODE:
      SC_X1_SPD_OVRR2_1r_DESKEW_MODEf_SET(SC_X1_SPD_OVRR2_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_1r(pc, SC_X1_SPD_OVRR2_1r_reg));
      break;
    case OVERRIDE_DEC_FSM_MODE:
      SC_X1_SPD_OVRR2_1r_DEC_FSM_MODEf_SET(SC_X1_SPD_OVRR2_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_1r(pc, SC_X1_SPD_OVRR2_1r_reg));
      break;
    case OVERRIDE_CL72_EN:
      SC_X1_SPD_OVRR2_0r_CL72_ENf_SET(SC_X1_SPD_OVRR2_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_0r(pc, SC_X1_SPD_OVRR2_0r_reg));
      break;
    case OVERRIDE_CLOCKCNT0:
      SC_X1_SPD_OVRR2_2r_CLOCKCNT0f_SET(SC_X1_SPD_OVRR2_2r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_2r(pc, SC_X1_SPD_OVRR2_2r_reg));
      break;
    case OVERRIDE_CLOCKCNT1:
      SC_X1_SPD_OVRR2_3r_CLOCKCNT1f_SET(SC_X1_SPD_OVRR2_3r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_3r(pc, SC_X1_SPD_OVRR2_3r_reg));
      break;
    case OVERRIDE_LOOPCNT0:
      SC_X1_SPD_OVRR2_4r_LOOPCNT0f_SET(SC_X1_SPD_OVRR2_4r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_4r(pc, SC_X1_SPD_OVRR2_4r_reg));
      break;
    case OVERRIDE_LOOPCNT1:
      SC_X1_SPD_OVRR2_4r_LOOPCNT1f_SET(SC_X1_SPD_OVRR2_4r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_4r(pc, SC_X1_SPD_OVRR2_4r_reg));
      break;
    case OVERRIDE_MAC_CREDITGENCNT:
      SC_X1_SPD_OVRR2_5r_MAC_CREDITGENCNTf_SET(SC_X1_SPD_OVRR2_5r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR2_5r(pc, SC_X1_SPD_OVRR2_5r_reg));
      break;
    default:
      return PHYMOD_E_FAIL;
      break;
    }
  break;
  case 3:
    switch (st_control_field){
    case OVERRIDE_NUM_LANES:
      SC_X1_SPD_OVRR3_SPDr_NUM_LANESf_SET(SC_X1_SPD_OVRR3_SPDr_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_SPDr(pc, SC_X1_SPD_OVRR3_SPDr_reg));
      break;
    case OVERRIDE_OS_MODE:
      SC_X1_SPD_OVRR3_0r_OS_MODEf_SET(SC_X1_SPD_OVRR3_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_0r(pc, SC_X1_SPD_OVRR3_0r_reg));
      break;
    case OVERRIDE_T_FIFO_MODE:
      SC_X1_SPD_OVRR3_0r_T_FIFO_MODEf_SET(SC_X1_SPD_OVRR3_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_0r(pc, SC_X1_SPD_OVRR3_0r_reg));
      break;
    case OVERRIDE_T_ENC_MODE:
      SC_X1_SPD_OVRR3_0r_T_ENC_MODEf_SET(SC_X1_SPD_OVRR3_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_0r(pc, SC_X1_SPD_OVRR3_0r_reg));
      break;
    case OVERRIDE_T_HG2_ENABLE:
      SC_X1_SPD_OVRR3_0r_T_HG2_ENABLEf_SET(SC_X1_SPD_OVRR3_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_0r(pc, SC_X1_SPD_OVRR3_0r_reg));
      break;
    case OVERRIDE_SCR_MODE:
      SC_X1_SPD_OVRR3_0r_SCR_MODEf_SET(SC_X1_SPD_OVRR3_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_0r(pc, SC_X1_SPD_OVRR3_0r_reg));
      break;
    case OVERRIDE_DESCR_MODE_OEN:
      SC_X1_SPD_OVRR3_1r_DESCR_MODEf_SET(SC_X1_SPD_OVRR3_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_1r(pc, SC_X1_SPD_OVRR3_1r_reg));
      break;
    case OVERRIDE_DEC_TL_MODE:
      SC_X1_SPD_OVRR3_1r_DEC_TL_MODEf_SET(SC_X1_SPD_OVRR3_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_1r(pc, SC_X1_SPD_OVRR3_1r_reg));
      break;
    case OVERRIDE_DESKEW_MODE:
      SC_X1_SPD_OVRR3_1r_DESKEW_MODEf_SET(SC_X1_SPD_OVRR3_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_1r(pc, SC_X1_SPD_OVRR3_1r_reg));
      break;
    case OVERRIDE_DEC_FSM_MODE:
      SC_X1_SPD_OVRR3_1r_DEC_FSM_MODEf_SET(SC_X1_SPD_OVRR3_1r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_1r(pc, SC_X1_SPD_OVRR3_1r_reg));
      break;
    case OVERRIDE_CL72_EN:
      SC_X1_SPD_OVRR3_0r_CL72_ENf_SET(SC_X1_SPD_OVRR3_0r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_0r(pc, SC_X1_SPD_OVRR3_0r_reg));
      break;
    case OVERRIDE_CLOCKCNT0:
      SC_X1_SPD_OVRR3_2r_CLOCKCNT0f_SET(SC_X1_SPD_OVRR3_2r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_2r(pc, SC_X1_SPD_OVRR3_2r_reg));
      break;
    case OVERRIDE_CLOCKCNT1:
      SC_X1_SPD_OVRR3_3r_CLOCKCNT1f_SET(SC_X1_SPD_OVRR3_3r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_3r(pc, SC_X1_SPD_OVRR3_3r_reg));
      break;
    case OVERRIDE_LOOPCNT0:
      SC_X1_SPD_OVRR3_4r_LOOPCNT0f_SET(SC_X1_SPD_OVRR3_4r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_4r(pc, SC_X1_SPD_OVRR3_4r_reg));
      break;
    case OVERRIDE_LOOPCNT1:
      SC_X1_SPD_OVRR3_4r_LOOPCNT1f_SET(SC_X1_SPD_OVRR3_4r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_4r(pc, SC_X1_SPD_OVRR3_4r_reg));
      break;
    case OVERRIDE_MAC_CREDITGENCNT:
      SC_X1_SPD_OVRR3_5r_MAC_CREDITGENCNTf_SET(SC_X1_SPD_OVRR3_5r_reg, st_field_value);
      PHYMOD_IF_ERR_RETURN (MODIFY_SC_X1_SPD_OVRR3_5r(pc, SC_X1_SPD_OVRR3_5r_reg));
      break;
    default:
      return PHYMOD_E_FAIL;
      break;
    }
  break;
  }
  return PHYMOD_E_NONE;
}

/**
@brief   EEE Control Set
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   enable EEE control(enable/disable)
@returns The value PHYMOD_E_NONE upon successful completion
@details EEE Control
         enable =1 : Allow LPI pass through. i.e. dont convert LPI.
         enable =0 : Convert LPI to idle. So  MAC will not see it.

*/
int tefmod16_eee_control_set(PHYMOD_ST* pc, uint32_t enable)
{
  RX_X4_PCS_CTL0r_t reg_pcs_ctrl0;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  RX_X4_PCS_CTL0r_CLR(reg_pcs_ctrl0);
  READ_RX_X4_PCS_CTL0r(pc, &reg_pcs_ctrl0);

  RX_X4_PCS_CTL0r_LPI_ENABLEf_SET(reg_pcs_ctrl0, enable & 0x1);
  PHYMOD_IF_ERR_RETURN (MODIFY_RX_X4_PCS_CTL0r(pc, reg_pcs_ctrl0));
  return PHYMOD_E_NONE;
}

/**
@brief   EEE Control Get
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   enable handle to receive EEE control
@returns The value PHYMOD_E_NONE upon successful completion
@details EEE Control
         enable =1 : Allow LPI pass through. i.e. dont convert LPI.
         enable =0 : Convert LPI to idle. So  MAC will not see it.

*/
int tefmod16_eee_control_get(PHYMOD_ST* pc, uint32_t *enable)
{
  RX_X4_PCS_CTL0r_t reg_pcs_ctrl0;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  RX_X4_PCS_CTL0r_CLR(reg_pcs_ctrl0);

  PHYMOD_IF_ERR_RETURN (READ_RX_X4_PCS_CTL0r(pc, &reg_pcs_ctrl0));
  *enable = RX_X4_PCS_CTL0r_LPI_ENABLEf_GET(reg_pcs_ctrl0);
  return PHYMOD_E_NONE;
}

/**
@brief   Setup the default for cl74 to ieee compliance
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details cl74 control set to ieee compliance
*/
int tefmod16_cl74_chng_default (PHYMOD_ST* pc)
{
  RX_X4_FEC1r_t RX_X4_FEC1reg;
  phymod_access_t pa_copy;
  int i;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
  RX_X4_FEC1r_CLR(RX_X4_FEC1reg);

  for (i = 0; i <4; i++) {
    pa_copy.lane_mask = 0x1 << i;
    PHYMOD_IF_ERR_RETURN(WRITE_RX_X4_FEC1r(&pa_copy, RX_X4_FEC1reg));
  }
  return PHYMOD_E_NONE;
}

int tefmod16_an_msa_mode_set(PHYMOD_ST* pc, uint32_t val) 
{
  
      AN_X4_LD_PAGE0r_t AN_X4_LD_PAGE0r_reg;
      TEFMOD16_DBG_IN_FUNC_INFO(pc);
      AN_X4_LD_PAGE0r_CLR(AN_X4_LD_PAGE0r_reg);
      AN_X4_LD_PAGE0r_SET(AN_X4_LD_PAGE0r_reg, val & 0x1);

      /********Setting AN_X4_LD_PAGE0r_t 0xc1e2*****/
      PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_PAGE0r(pc, AN_X4_LD_PAGE0r_reg));

  return PHYMOD_E_NONE;
}

int tefmod16_an_msa_mode_get(PHYMOD_ST* pc, uint32_t* val) 
{
  
      AN_X4_LD_PAGE0r_t AN_X4_LD_PAGE0r_reg;
      TEFMOD16_DBG_IN_FUNC_INFO(pc);
      AN_X4_LD_PAGE0r_CLR(AN_X4_LD_PAGE0r_reg);
      /********Getting AN_X4_LD_PAGE0r_t 0xc1e2*****/
      PHYMOD_IF_ERR_RETURN (READ_AN_X4_LD_PAGE0r(pc, &AN_X4_LD_PAGE0r_reg));
      *val = AN_X4_LD_PAGE0r_LD_PAGE_0f_GET(AN_X4_LD_PAGE0r_reg);
      return PHYMOD_E_NONE;
}

int tefmod16_an_oui_set(PHYMOD_ST* pc, tefmod16_an_oui_t oui) 
{

  AN_X1_OUI_LWRr_t   AN_X1_CONTROL_OUI_LOWERr_reg;
  AN_X1_OUI_UPRr_t   AN_X1_CONTROL_OUI_UPPERr_reg;
  AN_X4_CL73_CTLSr_t AN_X4_CL73_CTLSr_t_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  /********Setting AN X1 OUI LOWER 16 BITS 0x9241*****/
  AN_X1_OUI_LWRr_CLR(AN_X1_CONTROL_OUI_LOWERr_reg);
  AN_X1_OUI_LWRr_SET(AN_X1_CONTROL_OUI_LOWERr_reg, oui.oui & 0xffff);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_OUI_LWRr(pc, AN_X1_CONTROL_OUI_LOWERr_reg));
 
  /********Setting AN X1 OUI UPPER 8 BITS 0x9240*****/
  AN_X1_OUI_UPRr_CLR(AN_X1_CONTROL_OUI_UPPERr_reg);
  AN_X1_OUI_UPRr_SET(AN_X1_CONTROL_OUI_UPPERr_reg, (oui.oui >> 16) & 0xff);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_OUI_UPRr(pc, AN_X1_CONTROL_OUI_UPPERr_reg));

  /********CL73 AN MISC CONTROL 0xC1C6*****/
  AN_X4_CL73_CTLSr_CLR(AN_X4_CL73_CTLSr_t_reg);
  AN_X4_CL73_CTLSr_AN_OUI_OVERRIDE_BAM73_ADVf_SET(AN_X4_CL73_CTLSr_t_reg, oui.oui_override_bam73_adv);
  AN_X4_CL73_CTLSr_AN_OUI_OVERRIDE_BAM73_DETf_SET(AN_X4_CL73_CTLSr_t_reg, oui.oui_override_bam73_det);
  AN_X4_CL73_CTLSr_AN_OUI_OVERRIDE_HPAM_ADVf_SET(AN_X4_CL73_CTLSr_t_reg,  oui.oui_override_hpam_adv);
  AN_X4_CL73_CTLSr_AN_OUI_OVERRIDE_HPAM_DETf_SET(AN_X4_CL73_CTLSr_t_reg,  oui.oui_override_hpam_det);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_CL73_CTLSr(pc, AN_X4_CL73_CTLSr_t_reg));

  return PHYMOD_E_NONE;
}

int tefmod16_an_oui_get(PHYMOD_ST* pc, tefmod16_an_oui_t* oui) 
{

  AN_X1_OUI_LWRr_t   AN_X1_CONTROL_OUI_LOWERr_reg;
  AN_X1_OUI_UPRr_t   AN_X1_CONTROL_OUI_UPPERr_reg;
  AN_X4_CL73_CTLSr_t AN_X4_CL73_CTLSr_t_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);

  /********Setting AN X1 OUI LOWER 16 BITS 0x9241*****/
  PHYMOD_IF_ERR_RETURN(READ_AN_X1_OUI_LWRr(pc, &AN_X1_CONTROL_OUI_LOWERr_reg));
  /********Setting AN X1 OUI UPPER 8 BITS 0x9240*****/
  PHYMOD_IF_ERR_RETURN(READ_AN_X1_OUI_UPRr(pc, &AN_X1_CONTROL_OUI_UPPERr_reg));
  /********CL73 AN MISC CONTROL 0xC1C6*****/
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_CL73_CTLSr(pc, &AN_X4_CL73_CTLSr_t_reg));

  oui->oui  = AN_X1_OUI_LWRr_GET(AN_X1_CONTROL_OUI_LOWERr_reg);
  oui->oui |= ((AN_X1_OUI_UPRr_GET(AN_X1_CONTROL_OUI_UPPERr_reg)) << 16);

  oui->oui_override_bam73_adv = AN_X4_CL73_CTLSr_AN_OUI_OVERRIDE_BAM73_ADVf_GET(AN_X4_CL73_CTLSr_t_reg);
  oui->oui_override_bam73_det = AN_X4_CL73_CTLSr_AN_OUI_OVERRIDE_BAM73_DETf_GET(AN_X4_CL73_CTLSr_t_reg);
  oui->oui_override_hpam_adv  = AN_X4_CL73_CTLSr_AN_OUI_OVERRIDE_HPAM_ADVf_GET(AN_X4_CL73_CTLSr_t_reg);
  oui->oui_override_hpam_det  = AN_X4_CL73_CTLSr_AN_OUI_OVERRIDE_HPAM_DETf_GET(AN_X4_CL73_CTLSr_t_reg);

  return PHYMOD_E_NONE;
}

int tefmod16_autoneg_status_get(PHYMOD_ST* pc, int *an_en, int *an_done) 
{
  AN_X4_CL73_CFGr_t  AN_X4_CL73_CFGr_reg;
  AN_X4_AN_MISC_STSr_t  AN_X4_AN_MISC_STSr_reg;

  AN_X4_CL73_CFGr_CLR(AN_X4_CL73_CFGr_reg);
  AN_X4_AN_MISC_STSr_CLR(AN_X4_AN_MISC_STSr_reg);

  PHYMOD_IF_ERR_RETURN (READ_AN_X4_CL73_CFGr(pc, &AN_X4_CL73_CFGr_reg));
  PHYMOD_IF_ERR_RETURN (READ_AN_X4_AN_MISC_STSr(pc, &AN_X4_AN_MISC_STSr_reg));

  *an_en = AN_X4_CL73_CFGr_CL73_AN_RESTARTf_GET(AN_X4_CL73_CFGr_reg);
  *an_done = AN_X4_AN_MISC_STSr_AN_COMPLETEf_GET(AN_X4_AN_MISC_STSr_reg);

  return PHYMOD_E_NONE;
}

int tefmod16_autoneg_lp_status_get(PHYMOD_ST* pc, tefmod16_an_adv_ability_t *an_ability_st) 
{
  AN_X4_LP_BASE1r_t   AN_X4_LP_BASE1r_reg;
  AN_X4_LP_BASE2r_t   AN_X4_LP_BASE2r_reg;
  AN_X4_LP_BASE3r_t   AN_X4_LP_BASE3r_reg;
  AN_X4_LP_OUI_UP4r_t AN_X4_LP_OUI_UP4r_reg;
  AN_X4_LP_OUI_UP5r_t AN_X4_LP_OUI_UP5r_reg;
  uint32_t  lp_base2;
  uint32_t  lp_base3;

  AN_X4_LP_BASE1r_CLR(  AN_X4_LP_BASE1r_reg);
  AN_X4_LP_BASE2r_CLR(  AN_X4_LP_BASE2r_reg);
  AN_X4_LP_BASE3r_CLR(  AN_X4_LP_BASE3r_reg);
  AN_X4_LP_OUI_UP4r_CLR(AN_X4_LP_OUI_UP4r_reg);
  AN_X4_LP_OUI_UP5r_CLR(AN_X4_LP_OUI_UP5r_reg);

  /**** 0xC1D5 ****/
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE1r(pc,  &AN_X4_LP_BASE1r_reg));
  /**** 0xC1D6 ****/
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE2r(pc,  &AN_X4_LP_BASE2r_reg));
  /**** 0xC1D7 ****/
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE3r(pc,  &AN_X4_LP_BASE3r_reg));
  /**** 0xC1DB ****/
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_OUI_UP4r(pc,&AN_X4_LP_OUI_UP4r_reg));
  /**** 0xC1DC ****/
  PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_OUI_UP5r(pc,&AN_X4_LP_OUI_UP5r_reg));

  /* cl73 technology abilities A0 - D21 */
  if (AN_X4_LP_BASE2r_BASE_1G_KXf_GET(AN_X4_LP_BASE2r_reg)){
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_1GBASE_KX);
  }
  /* cl73 technology abilities A2 - D23 */
  if (AN_X4_LP_BASE2r_BASE_10G_KRf_GET(AN_X4_LP_BASE2r_reg)){
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_10GBASE_KR1);
  }
  /* cl73 technology abilities A3 - D24 */
  if (AN_X4_LP_BASE2r_BASE_40G_KR4f_GET(AN_X4_LP_BASE2r_reg)){
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_40GBASE_KR4);
  }
  /* cl73 technology abilities A4 - D25 */
  if (AN_X4_LP_BASE2r_BASE_40G_CR4f_GET(AN_X4_LP_BASE2r_reg)){
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_40GBASE_CR4);
  }
  /* cl73 technology abilities A7 - D28 */
  if (AN_X4_LP_BASE2r_BASE_100G_KR4f_GET(AN_X4_LP_BASE2r_reg)){
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_100GBASE_KR4);
  }
  /* cl73 technology abilities A8 - D29 */
  if (AN_X4_LP_BASE2r_BASE_100G_CR4f_GET(AN_X4_LP_BASE2r_reg)){
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_100GBASE_CR4);
  }

  lp_base2 = AN_X4_LP_BASE2r_GET(AN_X4_LP_BASE2r_reg);
  /* Below two 25G Base abilities are not defined in RDB.
   * So here we use SW defined macros to get the ability bits.
  */
  /* cl73 technology abilities A9 - D30 */
  /* 25GBASE_KRS and 25GBASE_CRS share the same bit. */
  if ((lp_base2 >> AN_BASE_TECH_ABILITY_25GKRS_OFFSET) & AN_BASE_TECH_ABILITY_25GKRS_MASK) {
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_25GBASE_KRS1);
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_25GBASE_CRS1);
  }
  /* cl73 technology abilities A10 - D31 */
  /* 25GBASE_KR and 25GBASE_CR share the same bit. */
  if ((lp_base2 >> AN_BASE_TECH_ABILITY_25GKR_OFFSET) & AN_BASE_TECH_ABILITY_25GKR_MASK) {
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_25GBASE_KR1);
      an_ability_st->an_base_speed |= (1 << TEFMOD16_CL73_25GBASE_CR1);
  }

  an_ability_st->an_fec = 0;

  /* FEC[F0:F1] is encoded in bits [D46:D47] */
  if((AN_X4_LP_BASE3r_FEC_REQUESTEDf_GET(AN_X4_LP_BASE3r_reg)) &&
     (AN_X4_LP_BASE3r_FEC_ABILITYf_GET(AN_X4_LP_BASE3r_reg))) {
      an_ability_st->an_fec |= TEFMOD16_FEC_CL74_SUPRTD_REQSTD;
  }

  /* FEC[F2:F3] is encoded in bits [D44:D45] */
  lp_base3 = AN_X4_LP_BASE3r_GET(AN_X4_LP_BASE3r_reg);
  /* Below 25G FEC abilities are not defined in RDB.
   * So here we use SW defined macros to get 25G FEC abilities.
   */
  if ((lp_base3 >> AN_BASE_CL91_ABILITY_REQ_OFFSET) & AN_BASE_CL91_ABILITY_REQ_MASK) {
      an_ability_st->an_fec |= TEFMOD16_FEC_CL91_SUPRTD_REQSTD;
  }
  if ((lp_base3 >> AN_BASE_CL74_25GKRS_REQ_OFFSET) & AN_BASE_CL74_25GKRS_REQ_MASK) {
      an_ability_st->an_fec |= TEFMOD16_FEC_CL74_SUPRTD_REQSTD;
  }

  if (an_ability_st->an_fec == 0) {
      if (AN_X4_LP_BASE3r_FEC_ABILITYf_GET(AN_X4_LP_BASE3r_reg)) {
          an_ability_st->an_fec = TEFMOD16_FEC_SUPRTD_NOT_REQSTD;
      } else {
          an_ability_st->an_fec = TEFMOD16_FEC_NOT_SUPRTD;
      }
  }

  /* advertise pause capabilities C[0:1] - D[10:11]*/
  if (AN_X4_LP_BASE1r_SYMMETRIC_PAUSEf_GET(AN_X4_LP_BASE1r_reg)) {
      an_ability_st->an_pause |= TEFMOD16_SYMM_PAUSE;
  }
  if (AN_X4_LP_BASE1r_ASYMMETRIC_PAUSEf_GET(AN_X4_LP_BASE1r_reg)) {
      an_ability_st->an_pause |= TEFMOD16_ASYM_PAUSE;
  }

  /* BAM73 OUI UP bit 16 */
  if(AN_X4_LP_OUI_UP4r_BAM_20G_KR2f_GET(AN_X4_LP_OUI_UP4r_reg)) {
      an_ability_st->an_bam_speed |= (1 << TEFMOD16_CL73_BAM_20GBASE_KR2);
  }
  /* BAM73 OUI UP bit 17 */
  if(AN_X4_LP_OUI_UP4r_BAM_20G_CR2f_GET(AN_X4_LP_OUI_UP4r_reg)) {
      an_ability_st->an_bam_speed |= (1 << TEFMOD16_CL73_BAM_20GBASE_CR2);
  }
  /* BAM73 OUI UP bit 18 */
  if(AN_X4_LP_OUI_UP4r_BAM_20G_KR1f_GET(AN_X4_LP_OUI_UP4r_reg)) {
      an_ability_st->an_bam_speed1 |= (1 << TEFMOD16_CL73_BAM_20GBASE_KR1);
  }
  /* BAM73 OUI UP bit 19 */
  if(AN_X4_LP_OUI_UP4r_BAM_20G_CR1f_GET(AN_X4_LP_OUI_UP4r_reg)) {
      an_ability_st->an_bam_speed1 |= (1 << TEFMOD16_CL73_BAM_20GBASE_CR1);
  }
  /* BAM73 OUI UP bit 20 */
  if(AN_X4_LP_OUI_UP4r_BAM_25G_KR1f_GET(AN_X4_LP_OUI_UP4r_reg)) {
      an_ability_st->an_bam_speed1 |= (1 << TEFMOD16_CL73_BAM_25GBASE_KR1);
  }
  /* BAM73 OUI UP bit 21 */
  if(AN_X4_LP_OUI_UP4r_BAM_25G_CR1f_GET(AN_X4_LP_OUI_UP4r_reg)) {
      an_ability_st->an_bam_speed1 |= (1 << TEFMOD16_CL73_BAM_25GBASE_CR1);
  }
  /* BAM73 OUI UP bit 22 */
  if(AN_X4_LP_OUI_UP4r_BAM_40G_KR2f_GET(AN_X4_LP_OUI_UP4r_reg)) {
      an_ability_st->an_bam_speed |= (1 << TEFMOD16_CL73_BAM_40GBASE_KR2);
  }
  /* BAM73 OUI UP bit 23 */
  if(AN_X4_LP_OUI_UP4r_BAM_40G_CR2f_GET(AN_X4_LP_OUI_UP4r_reg)) {
      an_ability_st->an_bam_speed |= (1 << TEFMOD16_CL73_BAM_40GBASE_CR2);
  }
  /* BAM73 OUI UP bit 24 */
  if(AN_X4_LP_OUI_UP4r_BAM_50G_KR2f_GET(AN_X4_LP_OUI_UP4r_reg)) {
      an_ability_st->an_bam_speed |= (1 << TEFMOD16_CL73_BAM_50GBASE_KR2);
  }
  /* BAM73 OUI UP bit 25 */
  if(AN_X4_LP_OUI_UP4r_BAM_50G_CR2f_GET(AN_X4_LP_OUI_UP4r_reg)) {
      an_ability_st->an_bam_speed |= (1 << TEFMOD16_CL73_BAM_50GBASE_CR2);
  }

  /*
   * BAM 50G 4-lane is not supported in TSCF16(TH2 and TD3) chips.  
   * 
   * BAM73 OUI UP bit 32 */
  /*
  if(AN_X4_LP_OUI_UP5r_BAM_50G_KR4f_GET(AN_X4_LP_OUI_UP5r_reg)) {
      an_ability_st->an_bam_speed |= (1 << TEFMOD16_CL73_BAM_50GBASE_KR4);
  } */
  /* BAM73 OUI UP bit 33 */
  /*
  if(AN_X4_LP_OUI_UP5r_BAM_50G_CR4f_GET(AN_X4_LP_OUI_UP5r_reg)) {
      an_ability_st->an_bam_speed |= (1 << TEFMOD16_CL73_BAM_50GBASE_CR4);
  }
*/

  return PHYMOD_E_NONE;
}


int tefmod16_hg2_codec_set(PHYMOD_ST* pc, tefmod16_hg2_codec_t hg2_codec)
{
  TX_X4_ENC0r_t  TX_X4_ENC0_reg;
  RX_X4_DEC_CTL0r_t  RX_X4_DEC_CTL0_reg;

  READ_TX_X4_ENC0r(pc, &TX_X4_ENC0_reg);
  READ_RX_X4_DEC_CTL0r(pc, &RX_X4_DEC_CTL0_reg);
  switch (hg2_codec) {
  case TEFMOD16_HG2_CODEC_OFF:
    TX_X4_ENC0r_HG2_ENABLEf_SET(TX_X4_ENC0_reg, 0);
    TX_X4_ENC0r_HG2_CODECf_SET(TX_X4_ENC0_reg, 1);
    TX_X4_ENC0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(TX_X4_ENC0_reg, 1);
    RX_X4_DEC_CTL0r_HG2_ENABLEf_SET(RX_X4_DEC_CTL0_reg, 0);
    RX_X4_DEC_CTL0r_HG2_CODECf_SET(RX_X4_DEC_CTL0_reg, 1);
    RX_X4_DEC_CTL0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(RX_X4_DEC_CTL0_reg, 1);
    break;
  case TEFMOD16_HG2_CODEC_ON_8BYTE_IPG:
    TX_X4_ENC0r_HG2_ENABLEf_SET(TX_X4_ENC0_reg, 1);
    TX_X4_ENC0r_HG2_CODECf_SET(TX_X4_ENC0_reg, 1);
    TX_X4_ENC0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(TX_X4_ENC0_reg, 1);
    RX_X4_DEC_CTL0r_HG2_ENABLEf_SET(RX_X4_DEC_CTL0_reg, 1);
    RX_X4_DEC_CTL0r_HG2_CODECf_SET(RX_X4_DEC_CTL0_reg, 1);
    RX_X4_DEC_CTL0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(RX_X4_DEC_CTL0_reg, 1);
    break;
  case TEFMOD16_HG2_CODEC_ON_9BYTE_IPG:
    TX_X4_ENC0r_HG2_ENABLEf_SET(TX_X4_ENC0_reg, 1);
    TX_X4_ENC0r_HG2_CODECf_SET(TX_X4_ENC0_reg, 0);
    TX_X4_ENC0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(TX_X4_ENC0_reg, 1);
    RX_X4_DEC_CTL0r_HG2_ENABLEf_SET(RX_X4_DEC_CTL0_reg, 1);
    RX_X4_DEC_CTL0r_HG2_CODECf_SET(RX_X4_DEC_CTL0_reg, 0);
    RX_X4_DEC_CTL0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(RX_X4_DEC_CTL0_reg, 1);
    break;
  default:
    TX_X4_ENC0r_HG2_ENABLEf_SET(TX_X4_ENC0_reg, 0);
    TX_X4_ENC0r_HG2_CODECf_SET(TX_X4_ENC0_reg, 1);
    TX_X4_ENC0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(TX_X4_ENC0_reg, 1);
    RX_X4_DEC_CTL0r_HG2_ENABLEf_SET(RX_X4_DEC_CTL0_reg, 0);
    RX_X4_DEC_CTL0r_HG2_CODECf_SET(RX_X4_DEC_CTL0_reg, 1);
    RX_X4_DEC_CTL0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(RX_X4_DEC_CTL0_reg, 1);
    break;
  }
  MODIFY_RX_X4_DEC_CTL0r(pc, RX_X4_DEC_CTL0_reg);
  MODIFY_TX_X4_ENC0r(pc,TX_X4_ENC0_reg); 

  return PHYMOD_E_NONE;
}

int tefmod16_hg2_codec_get(PHYMOD_ST* pc, tefmod16_hg2_codec_t* hg2_codec)
{
  TX_X4_ENC0r_t  TX_X4_ENC0_reg;

  READ_TX_X4_ENC0r(pc, &TX_X4_ENC0_reg);
  if (TX_X4_ENC0r_HG2_ENABLEf_GET(TX_X4_ENC0_reg)) {
    if (TX_X4_ENC0r_HG2_CODECf_GET(TX_X4_ENC0_reg)) {
        *hg2_codec = TEFMOD16_HG2_CODEC_ON_8BYTE_IPG;
    } else {
        *hg2_codec = TEFMOD16_HG2_CODEC_ON_9BYTE_IPG;
    }
  } else {
        *hg2_codec = TEFMOD16_HG2_CODEC_OFF;
            
  }
  return PHYMOD_E_NONE;
}

int tefmod16_ams_pll_drv_hv_disable_set(PHYMOD_ST* pc, int disable)
{
  AMS_PLL_CTL1r_t AMS_PLL_CTL1_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  AMS_PLL_CTL1r_CLR(AMS_PLL_CTL1_reg);
  READ_AMS_PLL_CTL1r(pc, &AMS_PLL_CTL1_reg);
  AMS_PLL_CTL1r_AMS_PLL_DRV_HV_DISABLEf_SET(AMS_PLL_CTL1_reg, disable & 0x1);
  PHYMOD_IF_ERR_RETURN (MODIFY_AMS_PLL_CTL1r(pc, AMS_PLL_CTL1_reg));

  return PHYMOD_E_NONE;
}

int tefmod16_ams_pll_drv_hv_disable_get(PHYMOD_ST* pc, int* disable)
{
  AMS_PLL_CTL1r_t AMS_PLL_CTL1_reg;

  TEFMOD16_DBG_IN_FUNC_INFO(pc);
  READ_AMS_PLL_CTL1r(pc, &AMS_PLL_CTL1_reg);
  *disable = AMS_PLL_CTL1r_AMS_PLL_DRV_HV_DISABLEf_GET(AMS_PLL_CTL1_reg);

  return PHYMOD_E_NONE;
}

int tefmod16_pll_vco2_select(PHYMOD_ST* pc,  uint32_t vco_rate)
{
   AMS_PLL_CTL1r_t AMS_PLL_CTL1_reg;

   TEFMOD16_DBG_IN_FUNC_INFO(pc);
   AMS_PLL_CTL1r_CLR(AMS_PLL_CTL1_reg);
   READ_AMS_PLL_CTL1r(pc, &AMS_PLL_CTL1_reg);
   AMS_PLL_CTL1r_AMS_PLL_VCO2_15Gf_SET(AMS_PLL_CTL1_reg, (vco_rate <= TEFMOD16_VCO_RATE_15G)? 1 : 0 );
   PHYMOD_IF_ERR_RETURN(MODIFY_AMS_PLL_CTL1r(pc, AMS_PLL_CTL1_reg));

   return PHYMOD_E_NONE;

}

int tefmod16_default_init(PHYMOD_ST* pc)               /* default set*/
{
  MAIN0_SPD_CTLr_t MAIN0_SPD_CTLr_reg;
  MAIN0_SPD_CTLr_CLR(MAIN0_SPD_CTLr_reg);
  /* select tsc_clk_credit default is pmd_tx_clk */
  MAIN0_SPD_CTLr_TSC_CREDIT_SELf_SET(MAIN0_SPD_CTLr_reg, 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SPD_CTLr(pc, MAIN0_SPD_CTLr_reg));

  return PHYMOD_E_NONE;

}

int tefmod16_fec_correctable_counter_get(PHYMOD_ST* pc, uint32_t* count)
{
    RX_X4_FEC_CORRBLKSL0r_t countl_0;
    RX_X4_FEC_CORRBLKSL1r_t countl_1;
    RX_X4_FEC_CORRBLKSL2r_t countl_2;
    RX_X4_FEC_CORRBLKSL3r_t countl_3;
    RX_X4_FEC_CORRBLKSL4r_t countl_4;
    RX_X4_FEC_CORRBLKSH0r_t counth_0;
    RX_X4_FEC_CORRBLKSH1r_t counth_1;
    RX_X4_FEC_CORRBLKSH2r_t counth_2;
    RX_X4_FEC_CORRBLKSH3r_t counth_3;
    RX_X4_FEC_CORRBLKSH4r_t counth_4;
    uint32_t sum = 0, count_vl =0;

    /* Per lane based counter */
    /* 1 physical lane can map to 5 virtual lane */
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORRBLKSL0r(pc, &countl_0));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORRBLKSH0r(pc, &counth_0));
    sum = (RX_X4_FEC_CORRBLKSH0r_GET(counth_0) << 16) | (RX_X4_FEC_CORRBLKSL0r_GET(countl_0) & 0xffff);
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORRBLKSL1r(pc, &countl_1));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORRBLKSH1r(pc, &counth_1));
    count_vl = (RX_X4_FEC_CORRBLKSH1r_GET(counth_1) << 16) | (RX_X4_FEC_CORRBLKSL1r_GET(countl_1) & 0xffff);
    /* Check overflow */
    /* If sum+count_vl > 0xffffffff, will cause overflow, set sum=0xffffffff */
    if (count_vl > 0xffffffff - sum) {
        sum = 0xffffffff;
    } else {
        sum += count_vl;
    }
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORRBLKSL2r(pc, &countl_2));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORRBLKSH2r(pc, &counth_2));
    count_vl = (RX_X4_FEC_CORRBLKSH2r_GET(counth_2) << 16) | (RX_X4_FEC_CORRBLKSL2r_GET(countl_2) & 0xffff);
    /* Check overflow */
    if (count_vl > 0xffffffff - sum) {
        sum = 0xffffffff;
    } else {
        sum += count_vl;
    }
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORRBLKSL3r(pc, &countl_3));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORRBLKSH3r(pc, &counth_3));
    count_vl = (RX_X4_FEC_CORRBLKSH3r_GET(counth_3) << 16) | (RX_X4_FEC_CORRBLKSL3r_GET(countl_3) & 0xffff);
    /* Check overflow */
    if (count_vl > 0xffffffff - sum) {
        sum = 0xffffffff;
    } else {
        sum += count_vl;
    }
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORRBLKSL4r(pc, &countl_4));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORRBLKSH4r(pc, &counth_4));
    count_vl = (RX_X4_FEC_CORRBLKSH4r_GET(counth_4) << 16) | (RX_X4_FEC_CORRBLKSL4r_GET(countl_4) & 0xffff);
    /* Check overflow */
    if (count_vl > 0xffffffff - sum) {
        *count = 0xffffffff;
    } else {
        *count = sum + count_vl;
    }

    return PHYMOD_E_NONE;
}

int tefmod16_fec_uncorrectable_counter_get(PHYMOD_ST* pc, uint32_t* count)
{
    RX_X4_FEC_UNCORRBLKSL0r_t countl_0;
    RX_X4_FEC_UNCORRBLKSL1r_t countl_1;
    RX_X4_FEC_UNCORRBLKSL2r_t countl_2;
    RX_X4_FEC_UNCORRBLKSL3r_t countl_3;
    RX_X4_FEC_UNCORRBLKSL4r_t countl_4;
    RX_X4_FEC_UNCORRBLKSH0r_t counth_0;
    RX_X4_FEC_UNCORRBLKSH1r_t counth_1;
    RX_X4_FEC_UNCORRBLKSH2r_t counth_2;
    RX_X4_FEC_UNCORRBLKSH3r_t counth_3;
    RX_X4_FEC_UNCORRBLKSH4r_t counth_4;
    uint32_t sum = 0, count_vl = 0;

    /* Per lane based counter */
    /* 1 physical lane can map to 5 virtual lane */
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORRBLKSL0r(pc, &countl_0));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORRBLKSH0r(pc, &counth_0));
    sum = (RX_X4_FEC_UNCORRBLKSH0r_GET(counth_0) << 16) | (RX_X4_FEC_UNCORRBLKSL0r_GET(countl_0) & 0xffff);
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORRBLKSL1r(pc, &countl_1));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORRBLKSH1r(pc, &counth_1));
    count_vl = (RX_X4_FEC_UNCORRBLKSH1r_GET(counth_1) << 16) | (RX_X4_FEC_UNCORRBLKSL1r_GET(countl_1) & 0xffff);
    /* Check overflow */
    /* If sum+count_vl > 0xffffffff, will cause overflow, set sum=0xffffffff */
    if (count_vl > 0xffffffff - sum) {
        sum = 0xffffffff;
    } else {
        sum += count_vl;
    }
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORRBLKSL2r(pc, &countl_2));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORRBLKSH2r(pc, &counth_2));
    count_vl = (RX_X4_FEC_UNCORRBLKSH2r_GET(counth_2) << 16) | (RX_X4_FEC_UNCORRBLKSL2r_GET(countl_2) & 0xffff);
    /* Check overflow */
    if (count_vl > 0xffffffff - sum) {
        sum = 0xffffffff;
    } else {
        sum += count_vl;
    }
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORRBLKSL3r(pc, &countl_3));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORRBLKSH3r(pc, &counth_3));
    count_vl = (RX_X4_FEC_UNCORRBLKSH3r_GET(counth_3) << 16) | (RX_X4_FEC_UNCORRBLKSL3r_GET(countl_3) & 0xffff);
    /* Check overflow */
    if (count_vl > 0xffffffff - sum) {
        sum = 0xffffffff;
    } else {
        sum += count_vl;
    }
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORRBLKSL4r(pc, &countl_4));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORRBLKSH4r(pc, &counth_4));
    count_vl = (RX_X4_FEC_UNCORRBLKSH4r_GET(counth_4) << 16) | (RX_X4_FEC_UNCORRBLKSL4r_GET(countl_4) & 0xffff);
    /* Check overflow */
    if (count_vl > 0xffffffff - sum) {
        *count = 0xffffffff;
    } else {
        *count = sum + count_vl;
    }

    return PHYMOD_E_NONE;
}

int tefmod16_fec_cl91_correctable_counter_get(PHYMOD_ST* pc, uint32_t* count)
{
    RX_X4_FEC_CORR_CTR0r_t count0;
    RX_X4_FEC_CORR_CTR1r_t count1;

    /* Per port based counter */
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORR_CTR0r(pc, &count0));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORR_CTR1r(pc, &count1));
    *count = (RX_X4_FEC_CORR_CTR1r_GET(count1) << 16) | (RX_X4_FEC_CORR_CTR0r_GET(count0) & 0xffff);

    return PHYMOD_E_NONE;
}

int tefmod16_fec_cl91_uncorrectable_counter_get(PHYMOD_ST* pc, uint32_t* count)
{
    RX_X4_FEC_UNCORR_CTR0r_t count0;
    RX_X4_FEC_UNCORR_CTR1r_t count1;

    /* Per port based counter */
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORR_CTR0r(pc, &count0));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORR_CTR1r(pc, &count1));
    *count = (RX_X4_FEC_UNCORR_CTR1r_GET(count1) << 16) | (RX_X4_FEC_UNCORR_CTR0r_GET(count0) & 0xffff);

    return PHYMOD_E_NONE;
}

int tefmod16_setup_for_sw_an(PHYMOD_ST* pc, int num_lanes)
{
    AN_X4_LD_CTLr_t               AN_X4_LD_CTLr_reg;
    AN_X4_CL73_CFGr_t             AN_X4_ABILITIES_CL73_CFGr_reg;
    AN_X4_WAIT_ACK_COMPLETEr_t    AN_X4_WAIT_ACK_COMPLETE_reg;
    AN_X1_CL73_ERRr_t             AN_X1_CL73_ERR_reg;
    MAIN0_SETUPr_t                MAIN0_SETUP_reg;
    uint32_t                           plldiv_r_val;
    int                                num_lane_adv;


    /* Set cl73 err timer to 0 */
    AN_X1_CL73_ERRr_CLR(AN_X1_CL73_ERR_reg);
    AN_X1_CL73_ERRr_SET(AN_X1_CL73_ERR_reg, 0);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_ERRr(pc, AN_X1_CL73_ERR_reg));

    /* disable tscf */
    tefmod16_disable_set(pc);

    /* set the cl73 vc0 mode 1 */
    PHYMOD_IF_ERR_RETURN(tefmod16_get_plldiv(pc, &plldiv_r_val));
    MAIN0_SETUPr_CLR(MAIN0_SETUP_reg);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUPr(pc, &MAIN0_SETUP_reg));

    if (plldiv_r_val<=4) {  /* pll_div <= 132; for 156MHz only */
        /* for 25G VCO set the cl73 VCO bit to1 otherwise 0*/
        MAIN0_SETUPr_CL73_VCOf_SET(MAIN0_SETUP_reg, 0);
    } else {
        MAIN0_SETUPr_CL73_VCOf_SET(MAIN0_SETUP_reg, 1);
    }
    PHYMOD_IF_ERR_RETURN(WRITE_MAIN0_SETUPr(pc, MAIN0_SETUP_reg));

    /* clear the SW AN and cl73 an enable to 0 */
    AN_X4_LD_CTLr_CLR(AN_X4_LD_CTLr_reg);
    AN_X4_LD_CTLr_AN_TYPE_SWf_SET(AN_X4_LD_CTLr_reg, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_CTLr(pc, AN_X4_LD_CTLr_reg));

    switch(num_lanes) {
        case 1:
            num_lane_adv = 0;
            break;
        case 2:
            num_lane_adv = 1;
            break;
        case 4:
            num_lane_adv = 2;
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    AN_X4_CL73_CFGr_CLR(AN_X4_ABILITIES_CL73_CFGr_reg);
    AN_X4_CL73_CFGr_CL73_BAM_ENABLEf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, 0);
    AN_X4_CL73_CFGr_CL73_ENABLEf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, 0);
    AN_X4_CL73_CFGr_NUM_ADVERTISED_LANESf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, num_lane_adv);
    PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_CL73_CFGr(pc, AN_X4_ABILITIES_CL73_CFGr_reg));

    /* set the wait for ack FSM to 1 */
    AN_X4_WAIT_ACK_COMPLETEr_CLR(AN_X4_WAIT_ACK_COMPLETE_reg);
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_WAIT_ACK_COMPLETEr(pc, &AN_X4_WAIT_ACK_COMPLETE_reg));
    AN_X4_WAIT_ACK_COMPLETEr_WAIT_FOR_ACK_ENf_SET(AN_X4_WAIT_ACK_COMPLETE_reg, 1);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_WAIT_ACK_COMPLETEr(pc, AN_X4_WAIT_ACK_COMPLETE_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_sw_an_control(PHYMOD_ST* pc, int enable)
{
    AN_X4_LD_CTLr_t               AN_X4_LD_CTLr_reg;
    AN_X4_CL73_CFGr_t             AN_X4_ABILITIES_CL73_CFGr_reg;

    /* Enabling SW AN if sw_an is set */
    AN_X4_LD_CTLr_CLR(AN_X4_LD_CTLr_reg);
    AN_X4_LD_CTLr_AN_TYPE_SWf_SET(AN_X4_LD_CTLr_reg, enable);
    PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_CTLr(pc, AN_X4_LD_CTLr_reg));

    AN_X4_CL73_CFGr_CLR(AN_X4_ABILITIES_CL73_CFGr_reg);
    AN_X4_CL73_CFGr_CL73_ENABLEf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, enable);
    PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_CL73_CFGr(pc, AN_X4_ABILITIES_CL73_CFGr_reg));

    return PHYMOD_E_NONE;
}

/*!
@brief Programs the SW AN local device pages
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.

@details
This function programs SW AN local device pages.
per_lane_control bits [17:16] indicate
the page its trying to write, per_lane_control bits [15:0]
has the actual page contents
*/
int tefmod16_sw_an_ld_page_load(PHYMOD_ST* pc, phymod_an_page_t *pages, int is_base_page)
{
    AN_X4_LD_PAGE0r_t  AN_X4_LD_PAGE0r_reg;
    AN_X4_LD_PAGE1r_t  AN_X4_LD_PAGE1r_reg;
    AN_X4_LD_PAGE2r_t  AN_X4_LD_PAGE2r_reg;

    AN_X4_SW_AN_BASE_PAGE0r_t AN_X4_SW_AN_BASE_PAGE0r_reg;
    AN_X4_SW_AN_BASE_PAGE1r_t AN_X4_SW_AN_BASE_PAGE1r_reg;
    AN_X4_SW_AN_BASE_PAGE2r_t AN_X4_SW_AN_BASE_PAGE2r_reg;


    if (is_base_page) {

        AN_X4_SW_AN_BASE_PAGE2r_CLR(AN_X4_SW_AN_BASE_PAGE2r_reg);
        AN_X4_SW_AN_BASE_PAGE2r_SET(AN_X4_SW_AN_BASE_PAGE2r_reg, pages->page_2);
        PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_SW_AN_BASE_PAGE2r(pc, AN_X4_SW_AN_BASE_PAGE2r_reg));

        AN_X4_SW_AN_BASE_PAGE1r_CLR(AN_X4_SW_AN_BASE_PAGE1r_reg);
        AN_X4_SW_AN_BASE_PAGE1r_SET(AN_X4_SW_AN_BASE_PAGE1r_reg, pages->page_1);
        PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_SW_AN_BASE_PAGE1r(pc, AN_X4_SW_AN_BASE_PAGE1r_reg));

        AN_X4_SW_AN_BASE_PAGE0r_CLR(AN_X4_SW_AN_BASE_PAGE0r_reg);
        AN_X4_SW_AN_BASE_PAGE0r_SET(AN_X4_SW_AN_BASE_PAGE0r_reg, pages->page_0);
        PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_SW_AN_BASE_PAGE0r(pc, AN_X4_SW_AN_BASE_PAGE0r_reg));

    } else {

        AN_X4_LD_PAGE2r_CLR(AN_X4_LD_PAGE2r_reg);
        AN_X4_LD_PAGE2r_LD_PAGE_2f_SET(AN_X4_LD_PAGE2r_reg, pages->page_2);
        PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_PAGE2r(pc, AN_X4_LD_PAGE2r_reg));

        AN_X4_LD_PAGE1r_CLR(AN_X4_LD_PAGE1r_reg);
        AN_X4_LD_PAGE1r_LD_PAGE_1f_SET(AN_X4_LD_PAGE1r_reg, pages->page_1);
        PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_PAGE1r(pc, AN_X4_LD_PAGE1r_reg));

        AN_X4_LD_PAGE0r_CLR(AN_X4_LD_PAGE0r_reg);
        AN_X4_LD_PAGE0r_LD_PAGE_0f_SET(AN_X4_LD_PAGE0r_reg, pages->page_0);
        PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_PAGE0r(pc, AN_X4_LD_PAGE0r_reg));

    }
    return PHYMOD_E_NONE;
}

/*!
@brief Reads the SW AN link partner pages
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.

@details
This function reads SW AN link partner pages. The HW
requires you to read page0 register last.

*/
int tefmod16_sw_an_lp_page_read(PHYMOD_ST* pc, phymod_an_page_t *pages)
{
    AN_X4_LP_PAGE0r_t AN_X4_LP_PAGE0r_reg;
    AN_X4_LP_PAGE1r_t AN_X4_LP_PAGE1r_reg;
    AN_X4_LP_PAGE2r_t AN_X4_LP_PAGE2r_reg;

    AN_X4_LP_PAGE2r_CLR(AN_X4_LP_PAGE2r_reg);
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_PAGE2r(pc, &AN_X4_LP_PAGE2r_reg));
    pages->page_2 = AN_X4_LP_PAGE2r_LP_PAGE_2f_GET(AN_X4_LP_PAGE2r_reg);

    AN_X4_LP_PAGE1r_CLR(AN_X4_LP_PAGE1r_reg);
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_PAGE1r(pc, &AN_X4_LP_PAGE1r_reg));
    pages->page_1 = AN_X4_LP_PAGE1r_LP_PAGE_1f_GET(AN_X4_LP_PAGE1r_reg);

    AN_X4_LP_PAGE0r_CLR(AN_X4_LP_PAGE0r_reg);
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_PAGE0r(pc, &AN_X4_LP_PAGE0r_reg));
    pages->page_0 = AN_X4_LP_PAGE0r_LP_PAGE_0f_GET(AN_X4_LP_PAGE0r_reg);

    return PHYMOD_E_NONE;

}


int tefmod16_sw_an_ctl_sts_get(PHYMOD_ST* pc, phymod_sw_an_ctrl_status_t *an_ctrl_status)
{
    AN_X4_SW_CTL_STSr_t AN_X4_SW_CTL_STSr_reg;

    AN_X4_SW_CTL_STSr_CLR(AN_X4_SW_CTL_STSr_reg);
    PHYMOD_IF_ERR_RETURN (READ_AN_X4_SW_CTL_STSr(pc, &AN_X4_SW_CTL_STSr_reg));

    an_ctrl_status->seq_restart = AN_X4_SW_CTL_STSr_LD_SEQ_RESTARTf_GET(AN_X4_SW_CTL_STSr_reg);
    an_ctrl_status->lp_page_rdy = AN_X4_SW_CTL_STSr_LP_PAGE_RDYf_GET(AN_X4_SW_CTL_STSr_reg);
    an_ctrl_status->page_req = AN_X4_SW_CTL_STSr_LD_PAGE_REQf_GET(AN_X4_SW_CTL_STSr_reg);
    an_ctrl_status->an_completed = AN_X4_SW_CTL_STSr_AN_COMPLETEDf_GET(AN_X4_SW_CTL_STSr_reg);
    /* in mld mode HW does not return the AN complete state
     * rely on PXNG state machine to declare AN page exchange completeion
     */
    if (!an_ctrl_status->an_completed) {
        PHYMOD_IF_ERR_RETURN
        (tefmod16_pxgng_status_an_good_chk_get(pc, &an_ctrl_status->an_completed));
    }

    return PHYMOD_E_NONE;
}

int tefmod16_sw_an_set_ack(PHYMOD_ST* pc)
{
    AN_X4_WAIT_ACK_COMPLETEr_t AN_X4_WAIT_ACK_COMPLETEr_reg;

    AN_X4_WAIT_ACK_COMPLETEr_CLR(AN_X4_WAIT_ACK_COMPLETEr_reg);
    AN_X4_WAIT_ACK_COMPLETEr_SEND_ACKf_SET(AN_X4_WAIT_ACK_COMPLETEr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_WAIT_ACK_COMPLETEr(pc, AN_X4_WAIT_ACK_COMPLETEr_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_set_speed_id(PHYMOD_ST *pc, int speed_id)
{
    SC_X4_CTLr_t xgxs_x4_ctrl;
    phymod_access_t pa_copy;
    int start_lane = 0, num_lane = 0;


    /* need to figure out what's the starting lane */
    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
    pa_copy.lane_mask = 0x1 << start_lane;

    /* write the speed_id into the speed_change register */
    SC_X4_CTLr_CLR(xgxs_x4_ctrl);
    SC_X4_CTLr_SPEEDf_SET(xgxs_x4_ctrl, speed_id);
    MODIFY_SC_X4_CTLr(pc, xgxs_x4_ctrl);

    return PHYMOD_E_NONE;
}



int tefmod16_pxgng_status_an_ack_complete_get(PHYMOD_ST *pc, int *ack_complte)
{

    AN_X4_PXNG_STSr_t an_pxng_status_reg;

    AN_X4_PXNG_STSr_CLR(an_pxng_status_reg);
    PHYMOD_IF_ERR_RETURN
        (READ_AN_X4_PXNG_STSr(pc, &an_pxng_status_reg));
    PHYMOD_IF_ERR_RETURN
        (READ_AN_X4_PXNG_STSr(pc, &an_pxng_status_reg));

     *ack_complte = AN_X4_PXNG_STSr_COMPLETE_ACKf_GET(an_pxng_status_reg);

     return PHYMOD_E_NONE;
}

int tefmod16_pxgng_status_an_good_chk_get(PHYMOD_ST *pc, uint8_t *an_good_chk)
{

    AN_X4_PXNG_STSr_t an_pxng_status_reg;

    AN_X4_PXNG_STSr_CLR(an_pxng_status_reg);
    PHYMOD_IF_ERR_RETURN
      (READ_AN_X4_PXNG_STSr(pc, &an_pxng_status_reg));
    PHYMOD_IF_ERR_RETURN
      (READ_AN_X4_PXNG_STSr(pc, &an_pxng_status_reg));

    *an_good_chk = AN_X4_PXNG_STSr_AN_GOOD_CHECKf_GET(an_pxng_status_reg);

    return PHYMOD_E_NONE;

}


int tefmod16_sw_an_fec_override(PHYMOD_ST *pc, tefmod16_fec_type_t fec_type, int enable)
{
    SC_X4_SC_X4_OVRRr_t SC_X4_SC_X4_OVRR_reg;
    int fec_value = 0;

    if (enable) {
        if (fec_type == TEFMOD16_CL91) {
            fec_value = 0x3;
        } else if (fec_type == TEFMOD16_CL74) {
            fec_value = 0x2;
        } else {
            /* NO FEC */
            fec_value = 0x1;
        }
    } else {
        fec_value = 0x0;
    }
    SC_X4_SC_X4_OVRRr_CLR(SC_X4_SC_X4_OVRR_reg);
    SC_X4_SC_X4_OVRRr_AN_FEC_SEL_OVERRIDEf_SET(SC_X4_SC_X4_OVRR_reg, fec_value);
    PHYMOD_IF_ERR_RETURN
        (MODIFY_SC_X4_SC_X4_OVRRr(pc, SC_X4_SC_X4_OVRR_reg));

    return PHYMOD_E_NONE;
}


int tefmod16_sw_an_advert_set(PHYMOD_ST* pc, tefmod16_an_adv_ability_t *cl73_adv, phymod_sw_an_ctxt_t *an_ctxt)
{
    uint8_t pause_cap = 0;
    /* Initialize the base page */
    an_ctxt->tx_pages.base_page.page_0 = 0;
    an_ctxt->tx_pages.base_page.page_1 = 0;
    an_ctxt->tx_pages.base_page.page_2 = 0;

    /* Update it based on cl73_adv */
    an_ctxt->no_pages_to_be_txed = 1;

    an_ctxt->tx_pages.base_page.page_0 |= ((AN_BASE0_PAGE_IEEE_SEL_MASK & IEEE_SEL_802P3) <<
                              AN_BASE0_PAGE_IEEE_SEL_OFFSET);

    an_ctxt->tx_pages.base_page.page_0 &= ~(AN_BASE0_PAGE_CL73_NOUNCE_MASK <<
                             AN_BASE0_PAGE_CL73_NOUNCE_OFFSET); /* HW will set this bit */

    if (cl73_adv->an_pause == TEFMOD16_NO_PAUSE) {
        pause_cap = 0;
    } else if (cl73_adv->an_pause == TEFMOD16_SYMM_PAUSE) {
        pause_cap = 1;
    } else if (cl73_adv->an_pause == TEFMOD16_ASYM_PAUSE) {
        pause_cap = 2;
    } else if (cl73_adv->an_pause == TEFMOD16_ASYM_SYMM_PAUSE) {
        pause_cap = 3;
    } else {
        /* default to no pause ability */
        pause_cap = 0;
    }
    an_ctxt->tx_pages.base_page.page_0 |= ((AN_BASE0_PAGE_CAP_MASK & pause_cap) <<
                                            AN_BASE0_PAGE_CAP_OFFSET);

    an_ctxt->tx_pages.base_page.page_0 &= ~(AN_BASE0_PAGE_REMOTE_FAULT_MASK <<
                             AN_BASE0_PAGE_REMOTE_FAULT_OFFSET); /* Need more clarity */

    an_ctxt->tx_pages.base_page.page_0 &= ~(AN_BASE0_PAGE_ACK_MASK <<
                                           AN_BASE0_PAGE_ACK_OFFSET);  /* Set by HW */

    /* set tech Next page to 0*/
    an_ctxt->tx_pages.base_page.page_0 &= ~(AN_BASE0_PAGE_NP_MASK <<
                                             AN_BASE0_PAGE_NP_OFFSET);
    /* coverity[DC.WEAK_CRYPTO: False] */
    PHYMOD_SRAND(PHYMOD_TIME_USECS());
   /* need to generate a 5 bit random number for Tx nounce for now set to 1 */
    an_ctxt->tx_pages.base_page.page_1 |= ((AN_BASE1_PAGE_TX_NOUNCE_MASK & (PHYMOD_RAND()))  <<
                                            AN_BASE1_PAGE_TX_NOUNCE_OFFSET);

    if (an_ctxt->an_mode == phymod_AN_MODE_MSA) {
        cl73_adv->an_base_speed = 0;
    }

    if (cl73_adv->an_base_speed) {
        /* populate the IEE base ability */
        if (((cl73_adv->an_base_speed  >> TEFMOD16_CL73_25GBASE_KR1 ) & 1) ||
            ((cl73_adv->an_base_speed  >> TEFMOD16_CL73_25GBASE_CR1 ) & 1)) {
            an_ctxt->tx_pages.base_page.page_1 |= (AN_BASE_TECH_ABILITY_25GKR_MASK <<
                                                   AN_BASE_TECH_ABILITY_25GKR_OFFSET);
        }
        if (((cl73_adv->an_base_speed  >> TEFMOD16_CL73_25GBASE_KRS1 ) & 1) ||
                  ((cl73_adv->an_base_speed  >> TEFMOD16_CL73_25GBASE_CRS1 ) & 1)) {
            an_ctxt->tx_pages.base_page.page_1 |= (AN_BASE_TECH_ABILITY_25GKRS_MASK <<
                                                   AN_BASE_TECH_ABILITY_25GKRS_OFFSET);
        }

             /* Set cl91/74 supported to 1 */
        an_ctxt->tx_pages.base_page.page_2 |= (AN_BASE_CL74_ABILITY_SUP_MASK <<
                                               AN_BASE_CL74_ABILITY_SUP_OFFSET);

        if (cl73_adv->an_fec == TEFMOD16_FEC_SUPRTD_NOT_REQSTD) {
            an_ctxt->tx_pages.base_page.page_2 &= ~(AN_BASE_CL74_ABILITY_REQ_MASK <<
                                                    AN_BASE_CL74_ABILITY_REQ_OFFSET);
            an_ctxt->tx_pages.base_page.page_2 &= ~(AN_BASE_CL91_ABILITY_REQ_MASK <<
                                                    AN_BASE_CL91_ABILITY_REQ_OFFSET);
            an_ctxt->tx_pages.base_page.page_2 &= ~(AN_BASE_CL74_25GKRS_REQ_MASK <<
                                                    AN_BASE_CL74_25GKRS_REQ_OFFSET);
        } else {
            if (cl73_adv->an_fec & TEFMOD16_FEC_CL74_SUPRTD_REQSTD) {
                an_ctxt->tx_pages.base_page.page_2 |= (AN_BASE_CL74_ABILITY_REQ_MASK <<
                                                        AN_BASE_CL74_ABILITY_REQ_OFFSET);
                an_ctxt->tx_pages.base_page.page_2 |= (AN_BASE_CL74_25GKRS_REQ_MASK <<
                                                    AN_BASE_CL74_25GKRS_REQ_OFFSET);
            }
            if (cl73_adv->an_fec & TEFMOD16_FEC_CL91_SUPRTD_REQSTD) {
                an_ctxt->tx_pages.base_page.page_2 |= (AN_BASE_CL91_ABILITY_REQ_MASK <<
                                                       AN_BASE_CL91_ABILITY_REQ_OFFSET);
            }
        }
    }



    if (cl73_adv->an_bam_speed || cl73_adv->an_bam_speed1) {

        /* If BAM speed is enabled set the NP bit in base page to 1*/
        an_ctxt->tx_pages.base_page.page_0 |= (AN_BASE0_PAGE_NP_MASK << AN_BASE0_PAGE_NP_OFFSET);

        an_ctxt->tx_pages.msg_page.page_0 = 0;
        an_ctxt->tx_pages.msg_page.page_1 = 0;
        an_ctxt->tx_pages.msg_page.page_2 = 0;

        /* populate the Message page with Consortium information */

        /* Populate page0 with
         * 1. Msg code
         * 2. Toggle bit ==> set to 0 (will be handled by HW)
         * 3. ACK2 ==> Set to 0 (will be handled buy HW)
         * 4. set MP bit to 1
         * 5. Set NP bit to 1
         */
        an_ctxt->tx_pages.msg_page.page_0 |= ((AN_MSG_PAGE0_MSG_CODE_MASK & MSA_MSG_CODE) << AN_MSG_PAGE0_MSG_CODE_OFFSET);
        an_ctxt->tx_pages.msg_page.page_0 |= (AN_MSG_PAGE0_MSG_PAGE_MASK << AN_MSG_PAGE0_MSG_PAGE_OFFSET);
        an_ctxt->tx_pages.msg_page.page_0 |= (AN_MSG_PAGE0_NP_MASK << AN_MSG_PAGE0_NP_OFFSET);

          /* populate msg page 1 & 2 with consortium information
         * OUI 13:23
         * OUI 2:12
         */
        an_ctxt->tx_pages.msg_page.page_1 |= ((AN_MSG_PAGE1_OUI_13to23_MASK & MSA_OUI_13to23) <<
                                               AN_MSG_PAGE1_OUI_13to23_OFFSET);
        an_ctxt->tx_pages.msg_page.page_2 |= ((AN_MSG_PAGE2_OUI_2to12_MASK & MSA_OUT_2to12) <<
                                               AN_MSG_PAGE2_OUI_2to12_OFFSET);

        an_ctxt->tx_pages.ufmt_page.page_0 = 0;
        an_ctxt->tx_pages.ufmt_page.page_1 = 0;
        an_ctxt->tx_pages.ufmt_page.page_2 = 0;

        /* Populate UFMT page */

        an_ctxt->tx_pages.ufmt_page.page_0 |= ((AN_UF_PAGE0_EXT_TECH_ABILITY_MASK & EXT_TECH_ABILITY_CODE) <<
                                                AN_UF_PAGE0_EXT_TECH_ABILITY_OFFSET);
        an_ctxt->tx_pages.ufmt_page.page_0 |= ((AN_UF_PAGE0_OUI_MASK & MSA_OUI_0to1) <<
                                                AN_UF_PAGE0_OUI_OFFSET);

        if ((cl73_adv->an_bam_speed1 >> TEFMOD16_CL73_BAM_25GBASE_KR1) & 0x1) {
            an_ctxt->tx_pages.ufmt_page.page_1 |= (AN_UF_PAGE1_25G_KR1_ABILITY_MASK <<
                                                  AN_UF_PAGE1_25G_KR1_ABILITY_OFFSET);
        }
        if ((cl73_adv->an_bam_speed1 >> TEFMOD16_CL73_BAM_25GBASE_CR1) & 0x1) {
            an_ctxt->tx_pages.ufmt_page.page_1 |= (AN_UF_PAGE1_25G_CR1_ABILITY_MASK <<
                                                  AN_UF_PAGE1_25G_CR1_ABILITY_OFFSET);
        }
        if ((cl73_adv->an_bam_speed >> TEFMOD16_CL73_BAM_50GBASE_KR2) & 0x1) {
            an_ctxt->tx_pages.ufmt_page.page_1 |= (AN_UF_PAGE1_50G_KR2_ABILITY_MASK <<
                                                  AN_UF_PAGE1_50G_KR2_ABILITY_OFFSET);
        }
        if ((cl73_adv->an_bam_speed >> TEFMOD16_CL73_BAM_50GBASE_CR2) & 0x1) {
            an_ctxt->tx_pages.ufmt_page.page_1 |= (AN_UF_PAGE1_50G_CR2_ABILITY_MASK <<
                                                  AN_UF_PAGE1_50G_CR2_ABILITY_OFFSET);
        }

        /* Update FEC Information */
        an_ctxt->tx_pages.ufmt_page.page_2 |= (AN_UF_PAGE2_CL91_SUPPORT_MASK <<
                                               AN_UF_PAGE2_CL91_SUPPORT_OFFSET);
        an_ctxt->tx_pages.ufmt_page.page_2 |= (AN_UF_PAGE2_CL74_SUPPORT_MASK <<
                                               AN_UF_PAGE2_CL74_SUPPORT_OFFSET);

        if (cl73_adv->an_fec & TEFMOD16_FEC_CL74_SUPRTD_REQSTD) {
            an_ctxt->tx_pages.ufmt_page.page_2 |= (AN_UF_PAGE2_CL74_REQ_MASK <<
                                                   AN_UF_PAGE2_CL74_REQ_OFFSET);
        }
        if (cl73_adv->an_fec & TEFMOD16_FEC_CL91_SUPRTD_REQSTD) {
            an_ctxt->tx_pages.ufmt_page.page_2 |= (AN_UF_PAGE2_CL91_REQ_MASK <<
                                                   AN_UF_PAGE2_CL91_REQ_OFFSET);
        }
    }

    /* Populate the null page */
    an_ctxt->tx_pages.null_page.page_2 = 0;
    an_ctxt->tx_pages.null_page.page_1 = 0;
    an_ctxt->tx_pages.null_page.page_0 = 1;

    return PHYMOD_E_NONE;

}

int tefmod16_25g_rsfec_am_init(PHYMOD_ST *pc) 
{
    SC_X4_IEEE_25G_CTLr_t               ieee_25G_am_reg;
    RX_X4_IEEE_25G_PARLLEL_DET_CTRr_t   ieee_25g_parallel_det_reg;
    SC_X4_MSA_25G_50G_CTLr_t       msa_25g_50g_am_reg; 

    SC_X4_IEEE_25G_CTLr_CLR(ieee_25G_am_reg);
    RX_X4_IEEE_25G_PARLLEL_DET_CTRr_CLR(ieee_25g_parallel_det_reg);
    SC_X4_MSA_25G_50G_CTLr_CLR(msa_25g_50g_am_reg);

    /* Program AM0 value to use 100G AM  AM123 should use 40G AM 
     * which is a default 
     */
    PHYMOD_IF_ERR_RETURN(
        READ_SC_X4_IEEE_25G_CTLr(pc, &ieee_25G_am_reg));
    SC_X4_IEEE_25G_CTLr_IEEE_25G_AM0_FORMATf_SET(ieee_25G_am_reg, 1);
    PHYMOD_IF_ERR_RETURN(
        WRITE_SC_X4_IEEE_25G_CTLr(pc, ieee_25G_am_reg));

    /* MSA Mode Program 25G AM0 value to use 100G AM
     * The rest AM should use 40G AM, which is a default
     */
    PHYMOD_IF_ERR_RETURN(
        READ_SC_X4_MSA_25G_50G_CTLr(pc, &msa_25g_50g_am_reg));
    SC_X4_MSA_25G_50G_CTLr_MSA_25G_AM0_FORMATf_SET(msa_25g_50g_am_reg, 1);
    PHYMOD_IF_ERR_RETURN(
        WRITE_SC_X4_MSA_25G_50G_CTLr(pc, msa_25g_50g_am_reg));

    /* Enable AM parallel detect for incase if the legacy devices use 40G AM */
    PHYMOD_IF_ERR_RETURN(
        READ_RX_X4_IEEE_25G_PARLLEL_DET_CTRr(pc, &ieee_25g_parallel_det_reg));
    RX_X4_IEEE_25G_PARLLEL_DET_CTRr_SET(ieee_25g_parallel_det_reg, 1);
    PHYMOD_IF_ERR_RETURN(
        WRITE_RX_X4_IEEE_25G_PARLLEL_DET_CTRr(pc, ieee_25g_parallel_det_reg));

    return PHYMOD_E_NONE;        

}


/* clear ECC status register for FEC memory */
int tefmod16_cl91_ecc_clear(PHYMOD_ST *pc)
{
    RX_X1_ECC_STS_FEC_MEM0r_t            RX_X1_ECC_STS_FEC_MEM0r_reg;
    RX_X1_ECC_STS_FEC_MEM1r_t            RX_X1_ECC_STS_FEC_MEM1r_reg;
    RX_X1_ECC_STS_FEC_MEM2r_t            RX_X1_ECC_STS_FEC_MEM2r_reg;
    RX_X1_ECC_STS_FEC_MEM3r_t            RX_X1_ECC_STS_FEC_MEM3r_reg;
    RX_X1_ECC_INTR_STS_1BITr_t           RX_X1_ECC_INTR_STS_1BITr_reg;
    RX_X1_ECC_INTR_STS_2BITr_t           RX_X1_ECC_INTR_STS_2BITr_reg;
    RX_X4_ECC_STS_CL91_BUFFER_BLK0r_t    RX_X4_ECC_STS_CL91_BUFFER_BLK0r_reg;
    RX_X4_ECC_STS_CL91_BUFFER_BLK1r_t    RX_X4_ECC_STS_CL91_BUFFER_BLK1r_reg;
    RX_X4_ECC_STS_CL91_BUFFER_BLK2r_t    RX_X4_ECC_STS_CL91_BUFFER_BLK2r_reg;
    RX_X4_ECC_STS_DESKEW_MEM0r_t         RX_X4_ECC_STS_DESKEW_MEM0r_reg;
    RX_X4_ECC_STS_DESKEW_MEM1r_t         RX_X4_ECC_STS_DESKEW_MEM1r_reg;
    RX_X4_ECC_STS_DESKEW_MEM2r_t         RX_X4_ECC_STS_DESKEW_MEM2r_reg;
    RX_X4_ECC_INTR_STS_1BITr_t           RX_X4_ECC_INTR_STS_1BITr_reg;
    RX_X4_ECC_INTR_STS_2BITr_t           RX_X4_ECC_INTR_STS_2BITr_reg;
    phymod_access_t pa_copy;
    int lane = 0;

    TEFMOD16_DBG_IN_FUNC_INFO(pc);

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    RX_X1_ECC_STS_FEC_MEM0r_CLR(RX_X1_ECC_STS_FEC_MEM0r_reg);
    RX_X1_ECC_STS_FEC_MEM1r_CLR(RX_X1_ECC_STS_FEC_MEM1r_reg);
    RX_X1_ECC_STS_FEC_MEM2r_CLR(RX_X1_ECC_STS_FEC_MEM2r_reg);
    RX_X1_ECC_STS_FEC_MEM3r_CLR(RX_X1_ECC_STS_FEC_MEM3r_reg);
    RX_X1_ECC_INTR_STS_1BITr_CLR(RX_X1_ECC_INTR_STS_1BITr_reg);
    RX_X1_ECC_INTR_STS_2BITr_CLR(RX_X1_ECC_INTR_STS_2BITr_reg);
    RX_X4_ECC_STS_CL91_BUFFER_BLK0r_CLR(RX_X4_ECC_STS_CL91_BUFFER_BLK0r_reg);
    RX_X4_ECC_STS_CL91_BUFFER_BLK1r_CLR(RX_X4_ECC_STS_CL91_BUFFER_BLK1r_reg);
    RX_X4_ECC_STS_CL91_BUFFER_BLK2r_CLR(RX_X4_ECC_STS_CL91_BUFFER_BLK2r_reg);
    RX_X4_ECC_STS_DESKEW_MEM0r_CLR(RX_X4_ECC_STS_DESKEW_MEM0r_reg);
    RX_X4_ECC_STS_DESKEW_MEM1r_CLR(RX_X4_ECC_STS_DESKEW_MEM1r_reg);
    RX_X4_ECC_STS_DESKEW_MEM2r_CLR(RX_X4_ECC_STS_DESKEW_MEM2r_reg);
    RX_X4_ECC_INTR_STS_1BITr_CLR(RX_X4_ECC_INTR_STS_1BITr_reg);
    RX_X4_ECC_INTR_STS_2BITr_CLR(RX_X4_ECC_INTR_STS_2BITr_reg);

    /* Read clear */
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_ECC_STS_FEC_MEM0r(&pa_copy, &RX_X1_ECC_STS_FEC_MEM0r_reg));
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_ECC_STS_FEC_MEM1r(&pa_copy, &RX_X1_ECC_STS_FEC_MEM1r_reg));
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_ECC_STS_FEC_MEM2r(&pa_copy, &RX_X1_ECC_STS_FEC_MEM2r_reg));
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_ECC_STS_FEC_MEM3r(&pa_copy, &RX_X1_ECC_STS_FEC_MEM3r_reg));
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_ECC_INTR_STS_1BITr(&pa_copy, &RX_X1_ECC_INTR_STS_1BITr_reg));
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_ECC_INTR_STS_2BITr(&pa_copy, &RX_X1_ECC_INTR_STS_2BITr_reg));

    for (lane = 0; lane < 4; lane++) {
        pa_copy.lane_mask = 0x1 << lane;
        PHYMOD_IF_ERR_RETURN(READ_RX_X4_ECC_STS_CL91_BUFFER_BLK0r(&pa_copy, &RX_X4_ECC_STS_CL91_BUFFER_BLK0r_reg));
        PHYMOD_IF_ERR_RETURN(READ_RX_X4_ECC_STS_CL91_BUFFER_BLK1r(&pa_copy, &RX_X4_ECC_STS_CL91_BUFFER_BLK1r_reg));
        PHYMOD_IF_ERR_RETURN(READ_RX_X4_ECC_STS_CL91_BUFFER_BLK2r(&pa_copy, &RX_X4_ECC_STS_CL91_BUFFER_BLK2r_reg));
        PHYMOD_IF_ERR_RETURN(READ_RX_X4_ECC_STS_DESKEW_MEM0r(&pa_copy, &RX_X4_ECC_STS_DESKEW_MEM0r_reg));
        PHYMOD_IF_ERR_RETURN(READ_RX_X4_ECC_STS_DESKEW_MEM1r(&pa_copy, &RX_X4_ECC_STS_DESKEW_MEM1r_reg));
        PHYMOD_IF_ERR_RETURN(READ_RX_X4_ECC_STS_DESKEW_MEM2r(&pa_copy, &RX_X4_ECC_STS_DESKEW_MEM2r_reg));
        PHYMOD_IF_ERR_RETURN(READ_RX_X4_ECC_INTR_STS_1BITr(&pa_copy, &RX_X4_ECC_INTR_STS_1BITr_reg));
        PHYMOD_IF_ERR_RETURN(READ_RX_X4_ECC_INTR_STS_2BITr(&pa_copy, &RX_X4_ECC_INTR_STS_2BITr_reg));
    }

    return PHYMOD_E_NONE;
}

/*
Set FEC Bypass Error Indictator.
Note: FEC_bypass_indication_enable has no effect when FEC_bypass_correction_enable is asserted.
*/
int tefmod16_fec_bypass_indication_set(PHYMOD_ST *pc, uint32_t enable)
{
    RX_X4_CL91_RX_CTL0r_t RX_X4_CL91_RX_CTL0r_reg;

    RX_X4_CL91_RX_CTL0r_CLR(RX_X4_CL91_RX_CTL0r_reg);

    RX_X4_CL91_RX_CTL0r_FEC_BYP_IND_ENf_SET(RX_X4_CL91_RX_CTL0r_reg, enable);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_CL91_RX_CTL0r(pc, RX_X4_CL91_RX_CTL0r_reg));

    return PHYMOD_E_NONE;
}

/* Get FEC Bypass Error Indictator */
int tefmod16_fec_bypass_indication_get(PHYMOD_ST *pc, uint32_t *enable)
{
    RX_X4_CL91_RX_CTL0r_t RX_X4_CL91_RX_CTL0r_reg;

    RX_X4_CL91_RX_CTL0r_CLR(RX_X4_CL91_RX_CTL0r_reg);

    PHYMOD_IF_ERR_RETURN(READ_RX_X4_CL91_RX_CTL0r(pc, &RX_X4_CL91_RX_CTL0r_reg));
    *enable = RX_X4_CL91_RX_CTL0r_FEC_BYP_IND_ENf_GET(RX_X4_CL91_RX_CTL0r_reg);

    return PHYMOD_E_NONE;

}

STATIC
int _tefmod16_rx_phy_lane_get(PHYMOD_ST *pc, int rx_logical_lane,
                              int* rx_phy_lane)
{
    RX_X1_RX_LN_SWPr_t rx_lane_swap_reg;
    phymod_access_t pa_copy;

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));

    /*always use lane 0 copy since register is single copy */
    pa_copy.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (READ_RX_X1_RX_LN_SWPr(&pa_copy, &rx_lane_swap_reg));

    switch (rx_logical_lane) {
        case 0:
            *rx_phy_lane =
                    RX_X1_RX_LN_SWPr_LOGICAL0_TO_PHY_SELf_GET(rx_lane_swap_reg);
            break;
        case 1:
            *rx_phy_lane =
                    RX_X1_RX_LN_SWPr_LOGICAL1_TO_PHY_SELf_GET(rx_lane_swap_reg);
            break;
        case 2:
            *rx_phy_lane =
                    RX_X1_RX_LN_SWPr_LOGICAL2_TO_PHY_SELf_GET(rx_lane_swap_reg);
            break;
        case 3:
            *rx_phy_lane =
                    RX_X1_RX_LN_SWPr_LOGICAL3_TO_PHY_SELf_GET(rx_lane_swap_reg);
            break;
        default:
           return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}

int tefmod16_synce_stg0_mode_set(PHYMOD_ST *pc, int mode)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN0_SYNCE_CTL_STAGE0r_t MAIN0_SYNCE_CTL_STAGE0r_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    MAIN0_SYNCE_CTL_STAGE0r_CLR(MAIN0_SYNCE_CTL_STAGE0r_reg);

    PHYMOD_IF_ERR_RETURN
        (_tefmod16_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SYNCE_CTL_STAGE0r(pc,
                                                 &MAIN0_SYNCE_CTL_STAGE0r_reg));

    switch(phy_lane) {
        case 0:
           MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE0f_SET(
                                           MAIN0_SYNCE_CTL_STAGE0r_reg, mode);
           break;
        case 1:
           MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE1f_SET(
                                           MAIN0_SYNCE_CTL_STAGE0r_reg, mode);
           break;
        case 2:
           MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE2f_SET(
                                           MAIN0_SYNCE_CTL_STAGE0r_reg, mode);
           break;
        case 3:
           MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE3f_SET(
                                           MAIN0_SYNCE_CTL_STAGE0r_reg, mode);
           break;
        default:
           return PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SYNCE_CTL_STAGE0r(pc,
                                MAIN0_SYNCE_CTL_STAGE0r_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_synce_stg1_mode_set(PHYMOD_ST *pc, int mode)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN0_SYNCE_CTLr_t MAIN0_SYNCE_CTLr_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    MAIN0_SYNCE_CTLr_CLR(MAIN0_SYNCE_CTLr_reg);

    PHYMOD_IF_ERR_RETURN
        (_tefmod16_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SYNCE_CTLr(pc, &MAIN0_SYNCE_CTLr_reg));

    switch(phy_lane) {
       case 0:
           MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE0f_SET(
                                           MAIN0_SYNCE_CTLr_reg, mode);
           break;
        case 1:
           MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE1f_SET(
                                           MAIN0_SYNCE_CTLr_reg, mode);
           break;
        case 2:
           MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE2f_SET(
                                           MAIN0_SYNCE_CTLr_reg, mode);
           break;
        case 3:
           MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE3f_SET(
                                           MAIN0_SYNCE_CTLr_reg, mode);
           break;
        default:
           return PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SYNCE_CTLr(pc, MAIN0_SYNCE_CTLr_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_synce_clk_ctrl_set(PHYMOD_ST *pc, uint32_t val)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    phymod_access_t pa_copy;
    RX_X4_SYNCE_FRACTIONAL_DIVr_t RX_X4_SYNCE_FRACTIONAL_DIVr_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_tefmod16_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    pa_copy.lane_mask = 0x1 << phy_lane;

    RX_X4_SYNCE_FRACTIONAL_DIVr_CLR(RX_X4_SYNCE_FRACTIONAL_DIVr_reg);
    RX_X4_SYNCE_FRACTIONAL_DIVr_SET(RX_X4_SYNCE_FRACTIONAL_DIVr_reg, val);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_SYNCE_FRACTIONAL_DIVr(&pa_copy,
                                        RX_X4_SYNCE_FRACTIONAL_DIVr_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_synce_stg0_mode_get(PHYMOD_ST *pc, int *mode)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN0_SYNCE_CTL_STAGE0r_t MAIN0_SYNCE_CTL_STAGE0r_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    MAIN0_SYNCE_CTL_STAGE0r_CLR(MAIN0_SYNCE_CTL_STAGE0r_reg);

    PHYMOD_IF_ERR_RETURN
        (_tefmod16_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SYNCE_CTL_STAGE0r(pc,
                                                 &MAIN0_SYNCE_CTL_STAGE0r_reg));

    switch(phy_lane) {
         case 0:
            *mode = MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE0f_GET(
                                            MAIN0_SYNCE_CTL_STAGE0r_reg);
            break;
         case 1:
            *mode = MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE1f_GET(
                                            MAIN0_SYNCE_CTL_STAGE0r_reg);
            break;
         case 2:
            *mode = MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE2f_GET(
                                            MAIN0_SYNCE_CTL_STAGE0r_reg);
            break;
         case 3:
            *mode = MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE3f_GET(
                                            MAIN0_SYNCE_CTL_STAGE0r_reg);
            break;
         default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int tefmod16_synce_stg1_mode_get(PHYMOD_ST *pc, int *mode)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN0_SYNCE_CTLr_t MAIN0_SYNCE_CTLr_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    MAIN0_SYNCE_CTLr_CLR(MAIN0_SYNCE_CTLr_reg);

    PHYMOD_IF_ERR_RETURN
        (_tefmod16_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SYNCE_CTLr(pc, &MAIN0_SYNCE_CTLr_reg));

    switch(phy_lane) {
         case 0:
            *mode = MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE0f_GET(
                                            MAIN0_SYNCE_CTLr_reg);
            break;
         case 1:
            *mode = MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE1f_GET(
                                            MAIN0_SYNCE_CTLr_reg);
            break;
         case 2:
            *mode = MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE2f_GET(
                                            MAIN0_SYNCE_CTLr_reg);
            break;
         case 3:
            *mode = MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE3f_GET(
                                            MAIN0_SYNCE_CTLr_reg);
            break;
         default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}

int tefmod16_synce_clk_ctrl_get(PHYMOD_ST *pc, uint32_t *val)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    phymod_access_t pa_copy;
    RX_X4_SYNCE_FRACTIONAL_DIVr_t RX_X4_SYNCE_FRACTIONAL_DIVr_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_tefmod16_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    pa_copy.lane_mask = 0x1 << phy_lane;

    RX_X4_SYNCE_FRACTIONAL_DIVr_CLR(RX_X4_SYNCE_FRACTIONAL_DIVr_reg);

    PHYMOD_IF_ERR_RETURN(READ_RX_X4_SYNCE_FRACTIONAL_DIVr(&pa_copy,
                                        &RX_X4_SYNCE_FRACTIONAL_DIVr_reg));

    *val = RX_X4_SYNCE_FRACTIONAL_DIVr_GET(RX_X4_SYNCE_FRACTIONAL_DIVr_reg);

    return PHYMOD_E_NONE;
}

STATIC
int _tefmod16_port_lane_config_get(PHYMOD_ST *pc, int port_mode, int *port_starting_lane, int *port_num_lane)
{
    int start_lane, num_of_lane;

    PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_of_lane));

    switch (port_mode) {
        case  TEFMOD16_SINGLE_PORT:
            *port_starting_lane = 0;
            *port_num_lane = 4;
            break;
        case  TEFMOD16_DXGXS:
            if (start_lane < 2) {
                *port_starting_lane = 0;
                *port_num_lane = 2;
            } else {
                *port_starting_lane = 2;
                *port_num_lane = 2;
            }
            break;
        case  TEFMOD16_TRI1_PORT:
            if (start_lane < 2) {
                *port_starting_lane = start_lane;
                *port_num_lane = 1;
            } else {
                *port_starting_lane = 2;
                *port_num_lane = 2;
            }
            break;
        case  TEFMOD16_TRI2_PORT:
            if (start_lane < 2) {
                *port_starting_lane = 0;
                *port_num_lane = 2;
            } else {
                *port_starting_lane = start_lane;
                *port_num_lane = 1;
            }
            break;
        default:
            *port_starting_lane = start_lane;
            *port_num_lane = num_of_lane;
    }

    return PHYMOD_E_NONE;
}

int tefmod16_rsfec_symbol_error_counter_get(PHYMOD_ST* pc,
                                            int max_count,
                                            int* actual_count,
                                            uint32_t* symb_err_cnt)
{
    phymod_access_t phy_copy;
    int start_lane, num_lane, port_start_lane, port_num_lanes;
    int i, fec_lane_id = 0;
    uint16_t symbol_reg_index = 0, symbol_reg_max_num = 4;
    uint32_t error_cnt = 0;
    int port_mode_sel=0;

    MAIN0_SETUPr_t  MAIN0_SETUPr_reg;
    RX_X4_CL91_SYNC_STSr_t RX_X4_CL91_SYNC_STSr_reg;
    RX_X4_CL91_FEC_SYM_ERR_CTR_LOW0r_t RX_X4_CL91_FEC_SYM_ERR_CTR_LOW0r_reg;
    RX_X4_CL91_FEC_SYM_ERR_CTR_LOW1r_t RX_X4_CL91_FEC_SYM_ERR_CTR_LOW1r_reg;
    RX_X4_CL91_FEC_SYM_ERR_CTR_LOW2r_t RX_X4_CL91_FEC_SYM_ERR_CTR_LOW2r_reg;
    RX_X4_CL91_FEC_SYM_ERR_CTR_LOW3r_t RX_X4_CL91_FEC_SYM_ERR_CTR_LOW3r_reg;
    RX_X4_CL91_FEC_SYM_ERR_CTR_UP0r_t RX_X4_CL91_FEC_SYM_ERR_CTR_UP0r_reg;
    RX_X4_CL91_FEC_SYM_ERR_CTR_UP1r_t RX_X4_CL91_FEC_SYM_ERR_CTR_UP1r_reg;
    RX_X4_CL91_FEC_SYM_ERR_CTR_UP2r_t RX_X4_CL91_FEC_SYM_ERR_CTR_UP2r_reg;
    RX_X4_CL91_FEC_SYM_ERR_CTR_UP3r_t RX_X4_CL91_FEC_SYM_ERR_CTR_UP3r_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    MAIN0_SETUPr_CLR(MAIN0_SETUPr_reg);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUPr(pc, &MAIN0_SETUPr_reg));
    port_mode_sel = MAIN0_SETUPr_PORT_MODE_SELf_GET(MAIN0_SETUPr_reg);
    PHYMOD_IF_ERR_RETURN
        (_tefmod16_port_lane_config_get(pc, port_mode_sel, &port_start_lane, &port_num_lanes));
    if (max_count < port_num_lanes) {
        return PHYMOD_E_PARAM;
    } else {
        *actual_count = num_lane;
    }

    PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));

    /*
     * Collect fec_lane counter per logical lane. The rule to map the register counters to fec_lane counters as below:
     * If we have a 100G port, the 4 symbol error counters for port 0 represent the counter values per FEC lane.
     * If we have a 25G port, add the 4 symbol error counter values of a port for the symbol error counter value for the FEC lane 0 for that port.
     * If we have a 50G port, add the counter value of 0 and 2 for FEC lane 0 and counter value of 1 and 3 for FEC lane 1.
     */
    for (i = 0; i < num_lane; i++) {

         /* Get the fec lane mapping for current logical lane */
         phy_copy.lane_mask = 0x1 << (i + start_lane);
         RX_X4_CL91_SYNC_STSr_CLR(RX_X4_CL91_SYNC_STSr_reg);
         READ_RX_X4_CL91_SYNC_STSr(&phy_copy, &RX_X4_CL91_SYNC_STSr_reg);
         fec_lane_id = RX_X4_CL91_SYNC_STSr_CL91_FEC_LANE_MAPf_GET(RX_X4_CL91_SYNC_STSr_reg);

         /* Need to figure out the start lane of the port */
         phy_copy.lane_mask = 0x1 << port_start_lane;
         symb_err_cnt[i] = 0;
         switch (port_mode_sel) {
             case TEFMOD16_SINGLE_PORT:
                 TEFMOD16_RD_SYMBOL_ERROR_CNTR(&phy_copy, fec_lane_id, &error_cnt);
                 TEFMOD16_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                 break;
             case TEFMOD16_DXGXS:
                 if (fec_lane_id < 2) {
                     TEFMOD16_RD_SYMBOL_ERROR_CNTR(&phy_copy, fec_lane_id, &error_cnt);
                     TEFMOD16_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                     TEFMOD16_RD_SYMBOL_ERROR_CNTR(&phy_copy, fec_lane_id + 2, &error_cnt);
                     TEFMOD16_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                 } else {
                     return PHYMOD_E_PARAM;
                 }
                 break;
             case TEFMOD16_TRI1_PORT:
             case TEFMOD16_TRI2_PORT:
                 if (1 == port_num_lanes) {
                     for (symbol_reg_index = 0; symbol_reg_index < symbol_reg_max_num; symbol_reg_index++) {
                          TEFMOD16_RD_SYMBOL_ERROR_CNTR(&phy_copy, symbol_reg_index, &error_cnt);
                          TEFMOD16_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                     }
                 } else {
                    if (fec_lane_id < 2) {
                        TEFMOD16_RD_SYMBOL_ERROR_CNTR(&phy_copy, fec_lane_id, &error_cnt);
                        TEFMOD16_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                        TEFMOD16_RD_SYMBOL_ERROR_CNTR(&phy_copy, fec_lane_id + 2, &error_cnt);
                        TEFMOD16_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                    } else {
                        return PHYMOD_E_PARAM;
                    }
                 }
                  break;
             case TEFMOD16_MULTI_PORT:
                 for (symbol_reg_index = 0; symbol_reg_index < symbol_reg_max_num; symbol_reg_index++) {
                     TEFMOD16_RD_SYMBOL_ERROR_CNTR(&phy_copy, symbol_reg_index, &error_cnt);
                     TEFMOD16_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                 }
                 break;
             default:
                return PHYMOD_E_PARAM;
        }
    }

    return PHYMOD_E_NONE;
}

int tefmod16_fec_error_inject_set(PHYMOD_ST *pc,
                                  uint16_t error_control_map,
                                  tefmod16_fec_error_mask_t bit_error_mask)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    int i = 0, cl74_en = 0;
    phymod_access_t pa_copy;

    PKTGEN_ERRMASK0r_t PKTGEN_ERRMASK0r_reg;
    PKTGEN_ERRMASK1r_t PKTGEN_ERRMASK1r_reg;
    PKTGEN_ERRMASK2r_t PKTGEN_ERRMASK2r_reg;
    PKTGEN_ERRMASK3r_t PKTGEN_ERRMASK3r_reg;
    PKTGEN_ERRMASK4r_t PKTGEN_ERRMASK4r_reg;
    PKTGEN_ERR_INJ_EN0r_t PKTGEN_ERR_INJ_EN0r_reg;
    PKTGEN_ERR_INJ_EN1r_t PKTGEN_ERR_INJ_EN1r_reg;
    TX_X1_TX_LN_SWPr_t TX_X1_TX_LN_SWPr_reg;

    PKTGEN_ERRMASK0r_CLR(PKTGEN_ERRMASK0r_reg);
    PKTGEN_ERRMASK1r_CLR(PKTGEN_ERRMASK1r_reg);
    PKTGEN_ERRMASK2r_CLR(PKTGEN_ERRMASK2r_reg);
    PKTGEN_ERRMASK3r_CLR(PKTGEN_ERRMASK3r_reg);
    PKTGEN_ERRMASK4r_CLR(PKTGEN_ERRMASK4r_reg);
    PKTGEN_ERR_INJ_EN0r_CLR(PKTGEN_ERR_INJ_EN0r_reg);
    PKTGEN_ERR_INJ_EN1r_CLR(PKTGEN_ERR_INJ_EN1r_reg);

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&pa_copy, &start_lane, &num_lane));

    /* The registers are core based */
    pa_copy.lane_mask = 0x1 << start_lane;

    PKTGEN_ERRMASK0r_ERROR_MASK_15_0f_SET(PKTGEN_ERRMASK0r_reg, bit_error_mask.error_mask_bit_31_0 & 0xffff);
    PHYMOD_IF_ERR_RETURN(WRITE_PKTGEN_ERRMASK0r(&pa_copy, PKTGEN_ERRMASK0r_reg));

    PKTGEN_ERRMASK1r_ERROR_MASK_31_16f_SET(PKTGEN_ERRMASK1r_reg, (bit_error_mask.error_mask_bit_31_0 >> 16) & 0xffff);
    PHYMOD_IF_ERR_RETURN(WRITE_PKTGEN_ERRMASK1r(&pa_copy, PKTGEN_ERRMASK1r_reg));

    PKTGEN_ERRMASK2r_ERROR_MASK_47_32f_SET(PKTGEN_ERRMASK2r_reg, bit_error_mask.error_mask_bit_63_32 & 0xffff);
    PHYMOD_IF_ERR_RETURN(WRITE_PKTGEN_ERRMASK2r(&pa_copy, PKTGEN_ERRMASK2r_reg));

    PKTGEN_ERRMASK3r_ERROR_MASK_63_48f_SET(PKTGEN_ERRMASK3r_reg, (bit_error_mask.error_mask_bit_63_32 >> 16) & 0xffff);
    PHYMOD_IF_ERR_RETURN(WRITE_PKTGEN_ERRMASK3r(&pa_copy, PKTGEN_ERRMASK3r_reg));

    PKTGEN_ERRMASK4r_ERROR_MASK_65_64f_SET(PKTGEN_ERRMASK4r_reg, bit_error_mask.error_mask_bit_79_64 & 0x3);
    PHYMOD_IF_ERR_RETURN(WRITE_PKTGEN_ERRMASK4r(&pa_copy, PKTGEN_ERRMASK4r_reg));

    /* Need to clear the reg first */
    PHYMOD_IF_ERR_RETURN(WRITE_PKTGEN_ERR_INJ_EN0r(&pa_copy, PKTGEN_ERR_INJ_EN0r_reg));
    PHYMOD_IF_ERR_RETURN(WRITE_PKTGEN_ERR_INJ_EN1r(&pa_copy, PKTGEN_ERR_INJ_EN1r_reg));

    PHYMOD_IF_ERR_RETURN(tefmod16_FEC_get(pc, TEFMOD16_CL74, &cl74_en));
    if (cl74_en) {
        for (i = 0; i < num_lane; i++) {
             /*Support non-consecutive lanes*/
             if (!PHYMOD_LANEPBMP_MEMBER(pc->lane_mask, start_lane + i)) {
                 continue;
             }
             /* Get physical lane mapping */
             PHYMOD_IF_ERR_RETURN(READ_TX_X1_TX_LN_SWPr(&pa_copy, &TX_X1_TX_LN_SWPr_reg));
             switch (start_lane + i) {
                 case 0x0:
                     phy_lane = TX_X1_TX_LN_SWPr_LOGICAL0_TO_PHY_SELf_GET(TX_X1_TX_LN_SWPr_reg);
                     break;
                 case 0x1:
                     phy_lane = TX_X1_TX_LN_SWPr_LOGICAL1_TO_PHY_SELf_GET(TX_X1_TX_LN_SWPr_reg);
                     break;
                 case 0x2:
                     phy_lane = TX_X1_TX_LN_SWPr_LOGICAL2_TO_PHY_SELf_GET(TX_X1_TX_LN_SWPr_reg);
                     break;
                 case 0x3:
                     phy_lane = TX_X1_TX_LN_SWPr_LOGICAL3_TO_PHY_SELf_GET(TX_X1_TX_LN_SWPr_reg);
                     break;
                 default:
                     return PHYMOD_E_PARAM;
            }
            TEFMOD16_PKTGEN_ERR_INJ_EN_FIELD_SET(cl74_en, phy_lane);
        }
    } else {
        /* RSFEC error injection is per port */
        TEFMOD16_PKTGEN_ERR_INJ_EN_FIELD_SET(cl74_en, start_lane);
    }

    PHYMOD_IF_ERR_RETURN(WRITE_PKTGEN_ERR_INJ_EN0r(&pa_copy, PKTGEN_ERR_INJ_EN0r_reg));
    PHYMOD_IF_ERR_RETURN(WRITE_PKTGEN_ERR_INJ_EN1r(&pa_copy, PKTGEN_ERR_INJ_EN1r_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_fec_error_inject_get(PHYMOD_ST *pc,
                                  uint16_t* error_control_map,
                                  tefmod16_fec_error_mask_t* bit_error_mask)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    int i = 0, cl74_en = 0;
    phymod_access_t pa_copy;
    uint16_t errgen_en = 0;

    PKTGEN_ERRMASK0r_t PKTGEN_ERRMASK0r_reg;
    PKTGEN_ERRMASK1r_t PKTGEN_ERRMASK1r_reg;
    PKTGEN_ERRMASK2r_t PKTGEN_ERRMASK2r_reg;
    PKTGEN_ERRMASK3r_t PKTGEN_ERRMASK3r_reg;
    PKTGEN_ERRMASK4r_t PKTGEN_ERRMASK4r_reg;
    PKTGEN_ERR_INJ_EN0r_t PKTGEN_ERR_INJ_EN0r_reg;
    PKTGEN_ERR_INJ_EN1r_t PKTGEN_ERR_INJ_EN1r_reg;
    TX_X1_TX_LN_SWPr_t TX_X1_TX_LN_SWPr_reg;

    PKTGEN_ERRMASK0r_CLR(PKTGEN_ERRMASK0r_reg);
    PKTGEN_ERRMASK1r_CLR(PKTGEN_ERRMASK1r_reg);
    PKTGEN_ERRMASK2r_CLR(PKTGEN_ERRMASK2r_reg);
    PKTGEN_ERRMASK3r_CLR(PKTGEN_ERRMASK3r_reg);
    PKTGEN_ERRMASK4r_CLR(PKTGEN_ERRMASK4r_reg);
    PKTGEN_ERR_INJ_EN0r_CLR(PKTGEN_ERR_INJ_EN0r_reg);
    PKTGEN_ERR_INJ_EN1r_CLR(PKTGEN_ERR_INJ_EN1r_reg);

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&pa_copy, &start_lane, &num_lane));

    pa_copy.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(READ_PKTGEN_ERRMASK0r(&pa_copy, &PKTGEN_ERRMASK0r_reg));
    PHYMOD_IF_ERR_RETURN(READ_PKTGEN_ERRMASK1r(&pa_copy, &PKTGEN_ERRMASK1r_reg));

    bit_error_mask->error_mask_bit_31_0 = (PKTGEN_ERRMASK1r_ERROR_MASK_31_16f_GET(PKTGEN_ERRMASK1r_reg) & 0xffff) << 16 |
                                          (PKTGEN_ERRMASK0r_ERROR_MASK_15_0f_GET(PKTGEN_ERRMASK0r_reg) & 0xffff);

    PHYMOD_IF_ERR_RETURN(READ_PKTGEN_ERRMASK2r(&pa_copy, &PKTGEN_ERRMASK2r_reg));
    PHYMOD_IF_ERR_RETURN(READ_PKTGEN_ERRMASK3r(&pa_copy, &PKTGEN_ERRMASK3r_reg));
    bit_error_mask->error_mask_bit_63_32 = (PKTGEN_ERRMASK3r_ERROR_MASK_63_48f_GET(PKTGEN_ERRMASK3r_reg) & 0xffff) << 16 |
                                           (PKTGEN_ERRMASK2r_ERROR_MASK_47_32f_GET(PKTGEN_ERRMASK2r_reg) & 0xffff);

    PHYMOD_IF_ERR_RETURN(READ_PKTGEN_ERRMASK4r(&pa_copy, &PKTGEN_ERRMASK4r_reg));
    bit_error_mask->error_mask_bit_79_64 = PKTGEN_ERRMASK4r_ERROR_MASK_65_64f_GET(PKTGEN_ERRMASK4r_reg) & 0x3;

    PHYMOD_IF_ERR_RETURN(READ_PKTGEN_ERR_INJ_EN0r(&pa_copy, &PKTGEN_ERR_INJ_EN0r_reg));
    PHYMOD_IF_ERR_RETURN(READ_PKTGEN_ERR_INJ_EN1r(&pa_copy, &PKTGEN_ERR_INJ_EN1r_reg));

    PHYMOD_IF_ERR_RETURN(tefmod16_FEC_get(pc, TEFMOD16_CL74, &cl74_en));
    if (cl74_en) {
        for (i = 0; i < num_lane; i++) {
            /* Get physical lane mapping */
            PHYMOD_IF_ERR_RETURN(READ_TX_X1_TX_LN_SWPr(&pa_copy, &TX_X1_TX_LN_SWPr_reg));
            switch (start_lane + i) {
                case 0x0:
                    phy_lane = TX_X1_TX_LN_SWPr_LOGICAL0_TO_PHY_SELf_GET(TX_X1_TX_LN_SWPr_reg);
                    break;
                case 0x1:
                    phy_lane = TX_X1_TX_LN_SWPr_LOGICAL1_TO_PHY_SELf_GET(TX_X1_TX_LN_SWPr_reg);
                    break;
                case 0x2:
                    phy_lane = TX_X1_TX_LN_SWPr_LOGICAL2_TO_PHY_SELf_GET(TX_X1_TX_LN_SWPr_reg);
                    break;
                case 0x3:
                    phy_lane = TX_X1_TX_LN_SWPr_LOGICAL3_TO_PHY_SELf_GET(TX_X1_TX_LN_SWPr_reg);
                    break;
                default:
                    return PHYMOD_E_PARAM;
           }
            TEFMOD16_PKTGEN_ERR_INJ_EN_FIELD_GET(cl74_en, phy_lane, &errgen_en);
            if (errgen_en) {
                *error_control_map |= 0x1 << (start_lane + i);
            }
        }
    } else {
        /* RSFEC error injection is per port */
        TEFMOD16_PKTGEN_ERR_INJ_EN_FIELD_GET(cl74_en, start_lane, &errgen_en);
        *error_control_map = errgen_en ? 1 : 0;
    }

    return PHYMOD_E_NONE;
}

#endif /* _SDK_TEFMOD16_ */
