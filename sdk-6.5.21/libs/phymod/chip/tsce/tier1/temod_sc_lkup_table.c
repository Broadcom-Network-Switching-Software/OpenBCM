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
 * File       : temod_sc_lkup_table.c
 * Description: c functions implementing SC Tier1s for TEMod Serdes Driver
 *---------------------------------------------------------------------*/

#ifndef _DV_TB_
 #define _SDK_TEMOD_ 1
#endif

#ifdef _DV_TB_
#include <stdio.h> 
#endif
#ifdef _SDK_TEMOD_
#include <phymod/phymod.h>
#include "temod_enum_defines.h"
#include "temod.h"
#endif
#include "sc_field_defines.h"
#include "tePCSRegEnums.h"

#ifdef _DV_TB_
#include "temod_sc_lkup_table.h" 
#endif

#ifndef PHYMOD_ST
#ifdef _SDK_TEMOD_
   #define PHYMOD_ST const phymod_access_t
#else
  #define PHYMOD_ST  temod_st
#endif
#endif

/*!
@brief Translate the software speed to PCS (hardware) speed.
@param  spd_intf speed type as specified in #temod_spd_intfc_type
@param  speed Recieves the PCS lookup table speed num. for the  spd_intf input
@returns The value PHYMOD_E_NONE upon successful completion
@details Takes in the speed enum type #temod_spd_intfc_type and returns the 
PCS lookup table speed num.
*/

int get_mapped_speed(temod_spd_intfc_type spd_intf, int *speed)
{
  switch(spd_intf) {
  case TEMOD_SPD_10_X1_SGMII       : *speed = digital_operationSpeeds_SPEED_10M_10p3125; break;
  case TEMOD_SPD_100_X1_SGMII      : *speed = digital_operationSpeeds_SPEED_100M_10p3125; break;
  case TEMOD_SPD_1000_X1_SGMII     : *speed = digital_operationSpeeds_SPEED_1000M_10p3125; break;
  case TEMOD_SPD_2500_X1           : *speed = digital_operationSpeeds_SPEED_2p5G_X1_10p3125; break;
  case TEMOD_SPD_10_SGMII          : *speed = digital_operationSpeeds_SPEED_10M; break;
  case TEMOD_SPD_100_SGMII         : *speed = digital_operationSpeeds_SPEED_100M; break;
  case TEMOD_SPD_1000_SGMII        : *speed = digital_operationSpeeds_SPEED_1000M; break;
  case TEMOD_SPD_2500              : *speed = digital_operationSpeeds_SPEED_2p5G_X1; break;
  case TEMOD_SPD_5000              : *speed = digital_operationSpeeds_SPEED_5G_X1; break;
  case TEMOD_SPD_1000_XFI          : *speed = digital_operationSpeeds_SPEED_1G_KX1; break;
  case TEMOD_SPD_5000_XFI          : *speed = digital_operationSpeeds_SPEED_5G_KR1; break;
  case TEMOD_SPD_10000_XFI         : *speed = digital_operationSpeeds_SPEED_10G_KR1; break;
  case TEMOD_SPD_10600_XFI_HG      : *speed = digital_operationSpeeds_SPEED_10p6_X1; break;
  case TEMOD_SPD_10000_HI          : *speed = digital_operationSpeeds_SPEED_10M; break;
  case TEMOD_SPD_10000             : *speed = digital_operationSpeeds_SPEED_10G_KX4; break;
  case TEMOD_SPD_12000_HI          : *speed = digital_operationSpeeds_SPEED_10M; break;
  case TEMOD_SPD_13000             : *speed = digital_operationSpeeds_SPEED_13G_X4; break;
  case TEMOD_SPD_15000             : *speed = digital_operationSpeeds_SPEED_15G_X4; break;
  case TEMOD_SPD_16000             : *speed = digital_operationSpeeds_SPEED_16G_X4; break;
  case TEMOD_SPD_20000_SCR         : *speed = digital_operationSpeeds_SPEED_20G_X4; break;  
  case TEMOD_SPD_20000             : *speed = digital_operationSpeeds_SPEED_20G_CX4; break;
  case TEMOD_SPD_21000             : *speed = digital_operationSpeeds_SPEED_21G_X4; break;
  case TEMOD_SPD_25455             : *speed = digital_operationSpeeds_SPEED_25p45G_X4; break;
  case TEMOD_SPD_31500             : *speed = digital_operationSpeeds_SPEED_31p5G_X4; break;
  case TEMOD_SPD_40G_X4            : *speed = digital_operationSpeeds_SPEED_40G_X4; break;  
  case TEMOD_SPD_42G_X4            : *speed = digital_operationSpeeds_SPEED_42G_X4; break;  /* 42G_X4 is MLD, 40G_X4 is BRCM */
  case TEMOD_SPD_40G_XLAUI         : *speed = digital_operationSpeeds_SPEED_40G_KR4; break; /* or RTL_SPEED_40G_CR4 */
  case TEMOD_SPD_42G_XLAUI         : *speed = digital_operationSpeeds_SPEED_42G_X4; break;
  case TEMOD_SPD_10000_X2          : *speed = digital_operationSpeeds_SPEED_10G_X2; break; /* or RTL_SPEED_10G_CX2 */
  case TEMOD_SPD_10000_HI_DXGXS    : *speed = digital_operationSpeeds_SPEED_10M; break;
  case TEMOD_SPD_10000_DXGXS       : *speed = digital_operationSpeeds_SPEED_10G_CX2; break;
  case TEMOD_SPD_10000_HI_DXGXS_SCR: *speed = digital_operationSpeeds_SPEED_10M; break;
  case TEMOD_SPD_10000_DXGXS_SCR   : *speed = digital_operationSpeeds_SPEED_10G_X2; break;
  case TEMOD_SPD_10500_HI_DXGXS    : *speed = digital_operationSpeeds_SPEED_10p5G_X2; break; 
  case TEMOD_SPD_10500_HI          : *speed = digital_operationSpeeds_SPEED_10p5G_X4; break; 
  case TEMOD_SPD_20G_MLD_DXGXS     : *speed = digital_operationSpeeds_SPEED_20G_KR2; break; /* or RTL_SPEED_20G_CR2 */
  case TEMOD_SPD_100G_CR10         : *speed = digital_operationSpeeds_SPEED_100G_CR10; break; 
  case TEMOD_SPD_107G_HG_CR10      : *speed = digital_operationSpeeds_SPEED_107G_X10; break;  
  case TEMOD_SPD_120G_CR12         : *speed = digital_operationSpeeds_SPEED_120G_X12; break; 
  case TEMOD_SPD_127G_HG_CR12      : *speed = digital_operationSpeeds_SPEED_127G_X12; break; 
  case TEMOD_SPD_12773_DXGXS       : *speed = digital_operationSpeeds_SPEED_12p7G_X2; break; 
  case TEMOD_SPD_15750_HI_DXGXS    : *speed = digital_operationSpeeds_SPEED_15p75G_X2; break; 
  case TEMOD_SPD_20G_DXGXS         : *speed = digital_operationSpeeds_SPEED_20G_X2; break; 
  case TEMOD_SPD_21G_HI_MLD_DXGXS  : *speed = digital_operationSpeeds_SPEED_21G_X2; break; 
  case TEMOD_SPD_31500_MLD         : *speed = digital_operationSpeeds_SPEED_31p5G_KR4; break; 
  default                          : break; 
  }
  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
/*!
@brief Read the 16 bit rev. id value etc.
@param  pc handle to current TE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful
@details Update the te_st with the values, corresponding to the speed
*/
int temod_get_ht_entries(PHYMOD_ST* pc)
{
      /*pc->speed =  sc_ht_entry[pc->spd_intf].speed; */

    if((pc->num_lanes & TEMOD_OVERRIDE_CFG) == 0) {
      pc->num_lanes =  sc_ht_entry[pc->speed].num_lanes;
      }
    else {
      pc->num_lanes =  pc->num_lanes & 0xffff;
      }

    if((pc->t_pma_os_mode & TEMOD_OVERRIDE_CFG) == 0) 
      pc->t_pma_os_mode = sc_ht_entry[pc->speed].t_pma_os_mode;
    else
      pc->t_pma_os_mode = pc->t_pma_os_mode & 0xffff;

    if ((pc->t_scr_mode & TEMOD_OVERRIDE_CFG) == 0)
      pc->t_scr_mode = sc_ht_entry[pc->speed].t_scr_mode;
    else
      pc->t_scr_mode = pc->t_scr_mode & 0xffff;

    if((pc->t_encode_mode & TEMOD_OVERRIDE_CFG) == 0) 
      pc->t_encode_mode = sc_ht_entry[pc->speed].t_encode_mode;
    else
      pc->t_encode_mode = pc->t_encode_mode & 0xffff;

    if((pc->cl48_check_end & TEMOD_OVERRIDE_CFG) == 0) 
      pc->cl48_check_end = sc_ht_entry[pc->speed].cl48_check_end;
    else
      pc->cl48_check_end = pc->cl48_check_end & 0xffff;
      
    if((pc->blk_sync_mode & TEMOD_OVERRIDE_CFG) == 0) 
      pc->blk_sync_mode = sc_ht_entry[pc->speed].blk_sync_mode;
    else
      pc->blk_sync_mode = pc->blk_sync_mode & 0xffff;

    if((pc->r_reorder_mode & TEMOD_OVERRIDE_CFG) == 0) 
      pc->r_reorder_mode = sc_ht_entry[pc->speed].r_reorder_mode;
    else
       pc->r_reorder_mode = pc->r_reorder_mode & 0xffff;

    if((pc->cl36_en & TEMOD_OVERRIDE_CFG) == 0) 
      pc->cl36_en = sc_ht_entry[pc->speed].cl36_en;
    else
      pc->cl36_en = pc->cl36_en & 0xffff;


    if((pc->r_descr1_mode & TEMOD_OVERRIDE_CFG) == 0) 
      pc->r_descr1_mode = sc_ht_entry[pc->speed].r_descr1_mode;
    else
      pc->r_descr1_mode = pc->r_descr1_mode & 0xffff;

    if((pc->r_dec1_mode & TEMOD_OVERRIDE_CFG) == 0)
      pc->r_dec1_mode = sc_ht_entry[pc->speed].r_dec1_mode;
    else 
      pc->r_dec1_mode = pc->r_dec1_mode & 0xffff;

    if((pc->r_deskew_mode & TEMOD_OVERRIDE_CFG) == 0)
      pc->r_deskew_mode = sc_ht_entry[pc->speed].r_deskew_mode;
    else
      pc->r_deskew_mode = pc->r_deskew_mode & 0xffff;

    if((pc->r_desc2_mode & TEMOD_OVERRIDE_CFG) == 0)
      pc->r_desc2_mode = sc_ht_entry[pc->speed].r_desc2_mode;
    else
      pc->r_desc2_mode = pc->r_desc2_mode & 0xffff;

    if((pc->r_desc2_byte_deletion & TEMOD_OVERRIDE_CFG) == 0)
      pc->r_desc2_byte_deletion = sc_ht_entry[pc->speed].r_desc2_byte_deletion;
    else
      pc->r_desc2_byte_deletion = pc->r_desc2_byte_deletion & 0xffff;

    if((pc->r_dec1_brcm64b66_descr & TEMOD_OVERRIDE_CFG) == 0)
      pc->r_dec1_brcm64b66_descr = sc_ht_entry[pc->speed].r_dec1_brcm64b66_descr;
    else
      pc->r_dec1_brcm64b66_descr = pc->r_dec1_brcm64b66_descr & 0xffff;

    if((pc->pll_mode & TEMOD_OVERRIDE_CFG) == 0)
      pc->pll_mode = sc_ht_entry[pc->speed].pll_mode;
    else
      pc->pll_mode = pc->pll_mode & 0xffff;

    if((pc->sgmii & TEMOD_OVERRIDE_CFG) == 0)
      pc->sgmii = sc_ht_entry[pc->speed].sgmii;
    else
      pc->sgmii = pc->sgmii & 0xffff;

    if((pc->clkcnt0 & TEMOD_OVERRIDE_CFG) == 0)
      pc->clkcnt0 = sc_ht_entry[pc->speed].clkcnt0;
    else
      pc->clkcnt0 =  pc->clkcnt0 & 0xffff;

    if((pc->clkcnt1 & TEMOD_OVERRIDE_CFG) == 0)
      pc->clkcnt1 = sc_ht_entry[pc->speed].clkcnt1;
    else
      pc->clkcnt1 = pc->clkcnt1 & 0xffff;
    if((pc->lpcnt0 & TEMOD_OVERRIDE_CFG) == 0)
      pc->lpcnt0 = sc_ht_entry[pc->speed].lpcnt0;
    else
      pc->lpcnt0 = pc->lpcnt0  & 0xffff;

    if((pc->lpcnt1 & TEMOD_OVERRIDE_CFG) == 0)
      pc->lpcnt1 = sc_ht_entry[pc->speed].lpcnt1;
    else
      pc->lpcnt1 = pc->lpcnt1 & 0xffff;

    if((pc->mac_cgc & TEMOD_OVERRIDE_CFG) == 0)
      pc->mac_cgc = sc_ht_entry[pc->speed].mac_cgc;
    else
      pc->mac_cgc = pc->mac_cgc & 0xffff;

    if((pc->pcs_repcnt & TEMOD_OVERRIDE_CFG) == 0)
      pc->pcs_repcnt = sc_ht_entry[pc->speed].pcs_repcnt;
    else
      pc->pcs_repcnt = pc->pcs_repcnt & 0xffff;

    if((pc->pcs_crdten & TEMOD_OVERRIDE_CFG) == 0)
      pc->pcs_crdten = sc_ht_entry[pc->speed].pcs_crdten;
    else
      pc->pcs_crdten = pc->pcs_crdten & 0xffff;

    if((pc->pcs_clkcnt & TEMOD_OVERRIDE_CFG) == 0)
      pc->pcs_clkcnt = sc_ht_entry[pc->speed].pcs_clkcnt;
    else
      pc->pcs_clkcnt = pc->pcs_clkcnt & 0xffff;

    if((pc->pcs_cgc & TEMOD_OVERRIDE_CFG) == 0)
      pc->pcs_cgc = sc_ht_entry[pc->speed].pcs_cgc;
    else
      pc->pcs_cgc = pc->pcs_cgc & 0xffff;

      
    if((pc->cl72_en & TEMOD_OVERRIDE_CFG) == 0)  {
      pc->cl72_en = sc_ht_entry[pc->speed].cl72_en | pc->cl72_en;
     } else {
      pc->cl72_en = pc->cl72_en | TEMOD_OVERRIDE_CFG;
     }
     return PHYMOD_E_NONE;
}

#endif

#ifdef _DV_TB_
int print_temod_sc_lkup_table(PHYMOD_ST* pc)
{
      printf("The Table Entry values for speed id %d is :\n", pc->speed);
      printf("pll_mode              : %d\n",pc->pll_mode);
      printf("sgmii                 : %d\n",pc->sgmii);
      printf("clkcnt0               : %d\n",pc->clkcnt0);
      printf("clkcnt1               : %d\n",pc->clkcnt1);
      printf("lpcnt0                : %d\n",pc->lpcnt0);
      printf("lpcnt1                : %d\n",pc->lpcnt1);
      printf("mac_cgc               : %d\n",pc->mac_cgc);
      printf("pcs_repcnt            : %d\n",pc->pcs_repcnt);
      printf("pcs_crdten            : %d\n",pc->pcs_crdten);
      printf("pcs_clkcnt            : %d\n",pc->pcs_clkcnt);
      printf("pcs_cgc               : %d\n",pc->pcs_cgc);
      printf("r_descr1_mode         : %d\n",pc->r_descr1_mode);
      printf("r_dec1_mode           : %d\n",pc->r_dec1_mode);
      printf("r_deskew_mode         : %d\n",pc->r_deskew_mode);
      printf("r_desc2_mode          : %d\n",pc->r_desc2_mode);
      printf("r_desc2_byte_deletion : %d\n",pc->r_desc2_byte_deletion);
      printf("r_dec1_brcm64b66_descr: %d\n",pc->r_dec1_brcm64b66_descr);
      printf("num_lanes             : %d\n",pc->num_lanes);
      printf("fec_en                : %d\n",pc->fec_en);
      printf("cl72_en               : %d\n",pc->cl72_en & 0xffff);
      printf("t_pma_os_mode         : %d\n",pc->t_pma_os_mode);
      printf("t_scr_mode            : %d\n",pc->t_scr_mode);
      /*printf("t_fec_enable        : %d\n",pc->t_fec_enable); */
      printf("t_encode_mode         : %d\n",pc->t_encode_mode);
      printf("cl48_check_end        : %d\n",pc->cl48_check_end);
      printf("blk_sync_mode         : %d\n",pc->blk_sync_mode);
      printf("r_reorder_mode        : %d\n",pc->r_reorder_mode);
      printf("cl36_en               : %d\n",pc->cl36_en);
      return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/*!
@brief Read the 16 bit rev. id value etc.
@param  pc handle to current TE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful
@details Update the te_st with the values, corresponding to the speed
*/
int post_process_ht_table(PHYMOD_ST* pc)
{
 /*
   will add Later, depending on the fec_en, set the block_sync_mode .....
 */

  if(pc->fec_en) {
    pc->blk_sync_mode = 4;
   
    if(pc->speed == 26) {
      pc->r_descr1_mode = 0;
      pc->r_dec1_brcm64b66_descr = 1;
      pc->t_scr_mode = 3;
      pc->cl72_en = 1;
    }
    if(pc->speed == 27) {
      pc->r_dec1_brcm64b66_descr = 1;
      pc->t_scr_mode = 3;
    }
  }
 return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/*!
@brief Read the 16 bit rev. id value etc.
@param  pc handle to current TE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful
@details Update the te_st with the values, corresponding to the speed
*/
int temod_sc_lkup_table(PHYMOD_ST* pc)
{
  int speed;

  get_mapped_speed(pc->spd_intf, &speed);
  pc->speed = speed;
  temod_get_ht_entries(pc);
  post_process_ht_table(pc);
  print_temod_sc_lkup_table(pc);
  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
/*!
@brief Read the 16 bit rev. id value etc.
@param  pc handle to current TE context (#PHYMOD_ST)
@returns The value PHYMOD_E_NONE upon successful
@details Update the te_st with the values, corresponding to the speed
*/
int _get_sc_speed_configuration(PHYMOD_ST* pc)
{
  int speed;
  if(pc->sc_mode == TEMOD_SC_MODE_ST){
    get_mapped_speed(pc->spd_intf, &speed);
    pc->speed = speed;
    temod_get_ht_entries(pc);
    print_temod_sc_lkup_table(pc);
  } else if(pc->sc_mode == TEMOD_SC_MODE_ST_OVERRIDE){
    /*Do nothing as the structure has all values to override.  */
  } else { /* TEMOD_SC_MODE_HT_WITH_BASIC_OVERRIDE  */
    temod_sc_lkup_table(pc);
  }
  return PHYMOD_E_NONE;
}
#endif

/*!
@brief Read the 16 bit rev. id value etc.
@param  speed_id Speed Number of the PCS speed lookup table
@param  speed Recieve the TEMod speed for PCS speed num.
@returns The value PHYMOD_E_NONE upon successful
@details Return the TEMOD speed given the PCS speed look up table number. Note
that an integer type is returned. Calling function will cast it to speed enum
type #temod_spd_intfc_type
*/

int get_actual_speed(int speed_id, int *speed)
{
  switch(speed_id) {
  case digital_operationSpeeds_SPEED_10M_10p3125        : *speed = TEMOD_SPD_10_X1_SGMII; break;
  case digital_operationSpeeds_SPEED_100M_10p3125       : *speed = TEMOD_SPD_100_X1_SGMII; break;
  case digital_operationSpeeds_SPEED_1000M_10p3125      : *speed = TEMOD_SPD_1000_X1_SGMII; break;
  case digital_operationSpeeds_SPEED_2p5G_X1_10p3125    : *speed = TEMOD_SPD_2500_X1; break;
  case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD_SPD_10_SGMII; break;
  case digital_operationSpeeds_SPEED_100M               : *speed = TEMOD_SPD_100_SGMII; break;
  case digital_operationSpeeds_SPEED_1000M              : *speed = TEMOD_SPD_1000_SGMII; break;
  case digital_operationSpeeds_SPEED_2p5G_X1            : *speed = TEMOD_SPD_2500; break;
  case digital_operationSpeeds_SPEED_5G_X1              : *speed = TEMOD_SPD_5000; break;
  case digital_operationSpeeds_SPEED_1G_KX1             : *speed = TEMOD_SPD_1000_XFI; break;
  case digital_operationSpeeds_SPEED_5G_KR1             : *speed = TEMOD_SPD_5000_XFI; break;
  case digital_operationSpeeds_SPEED_10G_KR1            : *speed = TEMOD_SPD_10000_XFI; break;
  case digital_operationSpeeds_SPEED_10p6_X1            : *speed = TEMOD_SPD_10600_XFI_HG; break;
  /* case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD_SPD_10000_HI; break; */
  case digital_operationSpeeds_SPEED_10G_KX4            : *speed = TEMOD_SPD_10000; break;
  /* case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD_SPD_12000_HI; break; */
  case digital_operationSpeeds_SPEED_13G_X4             : *speed = TEMOD_SPD_13000; break;
  case digital_operationSpeeds_SPEED_15G_X4             : *speed = TEMOD_SPD_15000; break;
  case digital_operationSpeeds_SPEED_16G_X4             : *speed = TEMOD_SPD_16000; break;
  case digital_operationSpeeds_SPEED_20G_X4             : *speed = TEMOD_SPD_20000_SCR; break;  /* or SPEED_20G_CX2 */
  case digital_operationSpeeds_SPEED_20G_CX4             : *speed = TEMOD_SPD_20000; break;
  case digital_operationSpeeds_SPEED_21G_X4             : *speed = TEMOD_SPD_21000; break;
  case digital_operationSpeeds_SPEED_25p45G_X4          : *speed = TEMOD_SPD_25455; break;
  case digital_operationSpeeds_SPEED_31p5G_X4           : *speed = TEMOD_SPD_31500; break;
  case digital_operationSpeeds_SPEED_40G_X4             : *speed = TEMOD_SPD_40G_X4; break;
  /* case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD_SPD_42G_X4; break; */
  case digital_operationSpeeds_SPEED_40G_KR4            : *speed = TEMOD_SPD_40G_XLAUI; break; /* or RTL_SPEED_40G_CR4 */
  case digital_operationSpeeds_SPEED_42G_X4             : *speed = TEMOD_SPD_42G_XLAUI; break;
  /* case digital_operationSpeeds_SPEED_10G_X2             : *speed = TEMOD_SPD_10000_X2; break; */ /* or RTL_SPEED_10G_CX2 */
  /* case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD_SPD_10000_HI_DXGXS; break; */
  case digital_operationSpeeds_SPEED_10G_X2             : *speed = TEMOD_SPD_10000_DXGXS; break;
  /* case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD_SPD_10000_HI_DXGXS_SCR; break; */
  /* case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD_SPD_10000_DXGXS_SCR; break; */
  case digital_operationSpeeds_SPEED_10p5G_X2           : *speed = TEMOD_SPD_10500_HI_DXGXS; break; 
  case digital_operationSpeeds_SPEED_10p5G_X4           : *speed = TEMOD_SPD_10500_HI; break; 
  case digital_operationSpeeds_SPEED_20G_KR2            : *speed = TEMOD_SPD_20G_MLD_DXGXS; break; /* or RTL_SPEED_20G_CR2 */
  case digital_operationSpeeds_SPEED_100G_CR10          : *speed = TEMOD_SPD_100G_CR10; break; 
  case digital_operationSpeeds_SPEED_107G_X10           : *speed = TEMOD_SPD_107G_HG_CR10; break;  
  case digital_operationSpeeds_SPEED_120G_X12           : *speed = TEMOD_SPD_120G_CR12; break; 
  case digital_operationSpeeds_SPEED_127G_X12           : *speed = TEMOD_SPD_127G_HG_CR12; break; 
  case digital_operationSpeeds_SPEED_12p7G_X2           : *speed = TEMOD_SPD_12773_DXGXS; break; 
  case digital_operationSpeeds_SPEED_15p75G_X2          : *speed = TEMOD_SPD_15750_HI_DXGXS; break; 
  /* case digital_operationSpeeds_SPEED_20G_X2             : *speed = TEMOD_SPD_20G_DXGXS; break;  */
  case digital_operationSpeeds_SPEED_21G_X2             : *speed = TEMOD_SPD_21G_HI_MLD_DXGXS; break; 
  case digital_operationSpeeds_SPEED_31p5G_KR4          : *speed = TEMOD_SPD_31500_MLD; break; 
  default                          : break; 
  }
  return PHYMOD_E_NONE;
}
