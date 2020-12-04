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
 #define _SDK_TEMOD2PLL_ 1
#endif

#ifdef _DV_TB_
#include <stdio.h>
#endif
#ifdef _SDK_TEMOD2PLL_
#include <phymod/phymod.h>
#include "temod_dpll_enum_defines.h"
#include "temod_dpll.h"
#endif
#include "temod_dpll_sc_field_defines.h"
#include "tedpll_PCSRegEnums.h"

#ifdef _DV_TB_
#include "temod_dpll_sc_lkup_table.h"
#endif

#ifndef PHYMOD_ST
#ifdef _SDK_TEMOD2PLL_
   #define PHYMOD_ST const phymod_access_t
#else
  #define PHYMOD_ST  temod_st
#endif
#endif

/*!
\brief Read the 16 bit rev. id value etc.

\param  pc handle to current TE context (#PHYMOD_ST)

\returns The value #PHYMOD_E_NONE upon successful

\details Update the te_st with the values, corresponding to the speed

<B>How to call:</B><br>
<p>Call this function directly or through the function
#temod2pll_get_mapped_speed() with the te_st.</p>
*/

int temod2pll_get_mapped_speed(temod2pll_spd_intfc_type spd_intf, int *speed)
{

  switch(spd_intf) {
  case TEMOD2PLL_SPD_10_X1_SGMII       : *speed = digital_operationSpeeds_SPEED_10M_10p3125; break;
  case TEMOD2PLL_SPD_100_X1_SGMII      : *speed = digital_operationSpeeds_SPEED_100M_10p3125; break;
  case TEMOD2PLL_SPD_1000_X1_SGMII     : *speed = digital_operationSpeeds_SPEED_1000M_10p3125; break;
  case TEMOD2PLL_SPD_2500_X1           : *speed = digital_operationSpeeds_SPEED_2p5G_X1_10p3125; break;
  case TEMOD2PLL_SPD_10_SGMII          : *speed = digital_operationSpeeds_SPEED_10M; break;
  case TEMOD2PLL_SPD_100_SGMII         : *speed = digital_operationSpeeds_SPEED_100M; break;
  case TEMOD2PLL_SPD_1000_SGMII        : *speed = digital_operationSpeeds_SPEED_1000M; break;
  case TEMOD2PLL_SPD_2500              : *speed = digital_operationSpeeds_SPEED_2p5G_X1; break;
  case TEMOD2PLL_SPD_5000              : *speed = digital_operationSpeeds_SPEED_5G_X1; break;
  case TEMOD2PLL_SPD_1000_XFI          : *speed = digital_operationSpeeds_SPEED_1G_KX1; break;
  case TEMOD2PLL_SPD_5000_XFI          : *speed = digital_operationSpeeds_SPEED_5G_KR1; break;
  case TEMOD2PLL_SPD_10000_XFI         : *speed = digital_operationSpeeds_SPEED_10G_KR1; break;
  case TEMOD2PLL_SPD_10600_XFI_HG      : *speed = digital_operationSpeeds_SPEED_10p6_X1; break;
  case TEMOD2PLL_SPD_10000_HI          : *speed = digital_operationSpeeds_SPEED_10M; break;
  case TEMOD2PLL_SPD_10000             : *speed = digital_operationSpeeds_SPEED_10G_KX4; break;
  case TEMOD2PLL_SPD_12000_HI          : *speed = digital_operationSpeeds_SPEED_10M; break;
  case TEMOD2PLL_SPD_13000             : *speed = digital_operationSpeeds_SPEED_13G_X4; break;
  case TEMOD2PLL_SPD_15000             : *speed = digital_operationSpeeds_SPEED_15G_X4; break;
  case TEMOD2PLL_SPD_16000             : *speed = digital_operationSpeeds_SPEED_16G_X4; break;
  case TEMOD2PLL_SPD_20000_SCR         : *speed = digital_operationSpeeds_SPEED_20G_X2; break;  /* or SPEED_20G_CX2 */
  case TEMOD2PLL_SPD_20000             : *speed = digital_operationSpeeds_SPEED_20G_CX4; break;
  case TEMOD2PLL_SPD_21000             : *speed = digital_operationSpeeds_SPEED_21G_X4; break;
  case TEMOD2PLL_SPD_25455             : *speed = digital_operationSpeeds_SPEED_25p45G_X4; break;
  case TEMOD2PLL_SPD_31500             : *speed = digital_operationSpeeds_SPEED_31p5G_X4; break;
  case TEMOD2PLL_SPD_40G_X4            : *speed = digital_operationSpeeds_SPEED_40G_X4; break;
  case TEMOD2PLL_SPD_42G_X4            : *speed = digital_operationSpeeds_SPEED_42G_X4; break;  /* 42G_X4 is MLD, 40G_X4 is BRCM */
  case TEMOD2PLL_SPD_40G_XLAUI         : *speed = digital_operationSpeeds_SPEED_40G_KR4; break; /* or RTL_SPEED_40G_CR4 */
  case TEMOD2PLL_SPD_42G_XLAUI         : *speed = digital_operationSpeeds_SPEED_42G_X4; break;
  case TEMOD2PLL_SPD_10000_X2          : *speed = digital_operationSpeeds_SPEED_10G_X2; break; /* or RTL_SPEED_10G_CX2 */
  case TEMOD2PLL_SPD_10000_HI_DXGXS    : *speed = digital_operationSpeeds_SPEED_10M; break;
  case TEMOD2PLL_SPD_10000_DXGXS       : *speed = digital_operationSpeeds_SPEED_10G_CX2; break;
  case TEMOD2PLL_SPD_10000_HI_DXGXS_SCR: *speed = digital_operationSpeeds_SPEED_10M; break;
  case TEMOD2PLL_SPD_10000_DXGXS_SCR   : *speed = digital_operationSpeeds_SPEED_10G_X2; break;
  case TEMOD2PLL_SPD_10500_HI_DXGXS    : *speed = digital_operationSpeeds_SPEED_10p5G_X2; break;
  case TEMOD2PLL_SPD_10500_HI          : *speed = digital_operationSpeeds_SPEED_10p5G_X4; break;
  case TEMOD2PLL_SPD_21P21G_HG_DXGXS  : *speed = digital_operationSpeeds_SPEED_21p21G_X2     ; break;
  case TEMOD2PLL_SPD_20G_MLD_DXGXS     : *speed = digital_operationSpeeds_SPEED_20G_KR2; break; /* or RTL_SPEED_20G_CR2 */
  case TEMOD2PLL_SPD_100G_CR10         : *speed = digital_operationSpeeds_SPEED_100G_CR10; break;
  case TEMOD2PLL_SPD_107G_HG_CR10      : *speed = digital_operationSpeeds_SPEED_107G_X10; break;
  case TEMOD2PLL_SPD_120G_CR12         : *speed = digital_operationSpeeds_SPEED_120G_X12; break;
  case TEMOD2PLL_SPD_127G_HG_CR12      : *speed = digital_operationSpeeds_SPEED_127G_X12; break;
  case TEMOD2PLL_SPD_12773_DXGXS       : *speed = digital_operationSpeeds_SPEED_12p7G_X2; break;
  case TEMOD2PLL_SPD_15750_HI_DXGXS    : *speed = digital_operationSpeeds_SPEED_15p75G_X2; break;
  case TEMOD2PLL_SPD_20G_DXGXS         : *speed = digital_operationSpeeds_SPEED_20G_X2; break;
  case TEMOD2PLL_SPD_21G_HI_MLD_DXGXS  : *speed = digital_operationSpeeds_SPEED_21G_X2; break;
  case TEMOD2PLL_SPD_31500_MLD         : *speed = digital_operationSpeeds_SPEED_31p5G_KR4; break;
  case TEMOD2PLL_SPD_10000_X4_10P3     : *speed = digital_operationSpeeds_SPEED_10G_X4_10p3125    ; break;
  case TEMOD2PLL_SPD_10000_CX4_10P3    : *speed = digital_operationSpeeds_SPEED_10G_CX4_10p3125   ; break;
  case TEMOD2PLL_SPD_10000_KX4_10P3    : *speed = digital_operationSpeeds_SPEED_10G_KX4_10p3125   ; break;
  case TEMOD2PLL_SPD_12P12_XFI         : *speed = digital_operationSpeeds_SPEED_12p12G_X1     ; break;
  case TEMOD2PLL_SPD_24P24_MLD_DXGXS   : *speed = digital_operationSpeeds_SPEED_24p24G_X2     ; break;
  case TEMOD2PLL_SPD_42P42G_HG2         : *speed = digital_operationSpeeds_SPEED_42p42G_X4     ; break;
  case TEMOD2PLL_SPD_48P48_MLD         : *speed = digital_operationSpeeds_SPEED_48p48G_X4     ; break;
  default                          : break;
  }
  return PHYMOD_E_NONE;

}

#ifdef _DV_TB_

/*!
\brief Read the 16 bit rev. id value etc.

\param  pc handle to current TE context (#PHYMOD_ST)

\returns The value #PHYMOD_E_NONE upon successful

\details Update the te_st with the values, corresponding to the speed

<B>How to call:</B><br>
<p>Call this function directly or through the function
#get_ht_entries() with the te_st.</p>
*/
int temod2pll_get_ht_entries(PHYMOD_ST* pc)
{
      /*pc->speed =  temod2pll_sc_ht_entry[pc->spd_intf].speed; */

    if((pc->num_lanes & TEMOD2PLL_OVERRIDE_CFG) == 0) {
      pc->num_lanes =  temod2pll_sc_ht_entry[pc->speed].num_lanes;
      }
    else {
      pc->num_lanes =  pc->num_lanes & 0xffff;
      }

    if((pc->t_pma_os_mode & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->t_pma_os_mode = temod2pll_sc_ht_entry[pc->speed].t_pma_os_mode;
    else
      pc->t_pma_os_mode = pc->t_pma_os_mode & 0xffff;

    if ((pc->t_scr_mode & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->t_scr_mode = temod2pll_sc_ht_entry[pc->speed].t_scr_mode;
    else
      pc->t_scr_mode = pc->t_scr_mode & 0xffff;

    if((pc->t_encode_mode & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->t_encode_mode = temod2pll_sc_ht_entry[pc->speed].t_encode_mode;
    else
      pc->t_encode_mode = pc->t_encode_mode & 0xffff;

    if((pc->cl48_check_end & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->cl48_check_end = temod2pll_sc_ht_entry[pc->speed].cl48_check_end;
    else
      pc->cl48_check_end = pc->cl48_check_end & 0xffff;

    if((pc->blk_sync_mode & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->blk_sync_mode = temod2pll_sc_ht_entry[pc->speed].blk_sync_mode;
    else
      pc->blk_sync_mode = pc->blk_sync_mode & 0xffff;

    if((pc->r_reorder_mode & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->r_reorder_mode = temod2pll_sc_ht_entry[pc->speed].r_reorder_mode;
    else
       pc->r_reorder_mode = pc->r_reorder_mode & 0xffff;

    if((pc->cl36_en & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->cl36_en = temod2pll_sc_ht_entry[pc->speed].cl36_en;
    else
      pc->cl36_en = pc->cl36_en & 0xffff;


    if((pc->r_descr1_mode & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->r_descr1_mode = temod2pll_sc_ht_entry[pc->speed].r_descr1_mode;
    else
      pc->r_descr1_mode = pc->r_descr1_mode & 0xffff;

    if((pc->r_dec1_mode & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->r_dec1_mode = temod2pll_sc_ht_entry[pc->speed].r_dec1_mode;
    else
      pc->r_dec1_mode = pc->r_dec1_mode & 0xffff;

    if((pc->r_deskew_mode & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->r_deskew_mode = temod2pll_sc_ht_entry[pc->speed].r_deskew_mode;
    else
      pc->r_deskew_mode = pc->r_deskew_mode & 0xffff;

    if((pc->r_desc2_mode & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->r_desc2_mode = temod2pll_sc_ht_entry[pc->speed].r_desc2_mode;
    else
      pc->r_desc2_mode = pc->r_desc2_mode & 0xffff;

    if((pc->r_desc2_byte_deletion & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->r_desc2_byte_deletion = temod2pll_sc_ht_entry[pc->speed].r_desc2_byte_deletion;
    else
      pc->r_desc2_byte_deletion = pc->r_desc2_byte_deletion & 0xffff;

    if((pc->r_dec1_brcm64b66_descr & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->r_dec1_brcm64b66_descr = temod2pll_sc_ht_entry[pc->speed].r_dec1_brcm64b66_descr;
    else
      pc->r_dec1_brcm64b66_descr = pc->r_dec1_brcm64b66_descr & 0xffff;

    if((pc->pll_mode & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->pll_mode = temod2pll_sc_ht_entry[pc->speed].pll_mode;
    else
      pc->pll_mode = pc->pll_mode & 0xffff;

    if((pc->sgmii & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->sgmii = temod2pll_sc_ht_entry[pc->speed].sgmii;
    else
      pc->sgmii = pc->sgmii & 0xffff;

    if((pc->clkcnt0 & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->clkcnt0 = temod2pll_sc_ht_entry[pc->speed].clkcnt0;
    else
      pc->clkcnt0 =  pc->clkcnt0 & 0xffff;

    if((pc->clkcnt1 & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->clkcnt1 = temod2pll_sc_ht_entry[pc->speed].clkcnt1;
    else
      pc->clkcnt1 = pc->clkcnt1 & 0xffff;
    if((pc->lpcnt0 & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->lpcnt0 = temod2pll_sc_ht_entry[pc->speed].lpcnt0;
    else
      pc->lpcnt0 = pc->lpcnt0  & 0xffff;

    if((pc->lpcnt1 & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->lpcnt1 = temod2pll_sc_ht_entry[pc->speed].lpcnt1;
    else
      pc->lpcnt1 = pc->lpcnt1 & 0xffff;

    if((pc->mac_cgc & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->mac_cgc = temod2pll_sc_ht_entry[pc->speed].mac_cgc;
    else
      pc->mac_cgc = pc->mac_cgc & 0xffff;

    if((pc->pcs_repcnt & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->pcs_repcnt = temod2pll_sc_ht_entry[pc->speed].pcs_repcnt;
    else
      pc->pcs_repcnt = pc->pcs_repcnt & 0xffff;

    if((pc->pcs_crdten & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->pcs_crdten = temod2pll_sc_ht_entry[pc->speed].pcs_crdten;
    else
      pc->pcs_crdten = pc->pcs_crdten & 0xffff;

    if((pc->pcs_clkcnt & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->pcs_clkcnt = temod2pll_sc_ht_entry[pc->speed].pcs_clkcnt;
    else
      pc->pcs_clkcnt = pc->pcs_clkcnt & 0xffff;

    if((pc->pcs_cgc & TEMOD2PLL_OVERRIDE_CFG) == 0)
      pc->pcs_cgc = temod2pll_sc_ht_entry[pc->speed].pcs_cgc;
    else
      pc->pcs_cgc = pc->pcs_cgc & 0xffff;

      /* this is just check the cl72 override enable, once all the cl72
       * tabe is implemented, we can remove this next line, will have to remove
       * the check in the compare_stats as well */
    if((pc->cl72_en & TEMOD2PLL_OVERRIDE_CFG) == 0)  {
      pc->cl72_en = temod2pll_sc_ht_entry[pc->speed].cl72_en | pc->cl72_en;
     } else {
      pc->cl72_en = pc->cl72_en | TEMOD2PLL_OVERRIDE_CFG;
     }
/*
    if((pc->fec_en & 0x80000000) == 0)
      pc->fec_en = 0;
    else
      pc->fec_en = pc->fec_en & 0xffff;
 */

      return PHYMOD_E_NONE;
}

#endif


#ifdef _DV_TB_

int print_temod2pll_sc_lkup_table(PHYMOD_ST* pc)
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

/*!
\brief Read the 16 bit rev. id value etc.

\param  pc handle to current TE context (#PHYMOD_ST)

\returns The value #PHYMOD_E_NONE upon successful

\details Update the te_st with the values, corresponding to the speed

<B>How to call:</B><br>
<p>Call this function directly or through the function
#temod2pll_post_process_ht_table() with the te_st.</p>
*/
#ifdef _DV_TB_

int temod2pll_post_process_ht_table(PHYMOD_ST* pc)
{
 /*
   will add Later, depending on the fec_en, set the block_sync_mode .....
 */

  printf("%-22s STATS_INFO : fec_en=%0d speed=%0d\n", __func__, pc->fec_en, pc->speed);

  if(pc->fec_en) {
    pc->blk_sync_mode = 4;

    if( (pc->speed == 26) || (pc->speed == 27) || (pc->speed == 43) || (pc->speed == 44) ) {
      pc->r_descr1_mode = 0;
      pc->r_dec1_brcm64b66_descr = 1;
      pc->t_scr_mode = 3;
      pc->cl72_en = 1;
    }
  }

 return PHYMOD_E_NONE;
}

#endif


/*!
\brief Read the 16 bit rev. id value etc.

\param  pc handle to current TE context (#PHYMOD_ST)

\returns The value #PHYMOD_E_NONE upon successful

\details Update the te_st with the values, corresponding to the speed

<B>How to call:</B><br>
<p>Call this function directly or through the function
#get_misc_controls() with the te_st.</p>
*/
#ifdef _DV_TB_

int temod2pll_sc_lkup_table(PHYMOD_ST* pc)
{
  int speed;

  temod2pll_get_mapped_speed(pc->spd_intf, &speed);
  pc->speed = speed;
  get_ht_entries(pc);
  temod2pll_post_process_ht_table(pc);
  print_temod2pll_sc_lkup_table(pc);
  return PHYMOD_E_NONE;
}

#endif

#ifdef _DV_TB_

/*!
\brief Read the 16 bit rev. id value etc.

\param  pc handle to current TE context (#PHYMOD_ST)

\returns The value #PHYMOD_E_NONE upon successful

\details Update the te_st with the values, corresponding to the speed

<B>How to call:</B><br>
<p>Call this function directly or through the function
#get_sc_speed_configuration() with the te_st.</p>
*/
int _temod2pll_get_sc_speed_configuration(PHYMOD_ST* pc)
{
	int speed;
 /*
  if the sc_mode == st_override
     use the soft table, all entries will come from DV-system verilog
  else   Default Basic override
     use ht and override the fec_en and cl72
 */
 if(pc->sc_mode == TEMOD2PLL_SC_MODE_ST){

     temod2pll_get_mapped_speed(pc->spd_intf, &speed);

     /*  printf("%-32s  get_mapped_speed (%x)=%s\n", __func__, pc->spd_intf,speed);*/

	  pc->speed = speed;
     get_ht_entries(pc);
     print_temod2pll_sc_lkup_table(pc);
 }
 else if(pc->sc_mode == TEMOD2PLL_SC_MODE_ST_OVERRIDE){
      /*Do nothing as the structure has all values to override.  */
 }
 else { /* TEMOD2PLL_SC_MODE_HT_WITH_BASIC_OVERRIDE  */
     temod2pll_sc_lkup_table(pc);
 }

 return PHYMOD_E_NONE;
}

#endif

/*!
\brief Read the 16 bit rev. id value etc.

\param  pc handle to current TE context (#PHYMOD_ST)

\returns The value #PHYMOD_E_NONE upon successful

\details Update the te_st with the values, corresponding to the speed

<B>How to call:</B><br>
<p>Call this function directly or through the function
#get_mapped_speed() with the te_st.</p>
*/

int temod2pll_get_actual_speed(int speed_id, int *speed)
{

  switch(speed_id) {
  case digital_operationSpeeds_SPEED_10M_10p3125        : *speed = TEMOD2PLL_SPD_10_X1_SGMII; break;
  case digital_operationSpeeds_SPEED_100M_10p3125       : *speed = TEMOD2PLL_SPD_100_X1_SGMII; break;
  case digital_operationSpeeds_SPEED_1000M_10p3125      : *speed = TEMOD2PLL_SPD_1000_X1_SGMII; break;
  case digital_operationSpeeds_SPEED_2p5G_X1_10p3125    : *speed = TEMOD2PLL_SPD_2500_X1; break;
  case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD2PLL_SPD_10_SGMII; break;
  case digital_operationSpeeds_SPEED_100M               : *speed = TEMOD2PLL_SPD_100_SGMII; break;
  case digital_operationSpeeds_SPEED_1000M              : *speed = TEMOD2PLL_SPD_1000_SGMII; break;
  case digital_operationSpeeds_SPEED_2p5G_X1            : *speed = TEMOD2PLL_SPD_2500; break;
  case digital_operationSpeeds_SPEED_5G_X1              : *speed = TEMOD2PLL_SPD_5000; break;
  case digital_operationSpeeds_SPEED_1G_KX1             : *speed = TEMOD2PLL_SPD_1000_XFI; break;
  case digital_operationSpeeds_SPEED_5G_KR1             : *speed = TEMOD2PLL_SPD_5000_XFI; break;
  case digital_operationSpeeds_SPEED_10G_KR1            : *speed = TEMOD2PLL_SPD_10000_XFI; break;
  case digital_operationSpeeds_SPEED_10p6_X1            : *speed = TEMOD2PLL_SPD_10600_XFI_HG; break;
  /* case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD2PLL_SPD_10000_HI; break; */
  case digital_operationSpeeds_SPEED_10G_KX4            : *speed = TEMOD2PLL_SPD_10000; break;
  /* case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD2PLL_SPD_12000_HI; break; */
  case digital_operationSpeeds_SPEED_13G_X4             : *speed = TEMOD2PLL_SPD_13000; break;
  case digital_operationSpeeds_SPEED_15G_X4             : *speed = TEMOD2PLL_SPD_15000; break;
  case digital_operationSpeeds_SPEED_16G_X4             : *speed = TEMOD2PLL_SPD_16000; break;
  case digital_operationSpeeds_SPEED_20G_X4             : *speed = TEMOD2PLL_SPD_20000_SCR; break;  /* or SPEED_20G_CX2 */
  case digital_operationSpeeds_SPEED_20G_CX4             : *speed = TEMOD2PLL_SPD_20000; break;
  case digital_operationSpeeds_SPEED_21G_X4             : *speed = TEMOD2PLL_SPD_21000; break;
  case digital_operationSpeeds_SPEED_25p45G_X4          : *speed = TEMOD2PLL_SPD_25455; break;
  case digital_operationSpeeds_SPEED_31p5G_X4           : *speed = TEMOD2PLL_SPD_31500; break;
  case digital_operationSpeeds_SPEED_40G_X4             : *speed = TEMOD2PLL_SPD_40G_X4; break;
  /* case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD2PLL_SPD_42G_X4; break; */
  case digital_operationSpeeds_SPEED_40G_KR4            : *speed = TEMOD2PLL_SPD_40G_XLAUI; break; /* or RTL_SPEED_40G_CR4 */
  case digital_operationSpeeds_SPEED_42G_X4             : *speed = TEMOD2PLL_SPD_42G_XLAUI; break;
  /* case digital_operationSpeeds_SPEED_10G_X2             : *speed = TEMOD2PLL_SPD_10000_X2; break; */ /* or RTL_SPEED_10G_CX2 */
  /* case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD2PLL_SPD_10000_HI_DXGXS; break; */
  case digital_operationSpeeds_SPEED_10G_X2             : *speed = TEMOD2PLL_SPD_10000_DXGXS; break;
  /* case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD2PLL_SPD_10000_HI_DXGXS_SCR; break; */
  /* case digital_operationSpeeds_SPEED_10M                : *speed = TEMOD2PLL_SPD_10000_DXGXS_SCR; break; */
  case digital_operationSpeeds_SPEED_10p5G_X2           : *speed = TEMOD2PLL_SPD_10500_HI_DXGXS; break;
  case digital_operationSpeeds_SPEED_10p5G_X4           : *speed = TEMOD2PLL_SPD_10500_HI; break;
  case digital_operationSpeeds_SPEED_20G_KR2            : *speed = TEMOD2PLL_SPD_20G_MLD_DXGXS; break; /* or RTL_SPEED_20G_CR2 */
  case digital_operationSpeeds_SPEED_100G_CR10          : *speed = TEMOD2PLL_SPD_100G_CR10; break;
  case digital_operationSpeeds_SPEED_107G_X10           : *speed = TEMOD2PLL_SPD_107G_HG_CR10; break;
  case digital_operationSpeeds_SPEED_120G_X12           : *speed = TEMOD2PLL_SPD_120G_CR12; break;
  case digital_operationSpeeds_SPEED_127G_X12           : *speed = TEMOD2PLL_SPD_127G_HG_CR12; break;
  case digital_operationSpeeds_SPEED_12p7G_X2           : *speed = TEMOD2PLL_SPD_12773_DXGXS; break;
  case digital_operationSpeeds_SPEED_15p75G_X2          : *speed = TEMOD2PLL_SPD_15750_HI_DXGXS; break;
    /*case digital_operationSpeeds_SPEED_20G_X2             : *speed = TEMOD2PLL_SPD_20G_DXGXS; break;  */
  case digital_operationSpeeds_SPEED_21G_X2             : *speed = TEMOD2PLL_SPD_21G_HI_MLD_DXGXS; break;
  case digital_operationSpeeds_SPEED_21p21G_X2         :  *speed = TEMOD2PLL_SPD_21P21G_HG_DXGXS; break;
  case digital_operationSpeeds_SPEED_31p5G_KR4          : *speed = TEMOD2PLL_SPD_31500_MLD; break;
  case digital_operationSpeeds_SPEED_10G_X4_10p3125        : *speed = TEMOD2PLL_SPD_10000_X4_10P3; break;
  case digital_operationSpeeds_SPEED_10G_CX4_10p3125       : *speed = TEMOD2PLL_SPD_10000_CX4_10P3; break;
  case digital_operationSpeeds_SPEED_10G_KX4_10p3125       : *speed = TEMOD2PLL_SPD_10000_KX4_10P3; break;
  case digital_operationSpeeds_SPEED_12p12G_X1         : *speed = TEMOD2PLL_SPD_12P12_XFI; break;
  case digital_operationSpeeds_SPEED_24p24G_X2         : *speed = TEMOD2PLL_SPD_24P24_MLD_DXGXS; break;
  case digital_operationSpeeds_SPEED_42p42G_X4         : *speed = TEMOD2PLL_SPD_42P42G_HG2; break;
  case digital_operationSpeeds_SPEED_48p48G_X4         : *speed = TEMOD2PLL_SPD_48P48_MLD; break;
  default                          : break;
  }

  return PHYMOD_E_NONE;

}


int temod2pll_get_pll_ndiv(int pll_mode, int *pll_ndiv)
{
  switch(pll_mode) {
  case TEMOD2PLL_PLL_MODE_DIV_ZERO        : *pll_ndiv = 46; break;
  case TEMOD2PLL_PLL_MODE_DIV_40          : *pll_ndiv = 40; break;
  case TEMOD2PLL_PLL_MODE_DIV_42          : *pll_ndiv = 42; break;
  case TEMOD2PLL_PLL_MODE_DIV_48          : *pll_ndiv = 48; break;
  case TEMOD2PLL_PLL_MODE_DIV_52          : *pll_ndiv = 52; break;
  case TEMOD2PLL_PLL_MODE_DIV_54          : *pll_ndiv = 54; break;
  case TEMOD2PLL_PLL_MODE_DIV_60          : *pll_ndiv = 60; break;
  case TEMOD2PLL_PLL_MODE_DIV_64          : *pll_ndiv = 64; break;
  case TEMOD2PLL_PLL_MODE_DIV_66          : *pll_ndiv = 66; break;
  case TEMOD2PLL_PLL_MODE_DIV_70          : *pll_ndiv = 70; break;
  case TEMOD2PLL_PLL_MODE_DIV_80          : *pll_ndiv = 80; break;
  case TEMOD2PLL_PLL_MODE_DIV_92          : *pll_ndiv = 92; break;
  default                             : *pll_ndiv = 0;  break;
  }

  return PHYMOD_E_NONE;

}

/*
 int temod2pll_get_mapped_frac_ns(int vco_rate, uint16_t *frac_ns)
{
  switch(vco_rate) {
  case 0    : *frac_ns = 0xa3d7 ; break; TSC_VCO_6PT25
  case 1    : *frac_ns = 0x9c09 ; break; TSC_VCO_6PT5625
  case 2    : *frac_ns = 0x0000 ; break; TSC_VCO_8PT125    
  case 3    : *frac_ns = 0x0000 ; break; TSC_VCO_9PT375   
  case 4    : *frac_ns = 0x0000 ; break; TSC_VCO_10       
  case 5    : *frac_ns = 0x634c ; break; TSC_VCO_10PT3125
  case 6    : *frac_ns = 0x5d9f ; break; TSC_VCO_10PT9375
  case 7    : *frac_ns = 0x51eb ; break; TSC_VCO_12PT5
  default   : *frac_ns = 0x634c ; break; DEFALT
  }

  return PHYMOD_E_NONE;
}
*/
int temod2pll_get_mapped_frac_ns(int speed, uint16_t *frac_ns)
{
  switch(speed) {
    case digital_operationSpeeds_SPEED_10M               : { *frac_ns =  0xa3d7 ; break; } /* TSC_SPEED_10M 6.25G VCO */
    case digital_operationSpeeds_SPEED_100M              : { *frac_ns =  0xa3d7 ; break; } /* TSC_SPEED_100M 6.25G VCO */
    case digital_operationSpeeds_SPEED_1000M             : { *frac_ns =  0xa3d7 ; break; } /* TSC_SPEED_1G 6.25G VCO */
    case digital_operationSpeeds_SPEED_1G_KX1            : { *frac_ns =  0xa3d7 ; break; } /* TSC_SPEED_1G 6.25G VCO */
    case digital_operationSpeeds_SPEED_2p5G_X1           : { *frac_ns =  0xa3d7 ; break; } /* TSC_SPEED_2P5G 6.25G VCO */
    case digital_operationSpeeds_SPEED_5G_X1             : { *frac_ns =  0xa3d7 ; break; } /* TSC_SPEED_5G 6.25G VCO */
    case digital_operationSpeeds_SPEED_10M_10p3125       : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_10M 10.3125G VCO */
    case digital_operationSpeeds_SPEED_100M_10p3125      : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_100M 10.3125G VCO */
    case digital_operationSpeeds_SPEED_1000M_10p3125     : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_1G 10.3125G VCO */
    case digital_operationSpeeds_SPEED_5G_KR1            : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_5G_XFI 10.3125G VCO */
    case digital_operationSpeeds_SPEED_10G_KR1           : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_10G_XFI 10.3125G VCO */
    case digital_operationSpeeds_SPEED_10p6_X1           : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_10P606G_XFI 10.9375G VCO */
    case digital_operationSpeeds_SPEED_12p12G_X1         : { *frac_ns =  0x51eb ; break; } /* TSC_SPEED_12P12G_XFI 12.5G VCO */
    case digital_operationSpeeds_SPEED_20G_CX2           : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_20G_HG2_XAUI_DXGXS 10.3125G VCO */
    case digital_operationSpeeds_SPEED_20G_X2            : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_20G_HG2_XAUI_DXGXS 10.3125G VCO */
    case digital_operationSpeeds_SPEED_21p21G_X2         : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_21P21G_HG_XAUI_DXGXS 10.9375G VCO */
    case digital_operationSpeeds_SPEED_20G_KR2           : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_20G_MLD_DXGXS 10.3125G VCO */
    case digital_operationSpeeds_SPEED_20G_CR2           : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_20G_MLD_DXGXS 10.3125G VCO ? */
    case digital_operationSpeeds_SPEED_24p24G_X2         : { *frac_ns =  0x51eb ; break; } /* TSC_SPEED_24P24G_XFI 12.5G VCO */
    case digital_operationSpeeds_SPEED_10G_KX4           : { *frac_ns =  0xa3d7 ; break; } /* TSC_SPEED_10G_XAUI 6.25G VCO */
    case digital_operationSpeeds_SPEED_20G_CX4           : { *frac_ns =  0xa3d7 ; break; } /* TSC_SPEED_20G_XAUI 6.25G VCO ?*/
    case digital_operationSpeeds_SPEED_20G_X4            : { *frac_ns =  0xa3d7 ; break; } /* TSC_SPEED_20G_XAUI 6.25G VCO */
    case digital_operationSpeeds_SPEED_21G_X4            : { *frac_ns =  0x9c09 ; break; } /* TSC_SPEED_21G_XAUI 6.5625G VCO */
    case digital_operationSpeeds_SPEED_40G_X4            : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_40G_HG_XAUI 10.3125G VCO */
    case digital_operationSpeeds_SPEED_40G_KR4           : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_40G_MLD 10.3125G VCO */
    case digital_operationSpeeds_SPEED_40G_CR4           : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_40G_MLD 10.3125G VCO ?*/
    case digital_operationSpeeds_SPEED_42p42G_X4         : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_42P42G_HG2_XAUI 10.9375G VCO */
    case digital_operationSpeeds_SPEED_42G_X4            : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_42G_HG2_MLD 10.9375G VCO */
    case digital_operationSpeeds_SPEED_48p48G_X4         : { *frac_ns =  0x51eb ; break; } /* TSC_SPEED_48P48G_XFI 12.5G VCO */
/*  case digital_operationSpeeds_SPEED_10G_CX4           : { *frac_ns =  0x0000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X4            : { *frac_ns =  0x0000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_CX2           : { *frac_ns =  0x0000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X2            : { *frac_ns =  0x0000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10p5G_X2          : { *frac_ns =  0x0000 ; break; } */
/*  case digital_operationSpeeds_SPEED_21G_X2            : { *frac_ns =  0x5d9f ; break; }    TSC_SPEED_21G_HG2_MLD_DXGXS 10.9375G VCO */
/*  case digital_operationSpeeds_SPEED_10G_CX4_10p3125   : { *frac_ns =  0x0000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X4_10p3125    : { *frac_ns =  0x0000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10p5G_X4          : { *frac_ns =  0x0000 ; break; } */
/*  case digital_operationSpeeds_SPEED_2p5G_X1_10p3125   : { *frac_ns =  0x634c ; break; }    TSC_SPEED_2P5G 10.3125G VCO */
/*  case digital_operationSpeeds_SPEED_10G_KX4_10p3125   : { *frac_ns =  0x0000 ; break; } */
    default                                              : { return PHYMOD_E_FAIL;break; }
  }

  return PHYMOD_E_NONE;
}



int temod2pll_get_ui_values(int speed, uint16_t *ui_msb, uint16_t *ui_lsb)
{

  switch(speed) {
    case digital_operationSpeeds_SPEED_10M               : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } /* TSC_SPEED_10M 6.25G VCO */
    case digital_operationSpeeds_SPEED_100M              : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } /* TSC_SPEED_100M 6.25G VCO */
    case digital_operationSpeeds_SPEED_1000M             : { *ui_msb = 0xCCCC ; *ui_lsb = 0x66 ; break; } /* TSC_SPEED_1G 6.25G VCO */
    case digital_operationSpeeds_SPEED_1G_KX1            : { *ui_msb = 0xCCCC ; *ui_lsb = 0x66 ; break; } /* TSC_SPEED_1G 6.25G VCO */
    case digital_operationSpeeds_SPEED_2p5G_X1           : { *ui_msb = 0x51EB ; *ui_lsb = 0x42 ; break; } /* TSC_SPEED_2P5G 6.25G VCO */
    case digital_operationSpeeds_SPEED_5G_X1             : { *ui_msb = 0x28F5 ; *ui_lsb = 0x61 ; break; } /* TSC_SPEED_5G 6.25G VCO */
    case digital_operationSpeeds_SPEED_10M_10p3125       : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } /* TSC_SPEED_10M 10.3125G VCO */
    case digital_operationSpeeds_SPEED_100M_10p3125      : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } /* TSC_SPEED_100M 10.3125G VCO */
    case digital_operationSpeeds_SPEED_1000M_10p3125     : { *ui_msb = 0xCCCC ; *ui_lsb = 0x66 ; break; } /* TSC_SPEED_1G 10.3125G VCO */
    case digital_operationSpeeds_SPEED_5G_KR1            : { *ui_msb = 0x31A6 ; *ui_lsb = 0x01 ; break; } /* TSC_SPEED_5G_XFI 10.3125G VCO */
    case digital_operationSpeeds_SPEED_10G_KR1           : { *ui_msb = 0x18D3 ; *ui_lsb = 0x00 ; break; } /* TSC_SPEED_10G_XFI 10.3125G VCO */
    case digital_operationSpeeds_SPEED_10p6_X1           : { *ui_msb = 0x1767 ; *ui_lsb = 0x6E ; break; } /* TSC_SPEED_10P606G_XFI 10.9375G VCO */
    case digital_operationSpeeds_SPEED_12p12G_X1         : { *ui_msb = 0x147A ; *ui_lsb = 0x70 ; break; } /* TSC_SPEED_12P12G_XFI 12.5G VCO */
    case digital_operationSpeeds_SPEED_20G_CX2           : { *ui_msb = 0x18D3 ; *ui_lsb = 0x00 ; break; } /* TSC_SPEED_20G_HG2_XAUI_DXGXS 10.3125G VCO */
    case digital_operationSpeeds_SPEED_20G_X2            : { *ui_msb = 0x18D3 ; *ui_lsb = 0x00 ; break; } /* TSC_SPEED_20G_HG2_XAUI_DXGXS 10.3125G VCO */
    case digital_operationSpeeds_SPEED_21p21G_X2         : { *ui_msb = 0x1767 ; *ui_lsb = 0x6E ; break; } /* TSC_SPEED_21P21G_HG_XAUI_DXGXS 10.9375G VCO */
    case digital_operationSpeeds_SPEED_20G_KR2           : { *ui_msb = 0x18D3 ; *ui_lsb = 0x03 ; break; } /* TSC_SPEED_20G_MLD_DXGXS 10.3125G VCO */
    case digital_operationSpeeds_SPEED_20G_CR2           : { *ui_msb = 0x18D3 ; *ui_lsb = 0x03 ; break; } /* TSC_SPEED_20G_MLD_DXGXS 10.3125G VCO ? */
    case digital_operationSpeeds_SPEED_24p24G_X2         : { *ui_msb = 0x147A ; *ui_lsb = 0x70 ; break; } /* TSC_SPEED_24P24G_XFI 12.5G VCO */
    case digital_operationSpeeds_SPEED_10G_KX4           : { *ui_msb = 0x51EB ; *ui_lsb = 0x42 ; break; } /* TSC_SPEED_10G_XAUI 6.25G VCO */
    case digital_operationSpeeds_SPEED_20G_CX4           : { *ui_msb = 0x28F5 ; *ui_lsb = 0x61 ; break; } /* TSC_SPEED_20G_XAUI 6.25G VCO ?*/
    case digital_operationSpeeds_SPEED_20G_X4            : { *ui_msb = 0x28F5 ; *ui_lsb = 0x61 ; break; } /* TSC_SPEED_20G_XAUI 6.25G VCO */
    case digital_operationSpeeds_SPEED_21G_X4            : { *ui_msb = 0x2702 ; *ui_lsb = 0x38 ; break; } /* TSC_SPEED_21G_XAUI 6.5625G VCO */
    case digital_operationSpeeds_SPEED_40G_X4            : { *ui_msb = 0x18D3 ; *ui_lsb = 0x00 ; break; } /* TSC_SPEED_40G_HG_XAUI 10.3125G VCO */
    case digital_operationSpeeds_SPEED_40G_KR4           : { *ui_msb = 0x18D3 ; *ui_lsb = 0x00 ; break; } /* TSC_SPEED_40G_MLD 10.3125G VCO */
    case digital_operationSpeeds_SPEED_40G_CR4           : { *ui_msb = 0x18D3 ; *ui_lsb = 0x00 ; break; } /* TSC_SPEED_40G_MLD 10.3125G VCO ?*/
    case digital_operationSpeeds_SPEED_42p42G_X4         : { *ui_msb = 0x1767 ; *ui_lsb = 0x6E ; break; } /* TSC_SPEED_42P42G_HG2_XAUI 10.9375G VCO */
    case digital_operationSpeeds_SPEED_42G_X4            : { *ui_msb = 0x1767 ; *ui_lsb = 0x6E ; break; } /* TSC_SPEED_42G_HG2_MLD 10.9375G VCO */
    case digital_operationSpeeds_SPEED_48p48G_X4         : { *ui_msb = 0x147A ; *ui_lsb = 0x70 ; break; } /* TSC_SPEED_48P48G_XFI 12.5G VCO */
/*  case digital_operationSpeeds_SPEED_10G_CX4           : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X4            : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_CX2           : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X2            : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } */
/*  case digital_operationSpeeds_SPEED_10p5G_X2          : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } */
/*  case digital_operationSpeeds_SPEED_21G_X2            : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; }    TSC_SPEED_21G_HG2_MLD_DXGXS 10.9375G VCO */
/*  case digital_operationSpeeds_SPEED_10G_CX4_10p3125   : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X4_10p3125    : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } */
/*  case digital_operationSpeeds_SPEED_10p5G_X4          : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } */
/*  case digital_operationSpeeds_SPEED_2p5G_X1_10p3125   : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; }    TSC_SPEED_2P5G 10.3125G VCO */
/*  case digital_operationSpeeds_SPEED_10G_KX4_10p3125   : { *ui_msb = 0x0000 ; *ui_lsb = 0x00 ; break; } */
    default                                              : { return PHYMOD_E_FAIL   ;            break; }
  }
  return PHYMOD_E_NONE;
}


int temod2pll_get_no_pmd_txrx_fixed_lat(int speed, uint16_t *tx_fixed_int, uint16_t *tx_fixed_frac, uint16_t *rx_fixed_int, uint16_t *rx_fixed_frac)
{

  switch(speed) {
    case digital_operationSpeeds_SPEED_10M               : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFF0 ; *rx_fixed_frac = 0x000 ; break; } /* TSC_SPEED_10M 					: VCO  6.25G	: OS 5 */
    case digital_operationSpeeds_SPEED_100M              : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFF0 ; *rx_fixed_frac = 0x000 ; break; } /* TSC_SPEED_100M 					: VCO  6.25G	: OS 5 */
    case digital_operationSpeeds_SPEED_1000M             : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFF0 ; *rx_fixed_frac = 0x000 ; break; } /* TSC_SPEED_1G 					: VCO  6.25G	: OS 5 */
    case digital_operationSpeeds_SPEED_1G_KX1            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFF0 ; *rx_fixed_frac = 0x000 ; break; } /* TSC_SPEED_1G 					: VCO  6.25G	: OS 5 */

    case digital_operationSpeeds_SPEED_2p5G_X1           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFF9 ; *rx_fixed_frac = 0x266 ; break; } /* TSC_SPEED_2P5G 					: VCO  6.25G	: OS 2 */
    case digital_operationSpeeds_SPEED_10G_KX4           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFF9 ; *rx_fixed_frac = 0x266 ; break; } /* TSC_SPEED_10G_XAUI 				: VCO  6.25G	: OS 2 */

    case digital_operationSpeeds_SPEED_5G_X1             : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFC ; *rx_fixed_frac = 0x333 ; break; } /* TSC_SPEED_5G 					: VCO  6.25G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_CX4           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFC ; *rx_fixed_frac = 0x333 ; break; } /* TSC_SPEED_20G_XAUI 				: VCO  6.25G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_X4            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFC ; *rx_fixed_frac = 0x333 ; break; } /* TSC_SPEED_20G_XAUI 				: VCO  6.25G	: OS 1 */

    case digital_operationSpeeds_SPEED_21G_X4            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFC ; *rx_fixed_frac = 0x3CF ; break; } /* TSC_SPEED_21G_XAUI 				: VCO  6.5625G	: OS 1 */

    case digital_operationSpeeds_SPEED_10M_10p3125       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFF0 ; *rx_fixed_frac = 0x000 ; break; } /* TSC_SPEED_10M 					: VCO 10.3125G	: OS 8.25 */
    case digital_operationSpeeds_SPEED_100M_10p3125      : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFF0 ; *rx_fixed_frac = 0x000 ; break; } /* TSC_SPEED_100M 					: VCO 10.3125G	: OS 8.25 */
    case digital_operationSpeeds_SPEED_1000M_10p3125     : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFF0 ; *rx_fixed_frac = 0x000 ; break; } /* TSC_SPEED_1G 					: VCO 10.3125G	: OS 8.25 */

    case digital_operationSpeeds_SPEED_5G_KR1            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFC ; *rx_fixed_frac = 0x07C ; break; } /* TSC_SPEED_5G_XFI 				: VCO 10.3125G	: OS 2 */

    case digital_operationSpeeds_SPEED_10G_KR1           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x03E ; break; } /* TSC_SPEED_10G_XFI 				: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_CX2           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x03E ; break; } /* TSC_SPEED_20G_HG2_XAUI_DXGXS 	: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_X2            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x03E ; break; } /* TSC_SPEED_20G_HG2_XAUI_DXGXS 	: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_KR2           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x03E ; break; } /* TSC_SPEED_20G_MLD_DXGXS 			: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_CR2           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x03E ; break; } /* TSC_SPEED_20G_MLD_DXGXS 			: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_40G_X4            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x03E ; break; } /* TSC_SPEED_40G_HG_XAUI 			: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_40G_KR4           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x03E ; break; } /* TSC_SPEED_40G_MLD 				: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_40G_CR4           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x03E ; break; } /* TSC_SPEED_40G_MLD 				: VCO 10.3125G	: OS 1 */

    case digital_operationSpeeds_SPEED_10p6_X1           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x0AF ; break; } /* TSC_SPEED_10P606G_XFI 			: VCO 10.9375G	: OS 1 */
    case digital_operationSpeeds_SPEED_21p21G_X2         : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x0AF ; break; } /* TSC_SPEED_21P21G_HG_XAUI_DXGXS 	: VCO 10.9375G	: OS 1 */
    case digital_operationSpeeds_SPEED_42G_X4            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x0AF ; break; } /* TSC_SPEED_42G_HG2_MLD 			: VCO 10.9375G	: OS 1 */
    case digital_operationSpeeds_SPEED_42p42G_X4         : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x0AF ; break; } /* TSC_SPEED_42P42G_HG2_XAUI 		: VCO 10.9375G	: OS 1 */

    case digital_operationSpeeds_SPEED_12p12G_X1         : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x199 ; break; } /* TSC_SPEED_12P12G_XFI 			: VCO 12.5G		: OS 1 */
    case digital_operationSpeeds_SPEED_24p24G_X2         : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x199 ; break; } /* TSC_SPEED_24P24G_XFI 			: VCO 12.5G		: OS 1 */
    case digital_operationSpeeds_SPEED_48p48G_X4         : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xFFE ; *rx_fixed_frac = 0x199 ; break; } /* TSC_SPEED_48P48G_XFI 			: VCO 12.5G		: OS 1 */

/*  case digital_operationSpeeds_SPEED_10G_CX4           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X4            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_CX2           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X2            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10p5G_X2          : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_21G_X2            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; }    TSC_SPEED_21G_HG2_MLD_DXGXS 10.9375G VCO */
/*  case digital_operationSpeeds_SPEED_10G_CX4_10p3125   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X4_10p3125    : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10p5G_X4          : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_2p5G_X1_10p3125   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; }    TSC_SPEED_2P5G 10.3125G VCO */
/*  case digital_operationSpeeds_SPEED_10G_KX4_10p3125   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
    default                                              :  return PHYMOD_E_FAIL  ;                                                                            break; }

  return PHYMOD_E_NONE;

}
int temod2pll_get_txrx_fixed_lat(int speed, uint16_t *tx_fixed_int, uint16_t *tx_fixed_frac, uint16_t *rx_fixed_int, uint16_t *rx_fixed_frac)
{

  switch(speed) {
    case digital_operationSpeeds_SPEED_10M               : { *tx_fixed_int = 0x08 ; *tx_fixed_frac = 0x333 ;  *rx_fixed_int = 0xFE3 ; *rx_fixed_frac = 0x265 ; break; } /* TSC_SPEED_10M 					: VCO  6.25G	: OS 5 */
    case digital_operationSpeeds_SPEED_100M              : { *tx_fixed_int = 0x08 ; *tx_fixed_frac = 0x333 ;  *rx_fixed_int = 0xFE3 ; *rx_fixed_frac = 0x265 ; break; } /* TSC_SPEED_100M 					: VCO  6.25G	: OS 5 */
    case digital_operationSpeeds_SPEED_1000M             : { *tx_fixed_int = 0x08 ; *tx_fixed_frac = 0x333 ;  *rx_fixed_int = 0xFE3 ; *rx_fixed_frac = 0x265 ; break; } /* TSC_SPEED_1G 					: VCO  6.25G	: OS 5 */
    case digital_operationSpeeds_SPEED_1G_KX1            : { *tx_fixed_int = 0x08 ; *tx_fixed_frac = 0x333 ;  *rx_fixed_int = 0xFE3 ; *rx_fixed_frac = 0x265 ; break; } /* TSC_SPEED_1G 					: VCO  6.25G	: OS 5 */

    case digital_operationSpeeds_SPEED_2p5G_X1           : { *tx_fixed_int = 0x08 ; *tx_fixed_frac = 0x333 ;  *rx_fixed_int = 0xFED ; *rx_fixed_frac = 0x215 ; break; } /* TSC_SPEED_2P5G 					: VCO  6.25G	: OS 2 */
    case digital_operationSpeeds_SPEED_10G_KX4           : { *tx_fixed_int = 0x08 ; *tx_fixed_frac = 0x333 ;  *rx_fixed_int = 0xFED ; *rx_fixed_frac = 0x215 ; break; } /* TSC_SPEED_10G_XAUI 				: VCO  6.25G	: OS 2 */

    case digital_operationSpeeds_SPEED_5G_X1             : { *tx_fixed_int = 0x08 ; *tx_fixed_frac = 0x333 ;  *rx_fixed_int = 0xFF3 ; *rx_fixed_frac = 0x3B2 ; break; } /* TSC_SPEED_5G 					: VCO  6.25G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_CX4           : { *tx_fixed_int = 0x08 ; *tx_fixed_frac = 0x333 ;  *rx_fixed_int = 0xFF3 ; *rx_fixed_frac = 0x3B2 ; break; } /* TSC_SPEED_20G_XAUI 				: VCO  6.25G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_X4            : { *tx_fixed_int = 0x08 ; *tx_fixed_frac = 0x333 ;  *rx_fixed_int = 0xFF3 ; *rx_fixed_frac = 0x3B2 ; break; } /* TSC_SPEED_20G_XAUI 				: VCO  6.25G	: OS 1 */

    case digital_operationSpeeds_SPEED_21G_X4            : { *tx_fixed_int = 0x08 ; *tx_fixed_frac = 0x185 ;  *rx_fixed_int = 0xFF4 ; *rx_fixed_frac = 0x1FA ; break; } /* TSC_SPEED_21G_XAUI 				: VCO  6.5625G	: OS 1 */

    case digital_operationSpeeds_SPEED_10M_10p3125       : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x03D ;  *rx_fixed_int = 0xFE8 ; *rx_fixed_frac = 0x072 ; break; } /* TSC_SPEED_10M 					: VCO 10.3125G	: OS 8.25 */
    case digital_operationSpeeds_SPEED_100M_10p3125      : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x03D ;  *rx_fixed_int = 0xFE8 ; *rx_fixed_frac = 0x072 ; break; } /* TSC_SPEED_100M 					: VCO 10.3125G	: OS 8.25 */
    case digital_operationSpeeds_SPEED_1000M_10p3125     : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x03D ;  *rx_fixed_int = 0xFE8 ; *rx_fixed_frac = 0x072 ; break; } /* TSC_SPEED_1G 					: VCO 10.3125G	: OS 8.25 */

    case digital_operationSpeeds_SPEED_5G_KR1            : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x155 ;  *rx_fixed_int = 0xFF4 ; *rx_fixed_frac = 0x332 ; break; } /* TSC_SPEED_5G_XFI 				: VCO 10.3125G	: OS 2 */

    case digital_operationSpeeds_SPEED_10G_KR1           : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x155 ;  *rx_fixed_int = 0xFF8 ; *rx_fixed_frac = 0x2B6 ; break; } /* TSC_SPEED_10G_XFI 				: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_CX2           : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x155 ;  *rx_fixed_int = 0xFF8 ; *rx_fixed_frac = 0x2B6 ; break; } /* TSC_SPEED_20G_HG2_XAUI_DXGXS 	: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_X2            : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x155 ;  *rx_fixed_int = 0xFF8 ; *rx_fixed_frac = 0x2B6 ; break; } /* TSC_SPEED_20G_HG2_XAUI_DXGXS 	: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_KR2           : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x155 ;  *rx_fixed_int = 0xFF8 ; *rx_fixed_frac = 0x2B6 ; break; } /* TSC_SPEED_20G_MLD_DXGXS 			: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_CR2           : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x155 ;  *rx_fixed_int = 0xFF8 ; *rx_fixed_frac = 0x2B6 ; break; } /* TSC_SPEED_20G_MLD_DXGXS 			: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_40G_X4            : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x155 ;  *rx_fixed_int = 0xFF8 ; *rx_fixed_frac = 0x2B6 ; break; } /* TSC_SPEED_40G_HG_XAUI 			: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_40G_KR4           : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x155 ;  *rx_fixed_int = 0xFF8 ; *rx_fixed_frac = 0x2B6 ; break; } /* TSC_SPEED_40G_MLD 				: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_40G_CR4           : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x155 ;  *rx_fixed_int = 0xFF8 ; *rx_fixed_frac = 0x2B6 ; break; } /* TSC_SPEED_40G_MLD 				: VCO 10.3125G	: OS 1 */

    case digital_operationSpeeds_SPEED_10p6_X1           : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x01D ;  *rx_fixed_int = 0xFF9 ; *rx_fixed_frac = 0x063 ; break; } /* TSC_SPEED_10P606G_XFI 			: VCO 10.9375G	: OS 1 */
    case digital_operationSpeeds_SPEED_21p21G_X2         : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x01D ;  *rx_fixed_int = 0xFF9 ; *rx_fixed_frac = 0x063 ; break; } /* TSC_SPEED_21P21G_HG_XAUI_DXGXS 	: VCO 10.9375G	: OS 1 */
    case digital_operationSpeeds_SPEED_42G_X4            : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x01D ;  *rx_fixed_int = 0xFF9 ; *rx_fixed_frac = 0x063 ; break; } /* TSC_SPEED_42G_HG2_MLD 			: VCO 10.9375G	: OS 1 */
    case digital_operationSpeeds_SPEED_42p42G_X4         : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x01D ;  *rx_fixed_int = 0xFF9 ; *rx_fixed_frac = 0x063 ; break; } /* TSC_SPEED_42P42G_HG2_XAUI 		: VCO 10.9375G	: OS 1 */

    case digital_operationSpeeds_SPEED_12p12G_X1         : { *tx_fixed_int = 0x04 ; *tx_fixed_frac = 0x199 ;  *rx_fixed_int = 0xFF9 ; *rx_fixed_frac = 0x3D7 ; break; } /* TSC_SPEED_12P12G_XFI 			: VCO 12.5G		: OS 1 */
    case digital_operationSpeeds_SPEED_24p24G_X2         : { *tx_fixed_int = 0x04 ; *tx_fixed_frac = 0x199 ;  *rx_fixed_int = 0xFF9 ; *rx_fixed_frac = 0x3D7 ; break; } /* TSC_SPEED_24P24G_XFI 			: VCO 12.5G		: OS 1 */
    case digital_operationSpeeds_SPEED_48p48G_X4         : { *tx_fixed_int = 0x04 ; *tx_fixed_frac = 0x199 ;  *rx_fixed_int = 0xFF9 ; *rx_fixed_frac = 0x3D7 ; break; } /* TSC_SPEED_48P48G_XFI 			: VCO 12.5G		: OS 1 */

/*  case digital_operationSpeeds_SPEED_10G_CX4           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X4            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_CX2           : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X2            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10p5G_X2          : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_21G_X2            : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; }    TSC_SPEED_21G_HG2_MLD_DXGXS 10.9375G VCO */
/*  case digital_operationSpeeds_SPEED_10G_CX4_10p3125   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X4_10p3125    : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_10p5G_X4          : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
/*  case digital_operationSpeeds_SPEED_2p5G_X1_10p3125   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; }    TSC_SPEED_2P5G 10.3125G VCO */
/*  case digital_operationSpeeds_SPEED_10G_KX4_10p3125   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0x000 ; *rx_fixed_frac = 0x000 ; break; } */
    default                                              :  return PHYMOD_E_FAIL  ;                                                                            break; }

  return PHYMOD_E_NONE;

}

int temod2pll_get_am_bits(int speed, long *am_bits)
{

  switch(speed) {
    case digital_operationSpeeds_SPEED_10M               : { *am_bits = 0  ; break; } /* TSC_SPEED_10M 					: VCO  6.25G	: OS 5 */
    case digital_operationSpeeds_SPEED_100M              : { *am_bits = 0  ; break; } /* TSC_SPEED_100M 					: VCO  6.25G	: OS 5 */
    case digital_operationSpeeds_SPEED_1000M             : { *am_bits = 0  ; break; } /* TSC_SPEED_1G 					: VCO  6.25G	: OS 5 */
    case digital_operationSpeeds_SPEED_1G_KX1            : { *am_bits = 0  ; break; } /* TSC_SPEED_1G 					: VCO  6.25G	: OS 5 */

    case digital_operationSpeeds_SPEED_2p5G_X1           : { *am_bits = 0  ; break; } /* TSC_SPEED_2P5G 					: VCO  6.25G	: OS 2 */
    case digital_operationSpeeds_SPEED_10G_KX4           : { *am_bits = 10 ; break; } /* TSC_SPEED_10G_XAUI 				: VCO  6.25G	: OS 2 */

    case digital_operationSpeeds_SPEED_5G_X1             : { *am_bits = 0  ; break; } /* TSC_SPEED_5G 					: VCO  6.25G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_CX4           : { *am_bits = 10 ; break; } /* TSC_SPEED_20G_XAUI 				: VCO  6.25G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_X4            : { *am_bits = 10 ; break; } /* TSC_SPEED_20G_XAUI 				: VCO  6.25G	: OS 1 */

    case digital_operationSpeeds_SPEED_21G_X4            : { *am_bits = 10 ; break; } /* TSC_SPEED_21G_XAUI 				: VCO  6.5625G	: OS 1 */

    case digital_operationSpeeds_SPEED_10M_10p3125       : { *am_bits =  0 ; break; } /* TSC_SPEED_10M 					: VCO 10.3125G	: OS 8.25 */
    case digital_operationSpeeds_SPEED_100M_10p3125      : { *am_bits =  0 ; break; } /* TSC_SPEED_100M 					: VCO 10.3125G	: OS 8.25 */
    case digital_operationSpeeds_SPEED_1000M_10p3125     : { *am_bits =  0 ; break; } /* TSC_SPEED_1G 					: VCO 10.3125G	: OS 8.25 */

    case digital_operationSpeeds_SPEED_5G_KR1            : { *am_bits =  0 ; break; } /* TSC_SPEED_5G_XFI 				: VCO 10.3125G	: OS 2 */

    case digital_operationSpeeds_SPEED_10G_KR1           : { *am_bits =  0 ; break; } /* TSC_SPEED_10G_XFI 				: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_CX2           : { *am_bits = 66 ; break; } /* TSC_SPEED_20G_HG2_XAUI_DXGXS 	: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_X2            : { *am_bits = 66 ; break; } /* TSC_SPEED_20G_HG2_XAUI_DXGXS 	: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_KR2           : { *am_bits = 66 ; break; } /* TSC_SPEED_20G_MLD_DXGXS 			: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_20G_CR2           : { *am_bits = 66 ; break; } /* TSC_SPEED_20G_MLD_DXGXS 			: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_40G_X4            : { *am_bits = 66 ; break; } /* TSC_SPEED_40G_HG_XAUI 			: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_40G_KR4           : { *am_bits = 66 ; break; } /* TSC_SPEED_40G_MLD 				: VCO 10.3125G	: OS 1 */
    case digital_operationSpeeds_SPEED_40G_CR4           : { *am_bits = 66 ; break; } /* TSC_SPEED_40G_MLD 				: VCO 10.3125G	: OS 1 */

    case digital_operationSpeeds_SPEED_10p6_X1           : { *am_bits =  0 ; break; } /* TSC_SPEED_10P606G_XFI 			: VCO 10.9375G	: OS 1 */
    case digital_operationSpeeds_SPEED_21p21G_X2         : { *am_bits = 66 ; break; } /* TSC_SPEED_21P21G_HG_XAUI_DXGXS 	: VCO 10.9375G	: OS 1 */
    case digital_operationSpeeds_SPEED_42G_X4            : { *am_bits = 66 ; break; } /* TSC_SPEED_42G_HG2_MLD 			: VCO 10.9375G	: OS 1 */
    case digital_operationSpeeds_SPEED_42p42G_X4         : { *am_bits = 66 ; break; } /* TSC_SPEED_42P42G_HG2_XAUI 		: VCO 10.9375G	: OS 1 */

    case digital_operationSpeeds_SPEED_12p12G_X1         : { *am_bits =  0 ; break; } /* TSC_SPEED_12P12G_XFI 			: VCO 12.5G		: OS 1 */
    case digital_operationSpeeds_SPEED_24p24G_X2         : { *am_bits = 66 ; break; } /* TSC_SPEED_24P24G_XFI 			: VCO 12.5G		: OS 1 */
    case digital_operationSpeeds_SPEED_48p48G_X4         : { *am_bits = 66 ; break; } /* TSC_SPEED_48P48G_XFI 			: VCO 12.5G		: OS 1 */

/*  case digital_operationSpeeds_SPEED_10G_CX4           : { *am_bits =  0 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X4            : { *am_bits =  0 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_CX2           : { *am_bits =  0 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X2            : { *am_bits =  0 ; break; } */
/*  case digital_operationSpeeds_SPEED_10p5G_X2          : { *am_bits =  0 ; break; } */
/*  case digital_operationSpeeds_SPEED_21G_X2            : { *am_bits =  0 ; break; }    TSC_SPEED_21G_HG2_MLD_DXGXS 10.9375G VCO */
/*  case digital_operationSpeeds_SPEED_10G_CX4_10p3125   : { *am_bits =  0 ; break; } */
/*  case digital_operationSpeeds_SPEED_10G_X4_10p3125    : { *am_bits =  0 ; break; } */
/*  case digital_operationSpeeds_SPEED_10p5G_X4          : { *am_bits =  0 ; break; } */
/*  case digital_operationSpeeds_SPEED_2p5G_X1_10p3125   : { *am_bits =  0 ; break; }    TSC_SPEED_2P5G 10.3125G VCO */
/*  case digital_operationSpeeds_SPEED_10G_KX4_10p3125   : { *am_bits =  0 ; break; } */
    default                                              :  return PHYMOD_E_FAIL  ;                                                                            break; }

  return PHYMOD_E_NONE;

}

