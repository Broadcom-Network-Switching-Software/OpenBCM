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
 * File       : tefmod16_gen3_cfg_seq.c
 * Description: c functions implementing Tier1s for TEFMod Serdes Driver
 *---------------------------------------------------------------------*/
#define _SDK_TEFMOD16_GEN3_ 1

#include <phymod/phymod.h>
#include <phymod/phymod_types.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_tscf16_gen3_xgxs_defs.h>
#include "tefmod16_gen3_enum_defines.h"
#include "tefmod16_gen3.h"
#include "tefmod16_gen3_sc_lkup_table.h"

#ifdef _SDK_TEFMOD16_GEN3_
#define PHYMOD_ST const phymod_access_t
#else
#define PHYMOD_ST tefmod_st
#endif

#ifdef _SDK_TEFMOD16_GEN3_
#endif

#define TEFMOD_CL73_2P5GBASE_KX1_POS 11
#define TEFMOD_CL73_5GBASE_KR1_POS   12
#define TEFMOD_CL73_25GBASE_KR1_POS  10
#define TEFMOD_CL73_25GBASE_CR1_POS  10
#define TEFMOD_CL73_25GBASE_KRS1_POS  9
#define TEFMOD_CL73_25GBASE_CRS1_POS  9
#define TEFMOD_CL73_25GBASE_FEC_POS 24
#define TEFMOD_CL73_25G_RS_FEC_POS 23
#define TEFMOD_BAM_CL74_REQ 0x3
#define TEFMOD_BAM_CL91_REQ 0x3
#define REV_A 0x0
#define REV_B 0x1

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

#define TEFMOD16_GEN3_RD_SYMBOL_ERROR_CNTR(pc, index, counter) \
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

#define TEFMOD16_GEN3_SYMBOL_ERROR_CNTR_ADD(dst, src)      \
        do {  \
             /* Check overflow */                \
             if ((src) > (0xffffffff - (dst))) { \
                 (dst) = 0xffffffff;           \
             } else {                        \
                 (dst) += (src);                 \
             }                               \
       } while (0)

#ifdef _SDK_TEFMOD16_GEN3_
    #define TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc) \
        PHYMOD_VDBG(TEFMOD16_GEN3_DBG_FUNC,pc,("-22%s: Adr:%08x Ln:%02d\n", __func__, (unsigned int)pc->addr, (int)pc->lane_mask))
    #define TEFMOD16_GEN3_DBG_IN_FUNC_VIN_INFO(pc,_print_) \
        PHYMOD_VDBG(TEFMOD16_GEN3_DBG_FUNCVALIN,pc,_print_)
    #define TEFMOD16_GEN3_DBG_IN_FUNC_VOUT_INFO(pc,_print_) \
        PHYMOD_VDBG(TEFMOD16_GEN3_DBG_FUNCVALOUT,pc,_print_)
    #define TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc,_print_) \
        PHYMOD_VDBG(TEFMOD16_GEN3_DBG_FUNCVAR,pc,_print_)
#endif

static uint32_t my_am_ts_calc_value_cnt[4][20] = {{0}};
static int64 my_am_ts_calc_value_avg[4][20];

static int64 my_am_ts_calc_value_low[4][20];
static int64 my_am_ts_calc_value_high[4][20];
static int64 my_am_ts_calc_value_diff[4][20];

/* deskew value (23-bit)={deskew_int(12-bits),deskew_frac(10-bits)} */
#define TEFMOD16_GEN3_1588_RX_DESKEW_SET(my_deskew_val_int, my_deskew_val_frac, my_deskew_val) \
        do {                                                       \
            (my_deskew_val_int) = ((my_deskew_val) >> 16) & 0xfff;              \
            (my_deskew_val_frac) = ((my_deskew_val) >> 6) & 0x3ff;              \
        }while(0)

#define TEFMOD16_GEN3_PKTGEN_ERR_INJ_EN_FIELD_SET(cl74_en, index) \
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

#define TEFMOD16_GEN3_PKTGEN_ERR_INJ_EN_FIELD_GET(cl74_en, index, errgen_en) \
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

/* DV code for reference */
#ifdef _DV_TB_

int tefmod_pmd_lock_get(PHYMOD_ST* pc, uint32_t* lockStatus)
{
  PMD_X4_STSr_t  reg_pmd_x4_sts;
  int i;
  int lane_mask;
  PHYMOD_ST* pc_copy;
  TEFMOD_DBG_IN_FUNC_INFO(pc);

  pc_copy = pc;
  pc_copy->port_type = TEFMOD_MULTI_PORT;
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
    PMD_X4_CTLr_LN_DP_H_RSTBf_SET(PMD_X4_PMD_X4_CONTROLr_reg,1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg)); 

  } else {
    PMD_X4_OVRRr_CLR(PMD_X4_OVERRIDEr_reg);
    PMD_X4_OVRRr_LN_DP_H_RSTB_OENf_SET(PMD_X4_OVERRIDEr_reg,0);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_OVRRr(pc, PMD_X4_OVERRIDEr_reg));

    PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);
    PMD_X4_CTLr_LN_DP_H_RSTBf_SET(PMD_X4_PMD_X4_CONTROLr_reg,1);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));
  }


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

int tefmod_autoneg_set(PHYMOD_ST* pc)               /* AUTONEG_SET */
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
  AN_X4_LD_BASE_ABIL5r_t AN_X4_LD_BASE_ABIL5r_reg; 

  
  AN_X4_LD_FEC_BASEPAGE_ABILr_t AN_X4_LD_FEC_BASEPAGE_ABILr_reg;
  AN_X4_LD_PAGE0r_t  AN_X4_LD_PAGE0r_reg;
  
  TEFMOD_DBG_IN_FUNC_INFO(pc);


  

  /*****FEC Override ****/
  tefmod_an_fec_override(pc);

  /***********Override programming ********/
  AN_X4_CL73_CTLSr_CLR(AN_X4_ABILITIES_CL73_CONTROLSr_reg);
  data = pc->an_misc_ctrls;
  AN_X4_CL73_CTLSr_SET(AN_X4_ABILITIES_CL73_CONTROLSr_reg, data);
  /********Setting AN_X4_ABILITIES_cl73_controls 0xC186*****/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_CL73_CTLSr(pc, AN_X4_ABILITIES_CL73_CONTROLSr_reg));

  if(pc->sw_an) {
    return PHYMOD_E_NONE;
  }
  /*****MSA OUI Mode******/
  AN_X4_LD_PAGE0r_CLR(AN_X4_LD_PAGE0r_reg);
  AN_X4_LD_PAGE0r_LD_PAGE_0f_SET(AN_X4_LD_PAGE0r_reg, pc->msa_oui_mode);
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_PAGE0r(pc, AN_X4_LD_PAGE0r_reg));
  

  AN_X1_OUI_LWRr_CLR(AN_X1_CONTROL_OUI_LOWERr_reg);
  AN_X1_OUI_LWRr_SET(AN_X1_CONTROL_OUI_LOWERr_reg, pc->oui & 0xffff);
  /********Setting AN_X1_OUI_LWR 0x9241*****/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_OUI_LWRr(pc, AN_X1_CONTROL_OUI_LOWERr_reg));
 
  AN_X1_OUI_UPRr_CLR(AN_X1_CONTROL_OUI_UPPERr_reg);
  AN_X1_OUI_UPRr_SET(AN_X1_CONTROL_OUI_UPPERr_reg, (pc->oui >> 16) & 0xffff);
  /********Setting AN_X1_OUI_UPR 0x9240*****/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_OUI_UPRr(pc, AN_X1_CONTROL_OUI_UPPERr_reg));




  /********BAM Code********/
  if(pc->an_type == TEFMOD_CL73_BAM || pc->an_type == TEFMOD_HPAM)
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
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_1G ) & 1 ) {
    AN_X4_LD_BASE_ABIL1r_BASE_1G_KX1f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_10G_KR ) & 1 ) {
    AN_X4_LD_BASE_ABIL1r_BASE_10G_KR1f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_40G_KR4 ) & 1 ) {
    AN_X4_LD_BASE_ABIL1r_BASE_40G_KR4f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_40G_CR4 ) & 1 ) {
    AN_X4_LD_BASE_ABIL1r_BASE_40G_CR4f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_100G_KR4 ) & 1 ) { 
    AN_X4_LD_BASE_ABIL1r_BASE_100G_KR4f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_100G_CR4 ) & 1 ) {
    AN_X4_LD_BASE_ABIL1r_BASE_100G_CR4f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  /* ???? Add 4 new IEEE speeds A9=25g-krs, A10=25g-crs, A11=2.5g-kx, A12=5g-kr1
13 RW BASE_5G_KR1 MP1024 UP1.1
12 RW BASE_2P5G_KX1 MP1024 UP1.0
   

  if( (pc->an_tech_ability >> TEFMOD_ABILITY_5G_KR ) & 1 ) {
    AN_X4_LD_BASE_ABIL1r_BASE_5G_KR1f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_2P5G_X1 ) & 1 ) {
    AN_X4_LD_BASE_ABIL1r_BASE_2P5G_KX1f_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }

*/
  /******* Pause Settings ********/
  if(pc->an_pause == TEFMOD_NO_PAUSE) 
    AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 0);
  if(pc->an_pause == TEFMOD_ASYM_PAUSE) 
    AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  if(pc->an_pause == TEFMOD_SYM_PAUSE) 
    AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 2);
  if(pc->an_pause == TEFMOD_ASYM_SYM_PAUSE) 
    AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 3);

  /****** FEC Settings ********/
  if(pc->an_fec == TEFMOD_FEC_NOT_SUPRTD) 
    AN_X4_LD_BASE_ABIL1r_FEC_REQf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 0);
  if(pc->an_fec == TEFMOD_FEC_SUPRTD_NOT_REQSTD) {
    AN_X4_LD_BASE_ABIL1r_FEC_REQf_SET(AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg, 1);
  }
  if(pc->an_fec == TEFMOD_FEC_SUPRTD_REQSTD) {
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


  AN_X4_LD_BASE_ABIL2r_CLR(AN_X4_LD_BASE_ABIL2r_reg);
  AN_X4_LD_BASE_ABIL2r_BASE_50G_KR2_SELf_SET(AN_X4_LD_BASE_ABIL2r_reg, (pc->base_50g_kr2 & 0x1f));
  AN_X4_LD_BASE_ABIL2r_BASE_50G_KR2_ENf_SET(AN_X4_LD_BASE_ABIL2r_reg, (pc->base_50g_kr2 >> 16) & 1);
  AN_X4_LD_BASE_ABIL2r_BASE_50G_CR2_SELf_SET(AN_X4_LD_BASE_ABIL2r_reg, (pc->base_50g_cr2 & 0x1f));
  AN_X4_LD_BASE_ABIL2r_BASE_50G_CR2_ENf_SET(AN_X4_LD_BASE_ABIL2r_reg, (pc->base_50g_cr2 >> 16) & 1);
  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_BASE_ABIL2r(pc, AN_X4_LD_BASE_ABIL2r_reg ));

  /*New speeds base abil5*/
  printf(" %s pc->base_5p0g %0x pc->base_2p5g %0x :: unit %0d port %0d \n",__func__,pc->base_5p0g,pc->base_2p5g,pc->unit, pc->this_lane);
   
  AN_X4_LD_BASE_ABIL5r_CLR(AN_X4_LD_BASE_ABIL5r_reg);
  AN_X4_LD_BASE_ABIL5r_BASE_5P0G_SELf_SET(AN_X4_LD_BASE_ABIL5r_reg, (pc->base_5p0g & 0x1f));
  AN_X4_LD_BASE_ABIL5r_BASE_5P0G_ENf_SET(AN_X4_LD_BASE_ABIL5r_reg, (pc->base_5p0g >> 16) & 1);
  AN_X4_LD_BASE_ABIL5r_BASE_2P5G_SELf_SET(AN_X4_LD_BASE_ABIL5r_reg, (pc->base_2p5g & 0x1f));
  AN_X4_LD_BASE_ABIL5r_BASE_2P5G_ENf_SET(AN_X4_LD_BASE_ABIL5r_reg, (pc->base_2p5g >> 16) & 1);
  PHYMOD_IF_ERR_RETURN ( MODIFY_AN_X4_LD_BASE_ABIL5r(pc,AN_X4_LD_BASE_ABIL5r_reg ));
  
 
  data =0;
  AN_X4_LD_UP1_ABIL0r_CLR(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg);
  
  /******* User page abilities*********/
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_20G_KR2) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_20G_KR2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_20G_CR2) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_20G_CR2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_40G_KR2) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_40G_KR2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_40G_CR2) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_40G_CR2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_50G_KR2) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_50G_KR2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_50G_CR2) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_50G_CR2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_50G_KR4) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_50G_KR4f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_50G_CR4) & 1 ) 
    AN_X4_LD_UP1_ABIL0r_BAM_50G_CR4f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, 1);
 
  AN_X4_LD_UP1_ABIL0r_BAM_HG2f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg, pc->an_higig2);
 
  /******** Setting AN_X4_ABILITIES_ld_up1_abilities_0 0xC181******/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_UP1_ABIL0r(pc, AN_X4_ABILITIES_LD_UP1_ABILITIES_0r_reg));
 
 
  data =0;
  AN_X4_LD_UP1_ABIL1r_CLR(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg);
  /******* User page abilities*********/
 if( (pc->an_tech_ability >> TEFMOD_ABILITY_20G_KR1) & 1 ) 
    AN_X4_LD_UP1_ABIL1r_BAM_20G_KR1f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_20G_CR1) & 1 ) 
    AN_X4_LD_UP1_ABIL1r_BAM_20G_CR1f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_25G_KR1) & 1 ) 
    AN_X4_LD_UP1_ABIL1r_BAM_25G_KR1f_SET(AN_X4_ABILITIES_LD_UP1_ABILITIES_1r_reg, 1);
  if( (pc->an_tech_ability >> TEFMOD_ABILITY_25G_CR1) & 1 ) 
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

int tefmod_autoneg_control(PHYMOD_ST* pc)
{
  uint16_t cl73_bam_enable, cl73_hpam_enable, cl73_enable;
  uint16_t cl73_nonce_match_over, cl73_nonce_match_val ;
  uint16_t bam_to_hpam_ad_en;
  uint16_t num_advertised_lanes;
  AN_X4_CL73_CFGr_t AN_X4_ABILITIES_CL73_CFGr_reg;
  AN_X4_WAIT_ACK_COMPLETEr_t AN_X4_WAIT_ACK_COMPLETEr_reg;
 
  TEFMOD_DBG_IN_FUNC_INFO(pc);
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
  #ifdef _DV_TB_
  printf("%22s AN_X4_ABILITIES_CL73 :: %d :: lane :: %d writing cl73_enable=%0d \n",__func__,pc->unit, pc->this_lane,cl73_enable);
#endif

 
  AN_X4_CL73_CFGr_CL73_HPAM_ENABLEf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, cl73_hpam_enable);
 
  AN_X4_CL73_CFGr_CL73_BAM_ENABLEf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, cl73_bam_enable);
 
  AN_X4_CL73_CFGr_BAM_TO_HPAM_AD_ENf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, bam_to_hpam_ad_en);
 
  AN_X4_CL73_CFGr_AD_TO_CL73_ENf_SET(AN_X4_ABILITIES_CL73_CFGr_reg, pc->hpam_to_cl73_ad_en);
  /*********Setting AN_X4_ABILITIES_cl73_cfg 0xC180 ******/
  PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_CL73_CFGr(pc, AN_X4_ABILITIES_CL73_CFGr_reg));

  return PHYMOD_E_NONE;
} /* tefmod_autoneg_control(PHYMOD_ST* pc) */

/*!
@brief Sets the core for autoneg
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
This function sets the tsc core for autoneg generic settings.
If per_lane_control=TEFMOD_CTL_AN_CHIP_INIT(0x1) PLL and RX path are reset.
If per_lane_control=TEFMOD_CTL_AN_MODE_INIT(0x2) PLL and RX path are not reset.
*/
int tefmod_set_an_port_mode(PHYMOD_ST* pc)    /* SET_AN_PORT_MODE */
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

  TEFMOD_DBG_IN_FUNC_INFO(pc);
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
  
/*  if ((pc-> an_type == TEFMOD_HPAM) | (pc-> an_type == TEFMOD_CL73_BAM)) {
#if defined (_DV_TB_) 
    cl73_error_timer_period       = 0x64;
#endif
  } */

  if ((pc-> an_type == TEFMOD_CL73) | (pc-> an_type == TEFMOD_HPAM) | 
      (pc-> an_type == TEFMOD_CL73_BAM)) {

    ref_clk_sel = main0_refClkSelect_clk_156p25MHz;


    /*FIX: USE PASS DOWN var  pc->single_port_mode */
    if ( pc->tsc_touched!=1 && pc->lane_select ) {
      single_port_mode=pc->single_port_mode ;
#ifdef _DV_TB_
      printf("%22s B4 -- MAIN0_SETUPr_SINGLE_PORT_MODE set  unit :: %d :: lane :: %d single_port_mode=%0d\n",__func__,pc->unit, pc->this_lane,single_port_mode,MAIN0_SETUPr_MASTER_PORT_NUMf_GET(MAIN0_SETUPr_reg));

      MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(MAIN0_SETUPr_reg,single_port_mode );

      printf("%22s MAIN0_SETUPr_SINGLE_PORT_MODE set  unit :: %d :: lane :: %d pc->single_port_mode::%0d pc->tsc_touched:: %0d  dual_pll %0d >> main0_setup.single_port_mode=%0d \n",__func__,pc->unit, pc->this_lane,pc->single_port_mode,pc->tsc_touched,pc->dual_pll,MAIN0_SETUPr_SINGLE_PORT_MODEf_GET(MAIN0_SETUPr_reg));

#endif
    }
  /*  plldiv = 66; */
  }

  if(pc->per_lane_control == TEFMOD_CTL_AN_CHIP_INIT) {
    MAIN0_SETUPr_REFCLK_SELf_SET(MAIN0_SETUPr_reg, ref_clk_sel);
  }
 

  if(port_mode_sel_reg == TEFMOD_DXGXS) {
    modify_port_mode = 1;
    if(pc->this_lane == 0) {
      new_port_mode_sel = TEFMOD_TRI1_PORT;
    }
    else if(pc->this_lane == 2) {
      new_port_mode_sel = TEFMOD_TRI2_PORT;
    }
  }
  if(port_mode_sel_reg == TEFMOD_TRI1_PORT) {
    if(pc->this_lane == 2) {
      modify_port_mode = 1;
      new_port_mode_sel = TEFMOD_MULTI_PORT;
    }
  }
  if(port_mode_sel_reg == TEFMOD_TRI2_PORT) {
    if(pc->this_lane == 0) {
      modify_port_mode = 1;
      new_port_mode_sel = TEFMOD_MULTI_PORT;
    }
  }
  if(port_mode_sel_reg == TEFMOD_SINGLE_PORT) {
    modify_port_mode = 1;
    new_port_mode_sel = TEFMOD_MULTI_PORT;
  }
  if(modify_port_mode == 1) {
    MAIN0_SETUPr_PORT_MODE_SELf_SET(MAIN0_SETUPr_reg, new_port_mode_sel);
  }
   
  /*mask |= MAIN0_SETUP_PORT_MODE_SEL_MASK; */ 
 
  MAIN0_SETUPr_CL72_ENf_SET(MAIN0_SETUPr_reg, pc->cl72_en);
 
  MAIN0_SETUPr_CL73_VCOf_SET(MAIN0_SETUPr_reg, pc->cl73_vco);
 
  /* 0x9000 */
  PHYMOD_IF_ERR_RETURN (MODIFY_MAIN0_SETUPr(pc, MAIN0_SETUPr_reg));
  
  pc->port_type = TEFMOD_MULTI_PORT; 

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
} /* tefmod_set_an_port_mode(PHYMOD_ST* pc) */


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
int tefmod_rx_pmd_loopback_control(PHYMOD_ST* pc, int pcs_rloop_en, int pmd_rloop_en, int lane)
{
  int    cntl;
  TLB_TX_RMT_LPBK_CFGr_t TLB_TX_RMT_LPBK_CONFIGr_reg;
  DSC_CDR_CTL2r_t        DSC_C_CDR_CONTROL_2r_reg;

  #ifdef TSC_16NM
  TX_PI_TX_PI_CTL0r_t          TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg;
  #else 
  TX_PI_MONTEREY_TX_PI_CTL0r_t TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg;
  #endif

  TEFMOD_DBG_IN_FUNC_INFO(pc);

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

    #ifdef TSC_16NM
    TX_PI_TX_PI_CTL0r_CLR(TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg);
    TX_PI_TX_PI_CTL0r_TX_PI_ENf_SET(TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg, 1);

    PHYMOD_IF_ERR_RETURN(MODIFY_TX_PI_TX_PI_CTL0r(pc, TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg));
    #else 
    TX_PI_MONTEREY_TX_PI_CTL0r_CLR(TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg);
    TX_PI_MONTEREY_TX_PI_CTL0r_TX_PI_ENf_SET(TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg, 1);

    PHYMOD_IF_ERR_RETURN(MODIFY_TX_PI_MONTEREY_TX_PI_CTL0r(pc, TX_PI_PMD_COM_TX_PI_PMD_COM_CONTROL_0r_reg));
    #endif

#ifdef _DV_TB_
  } else {

  }
#endif
  return PHYMOD_E_NONE;
}  /* tefmod_rx_pmd_loopback_control */


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

int tefmod_credit_set(PHYMOD_ST* pc, tefmod_spd_intfc_type_t spd_intf)   /* CREDIT_SET */
{
  uint16_t clockcnt0, clockcnt1, loopcnt0, loopcnt1, mac_credit;

  TX_X4_CRED0r_t TX_X4_CREDIT0_CREDIT0r_reg;
  TX_X4_CRED1r_t TX_X4_CREDIT0_CREDIT1r_reg;
  TX_X4_LOOPCNTr_t TX_X4_CREDIT0_LOOPCNTr_reg;
  TX_X4_MAC_CREDGENCNTr_t TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg;
  /* uint16_t data_sgmii_spd_switch;  */
  int cntl;

  TEFMOD_DBG_IN_FUNC_INFO(pc);
  /*dv_tfmod_update_st(pc);*/
  cntl = pc->per_lane_control;
  /* data_sgmii_spd_switch = 0; */
  clockcnt0        = 0;
  clockcnt1        = 0;
  loopcnt0         = 1;
  loopcnt1         = 0;
  mac_credit       = 0;
   
  if (cntl) {
    if (spd_intf == TEFMOD_SPD_1000_SGMII) {

    } else if (spd_intf == TEFMOD_SPD_2500) {
      clockcnt0  = 40;
      clockcnt1  = 79;
      loopcnt0   = 1;
      loopcnt1   = 2;
      mac_credit = 39;
    } else if ((spd_intf == TEFMOD_SPD_10000_XFI)||
       (spd_intf == TEFMOD_SPD_10600_XFI_HG)) {
      clockcnt0  = 9;
      clockcnt1  = 10;
      loopcnt0   = 1;
      loopcnt1   = 9;
      mac_credit = 6;

    } else if ((spd_intf == TEFMOD_SPD_20000_XFI) ||
       (spd_intf == TEFMOD_SPD_21200_XFI_HG)     ||
       (spd_intf == TEFMOD_SPD_25000_XFI)        ||
       (spd_intf == TEFMOD_SPD_26500_XFI_HG)) {
      clockcnt0  = 9;
      clockcnt1  = 5;
      loopcnt0   = 1;
      loopcnt1   = 18;
      mac_credit = 4;

    } else if ((spd_intf == TEFMOD_SPD_20G_MLD_X2) ||
       (spd_intf == TEFMOD_SPD_21G_MLD_HG_X2)) {
      clockcnt0  = 9;
      clockcnt1  = 5;
      loopcnt0   = 1;
      loopcnt1   = 18;
      mac_credit = 4;

    } else if ((spd_intf == TEFMOD_SPD_40G_MLD_X2) ||
       (spd_intf == TEFMOD_SPD_42G_MLD_HG_X2)      ||
       (spd_intf == TEFMOD_SPD_50G_MLD_X2)         ||
       (spd_intf == TEFMOD_SPD_53G_MLD_HG_X2)      ||
       (spd_intf == TEFMOD_SPD_40G_MLD_X4)         ||
       (spd_intf == TEFMOD_SPD_42G_MLD_HG_X4)      ||
       (spd_intf == TEFMOD_SPD_50G_MLD_X4)         ||
       (spd_intf == TEFMOD_SPD_53G_MLD_HG_X4) ) {
      clockcnt0  = 9;
      clockcnt1  = 5;
      loopcnt0   = 1;
      loopcnt1   = 18;
      mac_credit = 2;

    } else if ((spd_intf == TEFMOD_SPD_100G_MLD_X4) ||
       (spd_intf == TEFMOD_SPD_106G_MLD_HG_X4)) {
      clockcnt0  = 9;
      clockcnt1  = 5;
      loopcnt0   = 1;
      loopcnt1   = 18;
      mac_credit = 1;

    } else {
      PHYMOD_DEBUG_ERROR(("%-22s ERROR: p=%0d spd_intf=%d undefined %s cntl=%0x verb=%x\n",
                        __func__, pc->port, spd_intf,
              e2s_tefmod_spd_intfc_type_t[spd_intf], cntl, pc->verbosity));
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
int tefmod_encode_set(PHYMOD_ST* pc, int per_lane_control, tefmod_spd_intfc_type_t spd_intf, int tx_am_timer_init_val)         /* ENCODE_SET */
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

  TEFMOD_DBG_IN_FUNC_INFO(pc);
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

  if((spd_intf == TEFMOD_SPD_10_SGMII)  ||
     (spd_intf == TEFMOD_SPD_100_SGMII) ||
     (spd_intf == TEFMOD_SPD_1000_SGMII)) { 
    /* ------------------------------------- 0xc114 */
  } else if((spd_intf == TEFMOD_SPD_10000_XFI) ||
            (spd_intf == TEFMOD_SPD_10600_XFI_HG)) {
    t_fifo_mode      = 0;
    t_enc_mode       = 1;
    cl49_tx_tl_mode  = 0;
    cl49_bypass_txsm = 0;
    hg2_en           = 0;
    scr_mode         = 1;
    #ifdef TSC_16NM
    os_mode          = 0; /* OS was 1 : now 0 due to VCO 25 with 2.5 divider */
    #else
    os_mode          = 1;
    #endif
    t_pma_btmx_mode  = 0;
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 0;
    /* ------------------------------------ 0xc113 */
    /* scr_mode          = 0x1; */
    /* ------------------------------------ 0xa000 */
  } else if ((spd_intf == TEFMOD_SPD_20000_XFI)    ||
             (spd_intf == TEFMOD_SPD_25000_XFI)    ||         
             (spd_intf == TEFMOD_SPD_21200_XFI_HG) ||
             (spd_intf == TEFMOD_SPD_26500_XFI_HG) ||
             (spd_intf == TEFMOD_SPD_25G_CR_IEEE)  ||
             (spd_intf == TEFMOD_SPD_25G_CRS_IEEE) ||
             (spd_intf == TEFMOD_SPD_25G_KR_IEEE)  ||
             (spd_intf == TEFMOD_SPD_25G_KRS_IEEE) ||
             (spd_intf == TEFMOD_SPD_25G_CR1)      ||
             (spd_intf == TEFMOD_SPD_25G_KR1)      ||
             (spd_intf == TEFMOD_SPD_25G_CR1_HG2)  ||
             (spd_intf == TEFMOD_SPD_25G_KR1_HG2)) {
    t_fifo_mode      = 0;
    t_enc_mode       = 1;
    cl49_tx_tl_mode  = 0;
    cl49_bypass_txsm = 0;
    hg2_en           = 0;
    scr_mode         = 1;
    os_mode          = 0;
    t_pma_btmx_mode  = 0;
  } else if ((spd_intf == TEFMOD_SPD_20G_MLD_X2) ||
             (spd_intf == TEFMOD_SPD_21G_MLD_HG_X2)) {
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
  } else if ((spd_intf == TEFMOD_SPD_40G_MLD_X2)      ||
             (spd_intf == TEFMOD_SPD_42G_MLD_HG_X2)   ||
             (spd_intf == TEFMOD_SPD_50G_CR2)         ||
             (spd_intf == TEFMOD_SPD_50G_KR2)         ||
             (spd_intf == TEFMOD_SPD_50G_CR2_HG2)     ||
             (spd_intf == TEFMOD_SPD_50G_KR2_HG2)     ||
             (spd_intf == TEFMOD_SPD_53G_MLD_HG_X2)) {
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
  } else if ((spd_intf == TEFMOD_SPD_40G_MLD_X4) ||
             (spd_intf == TEFMOD_SPD_50G_MLD_X4) ||
             (spd_intf == TEFMOD_SPD_42G_MLD_HG_X4) ||
             (spd_intf == TEFMOD_SPD_53G_MLD_HG_X4)) {
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
  } else if ((spd_intf == TEFMOD_SPD_100G_MLD_X4) ||
             (spd_intf == TEFMOD_SPD_106G_MLD_HG_X4)) {
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
  } else if (spd_intf == TEFMOD_SPD_10000 ) {   /* 10G XAUI */
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;   
    /* ------------------------------------ 0xc113 */
    scr_mode          = 0x0;         /* bypass scramble */
    /* ------------------------------------ 0xa000 */
  } else if (spd_intf == TEFMOD_SPD_10000_HI ) {   /* 10.6G HI XAUI */
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;   
    /* ------------------------------------ 0xc113 */
    scr_mode          = 0x0;         /* bypass scramble */
    /* ------------------------------------ 0xa000 */
  } else if (spd_intf == TEFMOD_SPD_10000_DXGXS) {
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;   
    /* ------------------------------------ 0xc113 */
    scr_mode          = 0x0;         /* bypass scramble */
    /* ------------------------------------ 0xa000 */
  } else if (spd_intf == TEFMOD_SPD_20000) {  /* 20G XAIU */
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;   
    /* ------------------------------------ 0xc113 */
    scr_mode          = 0x0;                 
    /* ------------------------------------ 0xa000 */
  } else if (spd_intf == TEFMOD_SPD_21000) {  /* 20G XAIU */
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;   
    /* ------------------------------------ 0xc113 */
    scr_mode          = 0x2;                 
    /* ------------------------------------ 0xa000 */
  } else if (spd_intf == TEFMOD_SPD_20G_MLD_DXGXS ) {
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 0;
    /* --------------------- ---------------0xc113 */
    scr_mode          = 0x3;         /* 64b66b scrambled */
    /* ------------------------------------ 0xa000 */
  #ifdef TSCF_GEN1
    mld_swap_count = tx_am_timer_init_val;        /* shorten from 0xfffc.  For sim ? */      
  #endif

     /* 0xa000 default value in silicon is for dual port */
  } else if ((spd_intf ==TEFMOD_SPD_21G_HI_MLD_DXGXS)||
             (spd_intf ==TEFMOD_SPD_12773_HI_DXGXS)) {
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

  } else if ((spd_intf == TEFMOD_SPD_20G_DXGXS)||
             (spd_intf == TEFMOD_SPD_21G_HI_DXGXS)) {
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;
    /* ------------------------------------ 0xc113 */
    scr_mode          = 0x1;        /* 64b66b scrambled */
    /* ------------------------------------ 0xa000 */
  } else if ((spd_intf == TEFMOD_SPD_42G_X4)||
             (spd_intf == TEFMOD_SPD_40G_X4)||
             (spd_intf == TEFMOD_SPD_25455) ||
             (spd_intf == TEFMOD_SPD_12773_DXGXS)) {
    /* ------------------------------------ 0xc111 */
    cl49_bypass_txsm  = 1;
    /* ------------------------------------ 0xc113 */
    scr_mode     = 0x1;              /* 64b66b scrambled */
    /* ------------------------------------ 0xa000 */
  } else if (spd_intf == TEFMOD_SPD_40G_XLAUI ) {
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
  } else if (spd_intf == TEFMOD_SPD_42G_XLAUI ) {
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
  if((spd_intf == TEFMOD_SPD_10600_XFI_HG)   ||
     (spd_intf == TEFMOD_SPD_21200_XFI_HG)   ||
     (spd_intf == TEFMOD_SPD_26500_XFI_HG)   ||
     (spd_intf == TEFMOD_SPD_21G_MLD_HG_X2)  ||
     (spd_intf == TEFMOD_SPD_42G_MLD_HG_X4)  ||
     (spd_intf == TEFMOD_SPD_53G_MLD_HG_X4)  ||
     (spd_intf == TEFMOD_SPD_42G_MLD_HG_X2)  ||
     (spd_intf == TEFMOD_SPD_53G_MLD_HG_X2)  ||
     (spd_intf == TEFMOD_SPD_106G_MLD_HG_X4)) {
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

int tefmod_lf_rf_control(PHYMOD_ST* pc)         /* LF_RF_CONTROL */
{
  TEFMOD_DBG_IN_FUNC_INFO(pc);
  return PHYMOD_E_NONE;
}

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
int tefmod_decode_set(PHYMOD_ST* pc, tefmod_spd_intfc_type_t spd_intf)         /* DECODE_SET */
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

  TEFMOD_DBG_IN_FUNC_INFO(pc);
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
    if(spd_intf == TEFMOD_SPD_1000_SGMII) {
      dec_tl_mode     = 0x5;
      deskew_mode     = 0x4;
      dec_fsm_mode    = 0x5;
      hg2_message_ivalid_code_enable = 0x0;
      if (pc->plldiv == 40) {
      } else if (pc->plldiv == 66){
      }
    } else if(spd_intf == TEFMOD_SPD_1G_20G) {
    } else if(spd_intf == TEFMOD_SPD_1G_25G) {
    } else if(spd_intf == TEFMOD_SPD_2500) {
    } else if(spd_intf == TEFMOD_SPD_10000_XFI) {
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
    } else if(spd_intf == TEFMOD_SPD_10600_XFI_HG) {
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
    } else if ((spd_intf == TEFMOD_SPD_20000_XFI) ||
               (spd_intf == TEFMOD_SPD_21200_XFI_HG)      ||
               (spd_intf == TEFMOD_SPD_25000_XFI)         ||
               (spd_intf == TEFMOD_SPD_26500_XFI_HG)) {
      bs_sm_sync_mode = 1;
      bs_sync_en      = 1;
      bs_dist_mode    = 0;
      bs_btmx_mode    = 0;
      deskew_mode     = 5;
      descr_mode      = 1;
      dec_tl_mode     = 1;
      dec_fsm_mode    = 1;
    } else if ((spd_intf == TEFMOD_SPD_20G_MLD_X2) ||
               (spd_intf == TEFMOD_SPD_21G_MLD_HG_X2)) {
      bs_sm_sync_mode = 0;
      bs_sync_en      = 1;
      bs_dist_mode    = 2;
      bs_btmx_mode    = 0;
      deskew_mode     = 1;
      descr_mode      = 2;
      dec_tl_mode     = 2;
      dec_fsm_mode    = 2;
    } else if ((spd_intf == TEFMOD_SPD_40G_MLD_X4) ||
               (spd_intf == TEFMOD_SPD_50G_MLD_X4) ||
               (spd_intf == TEFMOD_SPD_42G_MLD_HG_X4) ||
               (spd_intf == TEFMOD_SPD_53G_MLD_HG_X4)) {
      bs_sm_sync_mode = 0;
      bs_sync_en      = 1;
      bs_dist_mode    = 2;
      bs_btmx_mode    = 0;
      deskew_mode     = 2;
      descr_mode      = 2;
      dec_tl_mode     = 2;
      dec_fsm_mode    = 2;
    } else if ((spd_intf == TEFMOD_SPD_40G_MLD_X2) ||
               (spd_intf == TEFMOD_SPD_42G_MLD_HG_X2)      ||
               (spd_intf == TEFMOD_SPD_50G_MLD_X2)         ||
               (spd_intf == TEFMOD_SPD_53G_MLD_HG_X2)) {
      bs_sm_sync_mode = 0;
      bs_sync_en      = 1;
      bs_dist_mode    = 1;
      bs_btmx_mode    = 1;
      deskew_mode     = 3;
      descr_mode      = 2;
      dec_tl_mode     = 2;
      dec_fsm_mode    = 2;
    } else if ((spd_intf == TEFMOD_SPD_100G_MLD_X4) ||
               (spd_intf == TEFMOD_SPD_106G_MLD_HG_X4)) {
      bs_sm_sync_mode = 0;
      bs_sync_en      = 1;
      bs_dist_mode    = 3;
      bs_btmx_mode    = 2;
      deskew_mode     = 4;
      descr_mode      = 2;
      dec_tl_mode     = 2;
      dec_fsm_mode    = 2;
    } else {
      PHYMOD_DEBUG_ERROR(("%-22s ERROR: p=%0d undefined spd_intf=%0d(%s)\n", __func__, pc->port, spd_intf,
              e2s_tefmod_spd_intfc_type_t[spd_intf])); 
      return PHYMOD_E_FAIL;
    }
    /* HG setting */
    /* if((pc->ctrl_type & TEFMOD_CTRL_TYPE_HG)) {} */
    if((spd_intf == TEFMOD_SPD_10600_XFI_HG)   ||
       (spd_intf == TEFMOD_SPD_21200_XFI_HG)   ||
       (spd_intf == TEFMOD_SPD_26500_XFI_HG)   ||
       (spd_intf == TEFMOD_SPD_21G_MLD_HG_X2)  ||
       (spd_intf == TEFMOD_SPD_42G_MLD_HG_X4)  ||
       (spd_intf == TEFMOD_SPD_53G_MLD_HG_X4)  ||
       (spd_intf == TEFMOD_SPD_42G_MLD_HG_X2)  ||
       (spd_intf == TEFMOD_SPD_53G_MLD_HG_X2)  ||
       (spd_intf == TEFMOD_SPD_106G_MLD_HG_X4)) {
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
    if((spd_intf ==TEFMOD_SPD_10000)||
       (spd_intf ==TEFMOD_SPD_10000_HI)||
       (spd_intf ==TEFMOD_SPD_10000_DXGXS)||
       (spd_intf ==TEFMOD_SPD_20000)){
      data = 0x0444;
      mask = 0xffff;
      sync_code_sm_en = 1;
    } else if ((spd_intf ==TEFMOD_SPD_42G_X4)||
               (spd_intf ==TEFMOD_SPD_40G_X4)||
               (spd_intf ==TEFMOD_SPD_25455)||
               (spd_intf ==TEFMOD_SPD_21G_HI_DXGXS)||
               (spd_intf ==TEFMOD_SPD_20G_DXGXS)||
               (spd_intf == TEFMOD_SPD_12773_DXGXS) ) {
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
    if((spd_intf == TEFMOD_SPD_20G_MLD_DXGXS) ||
       (spd_intf == TEFMOD_SPD_21G_HI_MLD_DXGXS) ||
       (spd_intf == TEFMOD_SPD_40G_XLAUI) || 
       (spd_intf == TEFMOD_SPD_42G_XLAUI) ||
       (spd_intf == TEFMOD_SPD_12773_HI_DXGXS) ) {
      /*data = 0; */
      /*PHYMOD_IF_ERR_RETURN (MODIFY_RX_X4_CONTROL0_FEC_0r(pc->unit, pc, data, mask));*/ 
    }
    /* --Addr=0x9123 ------------------------- */
    /* for simualtion                          */
/* #if defined (_DV_TRIDENT2) */

/*    #ifdef TSCF_GEN1  */
/*    CL82_RX_AM_TMRr_CLR(CL82_SHARED_CL82_RX_AM_TIMERr_reg);*/
/*    #else*/
    CL82_AM_TMRr_CLR(CL82_SHARED_CL82_AM_TIMERr_reg);
/*    #endif */

    /*ABHIif((spd_intf == TEFMOD_SPD_40G_XLAUI)||(spd_intf == TEFMOD_SPD_42G_XLAUI)) {
      data = pc->rx_am_timer_init_val;
      mask = 0xffff;
      CL82_RX_AM_TMRr_SET(CL82_SHARED_CL82_RX_AM_TIMERr_reg, pc->rx_am_timer_init_val);
      PHYMOD_IF_ERR_RETURN 
            (MODIFY_CL82_RX_AM_TMRr(pc, CL82_SHARED_CL82_RX_AM_TIMERr_reg));
    }*/
    if((spd_intf == TEFMOD_SPD_20G_MLD_DXGXS) ||
       (spd_intf == TEFMOD_SPD_20G_MLD_X2)    ||
       (spd_intf == TEFMOD_SPD_21G_MLD_HG_X2) ||
       (spd_intf == TEFMOD_SPD_40G_MLD_X4)    ||
       (spd_intf == TEFMOD_SPD_50G_MLD_X4)    ||
       (spd_intf == TEFMOD_SPD_40G_MLD_X2)    ||
       (spd_intf == TEFMOD_SPD_42G_MLD_HG_X2) ||
       (spd_intf == TEFMOD_SPD_50G_MLD_X2)    ||
       (spd_intf == TEFMOD_SPD_100G_MLD_X4)   ||
       (spd_intf == TEFMOD_SPD_53G_MLD_HG_X2) ||
       (spd_intf == TEFMOD_SPD_42G_MLD_HG_X4) ||
       (spd_intf == TEFMOD_SPD_53G_MLD_HG_X4) ||
       (spd_intf == TEFMOD_SPD_106G_MLD_HG_X4)||
       (spd_intf == TEFMOD_SPD_40G_XLAUI)     ||
       (spd_intf == TEFMOD_SPD_42G_XLAUI))  {
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
    if((spd_intf == TEFMOD_SPD_20G_MLD_DXGXS)) {
      CL82_LN_0_AM_BYTE10r_CLR(CL82_AMS_LANE_0_AM_BYTE10r_reg);
      CL82_LN_0_AM_BYTE10r_SET(CL82_AMS_LANE_0_AM_BYTE10r_reg, 0x7690);
      PHYMOD_IF_ERR_RETURN 
        (MODIFY_CL82_LN_0_AM_BYTE10r(pc, CL82_AMS_LANE_0_AM_BYTE10r_reg));
    }
    /* --Addr=0x9131 ------------------------- */
    if((spd_intf == TEFMOD_SPD_20G_MLD_DXGXS)) {
      CL82_LN_1_AM_BYTE10r_CLR(CL82_AMS_LANE_1_AM_BYTE10r_reg);
      CL82_LN_1_AM_BYTE10r_SET(CL82_AMS_LANE_1_AM_BYTE10r_reg, 0xc4f0);
      PHYMOD_IF_ERR_RETURN 
        (MODIFY_CL82_LN_1_AM_BYTE10r(pc, CL82_AMS_LANE_1_AM_BYTE10r_reg));
    }
    /* --Addr=0x9132 ------------------------- */
    if((spd_intf == TEFMOD_SPD_20G_MLD_DXGXS)) {
      CL82_LANES_1_0_AM_BYTE2r_CLR(CL82_AMS_LANES_1_0_AM_BYTE2r_reg);
      CL82_LANES_1_0_AM_BYTE2r_SET(CL82_AMS_LANES_1_0_AM_BYTE2r_reg, 0xe647);
      PHYMOD_IF_ERR_RETURN 
        (MODIFY_CL82_LANES_1_0_AM_BYTE2r(pc, CL82_AMS_LANES_1_0_AM_BYTE2r_reg));
    }
    
    if(pc->ctrl_type & TEFMOD_CTRL_TYPE_FAULT_DIS) {
      pc->per_lane_control = TEFMOD_FAULT_CL49 | TEFMOD_FAULT_CL48 | TEFMOD_FAULT_CL82;
      /* li, lf, rf =0 */
      tefmod_lf_rf_control(pc); 
    } else {
      pc->per_lane_control = 
         (TEFMOD_FAULT_RF | TEFMOD_FAULT_LF | TEFMOD_FAULT_LI)<<4 |
         (TEFMOD_FAULT_CL49 | TEFMOD_FAULT_CL48 | TEFMOD_FAULT_CL82);
       tefmod_lf_rf_control(pc); 
    }
  } else {  /* if ~cntl */
    /* no op */
    return PHYMOD_E_NONE;
  }
  return PHYMOD_E_NONE;
}

/*!
@brief set broadcast enable and broadcast address

@param  pc handle to current TSCF context (#PHYMOD_ST)

@returns The value PHYMOD_E_NONE upon successful completion.
*/

int tefmod_bypass_sc(PHYMOD_ST* pc)         /* BYPASS_SC */
{
  uint16_t bypass_en, fild_override_enable0, fild_override_enable1;
  SC_X4_BYPASSr_t SC_X4_CONTROL_BYPASSr_reg;
  SC_X4_FLD_OVRR_EN0_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg;
  SC_X4_FLD_OVRR_EN1_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg;

  TEFMOD_DBG_IN_FUNC_INFO(pc);
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

int tefmod_disable_pcs_falcon(PHYMOD_ST* pc)
{
  SC_X4_CTLr_t SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg;

  TEFMOD_DBG_IN_FUNC_INFO(pc);
  SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, 0);

  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

  return PHYMOD_E_NONE;
}

/*!
@brief removes resets from all the PMD lanes
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details
Removes resets
*/

int tefmod_pmd_reset_remove(PHYMOD_ST *pc, int pmd_touched)
{
  PMD_X1_CTLr_t PMD_X1_PMD_X1_CONTROLr_reg;
  PMD_X4_CTLr_t PMD_X4_PMD_X4_CONTROLr_reg;

  TEFMOD_DBG_IN_FUNC_INFO(pc);
  
  if(pmd_touched == 0) {
    PMD_X1_CTLr_CLR(PMD_X1_PMD_X1_CONTROLr_reg);
    /* setup the PMD reset */
    PMD_X1_CTLr_POR_H_RSTBf_SET(PMD_X1_PMD_X1_CONTROLr_reg, 1);
    PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(PMD_X1_PMD_X1_CONTROLr_reg, 1);

    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_CTLr(pc, PMD_X1_PMD_X1_CONTROLr_reg));
  }

  
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
 
  TEFMOD_DBG_IN_FUNC_INFO(pc);

  pc->speed = st_entry_speed;
  /* st_entry_num can be 0, 1, 2, or 3 */
  #ifdef _DV_TB_
  PHYMOD_DEBUG_ERROR(("Using speed_id: %x and st_entry_num: %x", st_entry_speed, st_entry_num));
  #endif

  if (st_entry_num == 0) {
    SC_X1_SPD_OVRR0_SPDr_CLR(SC_X1_SPEED_OVERRIDE0_OVERRIDE_SPEEDr_reg);
    SC_X1_SPD_OVRR0_SPDr_SPEEDf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_SPEEDr_reg, st_entry_speed);

    SC_X1_SPD_OVRR0_SPDr_NUM_LANESf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_SPEEDr_reg, (pc->num_lanes));

    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR0_SPDr(pc, SC_X1_SPEED_OVERRIDE0_OVERRIDE_SPEEDr_reg));


    SC_X1_SPD_OVRR0_0r_CLR(SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg);
    SC_X1_SPD_OVRR0_0r_CL72_ENf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg, pc->cl72_en); 
    SC_X1_SPD_OVRR0_0r_OS_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg, pc->os_mode);
    SC_X1_SPD_OVRR0_0r_T_FIFO_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg, pc->t_fifo_mode); 
    SC_X1_SPD_OVRR0_0r_T_ENC_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg, pc->t_enc_mode); 
    SC_X1_SPD_OVRR0_0r_T_HG2_ENABLEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg, pc->t_HG2_ENABLE); 
    SC_X1_SPD_OVRR0_0r_T_PMA_BTMX_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg, pc->t_pma_btmx_mode); 
    SC_X1_SPD_OVRR0_0r_SCR_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg, pc->scr_mode);

    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X1_SPD_OVRR0_0r(pc, SC_X1_SPEED_OVERRIDE0_OVERRIDE_0r_reg));

    SC_X1_SPD_OVRR0_1r_CLR(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg);
    SC_X1_SPD_OVRR0_1r_DESCR_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->descr_mode));

    SC_X1_SPD_OVRR0_1r_DEC_TL_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->dec_tl_mode));

    SC_X1_SPD_OVRR0_1r_DESKEW_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->deskew_mode));

    SC_X1_SPD_OVRR0_1r_DEC_FSM_MODEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->dec_fsm_mode));

    #ifdef TSCF_GEN1
    SC_X1_SPD_OVRR0_1r_R_HG2_ENABLEf_SET(SC_X1_SPEED_OVERRIDE0_OVERRIDE_1r_reg, (pc->r_HG2_ENABLE));
    #endif

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
    SC_X1_SPD_OVRR1_0r_CL72_ENf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg, pc->cl72_en); 
    SC_X1_SPD_OVRR1_0r_OS_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg, pc->os_mode);
    SC_X1_SPD_OVRR1_0r_T_FIFO_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg, pc->t_fifo_mode); 
    SC_X1_SPD_OVRR1_0r_T_ENC_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg, pc->t_enc_mode); 
    SC_X1_SPD_OVRR1_0r_T_HG2_ENABLEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg, pc->t_HG2_ENABLE); 
    SC_X1_SPD_OVRR1_0r_T_PMA_BTMX_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg, pc->t_pma_btmx_mode); 
    SC_X1_SPD_OVRR1_0r_SCR_MODEf_SET(SC_X1_SPEED_OVERRIDE1_OVERRIDE_0r_reg, pc->scr_mode);

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

int tefmod_set_an_override(PHYMOD_ST* pc)
{
  TEFMOD_DBG_IN_FUNC_INFO(pc);



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

/*!
@brief tefmod_init_pcs_an , program PCS for AN
@returns The value PHYMOD_E_NONE upon successful completion.
@details
If any override option switch is selected, it will call tier2 tefmod_set_an_override
This is a tier2 function, whenever something from Hard table needs to be overwritten
calls the other autoneg routines to perform AN.
Sets AN port mode 
Controls the setting/resetting of autoneg advertisement registers.
tefmod_autoneg_control starts the AN
*/
int tefmod_init_pcs_an(PHYMOD_ST* pc)
{
  TEFMOD_DBG_IN_FUNC_INFO(pc);
  if(pc->sc_mode == TEFMOD_SC_MODE_AN_CL73) {
    tefmod_set_an_override(pc);
  }
  tefmod_set_an_port_mode(pc);
  tefmod_autoneg_set(pc);
  tefmod_autoneg_control(pc);
  return PHYMOD_E_NONE;
}

/*!
@brief tefmod_set_sc_speed , sets the required speed ,  
@returns The value PHYMOD_E_NONE upon successful completion.
@details
After the resets, speed_ctrl regs are programmed(as per Ankits new init seq), fills up the lookup table by
Updating the expected value
*/
int tefmod_set_sc_speed(PHYMOD_ST* pc, int set_sw_speed_change, tefmod_spd_intfc_type_t spd_intf)  /* SET_SC_SPEED */
{
  int mapped_speed;

  SC_X4_CTLr_t SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg;
  TX_X4_ENC0r_t TX_X4_ENC0r_reg;
  RX_X4_PCS_CTL0r_t RX_X4_PCS_CTL0r_reg; 


  TEFMOD_DBG_IN_FUNC_INFO(pc);
  /* write 0 to the speed change */
  SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);

/*  PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc, &SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg)); */

  if(set_sw_speed_change) {
    SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, 0);
    PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));
  }
  
   /* Set speed and write 1 */
  SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);

  
  SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, set_sw_speed_change);

  tefmod_get_mapped_speed(spd_intf, &mapped_speed);
  if(pc->sc_mode == TEFMOD_SC_MODE_HT_OVERRIDE || pc->sc_mode == TEFMOD_SC_MODE_ST_OVERRIDE) { mapped_speed = 2;
  }
  SC_X4_CTLr_SPEEDf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, mapped_speed);
  PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

  /*  Update the speed */
  pc->speed= mapped_speed;

    /*  Do lookup here */
  /* redundant tefmod_fill_lkup_exp(pc); */
  tefmod_get_ht_entries(pc);

  if(pc->fc_mode == 1) {
    TX_X4_ENC0r_CLR(TX_X4_ENC0r_reg);
    TX_X4_ENC0r_BLOCK_NON_FC_BLK_TYPESf_SET(TX_X4_ENC0r_reg, 1);
    PHYMOD_IF_ERR_RETURN( MODIFY_TX_X4_ENC0r (pc, TX_X4_ENC0r_reg));

    RX_X4_PCS_CTL0r_CLR(RX_X4_PCS_CTL0r_reg);
    RX_X4_PCS_CTL0r_BLOCK_NON_FC_BLK_TYPESf_SET(RX_X4_PCS_CTL0r_reg, 1);
    PHYMOD_IF_ERR_RETURN( MODIFY_RX_X4_PCS_CTL0r (pc, RX_X4_PCS_CTL0r_reg));
  } 

  return PHYMOD_E_NONE;
}

/*!
@brief tefmod_init_pcs_fs , program PCS for Forced speed mode
@returns The value PHYMOD_E_NONE upon successful completion.
@details
If any SW override option switch is selected, it will call tier2 which for eg will convert from 10G to 20G XFI
Using HT, we do overrides of HT entries for eg will convert from 10G to 40G MLD
If none of the override options are selected, we just program the SC table
*/

int tefmod_init_pcs_fs(PHYMOD_ST* pc)
{
/*  int or_value;*/
  int st_entry;
  int speed_entry;
/*  SC_X4_FLD_OVRR_EN0_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg;*/
/*  SC_X4_FLD_OVRR_EN1_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg;*/
/*  SC_X4_SC_X4_OVRRr_t  SC_X4_SC_X4_OVRRr_reg;*/
/*  TX_X2_CL82_0r_t TX_X2_CONTROL0_CL82_0r_reg;*/
/*  TX_X4_ENC0r_t TX_X4_CONTROL0_ENCODE_0r_reg;*/
/*  TX_X4_MISCr_t TX_X4_CONTROL0_MISCr_reg;*/
/*  RX_X4_PCS_CTL0r_t RX_X4_CONTROL0_PCS_CONTROL_0r_reg;*/
/*  RX_X4_BLKSYNC_CFGr_t RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg;*/
/*  TX_X4_LOOPCNTr_t TX_X4_CREDIT0_LOOPCNTr_reg;*/
/*  TX_X4_CRED1r_t TX_X4_CREDIT0_CREDIT1r_reg;*/
/*  TX_X4_MAC_CREDGENCNTr_t TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg;*/
/*  SC_X4_FLD_OVRR_EN2_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg;*/
/*  RX_X4_CL91_RX_CTL0r_t RX_X4_CL91_RX_CTL0r_reg;*/
/**/
/*  TEFMOD_DBG_IN_FUNC_INFO(pc);*/
/*  if(pc->sc_mode == TEFMOD_SC_MODE_ST_OVERRIDE) {*/
/**/
    /*Set the override enable*/
    /*override from 10G XFI to 20G XFI */
/*    configure_st_entry(pc->sw_table_entry, pc->speed,  pc);*/
/*  }*/
/*  */
/*  if(pc->fc_mode) {*/
    /*write to address c062 and c133 Temp fix for override*/
/**/
/*    printf("PER_FIELD_OR is %x\n", pc->per_field_override_en);*/
    /*Set the override enable*/
/**/
/*    if(pc->per_field_override_en == 0x88000001) {*/
/*      SC_X4_FLD_OVRR_EN2_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg);*/
/*      SC_X4_FLD_OVRR_EN2_TYPEr_R_CL91_CW_SCRAMBLE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg, 1);*/
/*      PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN2_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg));*/
/**/
/*      RX_X4_CL91_RX_CTL0r_CLR(RX_X4_CL91_RX_CTL0r_reg);*/
/*      RX_X4_CL91_RX_CTL0r_CL91_CW_SCRAMBLEf_SET(RX_X4_CL91_RX_CTL0r_reg, 0);*/
/*      PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_CL91_RX_CTL0r(pc, RX_X4_CL91_RX_CTL0r_reg));*/
/**/
/*      SC_X4_FLD_OVRR_EN2_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg);*/
/*      SC_X4_FLD_OVRR_EN2_TYPEr_R_FIVE_BIT_XOR_EN_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg, 1);*/
/*      PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN2_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE2_TYPEr_reg));*/
/**/
/*      RX_X4_CL91_RX_CTL0r_CLR(RX_X4_CL91_RX_CTL0r_reg);*/
/*      RX_X4_CL91_RX_CTL0r_FIVE_BIT_XOR_ENf_SET(RX_X4_CL91_RX_CTL0r_reg, 1);*/
/*      PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_CL91_RX_CTL0r(pc, RX_X4_CL91_RX_CTL0r_reg));*/
/*    }*/
/*  }*/
/**/
/*  if(pc->sc_mode == TEFMOD_SC_MODE_HT_OVERRIDE) {*/
/**/
    /*override from 10G XFI to 40G MLD */
/**/
/*    SC_X4_SC_X4_OVRRr_CLR(SC_X4_SC_X4_OVRRr_reg); */
/*    SC_X4_SC_X4_OVRRr_NUM_LANES_OVERRIDE_VALUEf_SET( SC_X4_SC_X4_OVRRr_reg, 0);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_SC_X4_OVRRr(pc, SC_X4_SC_X4_OVRRr_reg));*/
/*      */
/*    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_NUM_LANES_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));*/
/**/
    /*write t_fifo_mode */
/*    or_value = 1;*/
/**/
/*    TX_X2_CL82_0r_CLR(TX_X2_CONTROL0_CL82_0r_reg);*/
/*    #ifdef TSCF_GEN1*/
/*    TX_X2_CL82_0r_T_FIFO_MODEf_SET(TX_X2_CONTROL0_CL82_0r_reg, or_value);*/
/*    #else*/
/*    TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);*/
/*    TX_X4_ENC0r_T_FIFO_MODEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, or_value);*/
/*    #endif*/
/**/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X2_CL82_0r(pc, TX_X2_CONTROL0_CL82_0r_reg));*/
/**/
    /*Set the override enable*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_T_FIFO_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));*/
/**/
     /*write t_enc_mode */
/*    or_value = 2;*/
/*    TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);*/
/*    TX_X4_ENC0r_T_ENC_MODEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, or_value);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc, TX_X4_CONTROL0_ENCODE_0r_reg));*/
/**/
    /*Set the override enable*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_T_ENC_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));*/
/**/
/**/
    /* scrambler mode*/
/*    or_value = 5;*/
/*    TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);*/
/*    TX_X4_MISCr_SCR_MODEf_SET(TX_X4_CONTROL0_MISCr_reg, or_value);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));*/
/**/
    /*Set the override enable*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_SCR_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));*/
/**/
    /*descrambler mode*/
/*    or_value = 2;*/
/*    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);*/
/*    RX_X4_PCS_CTL0r_DESCR_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));*/
/**/
    /*Set the override enable*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_DESCR_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));*/
/**/
    /*dec_tl_mode*/
/*    or_value = 2;*/
/*    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);*/
/*    RX_X4_PCS_CTL0r_DEC_TL_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));*/
/**/
    /*Set the override enable*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_DEC_TL_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));*/
/**/
    /*desjkew_mode*/
/*    or_value = 2;*/
/*    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);*/
/*    RX_X4_PCS_CTL0r_DESKEW_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));*/
    /*Set the override enable*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_DESKEW_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));*/
/**/
    /*dec_fsm_mode*/
    /*Set the override enable*/
/*    or_value = 2;*/
/*    RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);*/
/*    RX_X4_PCS_CTL0r_DEC_FSM_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN0_TYPEr_DEC_FSM_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));*/
/**/
    /*bs_sm_sync_mode*/
    /*Set the override enable*/
/*    or_value = 0;*/
/*    RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);*/
/*    RX_X4_BLKSYNC_CFGr_BS_SM_SYNC_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, or_value);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));*/
/*    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN1_TYPEr_BS_SM_SYNC_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));*/
/**/
    /*bs_dist_mode*/
    /*Set the override enable*/
/*    or_value = 2;*/
/*    RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);*/
/*    RX_X4_BLKSYNC_CFGr_BS_DIST_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, or_value);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));*/
/*    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN1_TYPEr_BS_DIST_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));*/
/**/
      /*  Program resets and credits */
    /*Set the override enable*/
/*    or_value = 18;*/
/*    TX_X4_LOOPCNTr_CLR(TX_X4_CREDIT0_LOOPCNTr_reg);*/
/*    TX_X4_LOOPCNTr_LOOPCNT1f_SET(TX_X4_CREDIT0_LOOPCNTr_reg, or_value);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_LOOPCNTr(pc, TX_X4_CREDIT0_LOOPCNTr_reg));*/
/*    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT1_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));*/
/**/
/*    or_value = 5;*/
/*    TX_X4_CRED1r_CLR(TX_X4_CREDIT0_CREDIT1r_reg);*/
/*    TX_X4_CRED1r_CLOCKCNT1f_SET(TX_X4_CREDIT0_CREDIT1r_reg, or_value);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED1r(pc, TX_X4_CREDIT0_CREDIT1r_reg));*/
/*    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT1_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));*/
/**/
/*    or_value = 2;*/
/*    TX_X4_MAC_CREDGENCNTr_CLR(TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg);*/
/*    TX_X4_MAC_CREDGENCNTr_MAC_CREDITGENCNTf_SET(TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg, or_value);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MAC_CREDGENCNTr(pc, TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg));*/
/*    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);*/
/*    SC_X4_FLD_OVRR_EN1_TYPEr_MAC_CREDITGENCNT_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));*/
/*      */
/*  }*/
  _tefmod_sw_override_to_ht_entry(pc, pc->fc_mode, pc->per_field_override_en, pc->sc_mode);

  if(pc->sc_mode == TEFMOD_SC_MODE_ST) {
    st_entry = pc->sw_table_entry & 0xff;
    speed_entry = (pc->sw_table_entry & 0xff00) >> 8;
    configure_st_entry(st_entry, speed_entry, pc);
  }
#ifdef _DV_TB_
  pc->adjust_port_mode = 1;
#endif
  tefmod_set_sc_speed(pc, 1, pc->spd_intf);
#ifdef _DV_TB_
  pc->adjust_port_mode = 0;
#endif
  return PHYMOD_E_NONE;
}

/* RACHANA: This is a tier2 function */
int tefmod_init_pcs_falcon(PHYMOD_ST* pc)
{
  /* Set_port_mode is common to both Autoneg and Forced_speed mode */
  int    bypass_int_cor;
  int    num_lane;

/*  PMD_X4_CTLr_t PMD_X4_PMD_X4_CONTROLr_reg;*/
/*  #ifdef TSCF_GEN1*/
/*  TX_X1_T_PMA_WTRMKr_t TX_X1_CONTROL0_T_PMA_WATERMARKr_reg;*/
/*  CL82_RX_AM_TMRr_t CL82_SHARED_CL82_RX_AM_TIMERr_reg;*/
/*  TX_X2_MLD_SWP_CNTr_t TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg;*/
/*  #else*/
/*  CL82_AM_TMRr_t CL82_SHARED_CL82_AM_TIMERr_reg;*/
/*  #endif*/
/*  RX_X1_CL91_CFGr_t RX_X1_CONTROL0_CL91_CONFIGr_reg;*/
/*  RX_X4_DEC_CTL0r_t RX_X4_CONTROL0_DECODE_CONTROL_0r_reg;*/
/*  TX_X4_ENC0r_t TX_X4_CONTROL0_ENCODE_0r_reg;*/
/*  SC_X4_FLD_OVRR_EN0_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg;*/
/*  SC_X4_FLD_OVRR_EN1_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg;*/
/*  AN_X4_INT_ENr_t AN_X4_INT_ENr_reg;*/
/*  MAIN0_SPD_CTLr_t MAIN0_SPD_CTLr_reg;*/
/*  TX_X4_TX_CTL0r_t TX_X4_TX_CTL0r_reg; */
/**/
/*  SC_X4_MSA_25G_50G_CTLr_t              SC_X4_MSA_25G_50G_CTLr_reg;*/
/*  SC_X4_IEEE_25G_CTLr_t                 SC_X4_IEEE_25G_CTLr_reg;*/
/*  RX_X4_IEEE_25G_PARLLEL_DET_CTRr_t     RX_X4_IEEE_25G_PARLLEL_DET_CTRr_reg;*/

  TEFMOD_DBG_IN_FUNC_INFO(pc);

  if(pc->tsc_touched == 0 && pc->pcs_lane_swap_en) {
    tefmod_pcs_rx_lane_swap(pc, pc->pcs_log_to_phy_rx_lane_swap);
    tefmod_pcs_tx_lane_swap(pc, pc->pcs_log_to_phy_tx_lane_swap);
  }
 
/*  if(pc->tsc_touched == 0) {*/
/*    MAIN0_SPD_CTLr_CLR(MAIN0_SPD_CTLr_reg);*/
/*    MAIN0_SPD_CTLr_TSC_CREDIT_SELf_SET(MAIN0_SPD_CTLr_reg, pc->credit_sel);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SPD_CTLr(pc, MAIN0_SPD_CTLr_reg));*/
/*    #ifdef _DV_TB_*/
/*    PHYMOD_DEBUG_ERROR(("FCSN_Info : tefmod_init_pcs_falcon : fc_mode %0d : tsc_credit_sel %0d\n", pc->fc_mode, pc->credit_sel));*/
/*    #endif*/
/*  }*/
   _tefmod_credit_sel(pc, pc->fc_mode, pc->tsc_touched, pc->credit_sel);

/*  if ((pc->spd_intf == TEFMOD_SPD_100G_MLD_X4) ||*/
/*      (pc->spd_intf == TEFMOD_SPD_106G_MLD_HG_X4)) {*/
/*    TX_X4_TX_CTL0r_CLR(TX_X4_TX_CTL0r_reg);*/
/*    TX_X4_TX_CTL0r_T_PMA_WATERMARKf_SET( TX_X4_TX_CTL0r_reg , 7);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_TX_CTL0r(pc,TX_X4_TX_CTL0r_reg)); */
/**/
/*    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg); */
/*    SC_X4_FLD_OVRR_EN1_TYPEr_T_PMA_WATERMARK_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc,SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));*/
/*   */
/*  }*/
/*  else {*/
/*    SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg); */
/*    SC_X4_FLD_OVRR_EN1_TYPEr_T_PMA_WATERMARK_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc,SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));*/
/*  } */
  _tefmod_pma_watermark_set(pc, pc->spd_intf);

  switch(pc->port_type) {
    case TEFMOD_MULTI_PORT: num_lane = 1; break;
    case TEFMOD_TRI1_PORT: if(pc->this_lane == 2) num_lane = 2;
                           else num_lane = 1;
                           break;
    case TEFMOD_TRI2_PORT: if(pc->this_lane == 0) num_lane = 2;
                           else num_lane = 1;
                           break;
    case TEFMOD_DXGXS: num_lane = 2; break;
    case TEFMOD_SINGLE_PORT: num_lane = 4; break;
    default : num_lane = 0;
  }

  if(pc->pcs_gloop_en != 0) {
    tefmod_tx_loopback_control(pc, pc->pcs_gloop_en, pc->this_lane, num_lane);
  }

  if(pc->pcs_rloop_en != 0) {
    tefmod_rx_loopback_control(pc, pc->pcs_rloop_en, pc->this_lane, num_lane);
  }

  /* FCSN */
  if(pc->fc_mode) {
    #ifdef _DV_TB_
    PHYMOD_DEBUG_ERROR(("FCSN_Info : tefmod_init_pcs_falcon : port %0d : fc_mode %0d : tx_fcsn_mode %02x : rx_fcsn_mode %02x\n", pc->this_lane, pc->fc_mode, pc->tx_fcsn_mode, pc->rx_fcsn_mode));
    #endif

    if(pc->pmd_touched == 0) {
      pc->pmd_reset_control = 0x1;
      tefmod_pmd_core_reset_bypass(pc, pc->pmd_reset_control);
    }
    pc->pmd_reset_control = 0x10;
    tefmod_pmd_lane_reset_bypass(pc, pc->pmd_reset_control);
  } 

  if(pc->pmd_rloop_en != 0) {
    tefmod_rx_pmd_loopback_control(pc, pc->pcs_rloop_en, pc->pmd_rloop_en, pc->this_lane);
  }
  tefmod_update_port_mode_select(pc, pc->port_type, pc->this_lane, pc->tsc_clk_freq_pll_by_48, pc->pll_reset_en);
  pc->per_lane_control = 3;
 
/*  PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);*/
/**/
/*  PMD_X4_CTLr_LN_RX_H_PWRDNf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 0);*/
/*  PMD_X4_CTLr_LN_TX_H_PWRDNf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 0);*/
/*  */
/*  PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));*/
/*  */
/*  PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);*/
/*  PMD_X4_CTLr_LN_H_RSTBf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 1);*/
/*  PMD_X4_CTLr_LN_DP_H_RSTBf_SET(PMD_X4_PMD_X4_CONTROLr_reg, 1);*/
/**/
/*  PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));*/
  tefmod_power_control(pc, 0, 0);
  _tefmod_pcs_reset_control(pc, 1, 1);

  /* RACHANA: not needed. */
/*  #ifdef TSCF_GEN1*/
/*  TX_X1_T_PMA_WTRMKr_CLR(TX_X1_CONTROL0_T_PMA_WATERMARKr_reg);*/
/*  TX_X1_T_PMA_WTRMKr_FIVE_LANE_BITMUX_WATERMARKf_SET(TX_X1_CONTROL0_T_PMA_WATERMARKr_reg, 7);*/
/*  TX_X1_T_PMA_WTRMKr_TWO_LANE_BITMUX_WATERMARKf_SET(TX_X1_CONTROL0_T_PMA_WATERMARKr_reg, 5);*/
/*  TX_X1_T_PMA_WTRMKr_SINGLE_LANE_BITMUX_WATERMARKf_SET(TX_X1_CONTROL0_T_PMA_WATERMARKr_reg, 4);*/
/**/
/*  PHYMOD_IF_ERR_RETURN(MODIFY_TX_X1_T_PMA_WTRMKr(pc, TX_X1_CONTROL0_T_PMA_WATERMARKr_reg));*/
/*  #endif*/

  if(pc->cl91_en != 0 && pc->an_en != 1) {
    bypass_int_cor = 0;
    if(pc->cl91_bypass_indication) {
      bypass_int_cor |= TEFMOD_CL91_IND_ONLY_EN_DIS;
    }
    if(pc->cl91_bypass_correction) {
      bypass_int_cor |= TEFMOD_CL91_COR_ONLY_EN_DIS;
    }
    tefmod_FEC_control(pc, (TEFMOD_CL91_TX_EN_DIS|TEFMOD_CL91_RX_EN_DIS|bypass_int_cor), 0, 0, pc->spd_intf);
  } else if(pc->cl91_en == 0 && pc->an_en != 1) {
    tefmod_FEC_control(pc, 0, (TEFMOD_CL91_TX_EN_DIS|TEFMOD_CL91_RX_EN_DIS), 0, pc->spd_intf);
  }

  if(pc->cl91_nofec == 1) {
    tefmod_FEC_control(pc, TEFMOD_CL74_CL91_EN_DIS, 0, 0, pc->spd_intf);
  }
  if(pc->cl91_cl74 == 1) {
    tefmod_FEC_control(pc, TEFMOD_CL74_CL91_EN_DIS, 0, 1, pc->spd_intf);
  }
  
  tefmod_tx_lane_control(pc, pc->per_lane_control&1, 0);
  tefmod_rx_lane_control(pc, pc->accData, pc->per_lane_control);


/*  RX_X1_CL91_CFGr_CLR(RX_X1_CONTROL0_CL91_CONFIGr_reg);*/
/*  if(pc->rx_am_timer_init_val == 0x400) {*/
/*    printf("CL91_AM_SPACING_1024 set.\n");*/
/*    RX_X1_CL91_CFGr_CL91_AM_SPACING_1024f_SET(RX_X1_CONTROL0_CL91_CONFIGr_reg, 1);*/
/*  }*/
/**/
/*  #ifdef TSCF_G3*/
/*  printf("CL91_FC_LOCK_CORR_CW clr.\n");*/
/*  RX_X1_CL91_CFGr_CL91_FC_LOCK_CORR_CWf_SET(RX_X1_CONTROL0_CL91_CONFIGr_reg, 0);*/
/*  PHYMOD_IF_ERR_RETURN(MODIFY_RX_X1_CL91_CFGr(pc, RX_X1_CONTROL0_CL91_CONFIGr_reg));*/
/*  #endif*/
  _tefmod_cl91_dv_init(pc, pc->rx_am_timer_init_val);
  
  tefmod_cl74_control_fec_0(pc);
  tefmod_cl74_control_fec_1(pc);
  tefmod_cl74_control_fec_2(pc);
  tefmod_cl74_control_fec_3(pc);

  /* cfg_rx_am_timer_init_val=3ff cfg_tx_am_timer_init_val=ffc */
/*  #ifdef TSCF_GEN1*/
/*  CL82_RX_AM_TMRr_CLR(CL82_SHARED_CL82_RX_AM_TIMERr_reg);*/
/*  TX_X2_MLD_SWP_CNTr_CLR(TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg);*/
/**/
/*  */
/*  if(pc->rx_am_timer_init_val != 0x3fff) {*/
/*    CL82_RX_AM_TMRr_SET(CL82_SHARED_CL82_RX_AM_TIMERr_reg, pc->rx_am_timer_init_val);*/
/*    PHYMOD_IF_ERR_RETURN(WRITE_CL82_RX_AM_TMRr(pc, CL82_SHARED_CL82_RX_AM_TIMERr_reg));*/
/*  }*/
/*  if(pc->tx_am_timer_init_val != 0xffff) {*/
/*    TX_X2_MLD_SWP_CNTr_SET(TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg, pc->tx_am_timer_init_val);*/
/*    PHYMOD_IF_ERR_RETURN(WRITE_TX_X2_MLD_SWP_CNTr(pc, TX_X2_CONTROL0_MLD_SWAP_COUNTr_reg));*/
/*  }*/
/*  #else*/
/*  CL82_AM_TMRr_CLR(CL82_SHARED_CL82_AM_TIMERr_reg);*/
/*  if(pc->rx_am_timer_init_val != 0x3fff) {*/
/*    CL82_AM_TMRr_SET(CL82_SHARED_CL82_AM_TIMERr_reg, pc->rx_am_timer_init_val);*/
/*    PHYMOD_IF_ERR_RETURN(WRITE_CL82_AM_TMRr(pc, CL82_SHARED_CL82_AM_TIMERr_reg));*/
/*  }*/
/**/
/*  #endif*/
   _tefmod_cl82_am_timer_init(pc, pc->rx_am_timer_init_val);

  /* +cfg_rx_higig2_oen +cfg_tx_higig2_oen */
    /* Set t_HG2_ENABLE_oen bit 10 in 0xc060 */
/*   SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);*/
/*   SC_X4_FLD_OVRR_EN0_TYPEr_T_HG2_ENABLE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, pc->tx_higig2_oen & 0x1);*/
/*   PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));*/
/**/
   /* Clr/Set HG2_ENABLE bit 10 in 0xc111 */
/*   TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);*/
/*   TX_X4_ENC0r_HG2_ENABLEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, pc->tx_higig2_en);*/
/*   PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc,TX_X4_CONTROL0_ENCODE_0r_reg));*/
/**/
  /* +cfg_tefmod_use_higig2_knobs +cfg_tx_higig2_codec */
/*  if(pc->tefmod_use_higig2_knobs == 0x1) {*/
/*    TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);*/
/*    TX_X4_ENC0r_HG2_CODECf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, pc->tx_higig2_codec);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc,TX_X4_CONTROL0_ENCODE_0r_reg));*/
/*  }*/
/**/
   /*CHAK setting tx scramble idle test mode*/
/*   */
/*   if(pc->scramble_idle_tx_test_mode==0x1) {*/
/*     printf("CHAK:eabling tx testmode cfg\n");*/
/*     TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);*/
/*     TX_X4_ENC0r_TX_TEST_MODE_CFGf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, pc->scramble_idle_tx_test_mode);*/
/*     PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc,TX_X4_CONTROL0_ENCODE_0r_reg));*/
/**/
/*   }*/
    /* Set r_HG2_ENABLE_oen bit 15 in 0xc061 */
/*   SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);*/
/*   SC_X4_FLD_OVRR_EN1_TYPEr_R_HG2_ENABLE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, pc->rx_higig2_oen & 0x1);*/
/*   PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));*/
/**/
   /* Clr/Set HG2_ENABLE bit 12 in 0xc134 */
/*   RX_X4_DEC_CTL0r_CLR(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg);*/
/*   RX_X4_DEC_CTL0r_HG2_ENABLEf_SET(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg, pc->rx_higig2_en);*/
/*   PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_DEC_CTL0r(pc, RX_X4_CONTROL0_DECODE_CONTROL_0r_reg));*/
/**/
    /* CHAK:set rx_test_mode to 1 */
/*   if(pc->scramble_idle_rx_test_mode == 0x1) {*/
/*     printf("CHAK:enabling testmode in rx\n");*/
/*     RX_X4_DEC_CTL0r_CLR(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg);*/
/*     RX_X4_DEC_CTL0r_R_TEST_MODE_CFGf_SET(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg, pc->scramble_idle_rx_test_mode);*/
/*     PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_DEC_CTL0r(pc, RX_X4_CONTROL0_DECODE_CONTROL_0r_reg));*/
/*   }*/
/**/
  /* +cfg_tefmod_use_higig2_knobs +cfg_rx_higig2_codec */
/*  if(pc->tefmod_use_higig2_knobs == 0x1) {*/
/*    RX_X4_DEC_CTL0r_CLR(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg);*/
/*    RX_X4_DEC_CTL0r_HG2_CODECf_SET(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg, pc->rx_higig2_codec);*/
/*    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_DEC_CTL0r(pc, RX_X4_CONTROL0_DECODE_CONTROL_0r_reg));*/
/*  }*/
/*  */
/*#ifdef _DV_TB_*/
/*  RX_X4_DEC_CTL0r_CLR(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg);*/
/*  RX_X4_DEC_CTL0r_DISABLE_CL49_BERMONf_SET(RX_X4_CONTROL0_DECODE_CONTROL_0r_reg, pc->disable_cl49_bermon);*/
/*  PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_DEC_CTL0r(pc, RX_X4_CONTROL0_DECODE_CONTROL_0r_reg));*/
/*#endif */

  _tefmod_hg2_init(pc, pc->tx_higig2_oen, pc->tx_higig2_en, pc->tefmod_use_higig2_knobs, pc->tx_higig2_codec, pc->scramble_idle_tx_test_mode, pc->rx_higig2_oen, pc->rx_higig2_en, pc->scramble_idle_rx_test_mode, pc->rx_higig2_codec, pc->disable_cl49_bermon);


   /* MSA Related configurations */
/*  printf("In tefmod_init_pcs_falcon and configuring the MSA registers msa_25G_am0_mode:%0d msa_25G_am123_mode:%0d msa_50G_am0_mode:%0d msa_50G_am123_mode:%0d cl74_shcorrupt_50msa: %0d cl74_shcorrupt_25msa :%0d \n",*/
/*                                                                  pc->msa_25G_am0_mode,pc->msa_25G_am123_mode,pc->msa_50G_am0_mode,pc->msa_50G_am123_mode,pc->cl74_shcorrupt_50msa,pc->cl74_shcorrupt_25msa);*/
/*  SC_X4_MSA_25G_50G_CTLr_CLR(SC_X4_MSA_25G_50G_CTLr_reg);*/
/*  SC_X4_MSA_25G_50G_CTLr_MSA_25G_CWSCR_ENf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_25G_cw_scr_en);*/
/*  SC_X4_MSA_25G_50G_CTLr_MSA_25G_5BIT_XORf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_25G_5bit_XOR); */
/*  SC_X4_MSA_25G_50G_CTLr_MSA_25G_AM_ENf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_25G_am_en);*/
/*  SC_X4_MSA_25G_50G_CTLr_MSA_25G_AM0_FORMATf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_25G_am0_mode);*/
/*  SC_X4_MSA_25G_50G_CTLr_MSA_25G_AM123_FORMATf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_25G_am123_mode);*/
/*  SC_X4_MSA_25G_50G_CTLr_MSA_50G_AM0_FORMATf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_50G_am0_mode);*/
/*  SC_X4_MSA_25G_50G_CTLr_MSA_50G_AM123_FORMATf_SET(SC_X4_MSA_25G_50G_CTLr_reg, pc->msa_50G_am123_mode);*/
/*  SC_X4_MSA_25G_50G_CTLr_MSA_WINDOW_SELf_SET(SC_X4_MSA_25G_50G_CTLr_reg,pc->msa_window_sel);*/
/*  SC_X4_MSA_25G_50G_CTLr_MSA_COUNT_SELf_SET(SC_X4_MSA_25G_50G_CTLr_reg,pc->msa_count_sel);*/
/*  SC_X4_MSA_25G_50G_CTLr_CL74_SHCORRUPT_50GMSAf_SET(SC_X4_MSA_25G_50G_CTLr_reg,pc->cl74_shcorrupt_50msa);*/
/*  SC_X4_MSA_25G_50G_CTLr_CL74_SHCORRUPT_25GMSAf_SET(SC_X4_MSA_25G_50G_CTLr_reg,pc->cl74_shcorrupt_25msa);*/
/*  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_MSA_25G_50G_CTLr(pc, SC_X4_MSA_25G_50G_CTLr_reg));*/
  /* _tefmod_pcs_msa_init is similar to tefmod_default_init in sdk */
  _tefmod_pcs_msa_init(pc, pc->msa_25G_cw_scr_en, pc->msa_25G_5bit_XOR, pc->msa_25G_am_en, pc->msa_25G_am0_mode, pc->msa_25G_am123_mode, pc->msa_50G_am0_mode, pc->msa_50G_am123_mode, pc->msa_window_sel, pc->msa_count_sel, pc->cl74_shcorrupt_50msa, pc->cl74_shcorrupt_25msa);

 
  /* IEEE Related configurations */
/*  printf("In tefmod_init_pcs_falcon and configuring the IEEE registers ieee_25G_am0_mode:%0d ieee_25G_am123_mode:%0d 25G shcorrupt :%0d \n",pc->ieee_25G_am0_mode,pc->ieee_25G_am123_mode,pc->cl74_shcorrupt_25ieee);*/
/*  SC_X4_IEEE_25G_CTLr_CLR(SC_X4_IEEE_25G_CTLr_reg);*/
/*  SC_X4_IEEE_25G_CTLr_IEEE_25G_CWSCR_ENf_SET(SC_X4_IEEE_25G_CTLr_reg, pc->ieee_25G_cw_scr_en);*/
/*  SC_X4_IEEE_25G_CTLr_IEEE_25G_5BIT_XORf_SET(SC_X4_IEEE_25G_CTLr_reg, pc->ieee_25G_5bit_XOR);*/
/*  SC_X4_IEEE_25G_CTLr_IEEE_25G_AM_ENf_SET(SC_X4_IEEE_25G_CTLr_reg, pc->ieee_25G_am_en);*/
/*  SC_X4_IEEE_25G_CTLr_IEEE_25G_AM0_FORMATf_SET(SC_X4_IEEE_25G_CTLr_reg, pc->ieee_25G_am0_mode);*/
/*  SC_X4_IEEE_25G_CTLr_IEEE_25G_AM123_FORMATf_SET(SC_X4_IEEE_25G_CTLr_reg, pc->ieee_25G_am123_mode);*/
/*  SC_X4_IEEE_25G_CTLr_IEEE_WINDOW_SELf_SET(SC_X4_IEEE_25G_CTLr_reg,pc->ieee_window_sel);*/
/*  SC_X4_IEEE_25G_CTLr_IEEE_COUNT_SELf_SET(SC_X4_IEEE_25G_CTLr_reg,pc->ieee_count_sel);*/
/*  SC_X4_IEEE_25G_CTLr_CL74_SHCORRUPT_25IEEEf_SET(SC_X4_IEEE_25G_CTLr_reg,pc->cl74_shcorrupt_25ieee);*/
/*  PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_IEEE_25G_CTLr(pc, SC_X4_IEEE_25G_CTLr_reg));*/
/**/
  /*
  SC_X4_IEEE_25G_CTLr_IEEE_WINDOW_SELf_SET(SC_X4_IEEE_25G_CTLr_reg,);
  SC_X4_IEEE_25G_CTLr_IEEE_COUNT_SELf_SET(SC_X4_IEEE_25G_CTLr_reg,);
  SC_X4_IEEE_25G_CTLr_CL74_SHCORRUPT_25IEEEf_SET(SC_X4_IEEE_25G_CTLr_reg,);
  */

  /* Auto detect settings */
/*  printf("In tefmod_init_pcs_falcon and configuring the parallel detect registers msa_ieee_det_en:%0x \n",pc->msa_ieee_det_en);*/
/*  RX_X4_IEEE_25G_PARLLEL_DET_CTRr_CLR(RX_X4_IEEE_25G_PARLLEL_DET_CTRr_reg);*/
/*  RX_X4_IEEE_25G_PARLLEL_DET_CTRr_MSA_IEEE_DET_ENf_SET(RX_X4_IEEE_25G_PARLLEL_DET_CTRr_reg, pc->msa_ieee_det_en);*/
/*  PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_IEEE_25G_PARLLEL_DET_CTRr(pc, RX_X4_IEEE_25G_PARLLEL_DET_CTRr_reg));*/
  _tefmod_pcs_ieee_25g_init(pc, pc->ieee_25G_cw_scr_en, pc->ieee_25G_5bit_XOR, pc->ieee_25G_am_en, pc->ieee_25G_am0_mode, pc->ieee_25G_am123_mode, pc->ieee_window_sel, pc->ieee_count_sel, pc->cl74_shcorrupt_25ieee, pc->msa_ieee_det_en);




  /*if FEC enabled */
  if(pc->t_fec_enable) {
    tefmod_FEC_control(pc, (TEFMOD_CL74_TX_EN_DIS|TEFMOD_CL74_RX_EN_DIS), 0, 0, pc->spd_intf);
  }

  /* Programming the an_good_check_entry, an_completed_sw, ld_page_req, lp_page_rdy interrupt enable    */
/*  AN_X4_INT_ENr_CLR(AN_X4_INT_ENr_reg);*/
/*  AN_X4_INT_ENr_AN_GOOD_CHK_ENf_SET(AN_X4_INT_ENr_reg, pc->an_gc_entry_int_en);*/
/*  AN_X4_INT_ENr_AN_COMPLETED_SW_ENf_SET(AN_X4_INT_ENr_reg, pc->sw_an_completed_en);*/
/*  AN_X4_INT_ENr_LD_PAGE_REQ_ENf_SET(AN_X4_INT_ENr_reg, pc->ld_page_req_en);*/
/*  AN_X4_INT_ENr_LP_PAGE_RDY_ENf_SET(AN_X4_INT_ENr_reg, pc->lp_page_rdy_en);*/
/*  PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_INT_ENr(pc, AN_X4_INT_ENr_reg));*/
  _tefmod_sw_an_sts_get(pc, pc->an_gc_entry_int_en, pc->sw_an_completed_en, pc->ld_page_req_en, pc->lp_page_rdy_en);


  /* call FS or AN mode */
  if(pc->an_en) {
    tefmod_init_pcs_an(pc);
  } else {
    tefmod_init_pcs_fs(pc);
  }
  if(pc->pmd_gloop_en != 0) {
    tefmod_tx_pmd_loopback_control(pc, pc->pmd_gloop_en);
  }


  if(pc->cl72_training_restart || pc->cl72_training_enabled) {
    pc->per_lane_control = pc->cl72_training_restart | (pc->cl72_training_enabled<<1); 
    tefmod_clause72_control(pc, pc->per_lane_control, pc->port_type);
  }
  return PHYMOD_E_NONE;
}

int tefmod_check_status(PHYMOD_ST* pc)
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

  TEFMOD_DBG_IN_FUNC_INFO(pc);
  spd_change_done = 0;


  if(!pc->an_en) {
    tefmod_get_mapped_speed(pc->spd_intf, &mapped_speed);
    pc->speed = mapped_speed;
    spd_change_done = tefmod_poll_for_sc_done(pc, pc->speed);
    if(spd_change_done) {
      printf(" tefmod_check_status: Speed change done for :: unit %0d port %0d \n",pc->unit, pc->this_lane);
    }
  }

  printf("\nSTATS_Info : tefmod_check_status (1a) : pc->an_en %01x : pc->speed %02x : pc->an_fec_sel_override %01x : pc->cl91_en %01x : pc->an_fec %01x : pc->bam_cl74_fec %01x : pc->t_fec_enable %1x : pc->cl91_nofec %1x : pc->cl91_cl74 %1x\n", pc->an_en, pc->speed, pc->an_fec_sel_override, pc->cl91_en, pc->an_fec, pc->bam_cl74_fec, pc->t_fec_enable, pc->t_fec_enable, pc->cl91_nofec, pc->cl91_cl74) ;

  printf("STATS_Info : tefmod_check_status (1b) : pc->sc_mode %2d : pc->fc_mode %1x : pc->fc_rx_spd_intf %02d : pc->fc_tx_spd_intf %02d\n", pc->sc_mode, pc->fc_mode, pc->fc_rx_spd_intf, pc->fc_tx_spd_intf) ;

  printf("STATS_Info : tefmod_check_status (1c) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;

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

  printf("STATS_Info : tefmod_check_status (2a) : pc->an_en %01x : pc->speed %02x : pc->an_fec_sel_override %01x : pc->cl91_en %01x : pc->an_fec %01x : pc->bam_cl74_fec %01x : pc->t_fec_enable %1x : pc->cl91_nofec %1x : pc->cl91_cl74 %1x\n", pc->an_en, pc->speed, pc->an_fec_sel_override, pc->cl91_en, pc->an_fec, pc->bam_cl74_fec, pc->t_fec_enable, pc->t_fec_enable, pc->cl91_nofec, pc->cl91_cl74) ;

  printf("STATS_Info : tefmod_check_status (2b) : pc->sc_mode %2d : pc->fc_mode %1x : pc->fc_rx_spd_intf %02d : pc->fc_tx_spd_intf %02d\n", pc->sc_mode, pc->fc_mode, pc->fc_rx_spd_intf, pc->fc_tx_spd_intf) ;

  printf("STATS_Info : tefmod_check_status (2c) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;

  tefmod_get_ht_entries(pc); /* Must be after an_fec_sel_override code */

  printf("\nSTATS_Info : tefmod_check_status (3a) : pc->an_en %01x : pc->speed %02x : pc->an_fec_sel_override %01x : pc->cl91_en %01x : pc->an_fec %01x : pc->bam_cl74_fec %01x : pc->t_fec_enable %1x : pc->cl91_nofec %1x : pc->cl91_cl74 %1x\n", pc->an_en, pc->speed, pc->an_fec_sel_override, pc->cl91_en, pc->an_fec, pc->bam_cl74_fec, pc->t_fec_enable, pc->t_fec_enable, pc->cl91_nofec, pc->cl91_cl74) ;

  printf("STATS_Info : tefmod_check_status (3b) : pc->sc_mode %2d : pc->fc_mode %1x : pc->fc_rx_spd_intf %02d : pc->fc_tx_spd_intf %02d\n", pc->sc_mode, pc->fc_mode, pc->fc_rx_spd_intf, pc->fc_tx_spd_intf) ;

  printf("STATS_Info : tefmod_check_status (3c) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;

  if(pc->sc_mode == TEFMOD_SC_MODE_ST_OVERRIDE) {
     pc->os_mode=0;
     pc->clkcnt1 = 0x5;
     pc->lpcnt0 = 0x1;
     pc->lpcnt1 = 0x12;
     pc->cgc = 0x4;
     pc->cl72_en = 0;
  }

  if(pc->sc_mode == TEFMOD_SC_MODE_AN_CL73) {
    pc->speed = 0x01;
  }


    printf("STATS_Info : tefmod_check_status (4a) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;


  /* This code below is used only in the case where AN resolves to 100G, and you want to
  * override the default 100G FEC */
  if(pc->cl91_nofec != 0 || pc->cl91_cl74) {
    pc->t_pma_btmx_mode = 2;
    pc->deskew_mode = 4;
    pc->bs_sync_en = 1;
    pc->bs_dist_mode = 3;
    pc->bs_btmx_mode = 2;

    printf("STATS_Info : tefmod_check_status (4d) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;
  }

  if(pc->cl91_cl74 != 0) {
    pc->bs_sync_en = 0;

    printf("STATS_Info : tefmod_check_status (4e) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;
  }

  if(pc->cl91_en != 0) {
    if(pc->speed == 0x30 || pc->speed == 0x31 || pc->speed == 0x32 ||
       pc->speed == 0x34 || pc->speed == 0x35 || pc->speed == 0x36) {
      pc->deskew_mode = 8;
      pc->bs_dist_mode = 0;
      pc->bs_btmx_mode = 0;

      printf("STATS_Info : tefmod_check_status (4f) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;
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

      printf("STATS_Info : tefmod_check_status (4g) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;
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

      printf("STATS_Info : tefmod_check_status (4h) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;
    }

    if(pc->fc_rx_spd_intf == TEFMOD_SPD_32G_FC || pc->fc_tx_spd_intf == TEFMOD_SPD_32G_FC) {
      if(pc->fc_mode == 1) {
        pc->t_fifo_mode = 0;
        pc->deskew_mode = 9;
        pc->bs_sm_sync_mode = 0;

        printf("STATS_Info : tefmod_check_status (4i) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;
      }
    }
  }

  printf("\nSTATS_Info : tefmod_check_status (5a) : pc->an_en %01x : pc->speed %02x : pc->an_fec_sel_override %01x : pc->cl91_en %01x : pc->an_fec %01x : pc->bam_cl74_fec %01x : pc->t_fec_enable %1x : pc->cl91_nofec %1x : pc->cl91_cl74 %1x\n", pc->an_en, pc->speed, pc->an_fec_sel_override, pc->cl91_en, pc->an_fec, pc->bam_cl74_fec, pc->t_fec_enable, pc->t_fec_enable, pc->cl91_nofec, pc->cl91_cl74) ;

  printf("STATS_Info : tefmod_check_status (5b) : pc->sc_mode %2d : pc->fc_mode %1x : pc->fc_rx_spd_intf %02d : pc->fc_tx_spd_intf %02d\n", pc->sc_mode, pc->fc_mode, pc->fc_rx_spd_intf, pc->fc_tx_spd_intf) ;

  printf("STATS_Info : tefmod_check_status (5c) : pc->t_pma_btmx_mode %1x : pc->deskew_mode %1x : pc->bs_sync_en %1x : pc->bs_dist_mode %1x : pc->bs_btmx_mode %1x : pc->t_fifo_mode %1x : pc->bs_sm_sync_mode %1x\n\n", pc->t_pma_btmx_mode, pc->deskew_mode, pc->bs_sync_en, pc->bs_dist_mode, pc->bs_btmx_mode, pc->t_fifo_mode, pc->bs_sm_sync_mode) ;

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
  printf("SC_Info : tefmod_cfg_seq.c : tefmod_check_status() : pc->spd_intf %02x :    mapped_speed %02x : pc->speed %02x : actual_speed %02x\n",
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
    print_tefmod_sc_lkup_table(sc_stats_print_actual);
    PHYMOD_DEBUG_ERROR(("Expected stats\n"));
    print_tefmod_sc_lkup_table(pc);
    return PHYMOD_E_FAIL;
    #endif
    pc->sc_done = 0;
  } else {
    pc->sc_done=1;
    #ifdef _DV_TB_
      PHYMOD_DEBUG_ERROR(("SC_DONE set\n"));
  if(pc->tefmod_st_print==1) {
    print_tefmod_st(pc);
  }
     return PHYMOD_E_NONE;
    #endif
  }
}

#endif

/*!
@brief   This function reads TX-PLL PLL_LOCK bit.
@param   pc  handle to current TSCF context (#PHYMOD_ST)
@param   lockStatus reference which is updated with lock status.
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Read PLL lock status. Returns  1 or 0 (locked/not)
*/

int tefmod16_gen3_pmd_lock_get(PHYMOD_ST* pc, uint32_t* lockStatus)
{
    PMD_X4_STSr_t  reg_pmd_x4_sts;
    int i;
    phymod_access_t pa_copy;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    *lockStatus = 1;
    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
 
    for(i=0; i<4; i++) {
        if(((pc->lane_mask >> i) & 0x1) == 1) {
            pa_copy.lane_mask = 0x1 << i;
            PHYMOD_IF_ERR_RETURN(READ_PMD_X4_STSr(&pa_copy, &reg_pmd_x4_sts));
            *lockStatus = *lockStatus & PMD_X4_STSr_RX_LOCK_STSf_GET(reg_pmd_x4_sts);
        }
    }

    return PHYMOD_E_NONE;
}

/*!
@brief   get  port speed id configured
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   speed_id Receives the resolved speed cfg in the speed_id
@returns The value PHYMOD_E_NONE upon successful completion.
@details get  port speed configured
*/
int tefmod16_gen3_speed_id_get(PHYMOD_ST* pc, int *speed_id)
{
    SC_X4_RSLVD_SPDr_t sc_final_resolved_speed;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    SC_X4_RSLVD_SPDr_CLR(sc_final_resolved_speed);
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD_SPDr(pc,&sc_final_resolved_speed));
    *speed_id = SC_X4_RSLVD_SPDr_SPEEDf_GET(sc_final_resolved_speed);

    TEFMOD16_GEN3_DBG_IN_FUNC_VOUT_INFO(pc,("speed_id: %d", *speed_id));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_refclk_set(PHYMOD_ST* pc, phymod_ref_clk_t ref_clock)
{
    DIG_TOP_USER_CTL0r_t dig_top_user_reg;

    PHYMOD_IF_ERR_RETURN(READ_DIG_TOP_USER_CTL0r(pc, &dig_top_user_reg));
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
    PHYMOD_IF_ERR_RETURN(MODIFY_DIG_TOP_USER_CTL0r(pc, dig_top_user_reg));

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
int tefmod16_gen3_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched) /* PMD_RESET_SEQ */
{
    PMD_X1_CTLr_t PMD_X1_CTLr_reg;
    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

    PMD_X1_CTLr_CLR(PMD_X1_CTLr_reg);

    if (pmd_touched == 0) {
        PMD_X1_CTLr_POR_H_RSTBf_SET(PMD_X1_CTLr_reg, 0);
        PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(PMD_X1_CTLr_reg, 0);
        PHYMOD_IF_ERR_RETURN(WRITE_PMD_X1_CTLr(pc, PMD_X1_CTLr_reg));

        PMD_X1_CTLr_CORE_DP_H_RSTBf_SET(PMD_X1_CTLr_reg, 1);
        PMD_X1_CTLr_POR_H_RSTBf_SET(PMD_X1_CTLr_reg, 1);
        PHYMOD_IF_ERR_RETURN(WRITE_PMD_X1_CTLr(pc,PMD_X1_CTLr_reg));
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
int tefmod16_gen3_master_port_num_set( PHYMOD_ST *pc,  int port_num)
{
    MAIN0_SETUPr_t main_reg;

    MAIN0_SETUPr_CLR(main_reg);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUPr(pc, &main_reg));
    MAIN0_SETUPr_MASTER_PORT_NUMf_SET(main_reg, port_num);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc, main_reg));

    return PHYMOD_E_NONE;
}

/**
@brief   update the port mode
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   pll_restart Receives info. on whether to restart pll.
@returns The value PHYMOD_E_NONE upon successful completion
*/
int tefmod16_gen3_port_mode_select( PHYMOD_ST *pc)
{
    MAIN0_SETUPr_t mode_reg;
    int port_mode_sel = 0, port_mode_sel_value;
    uint32_t single_port_mode = 0;
    uint32_t first_couple_mode = 0, second_couple_mode = 0;

    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUPr(pc, &mode_reg));
    port_mode_sel_value = MAIN0_SETUPr_PORT_MODE_SELf_GET(mode_reg);

    if(pc->lane_mask == 0xf){
        port_mode_sel = 4;
    } else {
        first_couple_mode = ((port_mode_sel_value == 2) || (port_mode_sel_value == 3) || (port_mode_sel_value == 4));
        second_couple_mode = ((port_mode_sel_value == 1) || (port_mode_sel_value == 3) || (port_mode_sel_value == 4));
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
                break;
        }

        if(first_couple_mode ){
            port_mode_sel =(second_couple_mode)? 3: 2;
        } else if(second_couple_mode) {
            port_mode_sel = 1;
        } else{
            port_mode_sel = 0 ;
        }
    }

    MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(mode_reg, single_port_mode);
    MAIN0_SETUPr_PORT_MODE_SELf_SET(mode_reg, port_mode_sel);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc, mode_reg));

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
int tefmod16_gen3_pll_reset_enable_set (PHYMOD_ST *pc, int enable)
{
    MAIN0_SPD_CTLr_t main_reg;

    MAIN0_SPD_CTLr_CLR(main_reg);
    MAIN0_SPD_CTLr_PLL_RESET_ENf_SET(main_reg, enable);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SPD_CTLr(pc, main_reg));

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
int tefmod16_gen3_get_pcs_link_status(PHYMOD_ST* pc, uint32_t *link)
{
    RX_X4_PCS_LIVE_STS1r_t reg_pcs_live_sts;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    RX_X4_PCS_LIVE_STS1r_CLR(reg_pcs_live_sts);
    PHYMOD_IF_ERR_RETURN(READ_RX_X4_PCS_LIVE_STS1r(pc, &reg_pcs_live_sts));
    *link = RX_X4_PCS_LIVE_STS1r_LINK_STATUSf_GET(reg_pcs_live_sts);

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_get_pcs_latched_link_status(PHYMOD_ST* pc, uint32_t *link)
{
    RX_X4_PCS_LIVE_STS1r_t reg_pcs_live_sts;
    RX_X4_PCS_LATCH_STS1r_t latched_sts ;
    int  latched_val ;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    RX_X4_PCS_LIVE_STS1r_CLR(reg_pcs_live_sts);
    RX_X4_PCS_LATCH_STS1r_CLR(latched_sts) ;

    PHYMOD_IF_ERR_RETURN(READ_RX_X4_PCS_LIVE_STS1r(pc, &reg_pcs_live_sts));
    PHYMOD_IF_ERR_RETURN(READ_RX_X4_PCS_LATCH_STS1r(pc, &latched_sts)) ;
    latched_val = RX_X4_PCS_LATCH_STS1r_LINK_STATUS_LLf_GET(latched_sts) ;
    if(latched_val) {
        *link = 0;
    } else {
        *link = RX_X4_PCS_LIVE_STS1r_LINK_STATUSf_GET(reg_pcs_live_sts);
    }

    return PHYMOD_E_NONE;
}

/**
@brief   Get the Port status
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   disabled  Receives status on port disabledness
@returns The value PHYMOD_E_NONE upon successful completion
@details Ports can be disabled in several ways. In this function we simply write
0 to the speed change which will bring the PCS down for that lane.

*/
int tefmod16_gen3_enable_get(PHYMOD_ST* pc, uint32_t* enabled)
{
  SC_X4_CTLr_t reg_sc_ctrl;

  TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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
int tefmod16_gen3_disable_set(PHYMOD_ST* pc)
{
    SC_X4_CTLr_t reg_sc_ctrl;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    SC_X4_CTLr_CLR(reg_sc_ctrl);

    /* write 0 to the speed change */
    SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc,reg_sc_ctrl));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_enable_set(PHYMOD_ST* pc)
{
    SC_X4_CTLr_t reg_sc_ctrl;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    SC_X4_CTLr_CLR(reg_sc_ctrl);

    /* write 1 to the speed change */
    SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc,reg_sc_ctrl));

    return PHYMOD_E_NONE;
}

/* internal supporting function */
int tefmod16_gen3_pll_div_get(PHYMOD_ST* pc, uint32_t *plldiv_r_val)
{
  PLL_CAL_CTL7r_t PLL_CAL_CTL7r_reg;

  TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
  PHYMOD_IF_ERR_RETURN (READ_PLL_CAL_CTL7r(pc, &PLL_CAL_CTL7r_reg));

  *plldiv_r_val = PLL_CAL_CTL7r_PLL_MODEf_GET(PLL_CAL_CTL7r_reg);

  return PHYMOD_E_NONE;
}

/**
@brief  Get the plldiv from lookup table
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  spd_intf  Input of enum type #tefmod_spd_intfc_type_t
@param  plldiv  Receives PLL Divider value
@returns The value PHYMOD_E_NONE upon successful completion
@details Get the plldiv from lookup table as a function of the speed.
*/

int tefmod16_gen3_pll_div_lkup_get(PHYMOD_ST* pc, tefmod16_gen3_spd_intfc_type_t spd_intf, phymod_ref_clk_t refclk, uint32_t* plldiv)
{
    int speed_id;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    PHYMOD_IF_ERR_RETURN(tefmod16_gen3_get_mapped_speed(spd_intf, &speed_id));
    if (refclk == phymodRefClk125Mhz) {
        *plldiv = sc_pmd16_gen3_entry_125M_ref[speed_id].pll_mode;
    } else {
        *plldiv = sc_pmd16_gen3_entry[speed_id].pll_mode;
    }

    TEFMOD16_GEN3_DBG_IN_FUNC_VOUT_INFO(pc,("plldiv: %d", *plldiv));

    return PHYMOD_E_NONE;
}

/**
@brief   Get the osmode from lookup table
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   spd_intf  Input of enum type #tefmod_spd_intfc_type_t
@param   osmode Receives the OS mode as assumed in the hard table.
@returns The value PHYMOD_E_NONE upon successful completion
@details Get the osmode from software version of Speed table as a function of
the speed
*/
int tefmod16_gen3_osmode_lkup_get(PHYMOD_ST* pc, tefmod16_gen3_spd_intfc_type_t spd_intf, phymod_ref_clk_t refclk, uint32_t *osmode)
{
    int speed_id;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    PHYMOD_IF_ERR_RETURN(tefmod16_gen3_get_mapped_speed(spd_intf, &speed_id));

    *osmode = sc_pmd16_gen3_entry[speed_id].pma_os_mode;

    return PHYMOD_E_NONE;
}

/**
@brief   Gets PMD TX lane swap values for all lanes simultaneously.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   tx_lane_map returns the pmd tx lane map
@returns The value PHYMOD_E_NONE upon successful completion
@details
This function gets the TX lane swap values for all lanes simultaneously.

*/
int tefmod16_gen3_pmd_lane_swap_tx_rx_get ( PHYMOD_ST *pc, uint32_t *tx_lane_map, uint32_t *rx_lane_map)
{
    uint16_t tx_lane_map_0, tx_lane_map_1, tx_lane_map_2, tx_lane_map_3;
    uint16_t rx_lane_map_0, rx_lane_map_1, rx_lane_map_2, rx_lane_map_3;
    LN_ADDR0r_t ln0_reg;
    LN_ADDR1r_t ln1_reg;
    LN_ADDR2r_t ln2_reg;
    LN_ADDR3r_t ln3_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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
int tefmod16_gen3_rx_lane_control_set(PHYMOD_ST* pc, int enable)         /* REG map not match */
{
    RX_X4_PMA_CTL0r_t    reg_pma_ctrl;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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
@param   enable Handle to get the configured reset lane.
@returns The value PHYMOD_E_NONE upon successful completion.
@details
          This function read back control bit
*/
int tefmod16_gen3_rx_lane_control_get(PHYMOD_ST* pc, int *enable)
{
    RX_X4_PMA_CTL0r_t    reg_pma_ctrl;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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
int tefmod16_gen3_tx_rx_polarity_get ( PHYMOD_ST *pc, uint32_t* tx_polarity, uint32_t* rx_polarity)
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
int tefmod16_gen3_tx_rx_polarity_set ( PHYMOD_ST *pc, uint32_t tx_polarity, uint32_t rx_polarity)
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
@brief   Trigger speed change.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PCS Init
*/
int tefmod16_gen3_trigger_speed_change(PHYMOD_ST* pc)
{
    SC_X4_CTLr_t    reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

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
@brief Squelch TX lane output.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  tx control lanes to disable
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details This function disables transmission on a specific lane. No reset is
required to restart the transmission. Lane control is done through 'tx' input.
Set bits 0, 8, 16, or 24 to <B>0 to disable TX on lanes 0/1/2/3</B>
Set the bits to <B>1 to enable TX</B>
*/

int tefmod16_gen3_tx_lane_disable (PHYMOD_ST* pc, int tx)
{
    PMD_X4_CTLr_t PMD_X4_PMD_X4_CONTROLr_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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
int tefmod16_gen3_pcs_ilkn_mode_set(PHYMOD_ST* pc, uint32_t enable)
{
    SC_X4_BYPASSr_t reg_sc_bypass;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    SC_X4_BYPASSr_CLR(reg_sc_bypass);
    SC_X4_BYPASSr_SC_BYPASSf_SET(reg_sc_bypass, enable ? 1 : 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_BYPASSr(pc,reg_sc_bypass));

    return PHYMOD_E_NONE;
}

/**
@brief   Select the ILKN path and bypass TSCF PCS
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details This will enable ILKN path. PCS is set to bypass to relinquish PMD
control. Expect PMD to be programmed elsewhere.
*/
int tefmod16_gen3_init_pcs_ilkn(PHYMOD_ST* pc, uint32_t enable)
{
    ILKN_CTL0r_t ILKN_CONTROL0r_reg;

    PHYMOD_IF_ERR_RETURN(READ_ILKN_CTL0r(pc, &ILKN_CONTROL0r_reg));
    ILKN_CTL0r_CREDIT_ENf_SET(ILKN_CONTROL0r_reg, enable ? 1 :0);
    ILKN_CTL0r_ILKN_SELf_SET(ILKN_CONTROL0r_reg, enable ? 1 : 0);
    PHYMOD_IF_ERR_RETURN(MODIFY_ILKN_CTL0r(pc, ILKN_CONTROL0r_reg));

    return PHYMOD_E_NONE;
}

/**
@brief   Check if ILKN is set
@param   pc handle to current TSCF context (#PHYMOD_ST), ilkn_set status
@returns The value PHYMOD_E_NONE upon successful completion
@details Check if ILKN is set, ilkn_set = 1 if it is set.
*/
int tefmod16_gen3_pcs_ilkn_chk(PHYMOD_ST* pc, int *ilkn_set)
{
    ILKN_CTL0r_t ILKN_CONTROL0r_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    PHYMOD_IF_ERR_RETURN(READ_ILKN_CTL0r(pc, &ILKN_CONTROL0r_reg));
    *ilkn_set = ILKN_CTL0r_ILKN_SELf_GET(ILKN_CONTROL0r_reg);

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_autoneg_ability_set(PHYMOD_ST* pc, tefmod16_gen3_an_adv_ability_t *cl73_adv)
{
    uint32_t override_v;
    AN_X4_LD_BASE_ABIL1r_t AN_X4_LD_BASE_ABIL1r_reg;
    AN_X4_LD_BASE_ABIL0r_t AN_X4_LD_BASE_ABILI0r_reg;
    AN_X4_LD_UP1_ABIL0r_t AN_X4_LD_UP1_ABIL0r_reg;
    AN_X4_LD_UP1_ABIL1r_t AN_X4_LD_UP1_ABIL1r_reg;
    AN_X4_LD_BASE_ABIL3r_t AN_X4_LD_BASE_ABIL3r_reg;
    AN_X4_LD_BASE_ABIL4r_t AN_X4_LD_BASE_ABIL4r_reg;
    AN_X4_LD_BASE_ABIL5r_t AN_X4_LD_BASE_ABIL5r_reg;
    AN_X4_LD_FEC_BASEPAGE_ABILr_t AN_X4_LD_FEC_BASEPAGE_ABILr_reg;
    int         adv100g = 0;

    AN_X4_LD_BASE_ABIL1r_CLR(AN_X4_LD_BASE_ABIL1r_reg);
    /******* Base Abilities 0xC1C4[5:0]****/
    AN_X4_LD_BASE_ABIL1r_SET(AN_X4_LD_BASE_ABIL1r_reg, cl73_adv->an_base_speed & 0x3f);

    /******* Pause Settings 0xC1C4[7:6]********/
    if (cl73_adv->an_pause == TEFMOD_NO_PAUSE){
        AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_LD_BASE_ABIL1r_reg, 0);
    }
    if (cl73_adv->an_pause == TEFMOD_SYMM_PAUSE){
        AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_LD_BASE_ABIL1r_reg, 1);
    }
    if (cl73_adv->an_pause == TEFMOD_ASYM_PAUSE){
        AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_LD_BASE_ABIL1r_reg, 2);
    }
    if (cl73_adv->an_pause == TEFMOD_ASYM_SYMM_PAUSE){
        AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_SET(AN_X4_LD_BASE_ABIL1r_reg, 3);
    }
    /****** FEC Settings 0xC1C4[9:8]********/
    if (cl73_adv->an_fec == TEFMOD_FEC_NOT_SUPRTD){
        AN_X4_LD_BASE_ABIL1r_FEC_REQf_SET(AN_X4_LD_BASE_ABIL1r_reg, 0);
    }
    if(cl73_adv->an_fec == TEFMOD_FEC_SUPRTD_NOT_REQSTD){
        AN_X4_LD_BASE_ABIL1r_FEC_REQf_SET(AN_X4_LD_BASE_ABIL1r_reg, 1);
    }
    if (AN_X4_LD_BASE_ABIL1r_BASE_100G_CR4f_GET(AN_X4_LD_BASE_ABIL1r_reg) ||
        AN_X4_LD_BASE_ABIL1r_BASE_100G_KR4f_GET(AN_X4_LD_BASE_ABIL1r_reg)) {
        adv100g = 1;
    }
    if ((cl73_adv->an_fec & TEFMOD_FEC_CL74_SUPRTD_REQSTD) ||
       ((cl73_adv->an_fec & TEFMOD_FEC_CL91_SUPRTD_REQSTD) && adv100g)){
       AN_X4_LD_BASE_ABIL1r_FEC_REQf_SET(AN_X4_LD_BASE_ABIL1r_reg, 3);
    }

    /****** Setting AN_X4_ABILITIES_ld_base_abilities_1 0xC1C4 ********/
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_BASE_ABIL1r(pc, AN_X4_LD_BASE_ABIL1r_reg));

    AN_X4_LD_BASE_ABIL3r_CLR(AN_X4_LD_BASE_ABIL3r_reg);
    AN_X4_LD_BASE_ABIL4r_CLR(AN_X4_LD_BASE_ABIL4r_reg);
    AN_X4_LD_BASE_ABIL5r_CLR(AN_X4_LD_BASE_ABIL5r_reg);
    AN_X4_LD_FEC_BASEPAGE_ABILr_CLR(AN_X4_LD_FEC_BASEPAGE_ABILr_reg);
    AN_X4_LD_UP1_ABIL1r_CLR(AN_X4_LD_UP1_ABIL1r_reg);

    /*****  25G base speed abilities_3 0xC1C8 ******/
    if((cl73_adv->an_base_speed  >> TEFMOD_CL73_25GBASE_KR1 ) & 1) {
        AN_X4_LD_BASE_ABIL3r_BASE_25G_KR1_ENf_SET(AN_X4_LD_BASE_ABIL3r_reg, 1);
        /* IEEE bit location for KR1/Cr1 is 10 */
        AN_X4_LD_BASE_ABIL3r_BASE_25G_KR1_SELf_SET(AN_X4_LD_BASE_ABIL3r_reg, TEFMOD_CL73_25GBASE_KR1_POS);
    }
    if((cl73_adv->an_base_speed  >> TEFMOD_CL73_25GBASE_CR1 ) & 1) {
        AN_X4_LD_BASE_ABIL3r_BASE_25G_CR1_ENf_SET(AN_X4_LD_BASE_ABIL3r_reg, 1);
        /* IEEE bit location for KR1/Cr1 is 10 */
        AN_X4_LD_BASE_ABIL3r_BASE_25G_CR1_SELf_SET(AN_X4_LD_BASE_ABIL3r_reg, TEFMOD_CL73_25GBASE_CR1_POS);
    }
    /*****  25G base speed abilities_4 0xC1C9 ******/
    if((cl73_adv->an_base_speed  >> TEFMOD_CL73_25GBASE_KRS1 ) & 1) {
        AN_X4_LD_BASE_ABIL4r_BASE_25G_KRS1_ENf_SET(AN_X4_LD_BASE_ABIL4r_reg, 1);
        /* IEEE bit location for KRS1/CrS1 is 9 */
        AN_X4_LD_BASE_ABIL4r_BASE_25G_KRS1_SELf_SET(AN_X4_LD_BASE_ABIL4r_reg, TEFMOD_CL73_25GBASE_KRS1_POS);
    }
    if((cl73_adv->an_base_speed  >> TEFMOD_CL73_25GBASE_CRS1 ) & 1) {
        AN_X4_LD_BASE_ABIL4r_BASE_25G_CRS1_ENf_SET(AN_X4_LD_BASE_ABIL4r_reg, 1);
        /* IEEE bit location for KRS1/CrS1 is 9 */
        AN_X4_LD_BASE_ABIL4r_BASE_25G_CRS1_SELf_SET(AN_X4_LD_BASE_ABIL4r_reg, TEFMOD_CL73_25GBASE_CRS1_POS);
    }
    /*****  2P5G base speed abilities_5 0xC1CA ******/
    if((cl73_adv->an_base_speed  >> TEFMOD_CL73_2P5GBASE_KX1 ) & 1) {
        AN_X4_LD_BASE_ABIL5r_BASE_2P5G_ENf_SET(AN_X4_LD_BASE_ABIL5r_reg, 1);
        AN_X4_LD_BASE_ABIL5r_BASE_2P5G_SELf_SET(AN_X4_LD_BASE_ABIL5r_reg, TEFMOD_CL73_2P5GBASE_KX1_POS);
    }
    /*****  5G base speed abilities_5 0xC1CA ******/
    if((cl73_adv->an_base_speed  >> TEFMOD_CL73_5GBASE_KR1 ) & 1) {
        AN_X4_LD_BASE_ABIL5r_BASE_5P0G_ENf_SET(AN_X4_LD_BASE_ABIL5r_reg, 1);
        AN_X4_LD_BASE_ABIL5r_BASE_5P0G_SELf_SET(AN_X4_LD_BASE_ABIL5r_reg, TEFMOD_CL73_5GBASE_KR1_POS);
    }
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_BASE_ABIL3r(pc, AN_X4_LD_BASE_ABIL3r_reg));
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_BASE_ABIL4r(pc, AN_X4_LD_BASE_ABIL4r_reg));
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_BASE_ABIL5r(pc, AN_X4_LD_BASE_ABIL5r_reg));

    /*
     * We always set the SEL bit for BASE_FEC (CL74 bits 11:07=24) and RS_FEC (CL91 bits 05:01 =23)
     * Irrespective of whether FEC74/FEC91 is requested or not.
     */
    AN_X4_LD_FEC_BASEPAGE_ABILr_BASE_R_FEC_REQ_SELf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, TEFMOD_CL73_25GBASE_FEC_POS);
    AN_X4_LD_FEC_BASEPAGE_ABILr_RS_FEC_REQ_SELf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, TEFMOD_CL73_25G_RS_FEC_POS);
    AN_X4_LD_FEC_BASEPAGE_ABILr_BASE_R_FEC_REQ_ENf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, 0);
    AN_X4_LD_FEC_BASEPAGE_ABILr_RS_FEC_REQ_ENf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, 0);

    /* Next we need to check which fec to enable */
    if (cl73_adv->an_fec & TEFMOD_FEC_CL74_SUPRTD_REQSTD) {
        AN_X4_LD_FEC_BASEPAGE_ABILr_BASE_R_FEC_REQ_ENf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, 1);
    } else {
        AN_X4_LD_FEC_BASEPAGE_ABILr_BASE_R_FEC_REQ_ENf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, 0);
    }
    if (cl73_adv->an_fec & TEFMOD_FEC_CL91_SUPRTD_REQSTD) {
        AN_X4_LD_FEC_BASEPAGE_ABILr_RS_FEC_REQ_ENf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, 1);
    } else {
        AN_X4_LD_FEC_BASEPAGE_ABILr_RS_FEC_REQ_ENf_SET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg, 0);
    }

    PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_FEC_BASEPAGE_ABILr(pc, AN_X4_LD_FEC_BASEPAGE_ABILr_reg));

    /****** Base selector 0xC1C3[4:0]*****/
    AN_X4_LD_BASE_ABIL0r_CLR(AN_X4_LD_BASE_ABILI0r_reg);
    AN_X4_LD_BASE_ABIL0r_CL73_BASE_SELECTORf_SET(AN_X4_LD_BASE_ABILI0r_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_BASE_ABIL0r(pc, AN_X4_LD_BASE_ABILI0r_reg));

    /****** User page abilities_0 0xC1C1[3:0] and 0xC1C1[9:6]******/
    AN_X4_LD_UP1_ABIL0r_CLR(AN_X4_LD_UP1_ABIL0r_reg);
    AN_X4_LD_UP1_ABIL0r_SET(AN_X4_LD_UP1_ABIL0r_reg, cl73_adv->an_bam_speed & 0x3cf);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_UP1_ABIL0r(pc, AN_X4_LD_UP1_ABIL0r_reg));

    /****** User page abilities_0 0xC1C1[15] ******/
    AN_X4_LD_UP1_ABIL0r_BAM_HG2f_SET(AN_X4_LD_UP1_ABIL0r_reg, cl73_adv->an_hg2);
    PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_UP1_ABIL0r(pc, AN_X4_LD_UP1_ABIL0r_reg));

    /****** User page abilities_1 0xC1C2[4:1] ******/
    AN_X4_LD_UP1_ABIL1r_CLR(AN_X4_LD_UP1_ABIL1r_reg);
    AN_X4_LD_UP1_ABIL1r_SET(AN_X4_LD_UP1_ABIL1r_reg, cl73_adv->an_bam_speed1 & 0x1e);
    /* the following MODIFY_ will clear 0xC1C2[15:14]&[13:12], as expected */
    PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_UP1_ABIL1r(pc, AN_X4_LD_UP1_ABIL1r_reg));

    /* Next check if BAM speed */
    /****** User page abilities_1 0xC1C2[15:14] and 0xC1C2[13:12] ******/
    if ((cl73_adv->an_bam_speed) || (cl73_adv->an_bam_speed1)) {
        AN_X4_LD_UP1_ABIL1r_CLR(AN_X4_LD_UP1_ABIL1r_reg);
        PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_UP1_ABIL1r(pc, &AN_X4_LD_UP1_ABIL1r_reg));
        /* Enable LD's 25G 50G CL74*/
        AN_X4_LD_UP1_ABIL1r_CL74_REQf_SET(AN_X4_LD_UP1_ABIL1r_reg, 0x1);
        /* Enable LD's 25G 50G CL91*/
        AN_X4_LD_UP1_ABIL1r_CL91_REQf_SET(AN_X4_LD_UP1_ABIL1r_reg, 0x1);
        if (cl73_adv->an_fec & TEFMOD_FEC_CL74_SUPRTD_REQSTD) {
            AN_X4_LD_UP1_ABIL1r_CL74_REQf_SET(AN_X4_LD_UP1_ABIL1r_reg, TEFMOD_BAM_CL74_REQ);
        }
        if (cl73_adv->an_fec & TEFMOD_FEC_CL91_SUPRTD_REQSTD) {
            AN_X4_LD_UP1_ABIL1r_CL91_REQf_SET(AN_X4_LD_UP1_ABIL1r_reg, TEFMOD_BAM_CL91_REQ);
        }
        PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_UP1_ABIL1r(pc, AN_X4_LD_UP1_ABIL1r_reg));
    }

    /* CL72 Enable logic
    if cl72_en = 1, use the default settings, no overrides.
    If cl72_en=0, then override to disable cl72.
    */
    if((cl73_adv->an_cl72 & 0x1) == 1) {
         override_v = ((OVERRIDE_CL72_EN_DIS<<16) | 1);
         tefmod16_gen3_set_override_1(pc, 0, override_v);
    } else {
         override_v = ((OVERRIDE_CL72_EN<<16) | 0);
         tefmod16_gen3_set_override_1(pc, 0, override_v);
    }

    return PHYMOD_E_NONE;
}

/*!
@brief To get autoneg advertisement registers.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details
*/
int tefmod16_gen3_autoneg_get(PHYMOD_ST* pc)
{
  TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
  /* TBD */
  return PHYMOD_E_NONE;
}

/*!
@brief   Controls the setting/resetting of autoneg ability and enabling/disabling
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   an_control handle to TSCF an control struct #tefmod_an_control_t
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.

@details
This function programs auto-negotiation (AN) modes for the TEF. It can
enable/disable clause37/clause73/BAM autonegotiation capabilities. Call this
function once for combo mode and once per lane in independent lane mode.

The autonegotiation mode is indicated by setting an_control as required.
*/
int tefmod16_gen3_autoneg_control(PHYMOD_ST* pc, tefmod16_gen3_an_control_t *an_control)
{
    phymod_access_t pa_copy;
    uint16_t num_advertised_lanes, cl73_bam_enable  ;
    int start_lane, num_of_lane, i;
    uint16_t cl73_hpam_enable, cl73_enable;
    uint16_t cl73_next_page;
    uint16_t cl73_restart;
    uint16_t cl73_bam_code;
    uint16_t msa_overrides;
    uint32_t pll_div;

    MAIN0_SETUPr_t         reg_setup;
    AN_X4_CL73_CFGr_t      AN_X4_CL73_CFGr_reg;
    AN_X1_CL73_ERRr_t      AN_X1_CL73_ERRr_reg;
    AN_X4_CL73_CTLSr_t     AN_X4_CL73_CTLSr_reg;
    AN_X4_LD_BASE_ABIL1r_t AN_X4_LD_BASE_ABIL1r_reg;
    AN_X4_LD_BAM_ABILr_t   AN_X4_ABILITIES_LD_BAM_ABILITIESr_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_of_lane));
    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    num_advertised_lanes          = an_control->num_lane_adv;
    cl73_bam_code                 = 0x0;
    cl73_bam_enable               = 0x0;
    cl73_hpam_enable              = 0x0;
    cl73_enable                   = 0x0;
    cl73_next_page                = 0x0;
    cl73_restart                  = 0x0;
    msa_overrides                 = 0x0;

    switch (an_control->an_type) {
        case TEFMOD_AN_MODE_CL73:
            cl73_restart                = an_control->enable;
            cl73_enable                 = an_control->enable;
            break;
        case TEFMOD_AN_MODE_CL73BAM:
            cl73_restart                = an_control->enable;
            cl73_enable                 = an_control->enable;
            cl73_bam_enable             = an_control->enable;
            cl73_bam_code               = 0x3;
            cl73_next_page              = 0x1;
            break;
        case TEFMOD_AN_MODE_CL73_MSA:
            cl73_restart                = an_control->enable;
            cl73_enable                 = an_control->enable;
            cl73_bam_enable             = an_control->enable;
            cl73_bam_code               = 0x3;
            cl73_next_page              = 0x1;
            msa_overrides               = 0x1;
            break;
        case TEFMOD_AN_MODE_MSA:
            cl73_restart                = an_control->enable;
            cl73_enable                 = an_control->enable;
            cl73_bam_enable             = an_control->enable;
            cl73_bam_code               = 0x3;
            cl73_next_page              = 0x1;
            msa_overrides               = 0x1;
            break;
        case TEFMOD_AN_MODE_HPAM:
            cl73_restart                = an_control->enable;
            cl73_enable                 = an_control->enable;
            cl73_hpam_enable            = an_control->enable;
            /* cl73_bam_code               = 0x4; */
            cl73_next_page              = 0x1;
            break;
        default:
            return PHYMOD_E_FAIL;
            break;
    }

    if(msa_overrides == 0x1) {
        tefmod16_gen3_an_oui_t oui;
        oui.oui = 0x6a737d;
        oui.oui_override_bam73_adv = 0x1;
        oui.oui_override_bam73_det = 0x1;
        oui.oui_override_hpam_adv  = 0x0;
        oui.oui_override_hpam_det  = 0x0;
        PHYMOD_IF_ERR_RETURN(tefmod16_gen3_an_oui_set(pc, oui));
        PHYMOD_IF_ERR_RETURN(tefmod16_gen3_an_msa_mode_set(pc, msa_overrides));
    }
    /* RESET Speed Change bit */
    if(an_control->enable){
        tefmod16_gen3_disable_set(pc);
    }

    MAIN0_SETUPr_CLR(reg_setup);

    if(an_control->enable){
        /*next we decide which cl73_vco to set based on the current VCO */
        PHYMOD_IF_ERR_RETURN(tefmod16_gen3_pll_div_get(pc, &pll_div));

        if(pll_div <= phymod_TSCF_PLL_DIV132){
            MAIN0_SETUPr_CL73_VCOf_SET(reg_setup, 0);
        } else {
            MAIN0_SETUPr_CL73_VCOf_SET(reg_setup, 1);
        }
        PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc, reg_setup));

    }

    /* Timer for the amount of time tot wait to receive a page from the link partner */
    AN_X1_CL73_ERRr_CLR(AN_X1_CL73_ERRr_reg);
    if(an_control->an_type == (tefmod16_gen3_an_mode_type_t)TEFMOD_AN_MODE_CL73) {
        AN_X1_CL73_ERRr_SET(AN_X1_CL73_ERRr_reg, 0);
    } else if(an_control->an_type == (tefmod16_gen3_an_mode_type_t)TEFMOD_AN_MODE_HPAM){
        AN_X1_CL73_ERRr_SET(AN_X1_CL73_ERRr_reg, 0xfff0);
    } else if (an_control->an_type == (tefmod16_gen3_an_mode_type_t)TEFMOD_AN_MODE_CL73BAM) {
        AN_X1_CL73_ERRr_SET(AN_X1_CL73_ERRr_reg, 0x1a10);
    }
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_CL73_ERRr(pc, AN_X1_CL73_ERRr_reg));

    /* Set cl73 next page probably*/
    AN_X4_LD_BASE_ABIL1r_CLR(AN_X4_LD_BASE_ABIL1r_reg);
    AN_X4_LD_BASE_ABIL1r_NEXT_PAGEf_SET(AN_X4_LD_BASE_ABIL1r_reg, cl73_next_page & 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_LD_BASE_ABIL1r(pc, AN_X4_LD_BASE_ABIL1r_reg));

    /* Writing bam_code */
    AN_X4_LD_BAM_ABILr_CLR(AN_X4_ABILITIES_LD_BAM_ABILITIESr_reg);
    AN_X4_LD_BAM_ABILr_CL73_BAM_CODEf_SET(AN_X4_ABILITIES_LD_BAM_ABILITIESr_reg, cl73_bam_code);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_BAM_ABILr(pc, AN_X4_ABILITIES_LD_BAM_ABILITIESr_reg));

    /*  Set 1G/2.5G PD function 0xC1C6 */
    AN_X4_CL73_CTLSr_CLR(AN_X4_CL73_CTLSr_reg);
    AN_X4_CL73_CTLSr_PD_KX_ENf_SET(AN_X4_CL73_CTLSr_reg, an_control->pd_kx_en);
    AN_X4_CL73_CTLSr_PD_2P5G_KX_ENf_SET(AN_X4_CL73_CTLSr_reg, an_control->pd_2P5G_kx_en);
    PHYMOD_IF_ERR_RETURN(MODIFY_AN_X4_CL73_CTLSr(pc, AN_X4_CL73_CTLSr_reg));

    AN_X4_CL73_CFGr_CLR(AN_X4_CL73_CFGr_reg);
    AN_X4_CL73_CFGr_CL73_ENABLEf_SET(AN_X4_CL73_CFGr_reg, 0);
    AN_X4_CL73_CFGr_CL73_AN_RESTARTf_SET(AN_X4_CL73_CFGr_reg, 0);
    PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_CL73_CFGr(pc, AN_X4_CL73_CFGr_reg));

    /* Set AN X4 abilities 0xC1C0 */
    AN_X4_CL73_CFGr_CLR(AN_X4_CL73_CFGr_reg);
    AN_X4_CL73_CFGr_CL73_BAM_ENABLEf_SET(AN_X4_CL73_CFGr_reg,cl73_bam_enable);
    AN_X4_CL73_CFGr_CL73_HPAM_ENABLEf_SET(AN_X4_CL73_CFGr_reg,cl73_hpam_enable);
    AN_X4_CL73_CFGr_CL73_ENABLEf_SET(AN_X4_CL73_CFGr_reg, cl73_enable);
    AN_X4_CL73_CFGr_CL73_AN_RESTARTf_SET(AN_X4_CL73_CFGr_reg,cl73_restart);
    if (an_control->an_property_type & TEFMOD_AN_PROPERTY_ENABLE_HPAM_TO_CL73_AUTO){
        AN_X4_CL73_CFGr_AD_TO_CL73_ENf_SET(AN_X4_CL73_CFGr_reg, 0x1);
    } else {
        AN_X4_CL73_CFGr_AD_TO_CL73_ENf_SET(AN_X4_CL73_CFGr_reg, 0x0);
        /* AN_X4_CL73_CFGr_HPAM_TO_CL73_AUTO_ENABLEf_SET(AN_X4_CL73_CFGr_reg,0x0); */
    }
    if (an_control->an_property_type & TEFMOD_AN_PROPERTY_ENABLE_CL73_BAM_TO_HPAM_AUTO){
        AN_X4_CL73_CFGr_BAM_TO_HPAM_AD_ENf_SET(AN_X4_CL73_CFGr_reg, 0x1);
    } else {
        AN_X4_CL73_CFGr_BAM_TO_HPAM_AD_ENf_SET(AN_X4_CL73_CFGr_reg, 0x0);
    }
    AN_X4_CL73_CFGr_NUM_ADVERTISED_LANESf_SET(AN_X4_CL73_CFGr_reg,num_advertised_lanes);
    PHYMOD_IF_ERR_RETURN (MODIFY_AN_X4_CL73_CFGr(pc, AN_X4_CL73_CFGr_reg));

    /* if AN is enabled, the restart bit needs to be cleared */
    if (an_control->enable) {
        AN_X4_CL73_CFGr_CLR(AN_X4_CL73_CFGr_reg);
        AN_X4_CL73_CFGr_CL73_AN_RESTARTf_SET(AN_X4_CL73_CFGr_reg, 0);
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
                tefmod16_gen3_clause72_control(&pa_copy, 0);
            }
        }
    }
    return PHYMOD_E_NONE;
} /* temod_autoneg_control */

/**
@brief   getRevDetails , calls revid_read
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details
*/
/* internal function */
STATIC
int _tefmod16_gen3_getRevDetails(PHYMOD_ST* pc)
{
    MAIN0_SERDESIDr_t MAIN0_SERDESIDr_reg;

    MAIN0_SERDESIDr_CLR(MAIN0_SERDESIDr_reg);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SERDESIDr(pc, &MAIN0_SERDESIDr_reg));
    return MAIN0_SERDESIDr_GET(MAIN0_SERDESIDr_reg);
}

/* to update port_mode_select value.  If the update warrants a pll reset,
   then return accData=1, otherwise return accData=0.  The update shall
   support flex port technology. Called forced speed modes */

int tefmod16_gen3_update_port_mode_select(PHYMOD_ST* pc, tefmod16_gen3_port_type_t port_type, 
                                          int master_port, int tsc_clk_freq_pll_by_48, int pll_reset_en)
{
    MAIN0_SETUPr_t MAIN0_SETUPr_reg;
    MAIN0_SPD_CTLr_t MAIN0_SPD_CTLr_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

    PHYMOD_IF_ERR_RETURN (READ_MAIN0_SETUPr(pc, &MAIN0_SETUPr_reg));

    if((port_type == TEFMOD_MULTI_PORT) ||(port_type == TEFMOD_DXGXS)||
       (port_type == TEFMOD_SINGLE_PORT)||(port_type == TEFMOD_TRI1_PORT)||
       (port_type == TEFMOD_TRI2_PORT)){
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

    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc, MAIN0_SETUPr_reg));

    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUPr(pc, &MAIN0_SETUPr_reg));

    MAIN0_SPD_CTLr_CLR(MAIN0_SPD_CTLr_reg);
    MAIN0_SPD_CTLr_PLL_RESET_ENf_SET(MAIN0_SPD_CTLr_reg, pll_reset_en);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SPD_CTLr(pc, MAIN0_SPD_CTLr_reg));

    return PHYMOD_E_NONE;
}

/*!
@brief Init routine sets various operating modes of TEF.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  refclk  Reference clock
@param  plldiv  PLL Divider value
@param  port_type Port type as in enum #tefmod_port_type_t
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
int tefmod16_gen3_set_port_mode(PHYMOD_ST* pc, int refclk, int plldiv, tefmod16_gen3_port_type_t port_type,
                               int master_port, int tsc_clk_freq_pll_by_48, int pll_reset_en)
{
    uint16_t dataref;
    MAIN0_SETUPr_t MAIN0_SETUPr_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    tefmod16_gen3_update_port_mode_select(pc, port_type, master_port, tsc_clk_freq_pll_by_48, pll_reset_en);

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
} /* tefmod_set_port_mode(PHYMOD_ST* pc) */


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

int tefmod16_gen3_tx_loopback_control(PHYMOD_ST* pc, int pcs_gloop_en, int starting_lane, int num_lanes)
{
    MAIN0_LPBK_CTLr_t MAIN0_LOOPBACK_CONTROLr_reg;
    PMD_X4_OVRRr_t PMD_X4_OVERRIDEr_reg;
    PMD_X4_CTLr_t PMD_X4_PMD_X4_CONTROLr_reg;
    SC_X4_CTLr_t  SC_X4_CTLr_reg;
    uint16_t lane_mask, i, data, tmp_data;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

    /* write 0 to the speed change */
    SC_X4_CTLr_CLR(SC_X4_CTLr_reg);
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc, &SC_X4_CTLr_reg));
    SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CTLr_reg, 0);
    PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_CTLr(pc, SC_X4_CTLr_reg));

    MAIN0_LPBK_CTLr_CLR(MAIN0_LOOPBACK_CONTROLr_reg);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_LPBK_CTLr(pc, &MAIN0_LOOPBACK_CONTROLr_reg));
    tmp_data = MAIN0_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_GET(MAIN0_LOOPBACK_CONTROLr_reg);

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
}

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
int tefmod16_gen3_rx_loopback_control(PHYMOD_ST* pc, int pcs_rloop_en)
{
    MAIN0_LPBK_CTLr_t MAIN0_LOOPBACK_CONTROLr_reg;
    DSC_CDR_CTL2r_t DSC_C_CDR_CONTROL_2r_reg;
    TX_PI_CTL0r_t TX_PI_CTL0r_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

    /* remote device is set in rloop */
    MAIN0_LPBK_CTLr_CLR(MAIN0_LOOPBACK_CONTROLr_reg);
    MAIN0_LPBK_CTLr_REMOTE_PCS_LOOPBACK_ENABLEf_SET(MAIN0_LOOPBACK_CONTROLr_reg, pcs_rloop_en);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_LPBK_CTLr(pc, MAIN0_LOOPBACK_CONTROLr_reg));

    /* set Tx_PI */
    DSC_CDR_CTL2r_CLR(DSC_C_CDR_CONTROL_2r_reg);
    DSC_CDR_CTL2r_TX_PI_LOOP_TIMING_SRC_SELf_SET(DSC_C_CDR_CONTROL_2r_reg, 1);

    PHYMOD_IF_ERR_RETURN(MODIFY_DSC_CDR_CTL2r(pc, DSC_C_CDR_CONTROL_2r_reg));

    TX_PI_CTL0r_CLR(TX_PI_CTL0r_reg);
    TX_PI_CTL0r_TX_PI_ENf_SET(TX_PI_CTL0r_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_PI_CTL0r(pc, TX_PI_CTL0r_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_tx_loopback_get(PHYMOD_ST* pc, uint32_t *enable)
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
@details Per lane PMD ln_rst and ln_dp_rst by writing to PMD_X4_CONTROL(0xc010)in pcs space
*/
int tefmod16_gen3_pmd_x4_reset(PHYMOD_ST* pc)
{
    PMD_X4_CTLr_t reg_pmd_x4_ctrl;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details Per lane PMD ln_rst and ln_dp_rst by writing to PMD_X4_CONTROL(0xc010)in pcs space
*/
int tefmod16_gen3_pmd_x4_reset_get(PHYMOD_ST* pc, int *is_in_reset)
{
    PMD_X4_CTLr_t reg_pmd_x4_ctrl;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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
@param   spd_intf speed type #tefmod_spd_intfc_type_t
@param   os_mode over sample rate.
@returns The value PHYMOD_E_NONE upon successful completion
@details Per Port PMD Init
*/
int tefmod16_gen3_pmd_os_mode_set(PHYMOD_ST* pc, tefmod16_gen3_spd_intfc_type_t spd_intf, phymod_ref_clk_t refclk, int os_mode) 
{
    RXTXCOM_OSR_MODE_CTLr_t  RXTXCOM_OSR_MODE_CTLr_reg;
    int speed;
    phymod_access_t pa_copy;
    int i, start_lane = 0, num_lane = 0;
    uint32_t  lane_mask;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    lane_mask = pa_copy.lane_mask;

    RXTXCOM_OSR_MODE_CTLr_CLR(RXTXCOM_OSR_MODE_CTLr_reg);
    tefmod16_gen3_get_mapped_speed(spd_intf, &speed);

    /* osr_mode
     * 0 = OS MODE 1
     * 1 = OS MODE 2
     * 2 = OS MODE 4
     * 4 = OS MODE 21P25
     * 5 = OS MODE 8
     * 7 = OS MODE 20
     * 8 = OS MODE 16P5
     * 9 = OS MODE 16
     * 11 = OS MODE 8P25
     * 12 = OS MODE 20P625;
     * 13 = OS MODE 32
     * 15 = OS MODE 6P6
     */
    if (os_mode & 0x80000000) {
        os_mode = (os_mode) & 0x0000ffff;
    } else {
        if (refclk == phymodRefClk125Mhz) {
            os_mode =  sc_pmd16_gen3_entry_125M_ref[speed].pma_os_mode;
        } else {
            os_mode =  sc_pmd16_gen3_entry[speed].pma_os_mode;
        }
    }

    RXTXCOM_OSR_MODE_CTLr_OSR_MODE_FRCf_SET(RXTXCOM_OSR_MODE_CTLr_reg, 1);
    RXTXCOM_OSR_MODE_CTLr_OSR_MODE_FRC_VALf_SET(RXTXCOM_OSR_MODE_CTLr_reg, os_mode);

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(lane_mask, start_lane + i)) {
          continue;
        }
        pa_copy.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
          (MODIFY_RXTXCOM_OSR_MODE_CTLr(&pa_copy, RXTXCOM_OSR_MODE_CTLr_reg));
    }

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
int _tefmod16_gen3_wait_sc_stats_set(PHYMOD_ST* pc)
{
    uint16_t data = 0;
    uint16_t i = 0;
    SC_X4_STSr_t reg_sc_ctrl_sts;

    SC_X4_STSr_CLR(reg_sc_ctrl_sts);

    for (i= 0; i < 10; i++) {
        PHYMOD_USLEEP(1);
        PHYMOD_IF_ERR_RETURN(READ_SC_X4_STSr(pc, &reg_sc_ctrl_sts));
        data = SC_X4_STSr_SW_SPEED_CONFIG_VLDf_GET(reg_sc_ctrl_sts);
        if(data == 1) {
            return PHYMOD_E_NONE;
        }
    }

    return PHYMOD_E_TIMEOUT;
}

/**
@brief   Set the port speed and enable the port
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   spd_intf the speed to set the port enum #tefmod_set_spd_intf
@returns The value PHYMOD_E_NONE upon successful completion
@details Sets the port to the specified speed and triggers the port, and waits
         for the port to be configured
*/
int tefmod16_gen3_set_spd_intf(PHYMOD_ST *pc, tefmod16_gen3_spd_intfc_type_t spd_intf)
{
    SC_X4_CTLr_t xgxs_x4_ctrl;
    phymod_access_t pa_copy;
    int speed_id = 0, start_lane = 0, num_lane = 0;
    TX_X4_TX_CTL0r_t TX_X4_TX_CTL0r_reg;
    RX_X4_PCS_CTL0r_t RX_X4_PCS_CTL0r_reg;
    int tx_fec_mode;
    int rx_fec_mode;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    TEFMOD16_GEN3_DBG_IN_FUNC_VIN_INFO(pc,("spd_intf: %d", spd_intf));

    /* Need to figure out what's the starting lane */
    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
    pa_copy.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN (tefmod16_gen3_get_mapped_speed(spd_intf, &speed_id));
    if (speed_id == sc_x4_control_sc_S_25G_X1) {
        TX_X4_TX_CTL0r_CLR(TX_X4_TX_CTL0r_reg);
        RX_X4_PCS_CTL0r_CLR(RX_X4_PCS_CTL0r_reg);

        PHYMOD_IF_ERR_RETURN(READ_TX_X4_TX_CTL0r(pc, &TX_X4_TX_CTL0r_reg));
        PHYMOD_IF_ERR_RETURN(READ_RX_X4_PCS_CTL0r(pc, &RX_X4_PCS_CTL0r_reg));

        tx_fec_mode = TX_X4_TX_CTL0r_CL91_FEC_MODEf_GET(TX_X4_TX_CTL0r_reg);
        rx_fec_mode = RX_X4_PCS_CTL0r_CL91_FEC_MODEf_GET(RX_X4_PCS_CTL0r_reg);

        /*In IEEE 25G FEC, speed id is S_25G_KR_IEEE (0x72)*/
        if ((tx_fec_mode == TEFMOD16_GEN3_IEEE_25G_CL91_SINGLE_LANE)
          && (rx_fec_mode == TEFMOD16_GEN3_IEEE_25G_CL91_SINGLE_LANE)) {
            speed_id = sc_x4_control_sc_S_25G_KR_IEEE;
        }
    }

    /* Write the speed_id into the speed_change register */
    SC_X4_CTLr_CLR(xgxs_x4_ctrl);
    SC_X4_CTLr_SPEEDf_SET(xgxs_x4_ctrl, speed_id);
    MODIFY_SC_X4_CTLr(pc, xgxs_x4_ctrl);
    /*next call the speed_change routine */
    PHYMOD_IF_ERR_RETURN (tefmod16_gen3_trigger_speed_change(&pa_copy));
    /* Check the speed_change_done nit*/
    PHYMOD_IF_ERR_RETURN (_tefmod16_gen3_wait_sc_stats_set(pc));

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
int tefmod16_gen3_set_an_single_port_mode(PHYMOD_ST* pc, int an_enable)
{
    MAIN0_SETUPr_t  reg_setup;
    SC_X4_CTLr_t xgxs_x4_ctrl;
    phymod_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));
    SC_X4_CTLr_CLR(xgxs_x4_ctrl);

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    TEFMOD16_GEN3_DBG_IN_FUNC_VIN_INFO(pc,("an_enable %d \n", an_enable));

    MAIN0_SETUPr_CLR(reg_setup);
    PHYMOD_IF_ERR_RETURN (READ_MAIN0_SETUPr(pc, &reg_setup));

    /* Ref clock selection tied to 156.25MHz */
    MAIN0_SETUPr_REFCLK_SELf_SET(reg_setup, main0_refClkSelect_clk_156p25MHz);

    /* Set single port mode*/
    MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(reg_setup, an_enable);
    if (an_enable) {
        SC_X4_CTLr_SW_SPEED_CHANGEf_SET(xgxs_x4_ctrl, 0);
        phy_copy.lane_mask= 0xf;
        MODIFY_SC_X4_CTLr(&phy_copy, xgxs_x4_ctrl);
    } else {
        SC_X4_CTLr_SW_SPEED_CHANGEf_SET(xgxs_x4_ctrl, 1);
        phy_copy.lane_mask= 0xf;
        MODIFY_SC_X4_CTLr(pc, xgxs_x4_ctrl);
    }
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc,reg_setup));

    return PHYMOD_E_NONE;
}

/**
@brief   Set port_mode_sel for AN of TSCF.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   num_of_lanes Number of lanes in this port
@param   starting_lane first lane in the port
@returns PHYMOD_E_NONE if successful. PHYMOD_E_FAIL else.
@details

This function is called to set the proper port_mode_sel when AN mode is enabled.
*/
int tefmod16_gen3_set_an_port_mode(PHYMOD_ST* pc, int num_of_lanes, int starting_lane)
{
    MAIN0_SETUPr_t  reg_setup;
    int port_mode_sel=0, new_port_mode_sel=0;

    MAIN0_SETUPr_CLR(reg_setup);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUPr(pc, &reg_setup));
    port_mode_sel = MAIN0_SETUPr_PORT_MODE_SELf_GET(reg_setup);

    switch(port_mode_sel) {
    case TEFMOD_MULTI_PORT:  /* 3, 2, 1, 0; QUAD */
        if (num_of_lanes==2) {
            if(starting_lane==0) {
                new_port_mode_sel = TEFMOD_TRI2_PORT;
            } else {
                new_port_mode_sel = TEFMOD_TRI1_PORT;
            }
        } else {
            new_port_mode_sel = TEFMOD_MULTI_PORT;
        }
        break;
    case TEFMOD_TRI1_PORT:  /* 3-2, 1, 0; TRI_1 */
        if (num_of_lanes==2 && starting_lane==0) {
            new_port_mode_sel = TEFMOD_DXGXS;
        } else if (num_of_lanes==4 || (num_of_lanes==1 && starting_lane >= 2)) {
            new_port_mode_sel = TEFMOD_MULTI_PORT;
        } else {
            new_port_mode_sel = TEFMOD_TRI1_PORT;
        }
        break;
    case TEFMOD_TRI2_PORT: /* 3, 2, 1-0; TRI_2 */
        if (num_of_lanes==2 && starting_lane==2) {
            new_port_mode_sel = TEFMOD_DXGXS;
        } else if (num_of_lanes==4 || (num_of_lanes==1 && starting_lane <= 1)) {
            new_port_mode_sel = TEFMOD_MULTI_PORT;
        } else {
            new_port_mode_sel = TEFMOD_TRI2_PORT;
        }
        break;
    case TEFMOD_DXGXS: /* 3-2, 1-0; DUAL */
        if (num_of_lanes==1 && starting_lane <= 1) {
            new_port_mode_sel = TEFMOD_TRI1_PORT;
        } else if (num_of_lanes==1 && starting_lane >= 2) {
            new_port_mode_sel = TEFMOD_TRI2_PORT;
        } else if (num_of_lanes==2) {
            new_port_mode_sel = TEFMOD_DXGXS;
        } else {
            new_port_mode_sel = TEFMOD_MULTI_PORT;
        }
        break;
    case TEFMOD_SINGLE_PORT: /* 3-2-1-0; SINGLE PORT */
        if (num_of_lanes==2) {
            new_port_mode_sel = TEFMOD_DXGXS;
        } else if (num_of_lanes==1 && starting_lane <= 1) {
            new_port_mode_sel = TEFMOD_TRI1_PORT;
        } else if (num_of_lanes==1 && starting_lane >= 2) {
            new_port_mode_sel = TEFMOD_TRI2_PORT;
        } else {
            new_port_mode_sel = TEFMOD_MULTI_PORT;
        }
        break;
      default:
          return PHYMOD_E_FAIL;
          break ;
    }
    MAIN0_SETUPr_PORT_MODE_SELf_SET(reg_setup, new_port_mode_sel);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc, reg_setup));

    return PHYMOD_E_NONE;
}

/*!
@brief supports per field override
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  per_field_override_en Controls which field to override
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details
  TBD
*/
int tefmod16_gen3_set_override_0(PHYMOD_ST* pc, int per_field_override_en)
{
    int or_value;
    override_type_t or_en;
    RX_X4_PCS_CTL0r_t    RX_X4_CONTROL0_PCS_CONTROL_0r_reg;
    SC_X4_SC_X4_OVRRr_t  SC_X4_CONTROL_OVERRIDEr_reg;
    TX_X4_ENC0r_t        TX_X4_CONTROL0_ENCODE_0r_reg;
    TX_X4_MISCr_t        TX_X4_CONTROL0_MISCr_reg;
    SC_X4_FLD_OVRR_EN0_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg;
    SC_X4_FLD_OVRR_EN1_TYPEr_t SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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
            SC_X4_SC_X4_OVRRr_CLR(SC_X4_CONTROL_OVERRIDEr_reg);
            SC_X4_SC_X4_OVRRr_NUM_LANES_OVERRIDE_VALUEf_SET(SC_X4_CONTROL_OVERRIDEr_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_SC_X4_OVRRr(pc, SC_X4_CONTROL_OVERRIDEr_reg));
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

            /* Set the override enable*/
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
            TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);
            TX_X4_ENC0r_T_FIFO_MODEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc, TX_X4_CONTROL0_ENCODE_0r_reg));

            /* Set the override enable*/
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

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_T_ENC_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_T_ENC_MODE_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_T_ENC_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_T_HG2_ENABLE:
            TX_X4_ENC0r_CLR(TX_X4_CONTROL0_ENCODE_0r_reg);
            TX_X4_ENC0r_HG2_ENABLEf_SET(TX_X4_CONTROL0_ENCODE_0r_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc, TX_X4_CONTROL0_ENCODE_0r_reg));
            /* Set the override enable*/
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
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_T_PMA_BTMX_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_T_PMA_BTMX_MODE_OEN_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_T_PMA_BTMX_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_SCR_MODE:
            TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
            TX_X4_MISCr_SCR_MODEf_SET(TX_X4_CONTROL0_MISCr_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_SCR_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_SCR_MODE_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_SCR_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_DESCR_MODE_OEN:
            RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
            RX_X4_PCS_CTL0r_DESCR_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_DESCR_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_DESCR_MODE_OEN_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_DESCR_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_DEC_TL_MODE:
            RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
            RX_X4_PCS_CTL0r_DEC_TL_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_DEC_TL_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_DEC_TL_MODE_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_DEC_TL_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_DESKEW_MODE:
            RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
            RX_X4_PCS_CTL0r_DESKEW_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_DESKEW_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_DESKEW_MODE_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_DESKEW_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_DEC_FSM_MODE:
            RX_X4_PCS_CTL0r_CLR(RX_X4_CONTROL0_PCS_CONTROL_0r_reg);
            RX_X4_PCS_CTL0r_DEC_FSM_MODEf_SET(RX_X4_CONTROL0_PCS_CONTROL_0r_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, RX_X4_CONTROL0_PCS_CONTROL_0r_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_DEC_FSM_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_DEC_FSM_MODE_DIS:
            /* Set the override enable*/
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
int tefmod16_gen3_set_override_1(PHYMOD_ST* pc, int per_lane_control, uint32_t per_field_override_en)   /* OVERRIDE_SET */
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

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    or_en = (override_type_t) (per_field_override_en >> 16);
    or_value = per_field_override_en & 0x0000ffff;

    switch(or_en) {
        case OVERRIDE_R_HG2_ENABLE:
            TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
            TX_X4_MISCr_SET(TX_X4_CONTROL0_MISCr_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_R_HG2_ENABLE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_R_HG2_ENABLE_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_R_HG2_ENABLE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_BS_SM_SYNC_MODE_OEN:
            RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);
            RX_X4_BLKSYNC_CFGr_BS_SM_SYNC_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_BS_SM_SYNC_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_BS_SM_SYNC_MODE_OEN_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_BS_SM_SYNC_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_BS_SYNC_EN_OEN:
            RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);
            RX_X4_BLKSYNC_CFGr_BS_SYNC_ENf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_BS_SYNC_EN_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_BS_SYNC_EN_OEN_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_BS_SYNC_EN_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_BS_DIST_MODE_OEN:
            RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);
            RX_X4_BLKSYNC_CFGr_BS_DIST_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_BS_DIST_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_BS_DIST_MODE_OEN_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_BS_DIST_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_BS_BTMX_MODE_OEN:
            RX_X4_BLKSYNC_CFGr_CLR(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg);
            RX_X4_BLKSYNC_CFGr_BS_BTMX_MODEf_SET(RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_BLKSYNC_CFGr(pc, RX_X4_CONTROL0_BLKSYNC_CONFIGr_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_BS_BTMX_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_BS_BTMX_MODE_OEN_DIS:
            /*Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_BS_BTMX_MODE_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_CL72_EN:
            MAIN0_SETUPr_CLR(MAIN0_SETUPr_reg);
            MAIN0_SETUPr_CL72_ENf_SET(MAIN0_SETUPr_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SETUPr(pc, MAIN0_SETUPr_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_CL72_EN_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_CL72_EN_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN0_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg);
            SC_X4_FLD_OVRR_EN0_TYPEr_CL72_EN_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN0_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE0_TYPEr_reg));
            break;
        case OVERRIDE_CLOCKCNT0:
            TX_X4_CRED0r_CLR(TX_X4_CREDIT0_CREDIT0r_reg);
            TX_X4_CRED0r_CLOCKCNT0f_SET(TX_X4_CREDIT0_CREDIT0r_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED0r(pc, TX_X4_CREDIT0_CREDIT0r_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT0_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_CLOCKCNT0_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT0_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_CLOCKCNT1:
            TX_X4_CRED1r_CLR(TX_X4_CREDIT0_CREDIT1r_reg);
            TX_X4_CRED1r_CLOCKCNT1f_SET(TX_X4_CREDIT0_CREDIT1r_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_CRED1r(pc, TX_X4_CREDIT0_CREDIT1r_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT1_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_CLOCKCNT1_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_CLOCKCNT1_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_LOOPCNT0:
            TX_X4_LOOPCNTr_CLR(TX_X4_CREDIT0_LOOPCNTr_reg);
            TX_X4_LOOPCNTr_LOOPCNT0f_SET(TX_X4_CREDIT0_LOOPCNTr_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_LOOPCNTr(pc, TX_X4_CREDIT0_LOOPCNTr_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT0_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_LOOPCNT0_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT0_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_LOOPCNT1:
            TX_X4_LOOPCNTr_CLR(TX_X4_CREDIT0_LOOPCNTr_reg);
            TX_X4_LOOPCNTr_LOOPCNT1f_SET(TX_X4_CREDIT0_LOOPCNTr_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_LOOPCNTr(pc, TX_X4_CREDIT0_LOOPCNTr_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT1_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_LOOPCNT1_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_LOOPCNT1_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_MAC_CREDITGENCNT:
            TX_X4_MAC_CREDGENCNTr_CLR(TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg);
            TX_X4_MAC_CREDGENCNTr_MAC_CREDITGENCNTf_SET(TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg, or_value);
            PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MAC_CREDGENCNTr(pc, TX_X4_CREDIT0_MAC_CREDITGENCNTr_reg));

            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_MAC_CREDITGENCNT_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 1);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
            break;
        case OVERRIDE_MAC_CREDITGENCNT_DIS:
            /* Set the override enable*/
            SC_X4_FLD_OVRR_EN1_TYPEr_CLR(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg);
            SC_X4_FLD_OVRR_EN1_TYPEr_MAC_CREDITGENCNT_OENf_SET(SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg, 0);
            PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_FLD_OVRR_EN1_TYPEr(pc, SC_X4_FIELD_OVERRIDE_ENABLE_FIELD_OVERRIDE_ENABLE1_TYPEr_reg));
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
int tefmod16_gen3_credit_control(PHYMOD_ST* pc, int per_lane_control)
{
    int cntl;
    TX_X4_CRED0r_t TX_X4_CREDIT0_CREDIT0r_reg;
    cntl = per_lane_control;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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
@param  tx_dis_type tx property to control, see enum #tefmod_tx_disable_enum_t
@returns The value PHYMOD_E_NONE upon successful completion.
@details
This function is used to reset tx lane and enable/disable tx lane of any
transmit lane.  Set bit 0 of per_lane_control to enable the TX_LANE
(1) or disable the TX lane (0).
When diable TX lane, two types of operations are inovked independently.
If per_lane_control bit [7:4] = 1, only traffic is disabled.
      (TEFMOD_TX_LANE_TRAFFIC =0x10)
If bit [7:4] = 2, only reset function is invoked.
      (TEFMOD_TX_LANE_RESET = 0x20)

This function reads/modifies the following registers:

\li rstb_tx_lane  :   0xc113[1]
\li enable_tx_lane:   0xc113[0]
*/

int tefmod16_gen3_tx_lane_control_set(PHYMOD_ST* pc,  tefmod16_gen3_tx_disable_enum_t tx_dis_type)
{
    TX_X4_MISCr_t TX_X4_CONTROL0_MISCr_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);

    switch(tx_dis_type) {
        case TEFMOD_TX_LANE_RESET:
            TX_X4_MISCr_RSTB_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 0);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
            TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
            TX_X4_MISCr_RSTB_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 1);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
            break;
        case TEFMOD_TX_LANE_TRAFFIC_ENABLE:
            TX_X4_MISCr_ENABLE_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 1);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
            break;
        case TEFMOD_TX_LANE_TRAFFIC_DISABLE:
            TX_X4_MISCr_ENABLE_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 0);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
            break;
        case TEFMOD_TX_LANE_RESET_TRAFFIC_ENABLE:
            TX_X4_MISCr_RSTB_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 1);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
            TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
            TX_X4_MISCr_ENABLE_TX_LANEf_SET(TX_X4_CONTROL0_MISCr_reg, 1);
            PHYMOD_IF_ERR_RETURN (MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
            break;
        case TEFMOD_TX_LANE_RESET_TRAFFIC_DISABLE:
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
int tefmod16_gen3_tx_lane_control_get(PHYMOD_ST* pc,  int *reset, int *enable)
{
    TX_X4_MISCr_t TX_X4_CONTROL0_MISCr_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);

    PHYMOD_IF_ERR_RETURN (READ_TX_X4_MISCr(pc, &TX_X4_CONTROL0_MISCr_reg));
    *reset = TX_X4_MISCr_RSTB_TX_LANEf_GET(TX_X4_CONTROL0_MISCr_reg);
    *enable = TX_X4_MISCr_ENABLE_TX_LANEf_GET(TX_X4_CONTROL0_MISCr_reg);

    return PHYMOD_E_NONE;
}

/*!
@brief Read the 16 bit rev. id value etc.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  revIdV receives the revid
@returns The value of the revid.
@details
This  fucntion reads the revid register that contains core number, revision
number and returns it.
*/
int tefmod16_gen3_revid_read(PHYMOD_ST* pc, uint32_t *revIdV)
{
    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

    *revIdV=_tefmod16_gen3_getRevDetails(pc);
    return PHYMOD_E_NONE;
}

int tefmod16_gen3_clause72_control(PHYMOD_ST* pc, int cl72en)
{
    int cl72_enable;
    SC_X4_CTLr_t                      SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg;
    CL93N72_IT_BASE_R_PMD_CTLr_t      CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg;
    int start_lane = 0, num_lane = 0 , i=0;
    uint32_t enable = 0;
    phymod_access_t pa_copy;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    cl72_enable  = cl72en & 0x1;

    /* Need to figure out what's the starting lane */
    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
    pa_copy.lane_mask = 0x1 << start_lane;

    CL93N72_IT_BASE_R_PMD_CTLr_CLR(CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg);
    PHYMOD_IF_ERR_RETURN(READ_CL93N72_IT_BASE_R_PMD_CTLr(&pa_copy, &CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg));

    if(CL93N72_IT_BASE_R_PMD_CTLr_CL93N72_IEEE_TRAINING_ENABLEf_GET(CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg)
        != (uint32_t)cl72_enable) {

        for (i = (num_lane-1); i >= 0; i--) {
            pa_copy.lane_mask = 0x1 << (i + start_lane);
            CL93N72_IT_BASE_R_PMD_CTLr_CLR(CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg);
            PHYMOD_IF_ERR_RETURN(READ_CL93N72_IT_BASE_R_PMD_CTLr(&pa_copy, &CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg));
            CL93N72_IT_BASE_R_PMD_CTLr_CL93N72_IEEE_TRAINING_ENABLEf_SET(CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg, cl72_enable);
            PHYMOD_IF_ERR_RETURN(MODIFY_CL93N72_IT_BASE_R_PMD_CTLr(&pa_copy, CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_CONTROLr_reg));
        }

        pa_copy.lane_mask = 0x1 << start_lane;
        tefmod16_gen3_enable_get(&pa_copy,&enable);
        if(enable == 0x1) {
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

int tefmod16_gen3_fec_override_set(PHYMOD_ST* pc, uint32_t enable)
{
    SC_X4_SC_X4_OVRRr_t sc_x4_sc_x4_overr_reg;

    SC_X4_SC_X4_OVRRr_CLR(sc_x4_sc_x4_overr_reg);
    SC_X4_SC_X4_OVRRr_AN_FEC_SEL_OVERRIDEf_SET(sc_x4_sc_x4_overr_reg, enable);
    PHYMOD_IF_ERR_RETURN
        (MODIFY_SC_X4_SC_X4_OVRRr(pc, sc_x4_sc_x4_overr_reg));

    return PHYMOD_E_NONE;
}


int tefmod16_gen3_fec_override_get(PHYMOD_ST* pc, uint32_t *enable)
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
@param  fec_type  fec type select tefmod16_gen3_fec_type_t
                  TEFMOD_CL74/TEFMOD_CL91/TEFMOD_CL108
@param  enable    enalbe/disable for various fec feature

@returns PHYMOD_E_NONE if no errors. PHYMOD_E_ERROR else.

@details
To use cl91 in forced speed mode (in 100G only) Tx only fec_en=TEFMOD_CL91_TX_EN_DIS
To use cl91 in forced speed mode (in 100G only) Rx only fec_en=TEFMOD_CL91_RX_EN_DIS
To use cl91 in forced speed mode (in 100G only) Tx Rx   fec_en=(TEFMOD_CL91_TX_EN_DIS|TEFMOD_CL91_RX_EN_DIS)

To enable cl74 Tx only fec_en = TEFMOD_CL74_TX_EN_DIS
To disable cl74 Tx Rx fec_dis =(TEFMOD_CL74_TX_EN_DIS|TEFMOD_CL74_RX_EN_DIS)

Note: cl74 will be enabled per port.
      cl91 is used only in 100G (so all 4 lanes make a port)

      cl74or91 is only used in autoneg. And other parm used in forced speed.
*/


int tefmod16_gen3_FEC_control(PHYMOD_ST* pc, tefmod16_gen3_fec_type_t fec_type, int enable)
{
    phymod_access_t pa_copy;
    int start_lane = 0, num_lane = 0 ;
    uint32_t local_enable = 0;
    tefmod16_gen3_cl91_fec_mode_t fec_mode = TEFMOD16_GEN3_CL91_COUNT;
    tefmod16_gen3_cl91_fec_mode_t current_fec_mode = TEFMOD16_GEN3_CL91_COUNT;
    TX_X4_TX_CTL0r_t tx_x4_tx_ctrl_reg;
    RX_X4_PCS_CTL0r_t rx_x4_pcs_crtl_reg;
    TX_X4_MISCr_t TX_X4_CONTROL0_MISCr_reg;
    SC_X4_CTLr_t SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg;
    int speed_id = 0, current_speed_id = 0;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
    pa_copy.lane_mask = 0x1 << start_lane;

    TX_X4_TX_CTL0r_CLR(tx_x4_tx_ctrl_reg);
    RX_X4_PCS_CTL0r_CLR(rx_x4_pcs_crtl_reg);
    TX_X4_MISCr_CLR(TX_X4_CONTROL0_MISCr_reg);
    SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);


    PHYMOD_IF_ERR_RETURN
        (READ_SC_X4_CTLr(pc, &SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));
    PHYMOD_IF_ERR_RETURN
        (READ_TX_X4_TX_CTL0r(pc, &tx_x4_tx_ctrl_reg));
    PHYMOD_IF_ERR_RETURN
        (READ_RX_X4_PCS_CTL0r(pc, &rx_x4_pcs_crtl_reg));
    PHYMOD_IF_ERR_RETURN
        (READ_TX_X4_MISCr(pc, &TX_X4_CONTROL0_MISCr_reg));

    current_fec_mode = TX_X4_TX_CTL0r_CL91_FEC_MODEf_GET(tx_x4_tx_ctrl_reg) ;

    if(enable < TEFMOD_PHY_CONTROL_FEC_AUTO) {
        if (fec_type == TEFMOD_CL91) {
            if (enable) {
                switch (num_lane) {
                    case 1:
                        fec_mode = TEFMOD16_GEN3_CL91_SINGLE_LANE_BRCM_PROP;
                        break;
                    case 2:
                        fec_mode = TEFMOD16_GEN3_CL91_DUAL_LANE_BRCM_PROP;
                        break;
                    case 4:
                        fec_mode = TEFMOD16_GEN3_CL91_QUAD_LANE;
                        break;
                    default:
                        fec_mode = TEFMOD16_GEN3_CL91_SINGLE_LANE_BRCM_PROP;
                        break;
                }
            } else {
                if (current_fec_mode == TEFMOD16_GEN3_IEEE_25G_CL91_SINGLE_LANE) {
                    return PHYMOD_E_INTERNAL;
                } else {
                    fec_mode = TEFMOD16_GEN3_NO_CL91_FEC;
                }
            }

            /*In Broadcom 25G FEC or in no FEC mode, speed id is S_25G_X1 (0x12)*/
            if (num_lane == 1) {
                if((fec_mode == TEFMOD16_GEN3_CL91_SINGLE_LANE_BRCM_PROP)
                    || (fec_mode == TEFMOD16_GEN3_NO_CL91_FEC))
                {
                    tefmod16_gen3_speed_id_get(pc, &speed_id);
                    /*If current speed id is IEEE 25G, it need to be recoverd to Broadcom 25G*/
                    if (speed_id == sc_x4_control_sc_S_25G_KR_IEEE) {
                        speed_id = sc_x4_control_sc_S_25G_X1;
                        SC_X4_CTLr_SPEEDf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, speed_id);
                        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));
                    }
                }
            }

            TX_X4_TX_CTL0r_CL91_FEC_MODEf_SET(tx_x4_tx_ctrl_reg, fec_mode);
            RX_X4_PCS_CTL0r_CL91_FEC_MODEf_SET(rx_x4_pcs_crtl_reg, fec_mode);
            PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_TX_CTL0r(pc, tx_x4_tx_ctrl_reg));
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, rx_x4_pcs_crtl_reg));
        } else if (fec_type == TEFMOD_CL108) {
            if (num_lane != 1) {
                return PHYMOD_E_INTERNAL;
            }

            if (enable) {
                fec_mode = TEFMOD16_GEN3_IEEE_25G_CL91_SINGLE_LANE;
            } else {
                if (current_fec_mode != TEFMOD16_GEN3_IEEE_25G_CL91_SINGLE_LANE) {
                    return PHYMOD_E_INTERNAL;
                } else {
                    fec_mode = TEFMOD16_GEN3_NO_CL91_FEC;
                }
            }

            tefmod16_gen3_speed_id_get(pc, &current_speed_id);
            speed_id = current_speed_id;

            if ((current_speed_id == sc_x4_control_sc_S_25G_KR_IEEE) ||
                (current_speed_id == sc_x4_control_sc_S_25G_X1)) {
                if (fec_mode == TEFMOD16_GEN3_NO_CL91_FEC) {
                    speed_id = sc_x4_control_sc_S_25G_X1;
                } else if (fec_mode == TEFMOD16_GEN3_IEEE_25G_CL91_SINGLE_LANE) {
                    speed_id = sc_x4_control_sc_S_25G_KR_IEEE;
                }

                if (speed_id != current_speed_id) {
                    SC_X4_CTLr_SPEEDf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, speed_id);
                    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));
                }
            }

            TX_X4_TX_CTL0r_CL91_FEC_MODEf_SET(tx_x4_tx_ctrl_reg, fec_mode);
            RX_X4_PCS_CTL0r_CL91_FEC_MODEf_SET(rx_x4_pcs_crtl_reg, fec_mode);
            PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_TX_CTL0r(pc, tx_x4_tx_ctrl_reg));
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, rx_x4_pcs_crtl_reg));
        } else /* cl74 */ {
            if (enable) {
                TX_X4_MISCr_FEC_ENABLEf_SET(TX_X4_CONTROL0_MISCr_reg, 1);
                RX_X4_PCS_CTL0r_FEC_ENABLEf_SET(rx_x4_pcs_crtl_reg, 1);
            } else {
                TX_X4_MISCr_FEC_ENABLEf_SET(TX_X4_CONTROL0_MISCr_reg, 0);
                RX_X4_PCS_CTL0r_FEC_ENABLEf_SET(rx_x4_pcs_crtl_reg, 0);
            }
            PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_MISCr(pc, TX_X4_CONTROL0_MISCr_reg));
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, rx_x4_pcs_crtl_reg));
        }
    }

    tefmod16_gen3_enable_get(&pa_copy,&local_enable);
    if (local_enable) {
        SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);
        SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, 0);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

        SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);
        SC_X4_CTLr_SW_SPEED_CHANGEf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, 1);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(&pa_copy, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));
    }

    return PHYMOD_E_NONE;
}


int tefmod16_gen3_FEC_get(PHYMOD_ST* pc,  tefmod16_gen3_fec_type_t fec_type, int *fec_en)
{
    SC_X4_FEC_STSr_t SC_X4_FEC_STSr_reg;
    uint32_t fec_mode;
    int speed_id;
    int rev_letter;
    MAIN0_SERDESIDr_t serdesid;

    *fec_en = 0;
    SC_X4_FEC_STSr_CLR(SC_X4_FEC_STSr_reg);
    READ_SC_X4_FEC_STSr(pc, &SC_X4_FEC_STSr_reg);
    fec_mode = SC_X4_FEC_STSr_T_CL91_FEC_MODEf_GET(SC_X4_FEC_STSr_reg);

    if ((fec_type == TEFMOD_CL91) || (fec_type == TEFMOD_CL108)) {
        switch (fec_mode) {
        case TEFMOD16_GEN3_NO_CL91_FEC:
            *fec_en = 0;
            break;
        case TEFMOD16_GEN3_CL91_SINGLE_LANE_BRCM_PROP:
            READ_MAIN0_SERDESIDr(pc, &serdesid);
            rev_letter = MAIN0_SERDESIDr_REV_LETTERf_GET(serdesid);

            /* revision A, B can't distinguish CL91 and CL108 FEC, so check speed ID */
            if ((rev_letter == REV_A) || (rev_letter == REV_B)) {
                PHYMOD_IF_ERR_RETURN(tefmod16_gen3_speed_id_get(pc, &speed_id));
                if (speed_id == sc_x4_control_sc_S_25G_KR_IEEE || speed_id == sc_x4_control_sc_S_25G_CR_IEEE) {
                    *fec_en = (fec_type == TEFMOD_CL108 ? 1 : 0);
                } else {
                   *fec_en = (fec_type == TEFMOD_CL91 ? 1 : 0);
                }
            } else {
                *fec_en = (fec_type == TEFMOD_CL91 ? 1 : 0);
            }
            break;
        case TEFMOD16_GEN3_CL91_SINGLE_LANE_FC:
        case TEFMOD16_GEN3_CL91_DUAL_LANE_BRCM_PROP:
        case TEFMOD16_GEN3_CL91_QUAD_LANE:
            *fec_en = (fec_type == TEFMOD_CL91 ? 1 : 0);
            break;
        case TEFMOD16_GEN3_IEEE_25G_CL91_SINGLE_LANE:
            *fec_en = (fec_type == TEFMOD_CL108 ? 1 : 0);
            break;
        default:
            break;
        }
    } else {
        *fec_en = SC_X4_FEC_STSr_T_FEC_ENABLEf_GET(SC_X4_FEC_STSr_reg);
    }

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
int tefmod16_gen3_power_control(PHYMOD_ST* pc, int tx, int rx)
{
    PMD_X4_CTLr_t PMD_X4_PMD_X4_CONTROLr_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    PMD_X4_CTLr_CLR(PMD_X4_PMD_X4_CONTROLr_reg);

    PMD_X4_CTLr_LN_RX_H_PWRDNf_SET(PMD_X4_PMD_X4_CONTROLr_reg, rx);
    PMD_X4_CTLr_LN_TX_H_PWRDNf_SET(PMD_X4_PMD_X4_CONTROLr_reg, tx);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_CTLr(pc, PMD_X4_PMD_X4_CONTROLr_reg));

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

int tefmod16_gen3_refclk_get(PHYMOD_ST* pc, phymod_ref_clk_t* ref_clk)
{
    MAIN0_SETUPr_t MAIN0_SETUPr_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    MAIN0_SETUPr_CLR(MAIN0_SETUPr_reg);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SETUPr(pc, &MAIN0_SETUPr_reg));
    *ref_clk = MAIN0_SETUPr_REFCLK_SELf_GET(MAIN0_SETUPr_reg);

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_pmd_lane_swap(PHYMOD_ST *pc, int tx_lane_swap, int rx_lane_swap)
{
    unsigned int tx_map, rx_map;
    LN_ADDR0r_t LN_ADDR0r_reg;
    LN_ADDR1r_t LN_ADDR1r_reg;
    LN_ADDR2r_t LN_ADDR2r_reg;
    LN_ADDR3r_t LN_ADDR3r_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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

int tefmod16_gen3_pcs_tx_lane_swap(PHYMOD_ST *pc, int tx_lane_swap)
{
    unsigned int map;
    TX_X1_TX_LN_SWPr_t TX_X1_TX_LN_SWPr_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    map = (unsigned int)tx_lane_swap;

    TX_X1_TX_LN_SWPr_CLR(TX_X1_TX_LN_SWPr_reg);
    TX_X1_TX_LN_SWPr_LOGICAL2_TO_PHY_SELf_SET(TX_X1_TX_LN_SWPr_reg, ((map >> 8)  & 0x3));
    TX_X1_TX_LN_SWPr_LOGICAL3_TO_PHY_SELf_SET(TX_X1_TX_LN_SWPr_reg, ((map >> 12) & 0x3));
    TX_X1_TX_LN_SWPr_LOGICAL1_TO_PHY_SELf_SET(TX_X1_TX_LN_SWPr_reg, ((map >> 4)  & 0x3));
    TX_X1_TX_LN_SWPr_LOGICAL0_TO_PHY_SELf_SET(TX_X1_TX_LN_SWPr_reg, ((map >> 0)  & 0x3));
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X1_TX_LN_SWPr(pc, TX_X1_TX_LN_SWPr_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_pcs_rx_lane_swap(PHYMOD_ST *pc, int rx_lane_swap)
{
  unsigned int map;
  RX_X1_RX_LN_SWPr_t RX_X1_RX_LN_SWPr_reg;

  TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
  map = (unsigned int)rx_lane_swap;

  RX_X1_RX_LN_SWPr_CLR(RX_X1_RX_LN_SWPr_reg);
  RX_X1_RX_LN_SWPr_LOGICAL3_TO_PHY_SELf_SET(RX_X1_RX_LN_SWPr_reg, ((map >> 12) & 0x3));
  RX_X1_RX_LN_SWPr_LOGICAL2_TO_PHY_SELf_SET(RX_X1_RX_LN_SWPr_reg, ((map >> 8)  & 0x3));
  RX_X1_RX_LN_SWPr_LOGICAL1_TO_PHY_SELf_SET(RX_X1_RX_LN_SWPr_reg, ((map >> 4)  & 0x3));
  RX_X1_RX_LN_SWPr_LOGICAL0_TO_PHY_SELf_SET(RX_X1_RX_LN_SWPr_reg, ((map >> 0)  & 0x3));

  PHYMOD_IF_ERR_RETURN(MODIFY_RX_X1_RX_LN_SWPr(pc, RX_X1_RX_LN_SWPr_reg));

  return PHYMOD_E_NONE;
}

int tefmod16_gen3_pcs_tx_lane_swap_get ( PHYMOD_ST *pc,  uint32_t *tx_swap)
{
  unsigned int pcs_map;
  TX_X1_TX_LN_SWPr_t reg;

  TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

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

int tefmod16_gen3_pcs_rx_lane_swap_get ( PHYMOD_ST *pc,  uint32_t *rx_swap)
{
  unsigned int pcs_map;
  RX_X1_RX_LN_SWPr_t reg;

  TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

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

/*!
@brief tefmod_poll_for_sc_done , polls for the sc_done bit,
@returns The value PHYMOD_E_NONE upon successful completion.
@details
Reads the SC_X4_COntrol_status reg and polls it to see if sc_done bit is set
Sets the sc_done flag=1/0, if speed control is done, after resolving to the correct speed
*/
int tefmod16_gen3_poll_for_sc_done(PHYMOD_ST* pc, int mapped_speed)
{
    int done;
    int spd_match, sc_done;
    int cnt;
    uint16_t data;
    SC_X4_STSr_t SC_X4_CONTROL_SC_X4_CONTROL_STATUSr_reg;
    SC_X4_RSLVD_SPDr_t SC_X4_FINAL_CONFIG_STATUS_RESOLVED_SPEEDr_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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

#ifdef _SDK_TEFMOD16_GEN3_
/*!
@brief Controls the init setting/resetting of autoneg  registers.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@param  an_init_st structure tefmod_an_init_t with all the one time autoneg init cfg.
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Get aneg features via #tefmod_an_init_t.
  This does not start the autoneg. That is done in tefmod_autoneg_control
*/
int tefmod16_gen3_autoneg_set_init(PHYMOD_ST* pc, tefmod16_gen3_an_init_t *an_init_st) /* AUTONEG_SET */
{
    AN_X4_CL73_CTLSr_t AN_X4_CL73_CTLSr_t_reg;
    AN_X4_LD_BASE_ABIL1r_t AN_X4_ABILITIES_LD_BASE_ABILITIES_1r_reg;
    AN_X4_LD_BASE_ABIL0r_t AN_X4_ABILITIES_LD_BASE_ABILITIES_0r_reg;
    AN_X4_CL73_CFGr_t AN_X4_CL73_CFGr_t_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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
#endif /* _SDK_TEFMOD16_GEN3_ */

/*!
@brief Controls the setting/resetting of autoneg timers.
@param  pc handle to current TSCF context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details

*/
int tefmod16_gen3_autoneg_timer_init(PHYMOD_ST* pc)               /* AUTONEG timer set*/
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

    AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_SET(AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r_reg, 0x3080);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r(pc, AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r_reg));

    AN_X1_LNK_FAIL_INHBT_TMR_CL72r_SET(AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r_reg, 0x8382);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_LNK_FAIL_INHBT_TMR_CL72r(pc, AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_credit_clock_set(PHYMOD_ST* pc, tefmod16_gen3_credit_clock_type_t clock)
{
    MAIN0_SPD_CTLr_t main_spd_ctlr_reg;
    MAIN0_SPD_CTLr_CLR(main_spd_ctlr_reg);
    MAIN0_SPD_CTLr_TSC_CREDIT_SELf_SET(main_spd_ctlr_reg, clock);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SPD_CTLr(pc, main_spd_ctlr_reg));

    return PHYMOD_E_NONE;
}

/**
@brief   To get autoneg control registers.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   an_control reference to struct with autoneg parms #tefmod_an_control_t
@param   an_complete status of AN completion
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Upper layer software calls this function to get autoneg
         info.
*/

int tefmod16_gen3_autoneg_control_get(PHYMOD_ST* pc, tefmod16_gen3_an_control_t *an_control, int *an_complete)
{
    AN_X4_CL73_CFGr_t  AN_X4_CL73_CFGr_reg;
    AN_X4_CL73_CTLSr_t AN_X4_CL73_CTLSr_reg;
    AN_X4_AN_MISC_STSr_t  AN_X4_AN_MISC_STSr_reg;
    AN_X4_LD_BASE_ABIL3r_t AN_X4_LD_BASE_ABIL3r_reg;
    AN_X4_LD_BASE_ABIL4r_t AN_X4_LD_BASE_ABIL4r_reg;
    AN_X4_LD_BASE_ABIL5r_t AN_X4_LD_BASE_ABIL5r_reg;
    uint32_t    msa_mode, base_ability,base_ability_1, base_ability_2;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

    PHYMOD_IF_ERR_RETURN(READ_AN_X4_CL73_CTLSr(pc, &AN_X4_CL73_CTLSr_reg));
    an_control->pd_kx_en = AN_X4_CL73_CTLSr_PD_KX_ENf_GET(AN_X4_CL73_CTLSr_reg);
    an_control->pd_2P5G_kx_en = AN_X4_CL73_CTLSr_PD_2P5G_KX_ENf_GET(AN_X4_CL73_CTLSr_reg);

    /*Setting X4 abilities*/
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_CL73_CFGr(pc, &AN_X4_CL73_CFGr_reg));

    if (AN_X4_CL73_CFGr_CL73_BAM_ENABLEf_GET(AN_X4_CL73_CFGr_reg) == 1) {
        PHYMOD_IF_ERR_RETURN(tefmod16_gen3_an_msa_mode_get(pc, &msa_mode));
        if (msa_mode) {
            /*
             * Read the msa_mode value.
             * If it is 1, it is a MSA/IEEE_MSA AN mode.
             * Read the base page values (abil_3 & abil_4).
             * If it is set, then set AN mode to IEEE_MSA, else set it to MSA mode.
 */

            PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_BASE_ABIL3r(pc, &AN_X4_LD_BASE_ABIL3r_reg));
            PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_BASE_ABIL4r(pc, &AN_X4_LD_BASE_ABIL4r_reg));
            PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_BASE_ABIL5r(pc, &AN_X4_LD_BASE_ABIL5r_reg));
            base_ability =  AN_X4_LD_BASE_ABIL3r_GET(AN_X4_LD_BASE_ABIL3r_reg) & 0x0fff;
            base_ability_1 = AN_X4_LD_BASE_ABIL4r_GET(AN_X4_LD_BASE_ABIL4r_reg) & 0x0fff;
            base_ability_2 = AN_X4_LD_BASE_ABIL5r_GET(AN_X4_LD_BASE_ABIL5r_reg) & 0x0fff;

            if (!(base_ability) && !(base_ability_1) && !(base_ability_2)) {
                an_control->an_type = TEFMOD_AN_MODE_MSA;
                an_control->enable = 1;
            } else {
                an_control->an_type = TEFMOD_AN_MODE_CL73_MSA;
                an_control->enable = 1;
            }
        } else {
            an_control->an_type = TEFMOD_AN_MODE_CL73BAM;
            an_control->enable = 1;
        }
    } else if (AN_X4_CL73_CFGr_CL73_HPAM_ENABLEf_GET(AN_X4_CL73_CFGr_reg) == 1) {
        an_control->an_type = TEFMOD_AN_MODE_HPAM;
        an_control->enable = 1;
    } else if (AN_X4_CL73_CFGr_CL73_ENABLEf_GET(AN_X4_CL73_CFGr_reg) == 1) {
        an_control->an_type = TEFMOD_AN_MODE_CL73;
        an_control->enable = 1;
    } else {
        an_control->an_type = TEFMOD_AN_MODE_NONE;
        an_control->enable = 0;
    }

    if(AN_X4_CL73_CFGr_AD_TO_CL73_ENf_GET(AN_X4_CL73_CFGr_reg) == 1) {
        an_control->an_property_type = TEFMOD_AN_PROPERTY_ENABLE_HPAM_TO_CL73_AUTO;
    } else if(AN_X4_CL73_CFGr_BAM_TO_HPAM_AD_ENf_GET(AN_X4_CL73_CFGr_reg) == 1) {
        an_control->an_property_type = TEFMOD_AN_PROPERTY_ENABLE_CL73_BAM_TO_HPAM_AUTO;
    }

    an_control->num_lane_adv = AN_X4_CL73_CFGr_NUM_ADVERTISED_LANESf_GET(AN_X4_CL73_CFGr_reg);

    /* an_complete status */
    AN_X4_AN_MISC_STSr_CLR(AN_X4_AN_MISC_STSr_reg);
    PHYMOD_IF_ERR_RETURN (READ_AN_X4_AN_MISC_STSr(pc, &AN_X4_AN_MISC_STSr_reg));
    *an_complete = AN_X4_AN_MISC_STSr_AN_COMPLETEf_GET(AN_X4_AN_MISC_STSr_reg);

    return PHYMOD_E_NONE;
}

/**
@brief   To get local autoneg advertisement registers.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   an_ability_st receives autoneg info. #tefmod_an_adv_ability_t)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Upper layer software calls this function to get local autoneg
         info. This function is currently not implemented
*/

int tefmod16_gen3_autoneg_ability_get(PHYMOD_ST* pc, tefmod16_gen3_an_adv_ability_t *an_ability_st)
{
    AN_X4_LD_BASE_ABIL1r_t AN_X4_LD_BASE_ABIL1r_reg;
    AN_X4_LD_UP1_ABIL0r_t  AN_X4_LD_UP1_ABIL0r_reg;
    AN_X4_LD_UP1_ABIL1r_t  AN_X4_LD_UP1_ABIL1r_reg;
    AN_X4_LD_BASE_ABIL3r_t AN_X4_LD_BASE_ABIL3r_reg;
    AN_X4_LD_BASE_ABIL4r_t AN_X4_LD_BASE_ABIL4r_reg;
    AN_X4_LD_BASE_ABIL5r_t AN_X4_LD_BASE_ABIL5r_reg;
    AN_X4_LD_FEC_BASEPAGE_ABILr_t AN_X4_LD_FEC_BASEPAGE_ABILr_reg;
    int         an_fec = 0;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

    /****** Getting local device UP1 abilities 0xC1C1 ******/
    READ_AN_X4_LD_UP1_ABIL0r(pc, &AN_X4_LD_UP1_ABIL0r_reg);
    an_ability_st->an_bam_speed = AN_X4_LD_UP1_ABIL0r_GET(AN_X4_LD_UP1_ABIL0r_reg) & 0x3ff;
    an_ability_st->an_hg2 = AN_X4_LD_UP1_ABIL0r_BAM_HG2f_GET(AN_X4_LD_UP1_ABIL0r_reg);

    /****** Getting local device UP1 abilities_1 0xC1C2 ******/
    READ_AN_X4_LD_UP1_ABIL1r(pc, &AN_X4_LD_UP1_ABIL1r_reg);
    an_ability_st->an_bam_speed1 = AN_X4_LD_UP1_ABIL1r_GET(AN_X4_LD_UP1_ABIL1r_reg) & 0x1f;

    /****** Getting local device base abilities_1 0xC1C4 ******/
    READ_AN_X4_LD_BASE_ABIL1r(pc, &AN_X4_LD_BASE_ABIL1r_reg);
    an_ability_st->an_base_speed = AN_X4_LD_BASE_ABIL1r_GET(AN_X4_LD_BASE_ABIL1r_reg) & 0x3f;

    /****** Getting local device base abilities_1 0xC1C4 ******/
    an_ability_st->an_pause = AN_X4_LD_BASE_ABIL1r_CL73_PAUSEf_GET(AN_X4_LD_BASE_ABIL1r_reg);

    an_fec = AN_X4_LD_BASE_ABIL1r_FEC_REQf_GET(AN_X4_LD_BASE_ABIL1r_reg);
    if (an_fec == 0x3) { /* FEC_SUPPORTED_AND_REQUESTED */
        if (AN_X4_LD_BASE_ABIL1r_BASE_100G_CR4f_GET(AN_X4_LD_BASE_ABIL1r_reg) ||
            AN_X4_LD_BASE_ABIL1r_BASE_100G_KR4f_GET(AN_X4_LD_BASE_ABIL1r_reg) ) {
            an_ability_st->an_fec = TEFMOD_FEC_CL91_SUPRTD_REQSTD;
        } else {
            an_ability_st->an_fec = TEFMOD_FEC_CL74_SUPRTD_REQSTD;
        }
    } else {
        an_ability_st->an_fec = (an_fec==0x1) ? TEFMOD_FEC_SUPRTD_NOT_REQSTD : TEFMOD_FEC_NOT_SUPRTD;
    }

    an_fec = 0;
    AN_X4_LD_BASE_ABIL3r_CLR(AN_X4_LD_BASE_ABIL3r_reg);
    AN_X4_LD_BASE_ABIL4r_CLR(AN_X4_LD_BASE_ABIL4r_reg);
    AN_X4_LD_BASE_ABIL5r_CLR(AN_X4_LD_BASE_ABIL5r_reg);
    AN_X4_LD_FEC_BASEPAGE_ABILr_CLR(AN_X4_LD_FEC_BASEPAGE_ABILr_reg);

    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_BASE_ABIL3r(pc, &AN_X4_LD_BASE_ABIL3r_reg));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_BASE_ABIL4r(pc, &AN_X4_LD_BASE_ABIL4r_reg));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_BASE_ABIL5r(pc, &AN_X4_LD_BASE_ABIL5r_reg));

    /****** Getting local device base abilities_3 0xC1C8 ******/
    if (AN_X4_LD_BASE_ABIL3r_BASE_25G_KR1_ENf_GET(AN_X4_LD_BASE_ABIL3r_reg)) {
        an_ability_st->an_base_speed |= (1<<TEFMOD_CL73_25GBASE_KR1);
    }
    if (AN_X4_LD_BASE_ABIL3r_BASE_25G_CR1_ENf_GET(AN_X4_LD_BASE_ABIL3r_reg)) {
        an_ability_st->an_base_speed |= (1<<TEFMOD_CL73_25GBASE_CR1);
    }
    /****** Getting local device base abilities_4 0xC1C9 ******/
    if (AN_X4_LD_BASE_ABIL4r_BASE_25G_KRS1_ENf_GET(AN_X4_LD_BASE_ABIL4r_reg)) {
        an_ability_st->an_base_speed |= (1<<TEFMOD_CL73_25GBASE_KRS1);
    }
    if (AN_X4_LD_BASE_ABIL4r_BASE_25G_CRS1_ENf_GET(AN_X4_LD_BASE_ABIL4r_reg)) {
        an_ability_st->an_base_speed |= (1<<TEFMOD_CL73_25GBASE_CRS1);
    }
    /****** Getting local device base abilities_5 0xC1CA ******/
    if (AN_X4_LD_BASE_ABIL5r_BASE_2P5G_ENf_GET(AN_X4_LD_BASE_ABIL5r_reg)) {
        an_ability_st->an_base_speed |= (1<<TEFMOD_CL73_2P5GBASE_KX1);
    }
    if (AN_X4_LD_BASE_ABIL5r_BASE_5P0G_ENf_GET(AN_X4_LD_BASE_ABIL5r_reg)) {
        an_ability_st->an_base_speed |= (1<<TEFMOD_CL73_5GBASE_KR1);
    }

    /****** Getting local device fec basepage abilities 0xC1Cb ******/
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_FEC_BASEPAGE_ABILr(pc, &AN_X4_LD_FEC_BASEPAGE_ABILr_reg));
    if (AN_X4_LD_FEC_BASEPAGE_ABILr_BASE_R_FEC_REQ_ENf_GET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg)) {
        an_fec |= 0x1;
    }
    if (AN_X4_LD_FEC_BASEPAGE_ABILr_RS_FEC_REQ_ENf_GET(AN_X4_LD_FEC_BASEPAGE_ABILr_reg)) {
        an_fec |= 0x2;
    }
    if ((an_ability_st->an_fec == TEFMOD_FEC_SUPRTD_NOT_REQSTD) ||
          (an_ability_st->an_fec == TEFMOD_FEC_NOT_SUPRTD)) {
        if (an_fec) {
            an_ability_st->an_fec = 0;
            if (an_fec & 0x1) {
                an_ability_st->an_fec |= TEFMOD_FEC_CL74_SUPRTD_REQSTD;
            }
            if (an_fec & 0x2) {
                an_ability_st->an_fec |= TEFMOD_FEC_CL91_SUPRTD_REQSTD;
            }
        }
    } else {
        if (an_fec & 0x1) {
            an_ability_st->an_fec |= TEFMOD_FEC_CL74_SUPRTD_REQSTD;
        }
        if (an_fec & 0x2) {
            an_ability_st->an_fec |= TEFMOD_FEC_CL91_SUPRTD_REQSTD;
        }
    }

    return PHYMOD_E_NONE;
}


/**
@brief   Controls port RX squelch
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable is the control to  RX  squelch
@returns The value PHYMOD_E_NONE upon successful completion.
@details Controls port RX squelch
*/
int tefmod16_gen3_rx_squelch_set(PHYMOD_ST *pc, int enable)
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
int tefmod16_gen3_tx_squelch_set(PHYMOD_ST *pc, int enable)
{
    TXFIR_MISC_CTL0r_t  TXFIR_MISC_CTL0r_reg;
    phymod_access_t pa_copy;
    int i, start_lane = 0, num_lane = 0;

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    TXFIR_MISC_CTL0r_CLR(TXFIR_MISC_CTL0r_reg);
    pa_copy.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(READ_TXFIR_MISC_CTL0r(&pa_copy, &TXFIR_MISC_CTL0r_reg));

    TXFIR_MISC_CTL0r_SDK_TX_DISABLEf_SET(TXFIR_MISC_CTL0r_reg, enable);

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(pc->lane_mask, start_lane + i)) {
            continue;
        }
        pa_copy.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN(MODIFY_TXFIR_MISC_CTL0r(&pa_copy, TXFIR_MISC_CTL0r_reg));
    }

    return PHYMOD_E_NONE;
}

/**
@brief   Controls port TX/RX squelch
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable is the control to  TX/RX  squelch. Enable=1 means enable the port,no squelch
@returns The value PHYMOD_E_NONE upon successful completion.
@details Controls port TX/RX squelch
*/
int tefmod16_gen3_port_enable_set(PHYMOD_ST *pc, int enable)
{
    if (enable)  {
        tefmod16_gen3_rx_squelch_set(pc, 0);
        tefmod16_gen3_tx_squelch_set(pc, 0);
    } else {
        tefmod16_gen3_rx_squelch_set(pc, 1);
        tefmod16_gen3_tx_squelch_set(pc, 1);
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
int tefmod16_gen3_tx_squelch_get(PHYMOD_ST *pc, int *val)
{
    TXFIR_MISC_CTL0r_t TXFIR_MISC_CTL0r_reg;

    TXFIR_MISC_CTL0r_CLR(TXFIR_MISC_CTL0r_reg);
    PHYMOD_IF_ERR_RETURN(READ_TXFIR_MISC_CTL0r(pc, &TXFIR_MISC_CTL0r_reg));
    *val = TXFIR_MISC_CTL0r_SDK_TX_DISABLEf_GET(TXFIR_MISC_CTL0r_reg);

    return PHYMOD_E_NONE;
}


/**
@brief   Gets port RX squelch settings
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   val Receiver for status of  RX  squelch
@returns The value PHYMOD_E_NONE upon successful completion.
@details Gets port RX squelch settings
*/
int tefmod16_gen3_rx_squelch_get(PHYMOD_ST *pc, int *val)
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
int tefmod16_gen3_port_enable_get(PHYMOD_ST *pc, int *tx_enable, int *rx_enable)
{

    tefmod16_gen3_rx_squelch_get(pc, rx_enable);
    tefmod16_gen3_tx_squelch_get(pc, tx_enable);

    return PHYMOD_E_NONE;
}

/**
@brief   To get remote autoneg advertisement registers.
@param   pc handle to current TSCF context (#PHYMOD_ST)
@param   an_ability_st receives autoneg info. #tefmod_an_adv_ability_t)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Upper layer software calls this function to get autoneg
         info. This function is not currently implemented.
*/
int tefmod16_gen3_autoneg_remote_ability_get(PHYMOD_ST* pc, tefmod16_gen3_an_adv_ability_t *an_ability_st)
{
    uint32_t data;
    uint32_t user_code;
    AN_X4_LP_BASE1r_t     AN_X4_LP_BASE1r_reg;
    AN_X4_LP_BASE3r_t     AN_X4_LP_BASE3r_reg;
    AN_X4_LP_OUI_UP3r_t   AN_X4_LP_OUI_UP3r_reg;
    AN_X4_LP_OUI_UP4r_t   AN_X4_LP_OUI_UP4r_reg;
    AN_X4_LD_BAM_ABILr_t  AN_X4_LD_BAM_ABILr_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE1r(pc, &AN_X4_LP_BASE1r_reg));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE3r(pc, &AN_X4_LP_BASE3r_reg));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_OUI_UP3r(pc, &AN_X4_LP_OUI_UP3r_reg));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_OUI_UP4r(pc, &AN_X4_LP_OUI_UP4r_reg));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_BAM_ABILr(pc, &AN_X4_LD_BAM_ABILr_reg));

    /* For now it is fine, because the Local would be same as remote. DOUBLE CHECK THIS CODE */
    /*an_ability_st->cl73_adv.an_bam_speed = 0xdead;*/
    an_ability_st->an_bam_speed = 0x0;

    user_code = AN_X4_LD_BAM_ABILr_CL73_BAM_CODEf_GET(AN_X4_LD_BAM_ABILr_reg) << 11 ;
    data = AN_X4_LP_OUI_UP4r_reg.v[0];

    if(data&(1<<1)) {
        an_ability_st->an_bam_speed |=(1<<0) ;
    } else {
        user_code |= data ;
        if(user_code == 0xabe20)
            an_ability_st->an_bam_speed  |=(1<<0) ;
    }
    if(data&(1<<0)) an_ability_st->an_bam_speed |=(1<<1) ;

    an_ability_st->an_pause = (AN_X4_LP_BASE1r_reg.v[0] >> 10 ) & 0x3;
    an_ability_st->an_fec = (AN_X4_LP_BASE3r_reg.v[0] >> 14) & 0x3;

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
int tefmod16_gen3_eee_control_set(PHYMOD_ST* pc, uint32_t enable)
{
    RX_X4_PCS_CTL0r_t reg_pcs_ctrl0;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

    RX_X4_PCS_CTL0r_CLR(reg_pcs_ctrl0);
    RX_X4_PCS_CTL0r_LPI_ENABLEf_SET(reg_pcs_ctrl0, enable & 0x1);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_PCS_CTL0r(pc, reg_pcs_ctrl0));

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
int tefmod16_gen3_eee_control_get(PHYMOD_ST* pc, uint32_t *enable)
{
    RX_X4_PCS_CTL0r_t reg_pcs_ctrl0;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
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
int tefmod16_gen3_cl74_chng_default (PHYMOD_ST* pc)
{
    RX_X4_FEC1r_t RX_X4_FEC1reg;
    phymod_access_t pa_copy;
    int i;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    RX_X4_FEC1r_CLR(RX_X4_FEC1reg);
    for (i = 0; i <4; i++) {
         pa_copy.lane_mask = 0x1 << i;
        PHYMOD_IF_ERR_RETURN(WRITE_RX_X4_FEC1r(&pa_copy, RX_X4_FEC1reg));
    }
    return PHYMOD_E_NONE;
}

int tefmod16_gen3_cl91_ecc_clear(PHYMOD_ST* pc)
{
    RX_X1_ECC_STS_FEC_MEM0r_t RX_X1_ECC_STS_FEC_MEM0r_reg;
    RX_X1_ECC_STS_FEC_MEM1r_t RX_X1_ECC_STS_FEC_MEM1r_reg;
    RX_X1_ECC_STS_FEC_MEM2r_t RX_X1_ECC_STS_FEC_MEM2r_reg;
    RX_X1_ECC_STS_FEC_MEM3r_t RX_X1_ECC_STS_FEC_MEM3r_reg;
    RX_X1_ECC_INTR_STS_1BITr_t RX_X1_ECC_INTR_STS_1BITr_reg;
    RX_X1_ECC_INTR_STS_2BITr_t RX_X1_ECC_INTR_STS_2BITr_reg;

    RX_X4_ECC_STS_CL91_BUFFER_BLK0r_t RX_X4_ECC_STS_CL91_BUFFER_BLK0r_reg;
    RX_X4_ECC_STS_CL91_BUFFER_BLK1r_t RX_X4_ECC_STS_CL91_BUFFER_BLK1r_reg;
    RX_X4_ECC_STS_CL91_BUFFER_BLK2r_t RX_X4_ECC_STS_CL91_BUFFER_BLK2r_reg;
    RX_X4_ECC_STS_DESKEW_MEM0r_t RX_X4_ECC_STS_DESKEW_MEM0r_reg;
    RX_X4_ECC_STS_DESKEW_MEM1r_t RX_X4_ECC_STS_DESKEW_MEM1r_reg;
    RX_X4_ECC_STS_DESKEW_MEM2r_t RX_X4_ECC_STS_DESKEW_MEM2r_reg;
    RX_X4_ECC_INTR_STS_1BITr_t RX_X4_ECC_INTR_STS_1BITr_reg;
    RX_X4_ECC_INTR_STS_2BITr_t RX_X4_ECC_INTR_STS_2BITr_reg;
    phymod_access_t pa_copy;
    int i;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

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

    /* Read Clear  */
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_ECC_STS_FEC_MEM0r(&pa_copy, &RX_X1_ECC_STS_FEC_MEM0r_reg));
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_ECC_STS_FEC_MEM1r(&pa_copy, &RX_X1_ECC_STS_FEC_MEM1r_reg));
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_ECC_STS_FEC_MEM2r(&pa_copy, &RX_X1_ECC_STS_FEC_MEM2r_reg));
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_ECC_STS_FEC_MEM3r(&pa_copy, &RX_X1_ECC_STS_FEC_MEM3r_reg));
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_ECC_INTR_STS_1BITr(&pa_copy, &RX_X1_ECC_INTR_STS_1BITr_reg));
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_ECC_INTR_STS_2BITr(&pa_copy, &RX_X1_ECC_INTR_STS_2BITr_reg));

    for (i = 0; i < 4; i++) {
        pa_copy.lane_mask = 0x1 << i;
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

int tefmod16_gen3_25g_rsfec_am_init(PHYMOD_ST *pc)
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
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_IEEE_25G_CTLr(pc, &ieee_25G_am_reg));
    SC_X4_IEEE_25G_CTLr_IEEE_25G_AM0_FORMATf_SET(ieee_25G_am_reg, 1);
    PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_IEEE_25G_CTLr(pc, ieee_25G_am_reg));

    /* MSA Mode Program 25G AM0 value to use 100G AM
     * The rest AM should use 40G AM, which is a default
     */
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_MSA_25G_50G_CTLr(pc, &msa_25g_50g_am_reg));
    SC_X4_MSA_25G_50G_CTLr_MSA_25G_AM0_FORMATf_SET(msa_25g_50g_am_reg, 1);
    PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_MSA_25G_50G_CTLr(pc, msa_25g_50g_am_reg));

    /* Enable AM parallel detect for in case if the legacy devices use 40G AM */
    PHYMOD_IF_ERR_RETURN(READ_RX_X4_IEEE_25G_PARLLEL_DET_CTRr(pc, &ieee_25g_parallel_det_reg));
    RX_X4_IEEE_25G_PARLLEL_DET_CTRr_SET(ieee_25g_parallel_det_reg, 1);
    PHYMOD_IF_ERR_RETURN(WRITE_RX_X4_IEEE_25G_PARLLEL_DET_CTRr(pc, ieee_25g_parallel_det_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_an_msa_mode_set(PHYMOD_ST* pc, uint32_t val)
{
    AN_X4_LD_PAGE0r_t AN_X4_LD_PAGE0r_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    AN_X4_LD_PAGE0r_CLR(AN_X4_LD_PAGE0r_reg);
    AN_X4_LD_PAGE0r_SET(AN_X4_LD_PAGE0r_reg, val & 0x1);

    /********Setting AN_X4_LD_PAGE0r_t 0xC1E2*****/
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X4_LD_PAGE0r(pc, AN_X4_LD_PAGE0r_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_an_msa_mode_get(PHYMOD_ST* pc, uint32_t* val)
{
    AN_X4_LD_PAGE0r_t AN_X4_LD_PAGE0r_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

    /******Getting AN_X4_LD_PAGE0r_t 0xC1E2******/
    AN_X4_LD_PAGE0r_CLR(AN_X4_LD_PAGE0r_reg);
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LD_PAGE0r(pc, &AN_X4_LD_PAGE0r_reg));
    *val = AN_X4_LD_PAGE0r_LD_PAGE_0f_GET(AN_X4_LD_PAGE0r_reg);

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_an_oui_set(PHYMOD_ST* pc, tefmod16_gen3_an_oui_t oui)
{
    AN_X1_OUI_LWRr_t   AN_X1_CONTROL_OUI_LOWERr_reg;
    AN_X1_OUI_UPRr_t   AN_X1_CONTROL_OUI_UPPERr_reg;
    AN_X4_CL73_CTLSr_t AN_X4_CL73_CTLSr_t_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

    /********Setting AN_X1_OUI_LWR 0x9241*****/
    AN_X1_OUI_LWRr_CLR(AN_X1_CONTROL_OUI_LOWERr_reg);
    AN_X1_OUI_LWRr_SET(AN_X1_CONTROL_OUI_LOWERr_reg, oui.oui & 0xffff);
    PHYMOD_IF_ERR_RETURN(WRITE_AN_X1_OUI_LWRr(pc, AN_X1_CONTROL_OUI_LOWERr_reg));

    /********Setting AN_X1_OUI_UPR 0x9240*****/
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

int tefmod16_gen3_an_oui_get(PHYMOD_ST* pc, tefmod16_gen3_an_oui_t* oui)
{
    AN_X1_OUI_LWRr_t   AN_X1_CONTROL_OUI_LOWERr_reg;
    AN_X1_OUI_UPRr_t   AN_X1_CONTROL_OUI_UPPERr_reg;
    AN_X4_CL73_CTLSr_t AN_X4_CL73_CTLSr_t_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);

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

int tefmod16_gen3_autoneg_status_get(PHYMOD_ST* pc, int *an_en, int *an_done)
{
    AN_X4_CL73_CFGr_t  AN_X4_CL73_CFGr_reg;
    AN_X4_AN_MISC_STSr_t  AN_X4_AN_MISC_STSr_reg;

    TEFMOD16_GEN3_DBG_IN_FUNC_INFO(pc);
    AN_X4_CL73_CFGr_CLR(AN_X4_CL73_CFGr_reg);
    AN_X4_AN_MISC_STSr_CLR(AN_X4_AN_MISC_STSr_reg);

    PHYMOD_IF_ERR_RETURN (READ_AN_X4_CL73_CFGr(pc, &AN_X4_CL73_CFGr_reg));
    PHYMOD_IF_ERR_RETURN (READ_AN_X4_AN_MISC_STSr(pc, &AN_X4_AN_MISC_STSr_reg));

    *an_en = AN_X4_CL73_CFGr_CL73_AN_RESTARTf_GET(AN_X4_CL73_CFGr_reg);
    *an_done = AN_X4_AN_MISC_STSr_AN_COMPLETEf_GET(AN_X4_AN_MISC_STSr_reg);

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_autoneg_lp_status_get(PHYMOD_ST* pc, uint16_t *baseP0, uint16_t *baseP1, uint16_t *baseP2, uint16_t *nextP3, uint16_t *nextP4)
{
    AN_X4_LP_BASE1r_t   AN_X4_LP_BASE1r_reg;
    AN_X4_LP_BASE2r_t   AN_X4_LP_BASE2r_reg;
    AN_X4_LP_BASE3r_t   AN_X4_LP_BASE3r_reg;
    AN_X4_LP_OUI_UP4r_t AN_X4_LP_OUI_UP4r_reg;
    AN_X4_LP_OUI_UP5r_t AN_X4_LP_OUI_UP5r_reg;

    AN_X4_LP_BASE1r_CLR(AN_X4_LP_BASE1r_reg);
    AN_X4_LP_BASE2r_CLR(AN_X4_LP_BASE2r_reg);
    AN_X4_LP_BASE3r_CLR(AN_X4_LP_BASE3r_reg);
    AN_X4_LP_OUI_UP4r_CLR(AN_X4_LP_OUI_UP4r_reg);
    AN_X4_LP_OUI_UP5r_CLR(AN_X4_LP_OUI_UP5r_reg);

    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE1r(pc,  &AN_X4_LP_BASE1r_reg));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE2r(pc,  &AN_X4_LP_BASE2r_reg));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_BASE3r(pc,  &AN_X4_LP_BASE3r_reg));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_OUI_UP4r(pc,&AN_X4_LP_OUI_UP4r_reg));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_LP_OUI_UP5r(pc,&AN_X4_LP_OUI_UP5r_reg));

    *baseP0 = AN_X4_LP_BASE1r_GET(AN_X4_LP_BASE1r_reg);
    *baseP1 = AN_X4_LP_BASE2r_GET(AN_X4_LP_BASE2r_reg);
    *baseP2 = AN_X4_LP_BASE3r_GET(AN_X4_LP_BASE3r_reg);
    *nextP3 = AN_X4_LP_OUI_UP4r_GET(AN_X4_LP_OUI_UP4r_reg);
    *nextP4 = AN_X4_LP_OUI_UP5r_GET(AN_X4_LP_OUI_UP5r_reg);

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_hg2_codec_set(PHYMOD_ST* pc, tefmod16_gen3_hg2_codec_t hg2_codec)
{
    TX_X4_ENC0r_t  TX_X4_ENC0_reg;
    RX_X4_DEC_CTL0r_t  RX_X4_DEC_CTL0_reg;

    PHYMOD_IF_ERR_RETURN(READ_TX_X4_ENC0r(pc, &TX_X4_ENC0_reg));
    PHYMOD_IF_ERR_RETURN(READ_RX_X4_DEC_CTL0r(pc, &RX_X4_DEC_CTL0_reg));
    switch (hg2_codec) {
    case TEFMOD_HG2_CODEC_OFF:
        TX_X4_ENC0r_HG2_ENABLEf_SET(TX_X4_ENC0_reg, 0);
        TX_X4_ENC0r_HG2_CODECf_SET(TX_X4_ENC0_reg, 1);
        TX_X4_ENC0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(TX_X4_ENC0_reg, 1);
        RX_X4_DEC_CTL0r_HG2_ENABLEf_SET(RX_X4_DEC_CTL0_reg, 0);
        RX_X4_DEC_CTL0r_HG2_CODECf_SET(RX_X4_DEC_CTL0_reg, 1);
        RX_X4_DEC_CTL0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(RX_X4_DEC_CTL0_reg, 1);
        break;
    case TEFMOD_HG2_CODEC_ON_8BYTE_IPG:
        TX_X4_ENC0r_HG2_ENABLEf_SET(TX_X4_ENC0_reg, 1);
        TX_X4_ENC0r_HG2_CODECf_SET(TX_X4_ENC0_reg, 1);
        TX_X4_ENC0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(TX_X4_ENC0_reg, 1);
        RX_X4_DEC_CTL0r_HG2_ENABLEf_SET(RX_X4_DEC_CTL0_reg, 1);
        RX_X4_DEC_CTL0r_HG2_CODECf_SET(RX_X4_DEC_CTL0_reg, 1);
        RX_X4_DEC_CTL0r_HG2_MESSAGE_INVALID_CODE_ENABLEf_SET(RX_X4_DEC_CTL0_reg, 1);
        break;
    case TEFMOD_HG2_CODEC_ON_9BYTE_IPG:
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
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_DEC_CTL0r(pc, RX_X4_DEC_CTL0_reg));
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_ENC0r(pc,TX_X4_ENC0_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_hg2_codec_get(PHYMOD_ST* pc, tefmod16_gen3_hg2_codec_t* hg2_codec)
{
    TX_X4_ENC0r_t  TX_X4_ENC0_reg;

    PHYMOD_IF_ERR_RETURN(READ_TX_X4_ENC0r(pc, &TX_X4_ENC0_reg));
    if (TX_X4_ENC0r_HG2_ENABLEf_GET(TX_X4_ENC0_reg)) {
        if (TX_X4_ENC0r_HG2_CODECf_GET(TX_X4_ENC0_reg)) {
            *hg2_codec = TEFMOD_HG2_CODEC_ON_8BYTE_IPG;
        } else {
            *hg2_codec = TEFMOD_HG2_CODEC_ON_9BYTE_IPG;
        }
    } else {
        *hg2_codec = TEFMOD_HG2_CODEC_OFF;
    }
    return PHYMOD_E_NONE;
}


int tefmod16_gen3_encode_mode_get(PHYMOD_ST* pc, tefmod16_gen3_encode_mode *mode)
{
    SC_X4_RSLVD0r_t    reg_resolved;

    SC_X4_RSLVD0r_CLR(reg_resolved);

    PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD0r(pc, &reg_resolved));
    *mode = SC_X4_RSLVD0r_T_ENC_MODEf_GET(reg_resolved);

  return PHYMOD_E_NONE;
}

int tefmod16_gen3_fec_correctable_counter_get(PHYMOD_ST* pc, uint32_t* count)
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
    uint32_t sum = 0, count_vl = 0;

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

int tefmod16_gen3_fec_uncorrectable_counter_get(PHYMOD_ST* pc, uint32_t* count)
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

int tefmod16_gen3_fec_cl91_correctable_counter_get(PHYMOD_ST* pc, uint32_t* count)
{
    RX_X4_FEC_CORR_CTR0r_t count0;
    RX_X4_FEC_CORR_CTR1r_t count1;

    /* Per port based counter */
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORR_CTR0r(pc, &count0));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_CORR_CTR1r(pc, &count1));
    *count = (RX_X4_FEC_CORR_CTR1r_GET(count1) << 16)
                 | (RX_X4_FEC_CORR_CTR0r_GET(count0) & 0xffff);

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_fec_cl91_uncorrectable_counter_get(PHYMOD_ST* pc, uint32_t* count)
{
    RX_X4_FEC_UNCORR_CTR0r_t count0;
    RX_X4_FEC_UNCORR_CTR1r_t count1;

    /* Per port based counter */
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORR_CTR0r(pc, &count0));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_FEC_UNCORR_CTR1r(pc, &count1));
    *count = (RX_X4_FEC_UNCORR_CTR1r_GET(count1) << 16)
                 | (RX_X4_FEC_UNCORR_CTR0r_GET(count0) & 0xffff);

    return PHYMOD_E_NONE;
}

/* The number of cl82 BIP error */
int tefmod16_gen3_bip_error_counter_get(PHYMOD_ST* pc, uint32_t* count)
{
    RX_X4_BIPCNT0r_t count0;
    RX_X4_BIPCNT1r_t count1;
    RX_X4_BIPCNT2r_t count2;

    PHYMOD_IF_ERR_RETURN (READ_RX_X4_BIPCNT0r(pc, &count0));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_BIPCNT1r(pc, &count1));
    PHYMOD_IF_ERR_RETURN (READ_RX_X4_BIPCNT2r(pc, &count2));
    *count =  RX_X4_BIPCNT0r_BIP_ERROR_COUNT_0f_GET(count0) + RX_X4_BIPCNT0r_BIP_ERROR_COUNT_1f_GET(count0);
    *count += RX_X4_BIPCNT1r_BIP_ERROR_COUNT_2f_GET(count1) + RX_X4_BIPCNT1r_BIP_ERROR_COUNT_3f_GET(count1);
    *count += RX_X4_BIPCNT2r_BIP_ERROR_COUNT_4f_GET(count2);

    return PHYMOD_E_NONE;
}

/* The number of times BER_BAD_SH state is entered for cl49. */
int tefmod16_gen3_cl49_ber_counter_get(PHYMOD_ST* pc, uint32_t* count)
{
    RX_X4_DEC_STS3r_t dec_sts;

    PHYMOD_IF_ERR_RETURN (READ_RX_X4_DEC_STS3r(pc, &dec_sts));
    *count = RX_X4_DEC_STS3r_CL49_BER_COUNTf_GET(dec_sts);

    return PHYMOD_E_NONE;
}

/* The number of times BER_BAD_SH state is entered for cl82. */
int tefmod16_gen3_cl82_ber_counter_get(PHYMOD_ST* pc, uint32_t* count)
{
    CL82_BER_HOr_t count_ho;
    CL82_BER_LOr_t count_lo;

    PHYMOD_IF_ERR_RETURN (READ_CL82_BER_HOr(pc, &count_ho));
    PHYMOD_IF_ERR_RETURN (READ_CL82_BER_LOr(pc, &count_lo));
    *count = (CL82_BER_HOr_BER_HOf_GET(count_ho) << 8) | (CL82_BER_LOr_BER_LOf_GET(count_lo));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_set_speed_id(PHYMOD_ST *pc, int speed_id)
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

int tefmod16_gen3_pxgng_status_an_ack_complete_get(PHYMOD_ST *pc, int *ack_complte)
{
    AN_X4_PXNG_STSr_t an_pxng_status_reg;

    AN_X4_PXNG_STSr_CLR(an_pxng_status_reg);
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_PXNG_STSr(pc, &an_pxng_status_reg));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_PXNG_STSr(pc, &an_pxng_status_reg));

    *ack_complte = AN_X4_PXNG_STSr_COMPLETE_ACKf_GET(an_pxng_status_reg);

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_pxgng_status_an_good_chk_get(PHYMOD_ST *pc, uint8_t *an_good_chk)
{
    AN_X4_PXNG_STSr_t an_pxng_status_reg;

    AN_X4_PXNG_STSr_CLR(an_pxng_status_reg);
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_PXNG_STSr(pc, &an_pxng_status_reg));

    *an_good_chk = AN_X4_PXNG_STSr_AN_GOOD_CHECKf_GET(an_pxng_status_reg);

    return PHYMOD_E_NONE;
}

/*
 * Translate pll_mode to pll multiplier
 */
int tefmod16_gen3_pll_multiplier_get(uint32_t pll_div, uint32_t *pll_multiplier)
{
    switch (pll_div) {
        case 0x0:
            *pll_multiplier = 64;
             break;
        case 0x1:
            *pll_multiplier = 66;
            break;
        case 0x2:
            *pll_multiplier = 80;
            break;
        case 0x3:
            *pll_multiplier = 128;
            break;
        case 0x4:
            *pll_multiplier = 132;
            break;
        case 0x5:
            *pll_multiplier = 140;
            break;
        case 0x6:
            *pll_multiplier = 160;
            break;
        case 0x7:
            *pll_multiplier = 165;
            break;
        case 0x8:
            *pll_multiplier = 168;
            break;
        case 0x9:
            *pll_multiplier = 170;
            break;
        case 0xa:
            *pll_multiplier = 175;
            break;
        case 0xb:
            *pll_multiplier = 180;
            break;
        case 0xc:
            *pll_multiplier = 184;
            break;
        case 0xd:
            *pll_multiplier = 200;
            break;
        case 0xe:
            *pll_multiplier = 224;
            break;
        case 0xf:
            *pll_multiplier = 264;
            break;
        case 0x10:
            *pll_multiplier = 96;
            break;
        case 0x11:
            *pll_multiplier = 120;
            break;
        case 0x12:
            *pll_multiplier = 144;
            break;
        case 0x13:
            *pll_multiplier = 198;
            break;
        default:
            *pll_multiplier = 165;
            break;
    }

    return PHYMOD_E_NONE;
}

/*
 * Caculate vco rate in MHz
 */
int tefmod16_gen3_pll_to_vco_rate(PHYMOD_ST *pc, phymod_tscf_pll_multiplier_t pll_mode, phymod_ref_clk_t ref_clk, uint32_t* vco_rate)
{
    uint32_t pll_multiplier = 0;

    tefmod16_gen3_pll_multiplier_get(pll_mode, &pll_multiplier);
    if (ref_clk == phymodRefClk156Mhz){
        *vco_rate = pll_multiplier * 156 + pll_multiplier *25/100;
    }
    if (ref_clk == phymodRefClk125Mhz){
       *vco_rate =  pll_multiplier * 125;
    }

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_hg_enable_get(PHYMOD_ST *pc, uint32_t *enable)
{
    SC_X4_RSLVD0r_t SC_X4_RSLVD0r_reg;
    SC_X4_RSLVD7r_t SC_X4_RSLVD7r_reg;

    SC_X4_RSLVD0r_CLR(SC_X4_RSLVD0r_reg);
    SC_X4_RSLVD7r_CLR(SC_X4_RSLVD7r_reg);

    PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD0r(pc, &SC_X4_RSLVD0r_reg));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_RSLVD7r(pc, &SC_X4_RSLVD7r_reg));

    *enable = SC_X4_RSLVD0r_T_HG2_ENABLEf_GET(SC_X4_RSLVD0r_reg);
    *enable |= SC_X4_RSLVD7r_R_HG2_ENABLEf_GET(SC_X4_RSLVD7r_reg);

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_1588_ts_enable_set(PHYMOD_ST *pc, uint32_t enable)
{
    MAIN0_TS_1588_CTLr_t  MAIN0_TS_1588_CTLr_reg;

    MAIN0_TS_1588_CTLr_CLR(MAIN0_TS_1588_CTLr_reg);
    READ_MAIN0_TS_1588_CTLr(pc, &MAIN0_TS_1588_CTLr_reg);
    MAIN0_TS_1588_CTLr_RXTX_1588_TS_ENf_SET(MAIN0_TS_1588_CTLr_reg, enable);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_TS_1588_CTLr(pc, MAIN0_TS_1588_CTLr_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_1588_ts_enable_get(PHYMOD_ST *pc, uint32_t *enable)
{
    MAIN0_TS_1588_CTLr_t  MAIN0_TS_1588_CTLr_reg;

    MAIN0_TS_1588_CTLr_CLR(MAIN0_TS_1588_CTLr_reg);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_TS_1588_CTLr(pc, &MAIN0_TS_1588_CTLr_reg));
    *enable = MAIN0_TS_1588_CTLr_RXTX_1588_TS_ENf_GET(MAIN0_TS_1588_CTLr_reg);

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_1588_ts_offset_set(PHYMOD_ST *pc, uint32_t offset)
{
    MAIN0_TS_TMR_OFFSr_t MAIN0_TS_TMR_OFFSr_reg;

    MAIN0_TS_TMR_OFFSr_CLR(MAIN0_TS_TMR_OFFSr_reg);
    PHYMOD_IF_ERR_RETURN(READ_MAIN0_TS_TMR_OFFSr(pc, &MAIN0_TS_TMR_OFFSr_reg));
    MAIN0_TS_TMR_OFFSr_U_TS_OFFSETf_SET(MAIN0_TS_TMR_OFFSr_reg, offset);
    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_TS_TMR_OFFSr(pc, MAIN0_TS_TMR_OFFSr_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_1588_pcs_control_set(PHYMOD_ST* pc)
{
    RX_X4_RXP_1588_SFD_TS_CTLr_t RX_X4_RXP_1588_SFD_TS_CTLr_reg;
    TX_X4_TX_TS_POINT_CTLr_t TX_X4_TX_TS_POINT_CTLr_reg;
    int tx_sfd_ts_enable, osts_crc_calc_mode;
    uint32_t hg_enable, ts_enable;

    RX_X4_RXP_1588_SFD_TS_CTLr_CLR(RX_X4_RXP_1588_SFD_TS_CTLr_reg);
    TX_X4_TX_TS_POINT_CTLr_CLR(TX_X4_TX_TS_POINT_CTLr_reg);

    tefmod16_gen3_1588_ts_enable_get(pc, &ts_enable);
    if (!ts_enable){
        return PHYMOD_E_NONE;
    }
    /*
     * CRC calculation mode is used in one-step timestamping pakets
     * osts_crc_cacl_mode:
     *                  0: be used with IEEE speeds only
     *                  1: Start CRC calculation from 1st byte of patcket, SOP byte is included for CRC, be used with HIGIG2 speed
     *                  2: Start CRC calculation from 2nd byte of patcket, SOP byte is excluded for CRC, be used with HIGIG2 speed
     * tx_sfd_timestamp_en:
     *                  0: timestamp TS packets based on SOP bype
     *                  1: timestamp TS packets based on SFD bype
     * when HIGIG2 speed, tx_sfd_timestamp_en should be set as 0
     */

    tefmod16_gen3_hg_enable_get(pc, &hg_enable);
    if (hg_enable) {
         tx_sfd_ts_enable = 0;
         osts_crc_calc_mode = 1;
    } else {
         tx_sfd_ts_enable = 1;
         osts_crc_calc_mode = 0;
    }
    /* Enable sfd and adjust SOP timestampe */
    PHYMOD_IF_ERR_RETURN(READ_RX_X4_RXP_1588_SFD_TS_CTLr(pc, &RX_X4_RXP_1588_SFD_TS_CTLr_reg));
    RX_X4_RXP_1588_SFD_TS_CTLr_SFD_TS_ENf_SET(RX_X4_RXP_1588_SFD_TS_CTLr_reg, tx_sfd_ts_enable);
    RX_X4_RXP_1588_SFD_TS_CTLr_RX_CL36_SOP_ADJ_10UIf_SET(RX_X4_RXP_1588_SFD_TS_CTLr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RXP_1588_SFD_TS_CTLr(pc, RX_X4_RXP_1588_SFD_TS_CTLr_reg));

    /* Set TXP SOP or SFD timestamping, osts_crc_calc_mode and timestamp interrupt  */
    PHYMOD_IF_ERR_RETURN(READ_TX_X4_TX_TS_POINT_CTLr(pc, &TX_X4_TX_TS_POINT_CTLr_reg));
    TX_X4_TX_TS_POINT_CTLr_TX_SFD_TIMESTAMP_ENf_SET(TX_X4_TX_TS_POINT_CTLr_reg,tx_sfd_ts_enable);
    TX_X4_TX_TS_POINT_CTLr_OSTS_CRC_CALC_MODEf_SET(TX_X4_TX_TS_POINT_CTLr_reg, osts_crc_calc_mode);
    TX_X4_TX_TS_POINT_CTLr_TS_INT_ENf_SET(TX_X4_TX_TS_POINT_CTLr_reg, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_TX_TS_POINT_CTLr(pc, TX_X4_TX_TS_POINT_CTLr_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_1588_fclk_set(PHYMOD_ST* pc)
{
    PMD_X1_FCLK_PERIODr_t PMD_X1_FCLK_PERIODr_reg;
    phymod_access_t pc_copy;
    int speed;
    uint16_t frac_ns;

    PHYMOD_MEMCPY(&pc_copy, pc, sizeof(pc_copy));
    PMD_X1_FCLK_PERIODr_CLR(PMD_X1_FCLK_PERIODr_reg);

    tefmod16_gen3_speed_id_get(pc, &speed);
    tefmod16_gen3_mapped_frac_ns_get(speed, &frac_ns);

    /* Program fractional nanosecond based on vco */
    PHYMOD_IF_ERR_RETURN(READ_PMD_X1_FCLK_PERIODr(pc, &PMD_X1_FCLK_PERIODr_reg));
    PMD_X1_FCLK_PERIODr_FRAC_NSf_SET(PMD_X1_FCLK_PERIODr_reg, frac_ns);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_FCLK_PERIODr(pc, PMD_X1_FCLK_PERIODr_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_1588_ui_values_set(PHYMOD_ST* pc)
{
    PMD_X4_UI_VALUE_HIr_t PMD_X4_UI_VALUE_HIr_reg;
    PMD_X4_UI_VALUE_LOr_t PMD_X4_UI_VALUE_LOr_reg;
    int speed;
    uint16_t ui_msb, ui_lsb;

    PMD_X4_UI_VALUE_HIr_CLR(PMD_X4_UI_VALUE_HIr_reg);
    PMD_X4_UI_VALUE_LOr_CLR(PMD_X4_UI_VALUE_LOr_reg);

    PHYMOD_IF_ERR_RETURN(tefmod16_gen3_speed_id_get(pc, &speed));
    tefmod16_gen3_ui_values_get(speed, &ui_msb, &ui_lsb);

    /* Program bit time in fractional nanosecond MSB */
    PHYMOD_IF_ERR_RETURN(READ_PMD_X4_UI_VALUE_HIr(pc, &PMD_X4_UI_VALUE_HIr_reg));
    PMD_X4_UI_VALUE_HIr_UI_FRAC_M1_TO_M16f_SET(PMD_X4_UI_VALUE_HIr_reg, ui_msb);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_UI_VALUE_HIr(pc, PMD_X4_UI_VALUE_HIr_reg));

    /* Program bit time in fractional nanosecond LSB */
    PHYMOD_IF_ERR_RETURN(READ_PMD_X4_UI_VALUE_LOr(pc, &PMD_X4_UI_VALUE_LOr_reg));
    PMD_X4_UI_VALUE_LOr_UI_FRAC_M17_TO_M25f_SET(PMD_X4_UI_VALUE_LOr_reg, ui_lsb);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_UI_VALUE_LOr(pc, PMD_X4_UI_VALUE_LOr_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_1588_txrx_fixed_latency_set(PHYMOD_ST* pc)
{
    PMD_X4_PMD_TX_FIXED_LATENCYr_t PMD_X4_PMD_TX_FIXED_LATENCYr_reg;
    PMD_X4_PMD_RX_FIXED_LATENCY_INTr_t PMD_X4_PMD_RX_FIXED_LATENCY_INTr_reg;
    PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_t PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_reg;
    int speed;
    uint16_t tx_fixed_int, tx_fixed_frac;
    uint16_t rx_fixed_int, rx_fixed_frac;

    PMD_X4_PMD_TX_FIXED_LATENCYr_CLR(PMD_X4_PMD_TX_FIXED_LATENCYr_reg);
    PMD_X4_PMD_RX_FIXED_LATENCY_INTr_CLR(PMD_X4_PMD_RX_FIXED_LATENCY_INTr_reg);
    PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_CLR(PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_reg);

    PHYMOD_IF_ERR_RETURN(tefmod16_gen3_speed_id_get(pc, &speed));
    tefmod16_gen3_txrx_fixed_latency_get(speed, &tx_fixed_int, &tx_fixed_frac, &rx_fixed_int, &rx_fixed_frac);

    /* Program transmit fixed latency - int and frac */
    PHYMOD_IF_ERR_RETURN(READ_PMD_X4_PMD_TX_FIXED_LATENCYr(pc, &PMD_X4_PMD_TX_FIXED_LATENCYr_reg));
    /* Integral part of PMD tx latency */
    PMD_X4_PMD_TX_FIXED_LATENCYr_PMD_TX_LATENCY_INT_NSf_SET(PMD_X4_PMD_TX_FIXED_LATENCYr_reg, tx_fixed_int);
    /* Franctional part of PMD tx latency */
    PMD_X4_PMD_TX_FIXED_LATENCYr_PMD_TX_LATENCY_FRAC_NSf_SET(PMD_X4_PMD_TX_FIXED_LATENCYr_reg, tx_fixed_frac);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_PMD_TX_FIXED_LATENCYr(pc, PMD_X4_PMD_TX_FIXED_LATENCYr_reg));

    /* Program receive fixed latency - int*/
    PHYMOD_IF_ERR_RETURN(READ_PMD_X4_PMD_RX_FIXED_LATENCY_INTr(pc, &PMD_X4_PMD_RX_FIXED_LATENCY_INTr_reg));
    PMD_X4_PMD_RX_FIXED_LATENCY_INTr_TAB_DP_INT_NSf_SET(PMD_X4_PMD_RX_FIXED_LATENCY_INTr_reg, rx_fixed_int);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_PMD_RX_FIXED_LATENCY_INTr(pc, PMD_X4_PMD_RX_FIXED_LATENCY_INTr_reg));

    /* Program receive fixed latency - frac*/
    PHYMOD_IF_ERR_RETURN(READ_PMD_X4_PMD_RX_FIXED_LATENCY_FRACr(pc, &PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_reg));
    PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_TAB_DP_FRAC_NSf_SET(PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_reg, rx_fixed_frac);
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_PMD_RX_FIXED_LATENCY_FRACr(pc, PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_reg));

   return PHYMOD_E_NONE;
}

/**
@brief   1588 RX Deskew Set
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details 1588 Deskew settings
AM_timestamp_vl[n] = Rx_base_timestamp{ns, sub_ns}
+/- (Bit_position_offset*num_bit_muxed_lanes*mod_ui_value)
+ (Fclk_adjust*fclk_period)
+ (AM_block_offset * block_size * num_bit_muxed_lanes*mod_ui_value)
- Cl74_fec_block_number * num_bit_muxed_lanes*mod_ui_value (applicable only for CL74)

Num_bit_muxed_lanes ? 1 /2/5
Mod_ui_value = vco_clk_period * os_mode
Block_size = 60 for CL91 speeds; 66 for non-CL91 speeds

*/
int _tefmod16_gen3_calculate_am_ts(PHYMOD_ST* pc, uint16_t speed,int64 my_base_ts,uint16_t bit_pos_adj,
                                            uint16_t base_ts_fclk_adj,uint16_t am_block_offset ,uint16_t  cl74_fec_block_num,
                                            uint16_t num_of_bitmuxed_lane, uint16_t bit_muxed_lane, int blk_size, int cl74_en, int64 *my_am_ts)
{
  uint16_t ui_msb = 0;
  uint16_t ui_lsb = 0;
  uint16_t frac_ns = 0;

  uint32_t my_ui_value = 0;
  int64 my_bit_pos_adj;
  int64 my_base_ts_fclk_adj;
  int64 my_am_block_offset;
  int64 my_cl74_fec_block_num;
  int64 my_bit_muxed_lane;
  int64 temp_compiler_64_value;

  COMPILER_64_SET(my_bit_pos_adj,         0, 0);
  COMPILER_64_SET(my_base_ts_fclk_adj,    0, 0);
  COMPILER_64_SET(my_am_block_offset,     0, 0);
  COMPILER_64_SET(my_cl74_fec_block_num,  0, 0);
  COMPILER_64_SET(my_bit_muxed_lane,      0, 0);
  COMPILER_64_SET(temp_compiler_64_value, 0, 0);
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x num_of_bitmuxed_lane=%0d, bit_muxed_lane=%0d, blk_size=%0d, cl74_en=%0d\n", __func__, pc->addr, num_of_bitmuxed_lane, bit_muxed_lane, blk_size, cl74_en));

  /* get ui values based on speed */
  tefmod16_gen3_ui_values_get(speed, &ui_msb, &ui_lsb);
  my_ui_value |= ui_msb;
  my_ui_value = my_ui_value << 9;
  my_ui_value |= ui_lsb;
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x my_ui_value           %016x = ui_msb %08x << 9 | ui_lsb %08x \n", __func__, pc->addr, my_ui_value , ui_msb, ui_lsb));

  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x my_base_ts            (%016x,%016x) \n", __func__, pc->addr, COMPILER_64_HI(my_base_ts), COMPILER_64_LO(my_base_ts)));

  /* Sign Extend if the bit offset is negative */
  /*  my_bit_pos_adj = (bit_pos_adj & 0x100) ? 0xfffffffffffffe00 | (int) bit_pos_adj : (int) bit_pos_adj ; */
  if (bit_pos_adj & 0x100) {
      COMPILER_64_SET(my_bit_pos_adj, 0xffffffff, 0xfffffe00 | (uint32_t)bit_pos_adj);
  } else {
      COMPILER_64_SET(my_bit_pos_adj, 0, bit_pos_adj);
  }
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x my_bit_pos_adj        (%016x,%016x) = (%0d,%0d) : Converted bit_pos_adj %03x to Long\n", __func__, pc->addr, COMPILER_64_HI(my_bit_pos_adj), COMPILER_64_LO(my_bit_pos_adj), COMPILER_64_HI(my_bit_pos_adj), COMPILER_64_LO(my_bit_pos_adj), bit_pos_adj)) ;

  /* my_bit_pos_adj = my_bit_pos_adj - (long) bit_muxed_lane ; */
  COMPILER_64_SUB_32(my_bit_pos_adj, (uint32_t)bit_muxed_lane);
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x my_bit_pos_adj        (%016x,%016x) = (%0d,%0d) : Subtracted (long) bit_muxed_lane %01x\n", __func__, pc->addr, COMPILER_64_HI(my_bit_pos_adj), COMPILER_64_LO(my_bit_pos_adj), COMPILER_64_HI(my_bit_pos_adj), COMPILER_64_LO(my_bit_pos_adj), bit_muxed_lane));

  /* my_bit_pos_adj = my_bit_pos_adj * (long) num_of_bitmuxed_lane * my_ui_value ; */
  COMPILER_64_UMUL_32(my_bit_pos_adj, (uint32_t)num_of_bitmuxed_lane);
  COMPILER_64_UMUL_32(my_bit_pos_adj, my_ui_value);
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x my_bit_pos_adj        (%016x,%016x) = my_bit_pos_adj * (long) num_of_bitmuxed_lane %01x * my_ui_value %016x\n", __func__, pc->addr, COMPILER_64_HI(my_bit_pos_adj), COMPILER_64_LO(my_bit_pos_adj), num_of_bitmuxed_lane, my_ui_value));

  /* Calculate bit muxed lanes */
  /* my_bit_muxed_lane = (long) bit_muxed_lane * my_ui_value; */
  COMPILER_64_SET(my_bit_muxed_lane, 0, (uint32_t)bit_muxed_lane);
  COMPILER_64_UMUL_32(my_bit_muxed_lane, my_ui_value);
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x my_bit_muxed_lane     (%016x,%016x) = (long) bit_muxed_lane %01x * my_ui_value %016x\n", __func__, pc->addr, COMPILER_64_HI(my_bit_muxed_lane), COMPILER_64_LO(my_bit_muxed_lane), bit_muxed_lane, my_ui_value));

  /* Calculate my_base_ts_fclk_adj */
  tefmod16_gen3_mapped_frac_ns_get(speed, &frac_ns);

  /* my_base_ts_fclk_adj = (long) base_ts_fclk_adj * (long) frac_ns; */
  COMPILER_64_SET(my_base_ts_fclk_adj, 0, (uint32_t)base_ts_fclk_adj);
  COMPILER_64_UMUL_32(my_base_ts_fclk_adj, (uint32_t)frac_ns);
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x my_base_ts_fclk_adj   (%016x,%016x) = (long) base_ts_fclk_adj %02x * (long) frac_ns %04x\n", __func__, pc->addr, COMPILER_64_HI(my_base_ts_fclk_adj), COMPILER_64_LO(my_base_ts_fclk_adj), base_ts_fclk_adj, frac_ns));

  /* Calculate my_am_block_offset */
  /* my_am_block_offset = (long) am_block_offset * (long) blk_size * (long) num_of_bitmuxed_lane * my_ui_value ; */
  COMPILER_64_SET(my_am_block_offset, 0, am_block_offset);
  COMPILER_64_UMUL_32(my_am_block_offset, (uint32_t)blk_size);
  COMPILER_64_UMUL_32(my_am_block_offset, (uint32_t)num_of_bitmuxed_lane);
  COMPILER_64_UMUL_32(my_am_block_offset, my_ui_value);

  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x my_am_block_offset    (%016x,%016x) = (long) am_block_offset %02x * (long) blk_size %08x * (long) num_of_bitmuxed_lane %01x * my_ui_value %016x\n", __func__, pc->addr, COMPILER_64_HI(my_am_block_offset), COMPILER_64_LO(my_am_block_offset) , am_block_offset, blk_size, num_of_bitmuxed_lane, my_ui_value ));

  /* Calculate my_cl74_fec_block_num */
  /* my_cl74_fec_block_num = (((long) cl74_fec_block_num + (long) am_block_offset) & (long) 0x1f) * (long) num_of_bitmuxed_lane * my_ui_value; */
  if(cl74_en ==1 ) {
     COMPILER_64_SET(my_cl74_fec_block_num, 0, cl74_fec_block_num + am_block_offset);
     COMPILER_64_SET(temp_compiler_64_value, 0, 0x1f);
     COMPILER_64_AND(my_cl74_fec_block_num, temp_compiler_64_value);
     COMPILER_64_UMUL_32(my_cl74_fec_block_num, (uint32_t)num_of_bitmuxed_lane);
     COMPILER_64_UMUL_32(my_cl74_fec_block_num, my_ui_value);
     TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x my_cl74_fec_block_num (%016x,%016x) = (((long) cl74_fec_block_num %02x + (long) am_block_offset %02x) & (long) 0x1f) * (long) num_of_bitmuxed_lane %04x * my_ui_value %016x : cl74_en = %01x\n", __func__, pc->addr, COMPILER_64_HI(my_cl74_fec_block_num), COMPILER_64_LO(my_cl74_fec_block_num), cl74_fec_block_num, am_block_offset, num_of_bitmuxed_lane, my_ui_value, cl74_en));
  } else {
     TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x my_cl74_fec_block_num (%016x,%016x) : cl74_en = %01x\n\n", __func__, pc->addr, COMPILER_64_HI(my_cl74_fec_block_num), COMPILER_64_LO(my_cl74_fec_block_num), cl74_en));
  }

  /* Calculate AM TS */
  /* *my_am_ts = (my_base_ts << 12) + (my_bit_pos_adj / 512) + (my_bit_muxed_lane >> 9) + my_base_ts_fclk_adj + (my_am_block_offset >> 9) - (my_cl74_fec_block_num >> 9); */
  COMPILER_64_SHL(my_base_ts, 12);
  COMPILER_64_UDIV_32(my_bit_pos_adj, 512);
  COMPILER_64_SHR(my_bit_muxed_lane, 9);
  COMPILER_64_SHR(my_am_block_offset, 9);
  COMPILER_64_SHR(my_cl74_fec_block_num, 9);

  COMPILER_64_COPY(*my_am_ts, my_base_ts);
  COMPILER_64_ADD_64(*my_am_ts, my_bit_pos_adj);
  COMPILER_64_ADD_64(*my_am_ts, my_bit_muxed_lane);
  COMPILER_64_ADD_64(*my_am_ts, my_base_ts_fclk_adj);
  COMPILER_64_ADD_64(*my_am_ts, my_am_block_offset);
  COMPILER_64_SUB_64(*my_am_ts, my_cl74_fec_block_num);

  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x Sum All\n", __func__, pc->addr));
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x  +  my_base_ts            %012x_%04x\n", __func__, pc->addr, COMPILER_64_HI(my_base_ts), COMPILER_64_LO(my_base_ts)));
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x +/- my_bit_pos_adj        %012x_%04x\n", __func__, pc->addr, COMPILER_64_HI(my_bit_pos_adj), COMPILER_64_LO(my_bit_pos_adj)));
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x  +  my_bit_muxed_lane     %012x_%04x\n", __func__, pc->addr, COMPILER_64_HI(my_bit_muxed_lane), COMPILER_64_LO(my_bit_muxed_lane)));
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x  +  my_base_ts_fclk_adj   %012x_%04x\n", __func__, pc->addr, COMPILER_64_HI(my_base_ts_fclk_adj), COMPILER_64_LO(my_base_ts_fclk_adj)));
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x  +  my_am_block_offset    %012x_%04x\n", __func__, pc->addr, COMPILER_64_HI(my_am_block_offset), COMPILER_64_LO(my_am_block_offset)));
  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x  -  my_cl74_fec_block_num %012x_%04x\n", __func__, pc->addr, COMPILER_64_HI(my_cl74_fec_block_num), COMPILER_64_LO(my_cl74_fec_block_num)));

  TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x     my_am_ts              %012x_%04x\n\n", __func__, pc->addr, COMPILER_64_HI(*my_am_ts), COMPILER_64_LO(*my_am_ts)));

  return PHYMOD_E_NONE;
}

/*
@brief   1588 RX Deskew Set
@param   pc handle to current TSCF context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful completion
@details 1588 Deskew settings
*/
int tefmod16_gen3_1588_rx_deskew_set(PHYMOD_ST* pc, int type, phymod_timesync_compensation_mode_t timesync_am_norm_mode)
{
    RX_X4_RXP_1588_SFD_TS_CTLr_t      reg_rxp_1588_sfd_ctrl;

    RX_X4_RX_DESKEW_BASE_TS_CALC0r_t  reg_base_ts_calc0;
    RX_X4_RX_DESKEW_BASE_TS_ADJ0r_t   reg_base_ts_adj0;
    RX_X4_RX_DESKEW_AM_OFFS0r_t       reg_am_offset0;
    RX_X4_RX_DESKEW_BASE_TS_CALC1r_t  reg_base_ts_calc1;
    RX_X4_RX_DESKEW_BASE_TS_ADJ1r_t   reg_base_ts_adj1;
    RX_X4_RX_DESKEW_AM_OFFS1r_t       reg_am_offset1;
    RX_X4_RX_DESKEW_BASE_TS_CALC2r_t  reg_base_ts_calc2;
    RX_X4_RX_DESKEW_BASE_TS_ADJ2r_t   reg_base_ts_adj2;
    RX_X4_RX_DESKEW_AM_OFFS2r_t       reg_am_offset2;
    RX_X4_RX_DESKEW_BASE_TS_CALC3r_t  reg_base_ts_calc3;
    RX_X4_RX_DESKEW_BASE_TS_ADJ3r_t   reg_base_ts_adj3;
    RX_X4_RX_DESKEW_AM_OFFS3r_t       reg_am_offset3;
    RX_X4_RX_DESKEW_BASE_TS_CALC4r_t  reg_base_ts_calc4;
    RX_X4_RX_DESKEW_BASE_TS_ADJ4r_t   reg_base_ts_adj4;
    RX_X4_RX_DESKEW_AM_OFFS4r_t       reg_am_offset4;

    RX_X4_RX_DESKEW_ADJ_INT0r_t reg_rx_deskew_int0;
    RX_X4_RX_DESKEW_ADJ_INT1r_t reg_rx_deskew_int1;
    RX_X4_RX_DESKEW_ADJ_INT2r_t reg_rx_deskew_int2;
    RX_X4_RX_DESKEW_ADJ_INT3r_t reg_rx_deskew_int3;
    RX_X4_RX_DESKEW_ADJ_INT4r_t reg_rx_deskew_int4;
    RX_X4_RX_DESKEW_ADJ_FRAC0r_t reg_rx_deskew_frac0;
    RX_X4_RX_DESKEW_ADJ_FRAC1r_t reg_rx_deskew_frac1;
    RX_X4_RX_DESKEW_ADJ_FRAC2r_t reg_rx_deskew_frac2;
    RX_X4_RX_DESKEW_ADJ_FRAC3r_t reg_rx_deskew_frac3;
    RX_X4_RX_DESKEW_ADJ_FRAC4r_t reg_rx_deskew_frac4;

    RX_X4_PSLL_TO_VL_MAP0r_t reg_deskew_ll_to_vl_map0;
    RX_X4_PSLL_TO_VL_MAP1r_t reg_deskew_ll_to_vl_map1;

    uint16_t frac_ns = 0;
    uint16_t ui_msb = 0;
    uint16_t ui_lsb = 0;
    uint16_t base_ts[20] = {0};
    uint16_t bit_pos_adj[20] = {0};
    uint16_t base_ts_fclk_adj[20] = {0};
    uint16_t am_ts_info_valid[20] = {0};
    uint16_t am_block_offset[20] = {0};
    uint16_t cl74_fec_block_num[20] = {0};
    uint16_t psll_to_vl_map[20] = {0};
    uint16_t bit_muxed_lane[20] = {0};

    int64 my_base_ts[20];
    int64 my_base_ts_max;
    int64 my_am_ts_calc_value[20];
    int64 my_am_ts_calc_value_norm[20];
    int64 my_am_ts_calc_value_prev_avg;
    int64 my_am_ts_calc_value_min;
    int64 my_ref_value;
    int64 my_am_ts;
    uint32_t my_ui_value = 0;
    int64 my_deskew_val;
    uint16_t my_deskew_val_int = 0;
    uint16_t my_deskew_val_frac = 0;
    uint32_t am_bits = 0;
    int64 temp_compiler_64_value;
    int64 temp_base_ts_max;

    int j=0;
    int k=0;
    int reg_copy = 0;
    int ref_lane=0;
    int calc_array_idx = 0;  /* selects 1-of-4 averaging arrays */
    int no_of_lanes_pp = 0;  /* count of lanes per port */
    uint16_t no_of_vl = 0;  /* virtual lanes per port */
    uint16_t no_of_bml = 0;  /* number of bit muxed VL on a physical lane */

    int fec_en = 0;
    int fec91_en = 0;
    int start_lane = 0;
    int num_lane = 0;
    int speed_id = 0;
    int time_cnt = 0;
    phymod_access_t pc_copy;

    for (j = 0; j < 20; j++) {
        COMPILER_64_SET(my_base_ts[j]                   , 0, 0);
        COMPILER_64_SET(my_am_ts_calc_value[j]          , 0, 0);
        COMPILER_64_SET(my_am_ts_calc_value_norm[j]     , 0, 0);
    }
    COMPILER_64_SET(my_base_ts_max                   , 0, 0);
    COMPILER_64_SET(my_am_ts_calc_value_prev_avg     , 0, 0);
    COMPILER_64_SET(my_am_ts_calc_value_min          , 0, 0);
    COMPILER_64_SET(my_ref_value                     , 0, 0);
    COMPILER_64_SET(my_am_ts                         , 0, 0);
    COMPILER_64_SET(my_deskew_val                    , 0, 0);
    COMPILER_64_SET(temp_compiler_64_value           , 0, 0);
    COMPILER_64_SET(temp_base_ts_max                 , 0, 0);

    PHYMOD_MEMCPY(&pc_copy, pc, sizeof(pc_copy));
    PHYMOD_IF_ERR_RETURN(phymod_util_lane_config_get(pc, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN(tefmod16_gen3_speed_id_get(pc, &speed_id));
    PHYMOD_IF_ERR_RETURN(tefmod16_gen3_mapped_frac_ns_get(speed_id, &frac_ns));
    PHYMOD_IF_ERR_RETURN(tefmod16_gen3_ui_values_get(speed_id, &ui_msb, &ui_lsb));
    my_ui_value |= ui_msb;
    my_ui_value = my_ui_value << 9;
    my_ui_value |= ui_lsb;

    /* calc_array_idx = (pc->unit-1) + pc->port ; */  /*DV code: 0,1 + 0,2 => 0,2,1,3 */

    if (pc->lane_mask == 0x3 || pc->lane_mask == 0xf) {
        calc_array_idx = 0;
    } else if (pc->lane_mask == 0xc) {
        calc_array_idx = 2;
    } else {
        calc_array_idx = 0;
    }

    TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x: my_ui_value=%016x = ui_msb %08x << 9 | ui_lsb %08x \n",
                                       __func__, pc_copy.addr, pc_copy.lane_mask, my_ui_value, ui_msb, ui_lsb));

    /* Step 1: get am_bits value based on speed */
    /* TBD yet to update this table
          tefmod_get_am_bits(speed, &am_bits);
          if((pc->cl91_en==1) && (am_bits==66)) am_bits=am_bits-6;
      */
    /* PHYMOD_IF_ERR_RETURN(tefmod16_gen3_am_bits_get(speed_id, &am_bits));*/
    PHYMOD_IF_ERR_RETURN(tefmod16_gen3_FEC_get(pc, TEFMOD_CL91, &fec91_en));
    PHYMOD_IF_ERR_RETURN(tefmod16_gen3_FEC_get(pc, TEFMOD_CL74, &fec_en));

    if(fec91_en           == 1) am_bits = 60 ;
    else if(fec_en        == 1) am_bits = 66 ;
    else                        am_bits = 66 ;

    TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x: idx=%0d am_bits=0x%x fec_en=%0d cl91_en=%d\n",
                                       __func__, pc_copy.addr, pc_copy.lane_mask, calc_array_idx, am_bits, fec_en, fec91_en));

    /* Step 2 : Get Number of VL and number of bit muxed lanes */
    tefmod16_gen3_vl_bml_get(speed_id, &no_of_vl, &no_of_bml);
    if(fec91_en == 1){
        if(no_of_bml == 2) { no_of_bml=1; no_of_vl=2; } /* 40G/50G CL91 */
        if(no_of_bml == 5) { no_of_bml=1; no_of_vl=4; } /* 100G CL91 */
    }

    /* Step 2b : Clear averaging arrays for the port. */
    if((type & TEFMOD_RX_DESKEW_INIT) && ((pc->lane_mask == 0xc) || (pc->lane_mask == 0x3) || (pc->lane_mask == 0xf))){
        for(j=0; j<20; j++) {
            my_am_ts_calc_value_cnt[calc_array_idx][j] = 0 ;
            COMPILER_64_SET(my_am_ts_calc_value_avg[calc_array_idx][j], 0, 0);

            COMPILER_64_SET(my_am_ts_calc_value_low[calc_array_idx][j], 0, 0);
            COMPILER_64_SET(my_am_ts_calc_value_high[calc_array_idx][j], 0, 0);
            COMPILER_64_SET(my_am_ts_calc_value_diff[calc_array_idx][j], 0, 0);
        }
    }

    if(type & TEFMOD_RX_DESKEW_CALC) {
        if((pc->lane_mask == 0xc) || (pc->lane_mask == 0x3) || (pc->lane_mask == 0xf)) {
          /* Step 2c : Enable capture_am_ts for the port. */
          RX_X4_RXP_1588_SFD_TS_CTLr_CLR(reg_rxp_1588_sfd_ctrl);
          RX_X4_RXP_1588_SFD_TS_CTLr_CAPTURE_AM_TSf_SET(reg_rxp_1588_sfd_ctrl,0);
          PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RXP_1588_SFD_TS_CTLr(pc, reg_rxp_1588_sfd_ctrl));
          RX_X4_RXP_1588_SFD_TS_CTLr_CAPTURE_AM_TSf_SET(reg_rxp_1588_sfd_ctrl,1);
          PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RXP_1588_SFD_TS_CTLr(pc, reg_rxp_1588_sfd_ctrl));

          TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : AM TS Capture Enabled. Waiting for am_ts_info_valid[0] != 0 : cl91_en=%0d no_of_vl=%0d no_of_bml=%0d\n",
                                             __func__, pc_copy.addr, pc_copy.lane_mask, fec91_en, no_of_vl, no_of_bml));
        /*
          am_ts_info_valid[0] = 0;
          while(am_ts_info_valid[0] == 0) {
            RX_X4_RX_DESKEW_AM_OFFS0r_CLR(reg_am_offset0);
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_AM_OFFS0r(pc, &reg_am_offset0));
            am_ts_info_valid[0] = RX_X4_RX_DESKEW_AM_OFFS0r_AM_TIMESTAMP_INFO_VALID_0f_GET(reg_am_offset0);
          }
        */
        }

        /* Step 3 : Read the required registers for AM timestamp per virtual lane calculation. */
        if((pc->lane_mask == 0xc) || (pc->lane_mask == 0x3)) {
          for(j=0; j<2; j++) {
            if (pc->lane_mask == 0xc) {
              reg_copy = 2;
            }
            pc_copy.lane_mask = 0x1 << reg_copy;
            RX_X4_RX_DESKEW_AM_OFFS0r_CLR(reg_am_offset0);
            time_cnt = 0;
            while((RX_X4_RX_DESKEW_AM_OFFS0r_AM_TIMESTAMP_INFO_VALID_0f_GET(reg_am_offset0) == 0) &&
                  (time_cnt < 1000)) {
                  PHYMOD_USLEEP(10);
                  PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_AM_OFFS0r(&pc_copy, &reg_am_offset0));
                  am_ts_info_valid[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS0r_AM_TIMESTAMP_INFO_VALID_0f_GET(reg_am_offset0);
                  time_cnt++;
            }
            if(am_ts_info_valid[no_of_lanes_pp] != 1) {
              TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : no_of_vl=%0d no_of_bml=%0d : *ERROR(-1) : am_ts_info_valid[%0d] != Exp 1 : Act %1x :\n",
                     __func__, pc_copy.addr, pc_copy.lane_mask, no_of_vl, no_of_bml, no_of_lanes_pp, am_ts_info_valid[no_of_lanes_pp]));
              return PHYMOD_E_TIMEOUT;
            }
            am_block_offset[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS0r_AM_BLOCK_OFFSET_0f_GET(reg_am_offset0);
            cl74_fec_block_num[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS0r_CL74_FEC_BLOCK_NUM_0f_GET(reg_am_offset0);

            RX_X4_RX_DESKEW_BASE_TS_CALC0r_CLR(reg_base_ts_calc0);
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_CALC0r(&pc_copy, &reg_base_ts_calc0));
            base_ts[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_CALC0r_GET(reg_base_ts_calc0);
            RX_X4_RX_DESKEW_BASE_TS_ADJ0r_CLR(reg_base_ts_adj0);
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_ADJ0r(&pc_copy, &reg_base_ts_adj0));
            bit_pos_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ0r_BIT_POS_ADJ_0f_GET(reg_base_ts_adj0);
            base_ts_fclk_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ0r_BASE_TS_FCLK_ADJ_0f_GET(reg_base_ts_adj0);
            RX_X4_PSLL_TO_VL_MAP0r_CLR(reg_deskew_ll_to_vl_map0);
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_PSLL_TO_VL_MAP0r(&pc_copy, &reg_deskew_ll_to_vl_map0));
            psll_to_vl_map[no_of_lanes_pp] = RX_X4_PSLL_TO_VL_MAP0r_PSLL0_TO_VL_MAPPINGf_GET(reg_deskew_ll_to_vl_map0);
            bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]] = 0 ;

            TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : Register values Read addr=0x%x lane_mask=0x%x : base_ts=%04x bit_pos_adj=%03x base_ts_fclk_adj=%01x am_block_offset=%02x cl74_fec_block_num=%02x psll_to_vl_map=%02x->bit_muxed_lane=%1x \n",
                                               __func__, pc_copy.addr, pc_copy.lane_mask, base_ts[no_of_lanes_pp], bit_pos_adj[no_of_lanes_pp], base_ts_fclk_adj[no_of_lanes_pp], am_block_offset[no_of_lanes_pp], cl74_fec_block_num[no_of_lanes_pp], psll_to_vl_map[no_of_lanes_pp], bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]]));

            reg_copy++;
            no_of_lanes_pp++;
          } /* for(j=0; j<2; j++) */
          if(no_of_vl == 4) { /* 40G MLD over 2 lanes has 4 VLs */
            reg_copy = 0;
            for (j = 2 ;j < 4; j++)  {
              if (pc->lane_mask == 0xc) {
                reg_copy = 2;
              }
              pc_copy.lane_mask = 0x1 << reg_copy;

              RX_X4_RX_DESKEW_AM_OFFS1r_CLR(reg_am_offset1);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_AM_OFFS1r(&pc_copy, &reg_am_offset1));
              am_ts_info_valid[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS1r_AM_TIMESTAMP_INFO_VALID_1f_GET(reg_am_offset1);
              if(am_ts_info_valid[no_of_lanes_pp] != 1) {
                TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : no_of_vl=%0d no_of_bml=%0d : *ERROR(-1) : am_ts_info_valid[%0d] != Exp 1 : Act %1x :\n",
                                                   __func__, pc_copy.addr, pc_copy.lane_mask, no_of_vl, no_of_bml, no_of_lanes_pp, am_ts_info_valid[no_of_lanes_pp]));
              }
              am_block_offset[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS1r_AM_BLOCK_OFFSET_1f_GET(reg_am_offset1);
              cl74_fec_block_num[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS1r_CL74_FEC_BLOCK_NUM_1f_GET(reg_am_offset1);

              RX_X4_RX_DESKEW_BASE_TS_CALC1r_CLR(reg_base_ts_calc1);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_CALC1r(&pc_copy, &reg_base_ts_calc1));
              base_ts[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_CALC1r_GET(reg_base_ts_calc1);
              RX_X4_RX_DESKEW_BASE_TS_ADJ1r_CLR(reg_base_ts_adj1);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_ADJ1r(&pc_copy, &reg_base_ts_adj1));
              bit_pos_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ1r_BIT_POS_ADJ_1f_GET(reg_base_ts_adj1);
              base_ts_fclk_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ1r_BASE_TS_FCLK_ADJ_1f_GET(reg_base_ts_adj1);
              RX_X4_PSLL_TO_VL_MAP0r_CLR(reg_deskew_ll_to_vl_map0);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_PSLL_TO_VL_MAP0r(&pc_copy, &reg_deskew_ll_to_vl_map0));
              psll_to_vl_map[no_of_lanes_pp] = RX_X4_PSLL_TO_VL_MAP0r_PSLL1_TO_VL_MAPPINGf_GET(reg_deskew_ll_to_vl_map0);
              bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]] = 1 ;

              TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : Register values Read addr=0x%x lane_mask=0x%x : base_ts=%04x bit_pos_adj=%03x base_ts_fclk_adj=%01x am_block_offset=%02x cl74_fec_block_num=%02x psll_to_vl_map=%02x->bit_muxed_lane=%1x \n",
                                                 __func__, pc_copy.addr, pc_copy.lane_mask, base_ts[no_of_lanes_pp], bit_pos_adj[no_of_lanes_pp], base_ts_fclk_adj[no_of_lanes_pp], am_block_offset[no_of_lanes_pp], cl74_fec_block_num[no_of_lanes_pp], psll_to_vl_map[no_of_lanes_pp], bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]]));

              reg_copy++;
              no_of_lanes_pp++;
            } /* for(j=0; j<2; j++) */
          } /* if(no_of_vl == 4) */
        } else if (pc->lane_mask == 0xf) {
          if(no_of_vl == 4) {
            for(j=0; j<4; j++) {
              pc_copy.lane_mask = 0x1 << j;
              RX_X4_RX_DESKEW_AM_OFFS0r_CLR(reg_am_offset0);
              time_cnt = 0;
              while((RX_X4_RX_DESKEW_AM_OFFS0r_AM_TIMESTAMP_INFO_VALID_0f_GET(reg_am_offset0) == 0) &&
                    (time_cnt < 1000)) {
                    PHYMOD_USLEEP(10);
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_AM_OFFS0r(&pc_copy, &reg_am_offset0));
                    am_ts_info_valid[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS0r_AM_TIMESTAMP_INFO_VALID_0f_GET(reg_am_offset0);
                    time_cnt++;
              }
              if(am_ts_info_valid[no_of_lanes_pp] != 1) {
                TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : no_of_vl=%0d no_of_bml=%0d : *ERROR(-1) : am_ts_info_valid[%0d] != Exp 1 : Act %1x :\n",
                                                   __func__, pc_copy.addr, pc_copy.lane_mask, no_of_vl, no_of_bml, no_of_lanes_pp, am_ts_info_valid[no_of_lanes_pp]));
                return PHYMOD_E_TIMEOUT;
              }
              am_block_offset[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS0r_AM_BLOCK_OFFSET_0f_GET(reg_am_offset0);
              cl74_fec_block_num[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS0r_CL74_FEC_BLOCK_NUM_0f_GET(reg_am_offset0);

              RX_X4_RX_DESKEW_BASE_TS_CALC0r_CLR(reg_base_ts_calc0);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_CALC0r(&pc_copy, &reg_base_ts_calc0));
              base_ts[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_CALC0r_GET(reg_base_ts_calc0);
              RX_X4_RX_DESKEW_BASE_TS_ADJ0r_CLR(reg_base_ts_adj0);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_ADJ0r(&pc_copy, &reg_base_ts_adj0));
              bit_pos_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ0r_BIT_POS_ADJ_0f_GET(reg_base_ts_adj0);
              base_ts_fclk_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ0r_BASE_TS_FCLK_ADJ_0f_GET(reg_base_ts_adj0);
              RX_X4_PSLL_TO_VL_MAP0r_CLR(reg_deskew_ll_to_vl_map0);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_PSLL_TO_VL_MAP0r(&pc_copy, &reg_deskew_ll_to_vl_map0));
              psll_to_vl_map[no_of_lanes_pp] = RX_X4_PSLL_TO_VL_MAP0r_PSLL0_TO_VL_MAPPINGf_GET(reg_deskew_ll_to_vl_map0);
              bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]] = 0 ;

              TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : Register values Read  addr=0x%x lane_mask=0x%x : base_ts=%04x bit_pos_adj=%03x base_ts_fclk_adj=%01x am_block_offset=%02x cl74_fec_block_num=%02x psll_to_vl_map=%02x->bit_muxed_lane=%1x \n",
                                                 __func__, pc_copy.addr, pc_copy.lane_mask, base_ts[no_of_lanes_pp], bit_pos_adj[no_of_lanes_pp], base_ts_fclk_adj[no_of_lanes_pp], am_block_offset[no_of_lanes_pp], cl74_fec_block_num[no_of_lanes_pp], psll_to_vl_map[no_of_lanes_pp], bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]]));

              no_of_lanes_pp++;
            } /* for(j=0; j<4; j++) */
          } /* no_of_vl == 4 */
          else if(no_of_vl == 20) {
            for(j=0; j<4; j++) {
              pc_copy.lane_mask = 0x1 << j;
              RX_X4_RX_DESKEW_AM_OFFS0r_CLR(reg_am_offset0);
              time_cnt = 0;
              while((RX_X4_RX_DESKEW_AM_OFFS0r_AM_TIMESTAMP_INFO_VALID_0f_GET(reg_am_offset0) == 0) &&
                    (time_cnt < 1000)) {
                    PHYMOD_USLEEP(10);
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_AM_OFFS0r(&pc_copy, &reg_am_offset0));
                    am_ts_info_valid[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS0r_AM_TIMESTAMP_INFO_VALID_0f_GET(reg_am_offset0);
                    time_cnt++;
              }
              if(am_ts_info_valid[no_of_lanes_pp] != 1) {
                TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : no_of_vl=%0d no_of_bml=%0d : *ERROR(-1) : am_ts_info_valid[%0d] != Exp 1 : Act %1x :\n",
                                                   __func__, pc_copy.addr, pc_copy.lane_mask, no_of_vl, no_of_bml, no_of_lanes_pp, am_ts_info_valid[no_of_lanes_pp]));
                return PHYMOD_E_TIMEOUT;
              }
              am_block_offset[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS0r_AM_BLOCK_OFFSET_0f_GET(reg_am_offset0);
              cl74_fec_block_num[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS0r_CL74_FEC_BLOCK_NUM_0f_GET(reg_am_offset0);

              RX_X4_RX_DESKEW_BASE_TS_CALC0r_CLR(reg_base_ts_calc0);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_CALC0r(&pc_copy, &reg_base_ts_calc0));
              base_ts[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_CALC0r_GET(reg_base_ts_calc0);
              RX_X4_RX_DESKEW_BASE_TS_ADJ0r_CLR(reg_base_ts_adj0);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_ADJ0r(&pc_copy, &reg_base_ts_adj0));
              bit_pos_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ0r_BIT_POS_ADJ_0f_GET(reg_base_ts_adj0);
              base_ts_fclk_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ0r_BASE_TS_FCLK_ADJ_0f_GET(reg_base_ts_adj0);
              RX_X4_PSLL_TO_VL_MAP0r_CLR(reg_deskew_ll_to_vl_map0);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_PSLL_TO_VL_MAP0r(&pc_copy, &reg_deskew_ll_to_vl_map0));
              psll_to_vl_map[no_of_lanes_pp] = RX_X4_PSLL_TO_VL_MAP0r_PSLL0_TO_VL_MAPPINGf_GET(reg_deskew_ll_to_vl_map0);
              bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]] = 0 ;

              TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : Register values Read  addr=0x%x lane_mask=0x%x : base_ts=%04x bit_pos_adj=%03x base_ts_fclk_adj=%01x am_block_offset=%02x cl74_fec_block_num=%02x psll_to_vl_map=%02x->bit_muxed_lane=%1x \n",
                                                 __func__, pc_copy.addr, pc_copy.lane_mask, base_ts[no_of_lanes_pp], bit_pos_adj[no_of_lanes_pp], base_ts_fclk_adj[no_of_lanes_pp], am_block_offset[no_of_lanes_pp], cl74_fec_block_num[no_of_lanes_pp], psll_to_vl_map[no_of_lanes_pp], bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]]));
              no_of_lanes_pp++;

              RX_X4_RX_DESKEW_AM_OFFS1r_CLR(reg_am_offset1);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_AM_OFFS1r(&pc_copy, &reg_am_offset1));
              am_ts_info_valid[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS1r_AM_TIMESTAMP_INFO_VALID_1f_GET(reg_am_offset1);
              if(am_ts_info_valid[no_of_lanes_pp] != 1) {
                TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : no_of_vl=%0d no_of_bml=%0d : *ERROR(-1) : am_ts_info_valid[%0d] != Exp 1 : Act %1x :\n",
                                                   __func__, pc_copy.addr, pc_copy.lane_mask, no_of_vl, no_of_bml, no_of_lanes_pp, am_ts_info_valid[no_of_lanes_pp]));
              }
              am_block_offset[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS1r_AM_BLOCK_OFFSET_1f_GET(reg_am_offset1);
              cl74_fec_block_num[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS1r_CL74_FEC_BLOCK_NUM_1f_GET(reg_am_offset1);

              RX_X4_RX_DESKEW_BASE_TS_CALC1r_CLR(reg_base_ts_calc1);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_CALC1r(&pc_copy, &reg_base_ts_calc1));
              base_ts[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_CALC1r_GET(reg_base_ts_calc1);
              RX_X4_RX_DESKEW_BASE_TS_ADJ1r_CLR(reg_base_ts_adj1);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_ADJ1r(&pc_copy, &reg_base_ts_adj1));
              bit_pos_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ1r_BIT_POS_ADJ_1f_GET(reg_base_ts_adj1);
              base_ts_fclk_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ1r_BASE_TS_FCLK_ADJ_1f_GET(reg_base_ts_adj1);
              RX_X4_PSLL_TO_VL_MAP0r_CLR(reg_deskew_ll_to_vl_map0);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_PSLL_TO_VL_MAP0r(&pc_copy, &reg_deskew_ll_to_vl_map0));
              psll_to_vl_map[no_of_lanes_pp] = RX_X4_PSLL_TO_VL_MAP0r_PSLL1_TO_VL_MAPPINGf_GET(reg_deskew_ll_to_vl_map0);
              bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]] = 1 ;

              TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : Register values Read  addr=0x%x lane_mask=0x%x : base_ts=%04x bit_pos_adj=%03x base_ts_fclk_adj=%01x am_block_offset=%02x cl74_fec_block_num=%02x psll_to_vl_map=%02x->bit_muxed_lane=%1x \n",
                                                 __func__, pc_copy.addr, pc_copy.lane_mask, base_ts[no_of_lanes_pp], bit_pos_adj[no_of_lanes_pp], base_ts_fclk_adj[no_of_lanes_pp], am_block_offset[no_of_lanes_pp], cl74_fec_block_num[no_of_lanes_pp], psll_to_vl_map[no_of_lanes_pp], bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]]));
              no_of_lanes_pp++;

              RX_X4_RX_DESKEW_AM_OFFS2r_CLR(reg_am_offset2);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_AM_OFFS2r(&pc_copy, &reg_am_offset2));
              am_ts_info_valid[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS2r_AM_TIMESTAMP_INFO_VALID_2f_GET(reg_am_offset2);
              if(am_ts_info_valid[no_of_lanes_pp] != 1) {
                TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : no_of_vl=%0d no_of_bml=%0d : *ERROR(-1) : am_ts_info_valid[%0d] != Exp 1 : Act %1x :\n",
                                                   __func__, pc_copy.addr, pc_copy.lane_mask, no_of_vl, no_of_bml, no_of_lanes_pp, am_ts_info_valid[no_of_lanes_pp]));
              }
              am_block_offset[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS2r_AM_BLOCK_OFFSET_2f_GET(reg_am_offset2);
              cl74_fec_block_num[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS2r_CL74_FEC_BLOCK_NUM_2f_GET(reg_am_offset2);

              RX_X4_RX_DESKEW_BASE_TS_CALC2r_CLR(reg_base_ts_calc2);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_CALC2r(&pc_copy, &reg_base_ts_calc2));
              base_ts[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_CALC2r_GET(reg_base_ts_calc2);
              RX_X4_RX_DESKEW_BASE_TS_ADJ2r_CLR(reg_base_ts_adj2);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_ADJ2r(&pc_copy, &reg_base_ts_adj2));
              bit_pos_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ2r_BIT_POS_ADJ_2f_GET(reg_base_ts_adj2);
              base_ts_fclk_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ2r_BASE_TS_FCLK_ADJ_2f_GET(reg_base_ts_adj2);
              RX_X4_PSLL_TO_VL_MAP0r_CLR(reg_deskew_ll_to_vl_map0);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_PSLL_TO_VL_MAP0r(&pc_copy, &reg_deskew_ll_to_vl_map0));
              psll_to_vl_map[no_of_lanes_pp] = RX_X4_PSLL_TO_VL_MAP0r_PSLL2_TO_VL_MAPPINGf_GET(reg_deskew_ll_to_vl_map0);
              bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]] = 2 ;

              TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : Register values Read addr=0x%x lane_mask=0x%x : base_ts=%04x bit_pos_adj=%03x base_ts_fclk_adj=%01x am_block_offset=%02x cl74_fec_block_num=%02x psll_to_vl_map=%02x->bit_muxed_lane=%1x \n",
                                                 __func__, pc_copy.addr, pc_copy.lane_mask, base_ts[no_of_lanes_pp], bit_pos_adj[no_of_lanes_pp], base_ts_fclk_adj[no_of_lanes_pp], am_block_offset[no_of_lanes_pp], cl74_fec_block_num[no_of_lanes_pp], psll_to_vl_map[no_of_lanes_pp], bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]]));
              no_of_lanes_pp++;

              RX_X4_RX_DESKEW_AM_OFFS3r_CLR(reg_am_offset3);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_AM_OFFS3r(&pc_copy, &reg_am_offset3));
              am_ts_info_valid[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS3r_AM_TIMESTAMP_INFO_VALID_3f_GET(reg_am_offset3);
              if(am_ts_info_valid[no_of_lanes_pp] != 1) {
                TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : no_of_vl=%0d no_of_bml=%0d : *ERROR(-1) : am_ts_info_valid[%0d] != Exp 1 : Act %1x :\n",
                                                   __func__, pc_copy.addr, pc_copy.lane_mask, no_of_vl, no_of_bml, no_of_lanes_pp, am_ts_info_valid[no_of_lanes_pp]));
              }
              am_block_offset[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS3r_AM_BLOCK_OFFSET_3f_GET(reg_am_offset3);
              cl74_fec_block_num[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS3r_CL74_FEC_BLOCK_NUM_3f_GET(reg_am_offset3);

              RX_X4_RX_DESKEW_BASE_TS_CALC3r_CLR(reg_base_ts_calc3);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_CALC3r(&pc_copy, &reg_base_ts_calc3));
              base_ts[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_CALC3r_GET(reg_base_ts_calc3);
              RX_X4_RX_DESKEW_BASE_TS_ADJ3r_CLR(reg_base_ts_adj3);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_ADJ3r(&pc_copy, &reg_base_ts_adj3));
              bit_pos_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ3r_BIT_POS_ADJ_3f_GET(reg_base_ts_adj3);
              base_ts_fclk_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ3r_BASE_TS_FCLK_ADJ_3f_GET(reg_base_ts_adj3);
              RX_X4_PSLL_TO_VL_MAP1r_CLR(reg_deskew_ll_to_vl_map1);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_PSLL_TO_VL_MAP1r(&pc_copy, &reg_deskew_ll_to_vl_map1));
              psll_to_vl_map[no_of_lanes_pp] = RX_X4_PSLL_TO_VL_MAP1r_PSLL3_TO_VL_MAPPINGf_GET(reg_deskew_ll_to_vl_map1);
              bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]] = 3 ;

              TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : Register values Read  addr=0x%x lane_mask=0x%x : base_ts=%04x bit_pos_adj=%03x base_ts_fclk_adj=%01x am_block_offset=%02x cl74_fec_block_num=%02x psll_to_vl_map=%02x->bit_muxed_lane=%1x \n",
                                                 __func__, pc_copy.addr, pc_copy.lane_mask, base_ts[no_of_lanes_pp], bit_pos_adj[no_of_lanes_pp], base_ts_fclk_adj[no_of_lanes_pp], am_block_offset[no_of_lanes_pp], cl74_fec_block_num[no_of_lanes_pp], psll_to_vl_map[no_of_lanes_pp], bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]]));
              no_of_lanes_pp++;

              RX_X4_RX_DESKEW_AM_OFFS4r_CLR(reg_am_offset4);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_AM_OFFS4r(&pc_copy, &reg_am_offset4));
              am_ts_info_valid[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS4r_AM_TIMESTAMP_INFO_VALID_4f_GET(reg_am_offset4);
              if(am_ts_info_valid[no_of_lanes_pp] != 1) {
                TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : no_of_vl=%0d no_of_bml=%0d : *ERROR(-1) : am_ts_info_valid[%0d] != Exp 1 : Act %1x :\n",
                                                   __func__, pc_copy.addr, pc_copy.lane_mask, no_of_vl, no_of_bml, no_of_lanes_pp, am_ts_info_valid[no_of_lanes_pp]));
              }
              am_block_offset[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS4r_AM_BLOCK_OFFSET_4f_GET(reg_am_offset4);
              cl74_fec_block_num[no_of_lanes_pp] = RX_X4_RX_DESKEW_AM_OFFS4r_CL74_FEC_BLOCK_NUM_4f_GET(reg_am_offset4);

              RX_X4_RX_DESKEW_BASE_TS_CALC4r_CLR(reg_base_ts_calc4);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_CALC4r(&pc_copy, &reg_base_ts_calc4));
              base_ts[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_CALC4r_GET(reg_base_ts_calc4);
              RX_X4_RX_DESKEW_BASE_TS_ADJ4r_CLR(reg_base_ts_adj4);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_DESKEW_BASE_TS_ADJ4r(&pc_copy, &reg_base_ts_adj4));
              bit_pos_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ4r_BIT_POS_ADJ_4f_GET(reg_base_ts_adj4);
              base_ts_fclk_adj[no_of_lanes_pp] = RX_X4_RX_DESKEW_BASE_TS_ADJ4r_BASE_TS_FCLK_ADJ_4f_GET(reg_base_ts_adj4);
              RX_X4_PSLL_TO_VL_MAP1r_CLR(reg_deskew_ll_to_vl_map1);
              PHYMOD_IF_ERR_RETURN(READ_RX_X4_PSLL_TO_VL_MAP1r(&pc_copy, &reg_deskew_ll_to_vl_map1));
              psll_to_vl_map[no_of_lanes_pp] = RX_X4_PSLL_TO_VL_MAP1r_PSLL4_TO_VL_MAPPINGf_GET(reg_deskew_ll_to_vl_map1);
              bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]] = 4 ;

              TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : Register values Read  addr=0x%x lane_mask=0x%x : base_ts=%04x bit_pos_adj=%03x base_ts_fclk_adj=%01x am_block_offset=%02x cl74_fec_block_num=%02x psll_to_vl_map=%02x->bit_muxed_lane=%1x \n",
                                                 __func__, pc_copy.addr, pc_copy.lane_mask, base_ts[no_of_lanes_pp], bit_pos_adj[no_of_lanes_pp], base_ts_fclk_adj[no_of_lanes_pp], am_block_offset[no_of_lanes_pp], cl74_fec_block_num[no_of_lanes_pp], psll_to_vl_map[no_of_lanes_pp], bit_muxed_lane[psll_to_vl_map[no_of_lanes_pp]]));
              no_of_lanes_pp++;
            } /* for(j=0; j<4; j++) */
            /* no_of_lanes_pp = 20; */
          } /* if(no_of_vl == 20) */
        } /* if(pc->port_type == TEFMOD_SINGLE_PORT) */

        /* Copy over base_ts[15:0] to my_base_ts[63:0] */
        for(j = 0 ; j < no_of_vl ; j++) { COMPILER_64_SET(my_base_ts[j], 0, base_ts[j]) ; }

        /* Find Max my_base_ts */
        /* if(my_base_ts_max < my_base_ts[j]) my_base_ts_max = my_base_ts[j] ; */
        COMPILER_64_COPY(my_base_ts_max, my_base_ts[0]);
        for(j = 1 ; j < no_of_vl ; j++) { if(COMPILER_64_LT(my_base_ts_max, my_base_ts[j])) COMPILER_64_COPY(my_base_ts_max, my_base_ts[j]) ; }

        /* Rollover Check and Adjust */
        for(j = 0 ; j < no_of_vl ; j++) {
            /* if((my_base_ts_max - my_base_ts[j]) > 0x08000) my_base_ts[j] |= 0x10000 ; */
            COMPILER_64_COPY(temp_base_ts_max, my_base_ts_max);
            COMPILER_64_SUB_64(temp_base_ts_max, my_base_ts[j]);
            COMPILER_64_SET(temp_compiler_64_value, 0, 0x08000);
            if(COMPILER_64_GT(temp_base_ts_max, temp_compiler_64_value)) {
               COMPILER_64_SET(temp_compiler_64_value, 0, 0x10000);
               COMPILER_64_OR(my_base_ts[j], temp_compiler_64_value);
            }
        }

        /* Step 4 : Calculate the AM TS values per virtual lane */

        no_of_lanes_pp = 0;
        for(j=0; j<no_of_vl; j++) {
          _tefmod16_gen3_calculate_am_ts(pc, speed_id, my_base_ts[no_of_lanes_pp], bit_pos_adj[no_of_lanes_pp], base_ts_fclk_adj[no_of_lanes_pp], am_block_offset[no_of_lanes_pp], cl74_fec_block_num[no_of_lanes_pp], no_of_bml, bit_muxed_lane[no_of_lanes_pp], am_bits, fec_en, &my_am_ts);
          COMPILER_64_COPY(my_am_ts_calc_value[no_of_lanes_pp], my_am_ts);
          no_of_lanes_pp++;
        }
        no_of_lanes_pp = 0;

        /* Find min my_am_ts_calc_value */
        COMPILER_64_COPY(my_am_ts_calc_value_min, my_am_ts_calc_value[0]) ;
        for(j = 1 ; j < no_of_vl ; j++) {
          /* if(my_am_ts_calc_value_min > my_am_ts_calc_value[j]) my_am_ts_calc_value_min = my_am_ts_calc_value[j] ; */
          if(COMPILER_64_GT(my_am_ts_calc_value_min, my_am_ts_calc_value[j])) COMPILER_64_COPY(my_am_ts_calc_value_min, my_am_ts_calc_value[j]) ;
        }

        /* normalized my_am_ts_calc_value */
        for(k = 0 ; k < no_of_vl ; k++) {
            COMPILER_64_COPY(temp_compiler_64_value, my_am_ts_calc_value[k]);
            COMPILER_64_SUB_64(temp_compiler_64_value, my_am_ts_calc_value_min);
          TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : After  am_calc adjust addr=0x%x lane_mask=0x%x : array_num %2d : my_am_ts_calc_value (%016x,%016x) : normalized (%016x,%016x)\n",
                                            __func__, pc_copy.addr, pc_copy.lane_mask, k, COMPILER_64_HI(my_am_ts_calc_value[k]), COMPILER_64_LO(my_am_ts_calc_value[k]),
                                           COMPILER_64_HI(temp_compiler_64_value), COMPILER_64_LO(temp_compiler_64_value)));
        }

        /* Step 5 : Get the ref. lane */ /* TBD : Review the comparison of positive or negative value */
        /* Align to earliest/largest/oldest -> ref_value=max AM TS */
        /* Align to latest/smallest/newest  -> ref_value=min AM TS */
        ref_lane = 0;
        COMPILER_64_COPY(my_ref_value, my_am_ts_calc_value[0]);
        for(j=1; j<no_of_vl; j++) {
          /* norm_to_earliest */
          if(timesync_am_norm_mode == phymodTimesyncCompensationModeEarliestLane) {
            if(COMPILER_64_GT(my_ref_value, my_am_ts_calc_value[j])) {
              COMPILER_64_COPY(my_ref_value, my_am_ts_calc_value[j]);
              ref_lane = j;
            }
          }
          /* norm_to_latest */
          else if(timesync_am_norm_mode == phymodTimesyncCompensationModeLatestlane) {
            if(COMPILER_64_LT(my_ref_value, my_am_ts_calc_value[j])) {
              COMPILER_64_COPY(my_ref_value, my_am_ts_calc_value[j]);
              ref_lane = j;
            }
          }
        }
        TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : ref_lane=%d.\n", __func__, pc_copy.addr, pc_copy.lane_mask, ref_lane));

        /*Step 6 get the normailzed values*/
        for(j=0; j<no_of_vl; j++) {
            /* my_am_ts_calc_value_norm[j] = my_am_ts_calc_value[ref_lane] - my_am_ts_calc_value[j]; */
            COMPILER_64_COPY(my_am_ts_calc_value_norm[j], my_am_ts_calc_value[ref_lane]);
            COMPILER_64_SUB_64(my_am_ts_calc_value_norm[j], my_am_ts_calc_value[j]);
            TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : Calc Normalized value addr=0x%x lane_mask=0x%x : array_num %2d : my_am_ts_calc_value=(%016x,%016x) my_am_ts_calc_value[ref_lane]=(%016x,%016x) my_am_ts_calc_value_norm=(%016x,%016x) \n",
                                             __func__, pc_copy.addr, pc_copy.lane_mask, j, COMPILER_64_HI(my_am_ts_calc_value[j]), COMPILER_64_LO(my_am_ts_calc_value[j]), COMPILER_64_HI(my_am_ts_calc_value[ref_lane]), COMPILER_64_LO(my_am_ts_calc_value[j]), COMPILER_64_HI(my_am_ts_calc_value_norm[j]), COMPILER_64_LO(my_am_ts_calc_value[j])));
        }

        /*Step 6b compute new avg values*/
        for(j=0; j<no_of_vl; j++) {
            /*
             * my_am_ts_calc_value_prev_avg = my_am_ts_calc_value_avg[calc_array_idx][j] ;
             * my_am_ts_calc_value_avg[calc_array_idx][j] = ((my_am_ts_calc_value_avg[calc_array_idx][j] * my_am_ts_calc_value_cnt[calc_array_idx][j]) + my_am_ts_calc_value_norm[j]) / (my_am_ts_calc_value_cnt[calc_array_idx][j] + 1) ;
             */
            COMPILER_64_COPY(my_am_ts_calc_value_prev_avg, my_am_ts_calc_value_avg[calc_array_idx][j]);

            COMPILER_64_UMUL_32(my_am_ts_calc_value_avg[calc_array_idx][j], my_am_ts_calc_value_cnt[calc_array_idx][j]);
            COMPILER_64_ADD_64(my_am_ts_calc_value_avg[calc_array_idx][j], my_am_ts_calc_value_norm[j]);
            COMPILER_64_SET(temp_compiler_64_value, 0, my_am_ts_calc_value_cnt[calc_array_idx][j] + 1);
            COMPILER_64_UDIV_64(my_am_ts_calc_value_avg[calc_array_idx][j], temp_compiler_64_value);

            COMPILER_64_SET(temp_compiler_64_value, 0, 0);
            if((COMPILER_64_EQ(my_am_ts_calc_value_high[calc_array_idx][j], temp_compiler_64_value)) || (COMPILER_64_LT(my_am_ts_calc_value_high[calc_array_idx][j], my_am_ts_calc_value_avg[calc_array_idx][j]))) {
                COMPILER_64_COPY(my_am_ts_calc_value_high[calc_array_idx][j], my_am_ts_calc_value_avg[calc_array_idx][j]);
            }
            if((COMPILER_64_EQ(my_am_ts_calc_value_low[calc_array_idx][j], temp_compiler_64_value)) || (COMPILER_64_GT(my_am_ts_calc_value_low[calc_array_idx][j], my_am_ts_calc_value_avg[calc_array_idx][j]))) {
                COMPILER_64_COPY(my_am_ts_calc_value_low[calc_array_idx][j], my_am_ts_calc_value_avg[calc_array_idx][j]);
            }

            /* my_am_ts_calc_value_diff[calc_array_idx][j] = my_am_ts_calc_value_high[calc_array_idx][j] - my_am_ts_calc_value_low[calc_array_idx][j] ; */
            COMPILER_64_COPY(my_am_ts_calc_value_diff[calc_array_idx][j], my_am_ts_calc_value_high[calc_array_idx][j]);
            COMPILER_64_SUB_64(my_am_ts_calc_value_diff[calc_array_idx][j], my_am_ts_calc_value_low[calc_array_idx][j]);
            TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : Calc new re-avg value : addr=0x%x lane_mask=0x%x :array_num %2d : my_am_ts_calc_value_prev_avg (%016x,%016x) : my_am_ts_calc_value_cnt %04d : my_am_ts_calc_value_norm (%016x, %016x) : re-average my_am_ts_calc_value_avg (%016x,%016x) Low (%016x,%016x) : High (%016x,%016x) : Diff (%016x,%016x) : frac_ns %x\n",
                                             __func__, pc_copy.addr, pc_copy.lane_mask, j, COMPILER_64_HI(my_am_ts_calc_value_prev_avg), COMPILER_64_LO(my_am_ts_calc_value_prev_avg), my_am_ts_calc_value_cnt[calc_array_idx][j], COMPILER_64_HI(my_am_ts_calc_value_norm[j]), COMPILER_64_LO(my_am_ts_calc_value_norm[j]),
                                             COMPILER_64_HI(my_am_ts_calc_value_avg[calc_array_idx][j]),  COMPILER_64_LO(my_am_ts_calc_value_avg[calc_array_idx][j]), COMPILER_64_HI(my_am_ts_calc_value_low[calc_array_idx][j]), COMPILER_64_LO(my_am_ts_calc_value_low[calc_array_idx][j]), COMPILER_64_HI(my_am_ts_calc_value_high[calc_array_idx][j]), COMPILER_64_LO(my_am_ts_calc_value_high[calc_array_idx][j]),
                                             COMPILER_64_HI(my_am_ts_calc_value_diff[calc_array_idx][j]), COMPILER_64_LO(my_am_ts_calc_value_diff[calc_array_idx][j]), frac_ns));

            my_am_ts_calc_value_cnt[calc_array_idx][j]++ ;
        }
    } /* if(pc->deskew_cnt >= 1) */

    pc_copy.lane_mask = 0x1 << start_lane;

    if(type & TEFMOD_RX_DESKEW_ADJUST) {
        /*Step 7 :  Program deskew value */
        RX_X4_RX_DESKEW_ADJ_INT0r_CLR(reg_rx_deskew_int0);
        RX_X4_RX_DESKEW_ADJ_FRAC0r_CLR(reg_rx_deskew_frac0);
        RX_X4_RX_DESKEW_ADJ_INT1r_CLR(reg_rx_deskew_int1);
        RX_X4_RX_DESKEW_ADJ_FRAC1r_CLR(reg_rx_deskew_frac1);
        RX_X4_RX_DESKEW_ADJ_INT2r_CLR(reg_rx_deskew_int2);
        RX_X4_RX_DESKEW_ADJ_FRAC2r_CLR(reg_rx_deskew_frac2);
        RX_X4_RX_DESKEW_ADJ_INT3r_CLR(reg_rx_deskew_int3);
        RX_X4_RX_DESKEW_ADJ_FRAC3r_CLR(reg_rx_deskew_frac3);
        RX_X4_RX_DESKEW_ADJ_INT4r_CLR(reg_rx_deskew_int4);
        RX_X4_RX_DESKEW_ADJ_FRAC4r_CLR(reg_rx_deskew_frac4);

        no_of_lanes_pp = 0;
        reg_copy = 0;
        if((pc->lane_mask == 0x3) || pc->lane_mask == 0xc) {
          for(j=0; j<2; j++) {
              if (pc->lane_mask == 0xc) {
                reg_copy = 2;
              }
              pc_copy.lane_mask = 0x1 << reg_copy;

              COMPILER_64_COPY(my_deskew_val, my_am_ts_calc_value_avg[calc_array_idx][no_of_lanes_pp]);
              /* deskew value (23-bit)={deskew_int(12-bits),deskew_frac(10-bits)} */
              TEFMOD16_GEN3_1588_RX_DESKEW_SET(my_deskew_val_int, my_deskew_val_frac, COMPILER_64_LO(my_deskew_val));

              RX_X4_RX_DESKEW_ADJ_INT0r_TAB_DS_INT_NS_0f_SET(reg_rx_deskew_int0,my_deskew_val_int);
              PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_INT0r(&pc_copy, reg_rx_deskew_int0));

              RX_X4_RX_DESKEW_ADJ_FRAC0r_TAB_DS_FRAC_NS_0f_SET(reg_rx_deskew_frac0,my_deskew_val_frac);
              PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_FRAC0r(&pc_copy, reg_rx_deskew_frac0));

              TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : array_num=%02d my_deskew_val=(%016x, %016x) my_deskew_val_int=%04x my_deskew_val_frac=%03x\n",
                                                 __func__, pc_copy.addr, pc_copy.lane_mask, no_of_lanes_pp, COMPILER_64_HI(my_deskew_val), COMPILER_64_LO(my_deskew_val), my_deskew_val_int, my_deskew_val_frac)) ;

              reg_copy++;
              no_of_lanes_pp++;
         }
         if(no_of_vl == 4) { /* 40G MLD over 2 lanes has 4 VLs */
           reg_copy = 0;
           for(j=2; j<4; j++) {
            if (pc->lane_mask == 0xc) {
              reg_copy = 2;
            }

            COMPILER_64_COPY(my_deskew_val, my_am_ts_calc_value_avg[calc_array_idx][no_of_lanes_pp]);
            /* deskew value (23-bit)={deskew_int(12-bits),deskew_frac(10-bits)} */
            TEFMOD16_GEN3_1588_RX_DESKEW_SET(my_deskew_val_int, my_deskew_val_frac, COMPILER_64_LO(my_deskew_val));

            RX_X4_RX_DESKEW_ADJ_INT1r_TAB_DS_INT_NS_1f_SET(reg_rx_deskew_int1,my_deskew_val_int);
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_INT1r(&pc_copy, reg_rx_deskew_int1));

            RX_X4_RX_DESKEW_ADJ_FRAC1r_TAB_DS_FRAC_NS_1f_SET(reg_rx_deskew_frac1,my_deskew_val_frac);
            PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_FRAC1r(&pc_copy, reg_rx_deskew_frac1));

            TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : array_num=%02d my_deskew_val=(%016x, %016x) my_deskew_val_int=%04x my_deskew_val_frac=%03x\n",
                                               __func__, pc_copy.addr, pc_copy.lane_mask, no_of_lanes_pp, COMPILER_64_HI(my_deskew_val), COMPILER_64_LO(my_deskew_val), my_deskew_val_int, my_deskew_val_frac)) ;

            reg_copy++;;
            no_of_lanes_pp++;
           } /* for(j=0; j<2; j++) */
          } /* if(no_of_vl == 4) */
         } else if (pc->lane_mask == 0xf) {
          if(no_of_vl == 4) {
           for(j=0; j<4; j++) {
             pc_copy.lane_mask = 0x1 << j;

             COMPILER_64_COPY(my_deskew_val, my_am_ts_calc_value_avg[calc_array_idx][no_of_lanes_pp]);
             /* deskew value (23-bit)={deskew_int(12-bits),deskew_frac(10-bits)} */
             TEFMOD16_GEN3_1588_RX_DESKEW_SET(my_deskew_val_int, my_deskew_val_frac, COMPILER_64_LO(my_deskew_val));

             RX_X4_RX_DESKEW_ADJ_INT0r_TAB_DS_INT_NS_0f_SET(reg_rx_deskew_int0,my_deskew_val_int);
             PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_INT0r(&pc_copy, reg_rx_deskew_int0));

             RX_X4_RX_DESKEW_ADJ_FRAC0r_TAB_DS_FRAC_NS_0f_SET(reg_rx_deskew_frac0,my_deskew_val_frac);
             PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_FRAC0r(&pc_copy, reg_rx_deskew_frac0));

            TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : array_num=%02d my_deskew_val=(%016x, %016x) my_deskew_val_int=%04x my_deskew_val_frac=%03x\n",
                                               __func__, pc_copy.addr, pc_copy.lane_mask, no_of_lanes_pp, COMPILER_64_HI(my_deskew_val), COMPILER_64_LO(my_deskew_val), my_deskew_val_int, my_deskew_val_frac)) ;
             no_of_lanes_pp++;
           } /* for(j=0; j<4; j++) */
          } /* (no_of_vl == 4) */
          else if(no_of_vl == 20) { /* 40G MLD over 2 lanes has 4 VLs */
           for(j=0; j<4; j++) {
             pc_copy.lane_mask = 0x1 << j;

             COMPILER_64_COPY(my_deskew_val, my_am_ts_calc_value_avg[calc_array_idx][no_of_lanes_pp]);
             /* deskew value (23-bit)={deskew_int(12-bits),deskew_frac(10-bits)} */
             TEFMOD16_GEN3_1588_RX_DESKEW_SET(my_deskew_val_int, my_deskew_val_frac, COMPILER_64_LO(my_deskew_val));

             RX_X4_RX_DESKEW_ADJ_INT0r_TAB_DS_INT_NS_0f_SET(reg_rx_deskew_int0,my_deskew_val_int);
             PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_INT0r(&pc_copy, reg_rx_deskew_int0));

             RX_X4_RX_DESKEW_ADJ_FRAC0r_TAB_DS_FRAC_NS_0f_SET(reg_rx_deskew_frac0,my_deskew_val_frac);
             PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_FRAC0r(&pc_copy, reg_rx_deskew_frac0));

             TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : array_num=%02d my_deskew_val=(%016x, %016x) my_deskew_val_int=%04x my_deskew_val_frac=%03x\n",
                                               __func__, pc_copy.addr, pc_copy.lane_mask, no_of_lanes_pp, COMPILER_64_HI(my_deskew_val), COMPILER_64_LO(my_deskew_val), my_deskew_val_int, my_deskew_val_frac)) ;
             no_of_lanes_pp++;

             COMPILER_64_COPY(my_deskew_val, my_am_ts_calc_value_avg[calc_array_idx][no_of_lanes_pp]);
             /* deskew value (23-bit)={deskew_int(12-bits),deskew_frac(10-bits)} */
             TEFMOD16_GEN3_1588_RX_DESKEW_SET(my_deskew_val_int, my_deskew_val_frac, COMPILER_64_LO(my_deskew_val));

             RX_X4_RX_DESKEW_ADJ_INT1r_TAB_DS_INT_NS_1f_SET(reg_rx_deskew_int1,my_deskew_val_int);
             PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_INT1r(&pc_copy, reg_rx_deskew_int1));

             RX_X4_RX_DESKEW_ADJ_FRAC1r_TAB_DS_FRAC_NS_1f_SET(reg_rx_deskew_frac1,my_deskew_val_frac);
             PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_FRAC1r(&pc_copy, reg_rx_deskew_frac1));

            TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : array_num=%02d my_deskew_val=(%016x, %016x) my_deskew_val_int=%04x my_deskew_val_frac=%03x\n",
                                               __func__, pc_copy.addr, pc_copy.lane_mask, no_of_lanes_pp, COMPILER_64_HI(my_deskew_val), COMPILER_64_LO(my_deskew_val), my_deskew_val_int, my_deskew_val_frac)) ;

             no_of_lanes_pp++;

             COMPILER_64_COPY(my_deskew_val, my_am_ts_calc_value_avg[calc_array_idx][no_of_lanes_pp]);
             /* deskew value (23-bit)={deskew_int(12-bits),deskew_frac(10-bits)} */
             TEFMOD16_GEN3_1588_RX_DESKEW_SET(my_deskew_val_int, my_deskew_val_frac, COMPILER_64_LO(my_deskew_val));

             RX_X4_RX_DESKEW_ADJ_INT2r_TAB_DS_INT_NS_2f_SET(reg_rx_deskew_int2,my_deskew_val_int);
             PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_INT2r(&pc_copy, reg_rx_deskew_int2));

             RX_X4_RX_DESKEW_ADJ_FRAC2r_TAB_DS_FRAC_NS_2f_SET(reg_rx_deskew_frac2,my_deskew_val_frac);
             PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_FRAC2r(&pc_copy, reg_rx_deskew_frac2));

            TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : array_num=%02d my_deskew_val=(%016x, %016x) my_deskew_val_int=%04x my_deskew_val_frac=%03x\n",
                                               __func__, pc_copy.addr, pc_copy.lane_mask, no_of_lanes_pp, COMPILER_64_HI(my_deskew_val), COMPILER_64_LO(my_deskew_val), my_deskew_val_int, my_deskew_val_frac)) ;
             no_of_lanes_pp++;

             COMPILER_64_COPY(my_deskew_val, my_am_ts_calc_value_avg[calc_array_idx][no_of_lanes_pp]);
             /* deskew value (23-bit)={deskew_int(12-bits),deskew_frac(10-bits)} */
             TEFMOD16_GEN3_1588_RX_DESKEW_SET(my_deskew_val_int, my_deskew_val_frac, COMPILER_64_LO(my_deskew_val));

             RX_X4_RX_DESKEW_ADJ_INT3r_TAB_DS_INT_NS_3f_SET(reg_rx_deskew_int3,my_deskew_val_int);
             PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_INT3r(&pc_copy, reg_rx_deskew_int3));

             RX_X4_RX_DESKEW_ADJ_FRAC3r_TAB_DS_FRAC_NS_3f_SET(reg_rx_deskew_frac3,my_deskew_val_frac);
             PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_FRAC3r(&pc_copy, reg_rx_deskew_frac3));

            TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : array_num=%02d my_deskew_val=(%016x, %016x) my_deskew_val_int=%04x my_deskew_val_frac=%03x\n",
                                               __func__, pc_copy.addr, pc_copy.lane_mask, no_of_lanes_pp, COMPILER_64_HI(my_deskew_val), COMPILER_64_LO(my_deskew_val), my_deskew_val_int, my_deskew_val_frac)) ;
             no_of_lanes_pp++;

             COMPILER_64_COPY(my_deskew_val, my_am_ts_calc_value_avg[calc_array_idx][no_of_lanes_pp]);
             /* deskew value (23-bit)={deskew_int(12-bits),deskew_frac(10-bits)} */
             TEFMOD16_GEN3_1588_RX_DESKEW_SET(my_deskew_val_int, my_deskew_val_frac, COMPILER_64_LO(my_deskew_val));

             RX_X4_RX_DESKEW_ADJ_INT4r_TAB_DS_INT_NS_4f_SET(reg_rx_deskew_int4,my_deskew_val_int);
             PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_INT4r(&pc_copy, reg_rx_deskew_int4));

             RX_X4_RX_DESKEW_ADJ_FRAC4r_TAB_DS_FRAC_NS_4f_SET(reg_rx_deskew_frac4,my_deskew_val_frac);
             PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_FRAC4r(&pc_copy, reg_rx_deskew_frac4));

            TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : array_num=%02d my_deskew_val=(%016x, %016x) my_deskew_val_int=%04x my_deskew_val_frac=%03x\n",
                                               __func__, pc_copy.addr, pc_copy.lane_mask, no_of_lanes_pp, COMPILER_64_HI(my_deskew_val), COMPILER_64_LO(my_deskew_val), my_deskew_val_int, my_deskew_val_frac)) ;
             no_of_lanes_pp++;
           } /* for(j=0; j<4; j++) */
           no_of_lanes_pp = 20;
           } /* if(no_of_vl == 20) */
         }  /* if(pc->port_type == TEFMOD_SINGLE_PORT) */
        else { /* QUAD ports 0-3 or TRI1 ports 0-1 or TRI2 ports 2-3 */
          pc_copy.lane_mask = 0x1 << start_lane;

          COMPILER_64_SET(my_deskew_val, 0, 0);
          my_deskew_val_int = 0;
          my_deskew_val_frac = 0;

          RX_X4_RX_DESKEW_ADJ_INT0r_TAB_DS_INT_NS_0f_SET(reg_rx_deskew_int0,my_deskew_val_int);
          PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_INT0r(&pc_copy, reg_rx_deskew_int0));

          RX_X4_RX_DESKEW_ADJ_FRAC0r_TAB_DS_FRAC_NS_0f_SET(reg_rx_deskew_frac0,my_deskew_val_frac);
          PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_DESKEW_ADJ_FRAC0r(&pc_copy, reg_rx_deskew_frac0));

            TEFMOD16_GEN3_DBG_IN_FUNC_VAR_INFO(pc, ("%-22s : addr=0x%x lane_mask=0x%x : array_num=%02d my_deskew_val=(%016x, %016x) my_deskew_val_int=%04x my_deskew_val_frac=%03x\n",
                                               __func__, pc_copy.addr, pc_copy.lane_mask, no_of_lanes_pp, COMPILER_64_HI(my_deskew_val), COMPILER_64_LO(my_deskew_val), my_deskew_val_int, my_deskew_val_frac)) ;
        }
    } /* if(pc->deskew_cnt == 1) */

    return PHYMOD_E_NONE;
} /* tefmod16_gen3_1588_rx_deskew_set */

int tefmod16_gen3_1588_tx_info_get(PHYMOD_ST* pc, tefmod16_gen3_ts_tx_info_t* ts_tx_info)
{
    TX_X4_TX_1588_TIMESTAMP_STSr_t reg_tx_1588_timestamp_sts;
    TX_X4_TX_1588_TIMESTAMP_HIr_t reg_tx_1588_timestamp_hi;
    TX_X4_TX_1588_TIMESTAMP_LOr_t reg_tx_1588_timestamp_lo;
    TX_X4_TX_TS_SEQ_IDr_t reg_tx_ts_seq_id;
    uint16_t ts_entry_valid;

    TX_X4_TX_1588_TIMESTAMP_STSr_CLR(reg_tx_1588_timestamp_sts);
    TX_X4_TX_1588_TIMESTAMP_HIr_CLR(reg_tx_1588_timestamp_hi);
    TX_X4_TX_1588_TIMESTAMP_LOr_CLR(reg_tx_1588_timestamp_lo);
    TX_X4_TX_TS_SEQ_IDr_CLR(reg_tx_ts_seq_id);

    PHYMOD_IF_ERR_RETURN(READ_TX_X4_TX_1588_TIMESTAMP_STSr(pc, &reg_tx_1588_timestamp_sts));
    ts_entry_valid = TX_X4_TX_1588_TIMESTAMP_STSr_TS_ENTRY_VALIDf_GET(reg_tx_1588_timestamp_sts);
    if (ts_entry_valid == 0) {
        return PHYMOD_E_NONE; /* TBD change to PHYMOD_E_EMPTY in master;*/
    }

    PHYMOD_IF_ERR_RETURN(READ_TX_X4_TX_1588_TIMESTAMP_HIr(pc, &reg_tx_1588_timestamp_hi));
    PHYMOD_IF_ERR_RETURN(READ_TX_X4_TX_1588_TIMESTAMP_LOr(pc, &reg_tx_1588_timestamp_lo));
    PHYMOD_IF_ERR_RETURN(READ_TX_X4_TX_TS_SEQ_IDr(pc, &reg_tx_ts_seq_id));

    ts_tx_info->ts_val_hi = TX_X4_TX_1588_TIMESTAMP_HIr_TS_VALUE_HIf_GET(reg_tx_1588_timestamp_hi);
    ts_tx_info->ts_val_lo = TX_X4_TX_1588_TIMESTAMP_LOr_TS_VALUE_LOf_GET(reg_tx_1588_timestamp_lo);
    ts_tx_info->ts_seq_id = TX_X4_TX_TS_SEQ_IDr_TS_SEQUENCE_IDf_GET(reg_tx_ts_seq_id);
    ts_tx_info->ts_sub_nanosec = TX_X4_TX_1588_TIMESTAMP_STSr_TS_SUB_NANO_FIELDf_GET(reg_tx_1588_timestamp_sts);

    return PHYMOD_E_NONE;
}

STATIC
int _tefmod16_gen3_rx_phy_lane_get(PHYMOD_ST *pc, int rx_logical_lane,
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
            *rx_phy_lane = RX_X1_RX_LN_SWPr_LOGICAL0_TO_PHY_SELf_GET(rx_lane_swap_reg);
            break;
        case 1:
            *rx_phy_lane = RX_X1_RX_LN_SWPr_LOGICAL1_TO_PHY_SELf_GET(rx_lane_swap_reg);
            break;
        case 2:
            *rx_phy_lane = RX_X1_RX_LN_SWPr_LOGICAL2_TO_PHY_SELf_GET(rx_lane_swap_reg);
            break;
        case 3:
            *rx_phy_lane = RX_X1_RX_LN_SWPr_LOGICAL3_TO_PHY_SELf_GET(rx_lane_swap_reg);
            break;
        default:
           return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}

int tefmod16_gen3_synce_stg0_mode_set(PHYMOD_ST *pc, int mode)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN0_SYNCE_CTL_STAGE0r_t MAIN0_SYNCE_CTL_STAGE0r_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    MAIN0_SYNCE_CTL_STAGE0r_CLR(MAIN0_SYNCE_CTL_STAGE0r_reg);

    PHYMOD_IF_ERR_RETURN
        (_tefmod16_gen3_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SYNCE_CTL_STAGE0r(pc,
                                                 &MAIN0_SYNCE_CTL_STAGE0r_reg));

    switch(phy_lane) {
        case 0:
           MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE0f_SET(MAIN0_SYNCE_CTL_STAGE0r_reg, mode);
           break;
        case 1:
           MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE1f_SET(MAIN0_SYNCE_CTL_STAGE0r_reg, mode);
           break;
        case 2:
           MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE2f_SET(MAIN0_SYNCE_CTL_STAGE0r_reg, mode);
           break;
        case 3:
           MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE3f_SET(MAIN0_SYNCE_CTL_STAGE0r_reg, mode);
           break;
        default:
           return PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SYNCE_CTL_STAGE0r(pc, MAIN0_SYNCE_CTL_STAGE0r_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_synce_stg1_mode_set(PHYMOD_ST *pc, int mode)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN0_SYNCE_CTLr_t MAIN0_SYNCE_CTLr_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    MAIN0_SYNCE_CTLr_CLR(MAIN0_SYNCE_CTLr_reg);

    PHYMOD_IF_ERR_RETURN
        (_tefmod16_gen3_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SYNCE_CTLr(pc, &MAIN0_SYNCE_CTLr_reg));

    switch(phy_lane) {
       case 0:
           MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE0f_SET(MAIN0_SYNCE_CTLr_reg, mode);
           break;
        case 1:
           MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE1f_SET(MAIN0_SYNCE_CTLr_reg, mode);
           break;
        case 2:
           MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE2f_SET(MAIN0_SYNCE_CTLr_reg, mode);
           break;
        case 3:
           MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE3f_SET(MAIN0_SYNCE_CTLr_reg, mode);
           break;
        default:
           return PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN(MODIFY_MAIN0_SYNCE_CTLr(pc, MAIN0_SYNCE_CTLr_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_synce_clk_ctrl_set(PHYMOD_ST *pc, uint32_t val)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    phymod_access_t pa_copy;
    RX_X4_SYNCE_FRACTIONAL_DIVr_t RX_X4_SYNCE_FRACTIONAL_DIVr_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_tefmod16_gen3_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    pa_copy.lane_mask = 0x1 << phy_lane;

    RX_X4_SYNCE_FRACTIONAL_DIVr_CLR(RX_X4_SYNCE_FRACTIONAL_DIVr_reg);
    RX_X4_SYNCE_FRACTIONAL_DIVr_SET(RX_X4_SYNCE_FRACTIONAL_DIVr_reg, val);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_SYNCE_FRACTIONAL_DIVr(&pa_copy, RX_X4_SYNCE_FRACTIONAL_DIVr_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_synce_stg0_mode_get(PHYMOD_ST *pc, int *mode)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN0_SYNCE_CTL_STAGE0r_t MAIN0_SYNCE_CTL_STAGE0r_reg;

    PHYMOD_IF_ERR_RETURN(phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    MAIN0_SYNCE_CTL_STAGE0r_CLR(MAIN0_SYNCE_CTL_STAGE0r_reg);

    PHYMOD_IF_ERR_RETURN(_tefmod16_gen3_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SYNCE_CTL_STAGE0r(pc, &MAIN0_SYNCE_CTL_STAGE0r_reg));

    switch(phy_lane) {
         case 0:
            *mode = MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE0f_GET(MAIN0_SYNCE_CTL_STAGE0r_reg);
            break;
         case 1:
            *mode = MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE1f_GET(MAIN0_SYNCE_CTL_STAGE0r_reg);
            break;
         case 2:
            *mode = MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE2f_GET(MAIN0_SYNCE_CTL_STAGE0r_reg);
            break;
         case 3:
            *mode = MAIN0_SYNCE_CTL_STAGE0r_SYNCE_STAGE0_MODE_PHY_LANE3f_GET(MAIN0_SYNCE_CTL_STAGE0r_reg);
            break;
         default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_synce_stg1_mode_get(PHYMOD_ST *pc, int *mode)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    MAIN0_SYNCE_CTLr_t MAIN0_SYNCE_CTLr_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    MAIN0_SYNCE_CTLr_CLR(MAIN0_SYNCE_CTLr_reg);

    PHYMOD_IF_ERR_RETURN
        (_tefmod16_gen3_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_IF_ERR_RETURN(READ_MAIN0_SYNCE_CTLr(pc, &MAIN0_SYNCE_CTLr_reg));

    switch(phy_lane) {
         case 0:
            *mode = MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE0f_GET(MAIN0_SYNCE_CTLr_reg);
            break;
         case 1:
            *mode = MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE1f_GET(MAIN0_SYNCE_CTLr_reg);
            break;
         case 2:
            *mode = MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE2f_GET(MAIN0_SYNCE_CTLr_reg);
            break;
         case 3:
            *mode = MAIN0_SYNCE_CTLr_SYNCE_MODE_PHY_LANE3f_GET(MAIN0_SYNCE_CTLr_reg);
            break;
         default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}

int tefmod16_gen3_synce_clk_ctrl_get(PHYMOD_ST *pc, uint32_t *val)
{
    int start_lane = 0, num_lane = 0, phy_lane = 0;
    phymod_access_t pa_copy;
    RX_X4_SYNCE_FRACTIONAL_DIVr_t RX_X4_SYNCE_FRACTIONAL_DIVr_reg;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (_tefmod16_gen3_rx_phy_lane_get(pc, start_lane, &phy_lane));

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    pa_copy.lane_mask = 0x1 << phy_lane;

    RX_X4_SYNCE_FRACTIONAL_DIVr_CLR(RX_X4_SYNCE_FRACTIONAL_DIVr_reg);

    PHYMOD_IF_ERR_RETURN(READ_RX_X4_SYNCE_FRACTIONAL_DIVr(&pa_copy, &RX_X4_SYNCE_FRACTIONAL_DIVr_reg));

    *val = RX_X4_SYNCE_FRACTIONAL_DIVr_GET(RX_X4_SYNCE_FRACTIONAL_DIVr_reg);

    return PHYMOD_E_NONE;
}

/*!
@brief Set FEC Bypass Error Indictator.
@param pc handle to current TSCF context (a.k.a #PHYMOD_ST)
@param enable control the fec_bypass_indication
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details
Note: FEC_bypass_indication_enable has no effect when FEC_bypass_correction_enable is asserted.
*/
int tefmod16_gen3_fec_bypass_indication_set(PHYMOD_ST *pc, uint32_t enable)
{
    RX_X4_CL91_RX_CTL0r_t RX_X4_CL91_RX_CTL0r_reg;

    RX_X4_CL91_RX_CTL0r_CLR(RX_X4_CL91_RX_CTL0r_reg);

    RX_X4_CL91_RX_CTL0r_FEC_BYP_IND_ENf_SET(RX_X4_CL91_RX_CTL0r_reg, enable);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_CL91_RX_CTL0r(pc, RX_X4_CL91_RX_CTL0r_reg));

    return PHYMOD_E_NONE;
}

/*!
@brief Get FEC Bypass Error Indictator.
@param pc handle to current TSCF context (a.k.a #PHYMOD_ST)
@param enable  the status fec_bypass_indication
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details
*/
int tefmod16_gen3_fec_bypass_indication_get(PHYMOD_ST *pc, uint32_t *enable)
{
    RX_X4_CL91_RX_CTL0r_t RX_X4_CL91_RX_CTL0r_reg;

    RX_X4_CL91_RX_CTL0r_CLR(RX_X4_CL91_RX_CTL0r_reg);

    PHYMOD_IF_ERR_RETURN(READ_RX_X4_CL91_RX_CTL0r(pc, &RX_X4_CL91_RX_CTL0r_reg));
    *enable = RX_X4_CL91_RX_CTL0r_FEC_BYP_IND_ENf_GET(RX_X4_CL91_RX_CTL0r_reg);

    return PHYMOD_E_NONE;

}

STATIC
int _tefmod16_gen3_port_lane_config_get(PHYMOD_ST *pc, int port_mode, int *port_starting_lane, int *port_num_lane)
{
    int start_lane, num_of_lane;

    PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_of_lane));

    switch (port_mode) {
        case  TEFMOD_SINGLE_PORT:
            *port_starting_lane = 0;
            *port_num_lane = 4;
            break;
        case  TEFMOD_DXGXS:
            if (start_lane < 2) {
                *port_starting_lane = 0;
                *port_num_lane = 2;
            } else {
                *port_starting_lane = 2;
                *port_num_lane = 2;
            }
            break;
        case  TEFMOD_TRI1_PORT:
            if (start_lane < 2) {
                *port_starting_lane = start_lane;
                *port_num_lane = 1;
            } else {
                *port_starting_lane = 2;
                *port_num_lane = 2;
            }
            break;
        case  TEFMOD_TRI2_PORT:
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

int tefmod16_gen3_rsfec_symbol_error_counter_get(PHYMOD_ST* pc,
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
        (_tefmod16_gen3_port_lane_config_get(pc, port_mode_sel, &port_start_lane, &port_num_lanes));
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
             case TEFMOD_SINGLE_PORT:
                 TEFMOD16_GEN3_RD_SYMBOL_ERROR_CNTR(&phy_copy, fec_lane_id, &error_cnt);
                 TEFMOD16_GEN3_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                 break;
             case TEFMOD_DXGXS:
                 if (fec_lane_id < 2) {
                     TEFMOD16_GEN3_RD_SYMBOL_ERROR_CNTR(&phy_copy, fec_lane_id, &error_cnt);
                     TEFMOD16_GEN3_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                     TEFMOD16_GEN3_RD_SYMBOL_ERROR_CNTR(&phy_copy, fec_lane_id + 2, &error_cnt);
                     TEFMOD16_GEN3_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                 } else {
                     return PHYMOD_E_PARAM;
                 }
                 break;
             case TEFMOD_TRI1_PORT:
             case TEFMOD_TRI2_PORT:
                 if (1 == port_num_lanes) {
                     for (symbol_reg_index = 0; symbol_reg_index < symbol_reg_max_num; symbol_reg_index++) {
                          TEFMOD16_GEN3_RD_SYMBOL_ERROR_CNTR(&phy_copy, symbol_reg_index, &error_cnt);
                          TEFMOD16_GEN3_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                     }
                 } else {
                    if (fec_lane_id < 2) {
                        TEFMOD16_GEN3_RD_SYMBOL_ERROR_CNTR(&phy_copy, fec_lane_id, &error_cnt);
                        TEFMOD16_GEN3_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                        TEFMOD16_GEN3_RD_SYMBOL_ERROR_CNTR(&phy_copy, fec_lane_id + 2, &error_cnt);
                        TEFMOD16_GEN3_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                    } else {
                        return PHYMOD_E_PARAM;
                    }
                 }
                  break;
             case TEFMOD_MULTI_PORT:
                 for (symbol_reg_index = 0; symbol_reg_index < symbol_reg_max_num; symbol_reg_index++) {
                     TEFMOD16_GEN3_RD_SYMBOL_ERROR_CNTR(&phy_copy, symbol_reg_index, &error_cnt);
                     TEFMOD16_GEN3_SYMBOL_ERROR_CNTR_ADD(symb_err_cnt[i], error_cnt);
                 }
                 break;
             default:
                return PHYMOD_E_PARAM;
        }
    }

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_fec_error_inject_set(PHYMOD_ST *pc,
                                       uint16_t error_control_map,
                                       tefmod16_gen3_fec_error_mask_t bit_error_mask)
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

    PHYMOD_IF_ERR_RETURN(tefmod16_gen3_FEC_get(pc, TEFMOD_CL74, &cl74_en));
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
            TEFMOD16_GEN3_PKTGEN_ERR_INJ_EN_FIELD_SET(cl74_en, phy_lane);
        }
    } else {
        /* RSFEC error injection is per port */
        TEFMOD16_GEN3_PKTGEN_ERR_INJ_EN_FIELD_SET(cl74_en, start_lane);
    }

    PHYMOD_IF_ERR_RETURN(WRITE_PKTGEN_ERR_INJ_EN0r(&pa_copy, PKTGEN_ERR_INJ_EN0r_reg));
    PHYMOD_IF_ERR_RETURN(WRITE_PKTGEN_ERR_INJ_EN1r(&pa_copy, PKTGEN_ERR_INJ_EN1r_reg));

    return PHYMOD_E_NONE;
}

int tefmod16_gen3_fec_error_inject_get(PHYMOD_ST *pc,
                                       uint16_t* error_control_map,
                                       tefmod16_gen3_fec_error_mask_t* bit_error_mask)
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

    PHYMOD_IF_ERR_RETURN(tefmod16_gen3_FEC_get(pc, TEFMOD_CL74, &cl74_en));
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
            TEFMOD16_GEN3_PKTGEN_ERR_INJ_EN_FIELD_GET(cl74_en, phy_lane, &errgen_en);
            if (errgen_en) {
                *error_control_map |= 0x1 << (start_lane + i);
            }
        }
    } else {
        /* RSFEC error injection is per port */
        TEFMOD16_GEN3_PKTGEN_ERR_INJ_EN_FIELD_GET(cl74_en, start_lane, &errgen_en);
        *error_control_map = errgen_en ? 1 : 0;
    }

    return PHYMOD_E_NONE;
}
