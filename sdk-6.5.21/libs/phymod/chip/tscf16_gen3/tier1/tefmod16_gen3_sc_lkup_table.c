/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description:  speed table
 *----------------------------------------------------------------------*/
#define _SDK_TEFMOD16_GEN3_ 1

#ifdef _SDK_TEFMOD16_GEN3_
#include "tefmod16_gen3_enum_defines.h"
#include "tefmod16_gen3.h"
#include <phymod/phymod.h>
#include "falcon16_v1l4p1/tier1/include/falcon16_v1l4p1_enum.h"
#endif

#ifdef _SDK_TEFMOD16_GEN3_
#define PHYMOD_ST const phymod_access_t
#else
#define PHYMOD_ST tefmod_st
#endif

#include "tefmod16_gen3_sc_lkup_table.h"

const sc_pmd_entry_st sc_pmd16_gen3_entry[] = {
  /*`SPEED_10G_CR1: 0*/             { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_10G_KR1: 1*/             { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_10G_X1: 2*/              { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 3*/                {         0, 0},
  /*`SPEED_10G_HG2_CR1: 4*/         { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_10G_HG2_KR1: 5*/         { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_10G_HG2_X1: 6*/          { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 7*/                {         0, 0},
  /*`SPEED_20G_CR1: 8*/             { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_132},
  /*`SPEED_20G_KR1: 9 */            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_132},
  /*`SPEED_20G_X1: 10*/             { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_132},
  /*`NULL ENTRY: 11*/               {         0, 0},
  /*`SPEED_20G_HG2_CR1: 12*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_140},
  /*`SPEED_20G_HG2_KR1: 13*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_140},
  /*`SPEED_20G_HG2_X1: 14*/         { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_140},
  /*`NULL ENTRY: 15*/               {         0, 0},
  /*`SPEED_25G_CR1: 16*/            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_25G_KR1:  17*/           { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_25G_X1: 18*/             { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 19*/               {         0, 0},
  /*`SPEED_25G_HG2_CR1: 20*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_25G_HG2_KR1: 21*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_25G_HG2_X1: 22*/         { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 23*/               {         0, 0},
  /*`SPEED_20G_CR2:  24*/           { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_132},
  /*`SPEED_20G_KR2: 25*/            { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_132},
  /*`SPEED_20G_X2: 26*/             { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_132},
  /*`NULL ENTRY: 27*/               {         0, 0},
  /*`SPEED_20G_HG2_CR2: 28*/        { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_140},
  /*`SPEED_20G_HG2_KR2: 29*/        { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_140},
  /*`SPEED_20G_HG2_X2: 30*/         { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_140},
  /*`NULL ENTRY: 31*/               {         0, 0},
  /*`SPEED_40G_CR2: 32*/            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_132},
  /*`SPEED_40G_KR2: 33*/            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_132},
  /*`SPEED_40G_X2: 34*/             { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_132},
  /*`NULL ENTRY: 35*/               {         0, 0},
  /*`SPEED_40G_HG2_CR2: 36*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_140},
  /*`SPEED_40G_HG2_KR2: 37*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_140},
  /*`SPEED_40G_HG2_X2: 38*/         { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_140},
  /*`NULL ENTRY: 39*/               {         0, 0},
  /*`SPEED_40G_CR4:  40*/           { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_132},
  /*`SPEED_40G_KR4:  41*/           { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_132},
  /*`SPEED_40G_X4: 42*/             { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_132},
  /*`NULL ENTRY: 43*/               {         0, 0},
  /*`SPEED_40G_HG2_CR4: 44*/        { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_140},
  /*`SPEED_40G_HG2_KR4: 45*/        { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_140},
  /*`SPEED_40G_HG2_X4: 46*/         { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_140},
  /*`NULL ENTRY: 47*/               {         0, 0},
  /*`SPEED_50G_CR2: 48*/            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_50G_KR2: 49*/            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_50G_X2: 50*/             { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 51*/               {         0, 0},
  /*`SPEED_50G_HG2_CR2: 52*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_50G_HG2_KR2: 53*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_50G_HG2_X2: 54*/         { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 55*/               {         0, 0},
  /*`SPEED_50G_CR4: 56*/            { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_50G_KR4:  57*/           { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_50G_X4: 58*/             { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 59*/               {         0, 0},
  /*`SPEED_50G_HG2_CR4: 60*/        { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_50G_HG2_KR4: 61*/        { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_50G_HG2_X4: 62*/         { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 63*/               {         0, 0},
  /*`SPEED_100G_CR4: 64*/           { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_100G_KR4: 65*/           { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_100G_X4: 66*/            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 67*/               {         0, 0},
  /*`SPEED_100G_HG2_CR4: 68*/       { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_100G_HG2_KR4: 69*/       { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_100G_HG2_X4: 70*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 71*/               {         0, 0},
  /*`SPEED_CL73_20GVCO: 72*/        { OS_MODE_16p5, FALCON16_V1L4P1_PLL_DIV_132},
  /*`NULL ENTRY: 73*/               {         0, 0},
  /*`NULL ENTRY: 74*/               {         0, 0},
  /*`NULL ENTRY: 75*/               {         0, 0},
  /*`NULL ENTRY: 76*/               {         0, 0},
  /*`NULL ENTRY: 77*/               {         0, 0},
  /*`NULL ENTRY: 78*/               {         0, 0},
  /*`NULL ENTRY: 79*/               {         0, 0},
  /*`SPEED_CL73_25GVCO: 80*/        { OS_MODE_20p625, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 81*/               {         0, 0},
  /*`NULL ENTRY: 82*/               {         0, 0},
  /*`NULL ENTRY: 83*/               {         0, 0},
  /*`NULL ENTRY: 84*/               {         0, 0},
  /*`NULL ENTRY: 84*/               {         0, 0},
  /*`NULL ENTRY: 86*/               {         0, 0},
  /*`NULL ENTRY: 87*/               {         0, 0},
  /*`SPEED_1G_20GVCO: 88*/          { OS_MODE_16p5, FALCON16_V1L4P1_PLL_DIV_132},
  /*`NULL ENTRY: 89*/               {         0, 0},
  /*`NULL ENTRY: 90*/               {         0, 0},
  /*`NULL ENTRY: 91*/               {         0, 0},
  /*`NULL ENTRY: 92*/               {         0, 0},
  /*`NULL ENTRY: 93*/               {         0, 0},
  /*`NULL ENTRY: 94*/               {         0, 0},
  /*`NULL ENTRY: 95*/               {         0, 0},
  /*`SPEED_1G_25GVCO: 96*/          { OS_MODE_20p625, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 97*/               {         0, 0},
  /*`SPEED_2P5G_KX1: 98*/           { OS_MODE_8p25, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_5G_KR1: 99*/             { OS_MODE_4, FALCON16_V1L4P1_PLL_DIV_132}
};

/* DV doesn't provide 125M table. Modify in future */
const sc_pmd_entry_st sc_pmd16_gen3_entry_125M_ref[] = {
  /*`SPEED_10G_CR1: 0*/             { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_10G_KR1: 1*/             { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_10G_X1: 2*/              { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 3*/                {         0, 0},
  /*`SPEED_10G_HG2_CR1: 4*/         { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_10G_HG2_KR1: 5*/         { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_10G_HG2_X1: 6*/          { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 7*/                {         0, 0},
  /*`SPEED_20G_CR1: 8*/             { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_20G_KR1: 9 */            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_20G_X1: 10*/             { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 11*/               {         0, 0},
  /*`SPEED_20G_HG2_CR1: 12*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_20G_HG2_KR1: 13*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_20G_HG2_X1: 14*/         { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 15*/               {         0, 0},
  /*`SPEED_25G_CR1: 16*/            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_25G_KR1:  17*/           { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_25G_X1: 18*/             { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 19*/               {         0, 0},
  /*`SPEED_25G_HG2_CR1: 20*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_25G_HG2_KR1: 21*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_25G_HG2_X1: 22*/         { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 23*/               {         0, 0},
  /*`SPEED_20G_CR2:  24*/           { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_20G_KR2: 25*/            { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_20G_X2: 26*/             { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 27*/               {         0, 0},
  /*`SPEED_20G_HG2_CR2: 28*/        { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_20G_HG2_KR2: 29*/        { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_20G_HG2_X2: 30*/         { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 31*/               {         0, 0},
  /*`SPEED_40G_CR2: 32*/            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_40G_KR2: 33*/            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_40G_X2: 34*/             { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 35*/               {         0, 0},
  /*`SPEED_40G_HG2_CR2: 36*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_40G_HG2_KR2: 37*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_40G_HG2_X2: 38*/         { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 39*/               {         0, 0},
  /*`SPEED_40G_CR4:  40*/           { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_40G_KR4:  41*/           { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_40G_X4: 42*/             { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 43*/               {         0, 0},
  /*`SPEED_40G_HG2_CR4: 44*/        { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_40G_HG2_KR4: 45*/        { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_40G_HG2_X4: 46*/         { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 47*/               {         0, 0},
  /*`SPEED_50G_CR2: 48*/            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_50G_KR2: 49*/            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_50G_X2: 50*/             { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 51*/               {         0, 0},
  /*`SPEED_50G_HG2_CR2: 52*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_50G_HG2_KR2: 53*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_50G_HG2_X2: 54*/         { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 55*/               {         0, 0},
  /*`SPEED_50G_CR4: 56*/            { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_50G_KR4:  57*/           { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_50G_X4: 58*/             { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 59*/               {         0, 0},
  /*`SPEED_50G_HG2_CR4: 60*/        { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_50G_HG2_KR4: 61*/        { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_50G_HG2_X4: 62*/         { OS_MODE_2, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 63*/               {         0, 0},
  /*`SPEED_100G_CR4: 64*/           { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_100G_KR4: 65*/           { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`SPEED_100G_X4: 66*/            { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 67*/               {         0, 0},
  /*`SPEED_100G_HG2_CR4: 68*/       { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_100G_HG2_KR4: 69*/       { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`SPEED_100G_HG2_X4: 70*/        { OS_MODE_1, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 71*/               {         0, 0},
  /*`SPEED_CL73_20GVCO: 72*/        { OS_MODE_16p5, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 73*/               {         0, 0},
  /*`NULL ENTRY: 74*/               {         0, 0},
  /*`NULL ENTRY: 75*/               {         0, 0},
  /*`NULL ENTRY: 76*/               {         0, 0},
  /*`NULL ENTRY: 77*/               {         0, 0},
  /*`NULL ENTRY: 78*/               {         0, 0},
  /*`NULL ENTRY: 79*/               {         0, 0},
  /*`SPEED_CL73_25GVCO: 80*/        { OS_MODE_20p625, FALCON16_V1L4P1_PLL_DIV_175},
  /*`NULL ENTRY: 81*/               {         0, 0},
  /*`NULL ENTRY: 82*/               {         0, 0},
  /*`NULL ENTRY: 83*/               {         0, 0},
  /*`NULL ENTRY: 84*/               {         0, 0},
  /*`NULL ENTRY: 84*/               {         0, 0},
  /*`NULL ENTRY: 86*/               {         0, 0},
  /*`NULL ENTRY: 87*/               {         0, 0},
  /*`SPEED_1G_20GVCO: 88*/          { OS_MODE_16p5, FALCON16_V1L4P1_PLL_DIV_165},
  /*`NULL ENTRY: 89*/               {         0, 0},
  /*`NULL ENTRY: 90*/               {         0, 0},
  /*`NULL ENTRY: 91*/               {         0, 0},
  /*`NULL ENTRY: 92*/               {         0, 0},
  /*`NULL ENTRY: 93*/               {         0, 0},
  /*`NULL ENTRY: 94*/               {         0, 0},
  /*`NULL ENTRY: 95*/               {         0, 0},
  /*`SPEED_1G_25GVCO: 96*/          { OS_MODE_20p625, FALCON16_V1L4P1_PLL_DIV_165}
};


int tefmod16_gen3_get_mapped_speed(tefmod16_gen3_spd_intfc_type_t spd_intf, int *speed)
{
  /* speeds covered in Forced-speed */
  if(spd_intf == TEFMOD_SPD_10000_XFI)           *speed = sc_x4_control_sc_S_10G_X1;
  if(spd_intf == TEFMOD_SPD_10600_XFI_HG)        *speed = sc_x4_control_sc_S_10G_HG2_X1;
  if(spd_intf == TEFMOD_SPD_20000_XFI)           *speed = sc_x4_control_sc_S_20G_X1;
  if(spd_intf == TEFMOD_SPD_21200_XFI_HG)        *speed = sc_x4_control_sc_S_20G_HG2_X1;
  if(spd_intf == TEFMOD_SPD_25000_XFI)           *speed = sc_x4_control_sc_S_25G_X1;
  if(spd_intf == TEFMOD_SPD_26500_XFI_HG)        *speed = sc_x4_control_sc_S_25G_HG2_X1;
  if(spd_intf == TEFMOD_SPD_20G_MLD_X2)          *speed = sc_x4_control_sc_S_20G_X2;
  if(spd_intf == TEFMOD_SPD_21G_MLD_HG_X2)       *speed = sc_x4_control_sc_S_20G_HG2_X2;
  if(spd_intf == TEFMOD_SPD_40G_MLD_X2)          *speed = sc_x4_control_sc_S_40G_X2;
  if(spd_intf == TEFMOD_SPD_42G_MLD_HG_X2)       *speed = sc_x4_control_sc_S_40G_HG2_X2;
  if(spd_intf == TEFMOD_SPD_40G_MLD_X4)          *speed = sc_x4_control_sc_S_40G_X4;
  if(spd_intf == TEFMOD_SPD_42G_MLD_HG_X4)       *speed = sc_x4_control_sc_S_40G_HG2_X4;
  if(spd_intf == TEFMOD_SPD_50G_MLD_X2)          *speed = sc_x4_control_sc_S_50G_X2;
  if(spd_intf == TEFMOD_SPD_53G_MLD_HG_X2)       *speed = sc_x4_control_sc_S_50G_HG2_X2;
  if(spd_intf == TEFMOD_SPD_50G_MLD_X4)          *speed = sc_x4_control_sc_S_50G_X4;
  if(spd_intf == TEFMOD_SPD_53G_MLD_HG_X4)       *speed = sc_x4_control_sc_S_50G_HG2_X4;
  if(spd_intf == TEFMOD_SPD_100G_MLD_X4)         *speed = sc_x4_control_sc_S_100G_X4;
  if(spd_intf == TEFMOD_SPD_106G_MLD_HG_X4)      *speed = sc_x4_control_sc_S_100G_HG2_X4;
  /*  These are the speeds that are not in forced mode or AN mode */
  if(spd_intf == TEFMOD_SPD_10000_XFI_CR1)       *speed = sc_x4_control_sc_S_10G_CR1;
  if(spd_intf == TEFMOD_SPD_10600_XFI_HG_CR1)    *speed = sc_x4_control_sc_S_10G_HG2_CR1;
  if(spd_intf == TEFMOD_SPD_CL73_20G)            *speed = sc_x4_control_sc_S_CL73_20GVCO;
  if(spd_intf == TEFMOD_SPD_CL73_25G)            *speed = sc_x4_control_sc_S_CL73_25GVCO;
  if(spd_intf == TEFMOD_SPD_1G_20G)              *speed = sc_x4_control_sc_S_CL36_20GVCO;
  if(spd_intf == TEFMOD_SPD_1G_25G)              *speed = sc_x4_control_sc_S_CL36_25GVCO;
  if(spd_intf == TEFMOD_SPD_2P5G_KX1)            *speed = sc_x4_control_sc_S_2P5G_KX1;
  if(spd_intf == TEFMOD_SPD_5G_KR1)              *speed = sc_x4_control_sc_S_5G_KR1;


  return PHYMOD_E_NONE;
}

int tefmod16_gen3_am_bits_get(int speed, uint32_t *am_bits)
{

  switch(speed) {
    case sc_x4_control_sc_S_CL36_20GVCO      : { *am_bits = 0  ; break; } /* TSC_SPEED_1G 20.625G VCO */
    case sc_x4_control_sc_S_CL36_25GVCO      : { *am_bits = 0  ; break; } /* TSC_SPEED_1G 25.78125G VCO */
    case sc_x4_control_sc_S_2P5G_KX1         : { *am_bits = 0  ; break; } /* TSC_SPEED_2P5G 25.78125G VCO */
    case sc_x4_control_sc_S_5G_KR1           : { *am_bits = 0  ; break; } /* TSC_SPEED_5G_XFI 20.625G VCO */
    case sc_x4_control_sc_S_10G_X1           : { *am_bits = 0  ; break; } /* TSC_SPEED_10G_XFI 20.625G VCO */
    case sc_x4_control_sc_S_10G_CR1          : { *am_bits = 0  ; break; } /* TSC_SPEED_10G_XFI 20.625G VCO */
    case sc_x4_control_sc_S_10G_KR1          : { *am_bits = 0  ; break; } /* TSC_SPEED_10G_XFI 20.625G VCO */
    case sc_x4_control_sc_S_10G_HG2_CR1      : { *am_bits = 0  ; break; } /* TSC_SPEED_10P606G_XFI 21.875G VCO */
    case sc_x4_control_sc_S_10G_HG2_KR1      : { *am_bits = 0  ; break; } /* TSC_SPEED_10P606G_XFI 21.875G VCO */
    case sc_x4_control_sc_S_10G_HG2_X1       : { *am_bits = 0  ; break; } /* TSC_SPEED_10P606G_XFI 21.875G VCO */
    case sc_x4_control_sc_S_20G_X1           : { *am_bits = 0  ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO */
    case sc_x4_control_sc_S_20G_CR1          : { *am_bits = 0  ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO */
    case sc_x4_control_sc_S_20G_KR1          : { *am_bits = 0  ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO */
    case sc_x4_control_sc_S_CL73_20GVCO      : { *am_bits = 0  ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO */
    case sc_x4_control_sc_S_20G_HG2_X1       : { *am_bits = 0  ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO */
    case sc_x4_control_sc_S_20G_HG2_CR1      : { *am_bits = 0  ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO */
    case sc_x4_control_sc_S_20G_HG2_KR1      : { *am_bits = 0  ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO */
    case sc_x4_control_sc_S_25G_X1           : { *am_bits = 0  ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO */
    case sc_x4_control_sc_S_25G_CR1          : { *am_bits = 0  ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO */
    case sc_x4_control_sc_S_25G_KR1          : { *am_bits = 0  ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO */
    case sc_x4_control_sc_S_25G_CR_IEEE      : { *am_bits = 0  ; break; } /* TSC_SPEED_25G_CR_IEEE 25.78125G VCO */
    case sc_x4_control_sc_S_25G_CRS_IEEE     : { *am_bits = 0  ; break; } /* TSC_SPEED_25G_CRS_IEEE 25.78125G VCO */
    case sc_x4_control_sc_S_25G_KR_IEEE      : { *am_bits = 0  ; break; } /* TSC_SPEED_25G_KR_IEEE 25.78125G VCO */
    case sc_x4_control_sc_S_25G_KRS_IEEE     : { *am_bits = 0  ; break; } /* TSC_SPEED_25G_KRS_IEEE 25.78125G VCO */
    case sc_x4_control_sc_S_CL73_25GVCO      : { *am_bits = 0  ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO */
    case sc_x4_control_sc_S_25G_HG2_X1       : { *am_bits = 0  ; break; } /* TSC_SPEED_25G_XFI_HG2 27.34375G VCO */
    case sc_x4_control_sc_S_25G_HG2_CR1      : { *am_bits = 0  ; break; } /* TSC_SPEED_25G_XFI_HG2 or TSC_SPEED_25G_CR1_HG2_MSA 27.34375G VCO */
    case sc_x4_control_sc_S_25G_HG2_KR1      : { *am_bits = 0  ; break; } /* TSC_SPEED_25G_XFI_HG2 or TSC_SPEED_25G_KR1_HG2_MSA 27.34375G VCO */
    case sc_x4_control_sc_S_20G_X2           : { *am_bits = 0  ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO */
    case sc_x4_control_sc_S_20G_CR2          : { *am_bits = 66  ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO */
    case sc_x4_control_sc_S_20G_KR2          : { *am_bits = 66  ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO */
    case sc_x4_control_sc_S_20G_HG2_X2       : { *am_bits = 0  ; break; } /* TSC_SPEED_21G_HG2_MLD_DXGXS 21.875G VCO */
    case sc_x4_control_sc_S_40G_X2           : { *am_bits = 66  ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO */
    case sc_x4_control_sc_S_40G_CR2          : { *am_bits = 66  ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO */
    case sc_x4_control_sc_S_40G_KR2          : { *am_bits = 66  ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO */
    case sc_x4_control_sc_S_40G_HG2_X2       : { *am_bits = 0  ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO */
    case sc_x4_control_sc_S_40G_HG2_CR2      : { *am_bits = 0  ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO */
    case sc_x4_control_sc_S_40G_HG2_KR2      : { *am_bits = 0  ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO */
    case sc_x4_control_sc_S_50G_X2           : { *am_bits = 66  ; break; } /* TSC_SPEED_50G_MLD_DXGXS 25.78125G VCO */
    case sc_x4_control_sc_S_50G_CR2          : { *am_bits = 66  ; break; } /* TSC_SPEED_50G_MLD_DXGXS or TSC_SPEED_50G_CR2_MSA 25.78125G VCO */
    case sc_x4_control_sc_S_50G_KR2          : { *am_bits = 0  ; break; } /* TSC_SPEED_50G_MLD_DXGXS or TSC_SPEED_50G_KR2_MSA 25.78125G VCO */
    case sc_x4_control_sc_S_50G_HG2_X2       : { *am_bits = 66  ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS 27.34375G VCO */
    case sc_x4_control_sc_S_50G_HG2_CR2      : { *am_bits = 0  ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS or TSC_SPEED_50G_CR2_MSA 25.78125G VCO */
    case sc_x4_control_sc_S_50G_HG2_KR2      : { *am_bits = 0  ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS or TSC_SPEED_50G_KR2_MSA 25.78125G VCO */
    case sc_x4_control_sc_S_40G_X4           : { *am_bits = 66  ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO */
    case sc_x4_control_sc_S_40G_CR4          : { *am_bits = 66  ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO */
    case sc_x4_control_sc_S_40G_KR4          : { *am_bits = 66  ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO */
    case sc_x4_control_sc_S_40G_HG2_X4       : { *am_bits = 0  ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO */
    case sc_x4_control_sc_S_40G_HG2_CR4      : { *am_bits = 0  ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO */
    case sc_x4_control_sc_S_40G_HG2_KR4      : { *am_bits = 0  ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO */
    case sc_x4_control_sc_S_50G_X4           : { *am_bits = 66  ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO */
    case sc_x4_control_sc_S_50G_CR4          : { *am_bits = 66  ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO */
    case sc_x4_control_sc_S_50G_KR4          : { *am_bits = 66  ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO */
    case sc_x4_control_sc_S_50G_HG2_X4       : { *am_bits = 0  ; break; } /* TSC_SPEED_53G_HG2_MLD 27.343755G VCO */
    case sc_x4_control_sc_S_50G_HG2_CR4      : { *am_bits = 0  ; break; } /* TSC_SPEED_53G_HG2_MLD 27.343755G VCO */
    case sc_x4_control_sc_S_50G_HG2_KR4      : { *am_bits = 0  ; break; } /* TSC_SPEED_53G_HG2_MLD 27.343755G VCO */
    case sc_x4_control_sc_S_100G_X4          : { *am_bits = 66  ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO */
    case sc_x4_control_sc_S_100G_CR4         : { *am_bits = 66  ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO */
    case sc_x4_control_sc_S_100G_KR4         : { *am_bits = 66  ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO */
    case sc_x4_control_sc_S_100G_HG2_X4      : { *am_bits = 0  ; break; } /* TSC_SPEED_106G_HG2_MLD 27.343755G VCO */
    case sc_x4_control_sc_S_100G_HG2_CR4     : { *am_bits = 0  ; break; } /* TSC_SPEED_106G_HG2_MLD 27.343755G VCO */
    case sc_x4_control_sc_S_100G_HG2_KR4     : { *am_bits = 0  ; break; } /* TSC_SPEED_106G_HG2_MLD 27.343755G VCO */
    default                                  : { return PHYMOD_E_FAIL  ; break; }
  }

  return PHYMOD_E_NONE;
}

int tefmod16_gen3_no_pmd_txrx_fixed_latency_get(int speed, uint16_t *tx_fixed_int, uint16_t *tx_fixed_frac, uint16_t *rx_fixed_int, uint16_t *rx_fixed_frac)
{
  switch(speed) {
    case sc_x4_control_sc_S_CL36_20GVCO   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x000 ; break; } /* TSC_SPEED_1G 20.625G VCO : 16.5 OS */
    case sc_x4_control_sc_S_CL73_20GVCO   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x000 ; break; } /* TSC_SPEED_1G_AN 20.625G VCO : 16.5 OS */

    case sc_x4_control_sc_S_CL36_25GVCO   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x000 ; break; } /* TSC_SPEED_1G 25.78125G VCO : 20.625 OS */
    case sc_x4_control_sc_S_CL73_25GVCO   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x000 ; break; } /* TSC_SPEED_1G_AN 25.78125G VCO : 20.625 OS */


    case sc_x4_control_sc_S_20G_X1        : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x03e ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO : 1 OS */
    case sc_x4_control_sc_S_20G_CR1       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x03e ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO : 1 OS */
    case sc_x4_control_sc_S_20G_KR1       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x03e ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO : 1 OS */

    case sc_x4_control_sc_S_40G_X2        : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x03e ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO : 1 OS */
    case sc_x4_control_sc_S_40G_CR2       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x03e ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO : 1 OS */
    case sc_x4_control_sc_S_40G_KR2       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x03e ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO : 1 OS */



    case sc_x4_control_sc_S_10G_X1        : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x07c ; break; } /* TSC_SPEED_10G_XFI 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_10G_CR1       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x07c ; break; } /* TSC_SPEED_10G_XFI 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_10G_KR1       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x07c ; break; } /* TSC_SPEED_10G_XFI 20.625G VCO : 2 OS */

    case sc_x4_control_sc_S_20G_X2        : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x07c ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_20G_CR2       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x07c ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_20G_KR2       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x07c ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO : 2 OS */

    case sc_x4_control_sc_S_40G_X4        : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x07c ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_40G_CR4       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x07c ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_40G_KR4       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x07c ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO : 2 OS */

    case sc_x4_control_sc_S_5G_KR1        : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xff8 ; *rx_fixed_frac = 0x0f8 ; break; } /* TSC_SPEED_5G_XFI 20.625G VCO : 4 OS */

    case sc_x4_control_sc_S_20G_HG2_X1    : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x0af ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO : 1 OS */
    case sc_x4_control_sc_S_20G_HG2_CR1   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x0af ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO : 1 OS */
    case sc_x4_control_sc_S_20G_HG2_KR1   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x0af ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO : 1 OS */

    case sc_x4_control_sc_S_40G_HG2_X2    : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x0af ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO : 1 OS */
    case sc_x4_control_sc_S_40G_HG2_CR2   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x0af ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO : 1 OS */
    case sc_x4_control_sc_S_40G_HG2_KR2   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x0af ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO : 1 OS */


    case sc_x4_control_sc_S_10G_HG2_CR1   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x15f ; break; } /* TSC_SPEED_10P606G_XFI 21.875G VCO : 2 OS */
    case sc_x4_control_sc_S_10G_HG2_KR1   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x15f ; break; } /* TSC_SPEED_10P606G_XFI 21.875G VCO : 2 OS */
    case sc_x4_control_sc_S_10G_HG2_X1    : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x15f ; break; } /* TSC_SPEED_10P606G_XFI 21.875G VCO : 2 OS */

    case sc_x4_control_sc_S_20G_HG2_X2    : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x15f ; break; } /* TSC_SPEED_21G_HG2_MLD_DXGXS 21.875G VCO : 2 OS */

    case sc_x4_control_sc_S_40G_HG2_X4    : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x15f ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO : 2 OS */
    case sc_x4_control_sc_S_40G_HG2_CR4   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x15f ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO : 2 OS */
    case sc_x4_control_sc_S_40G_HG2_KR4   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x15f ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO : 2 OS */


    case sc_x4_control_sc_S_25G_X1        : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_CR1       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_KR1       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_CR_IEEE   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_25G_CR_IEEE 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_CRS_IEEE  : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_25G_CRS_IEEE 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_KR_IEEE   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_25G_KR_IEEE 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_KRS_IEEE  : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_25G_KRS_IEEE 25.78125G VCO : 1 OS */

    case sc_x4_control_sc_S_50G_X2        : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_50G_MLD_DXGXS 25.78125G VCO */
    case sc_x4_control_sc_S_50G_CR2       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_50G_MLD_DXGXS or TSC_SPEED_50G_CR2_MSA 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_50G_KR2       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_50G_MLD_DXGXS or TSC_SPEED_50G_KR2_MSA 25.78125G VCO : 1 OS */

    case sc_x4_control_sc_S_100G_X4       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_100G_CR4      : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_100G_KR4      : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x1cb ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO : 1 OS */


    case sc_x4_control_sc_S_50G_X4        : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x396 ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO : 2 OS */
    case sc_x4_control_sc_S_50G_CR4       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x396 ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO : 2 OS */
    case sc_x4_control_sc_S_50G_KR4       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffc ; *rx_fixed_frac = 0x396 ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO : 2 OS */

    case sc_x4_control_sc_S_2P5G_KX1       : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x199 ; break; } /* TSC_SPEED_2P5G 25.78125G VCO : 8.25 OS */


    case sc_x4_control_sc_S_25G_HG2_X1    : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x226 ; break; } /* TSC_SPEED_25G_XFI_HG2 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_HG2_CR1   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x226 ; break; } /* TSC_SPEED_25G_XFI_HG2 or TSC_SPEED_25G_CR1_HG2_MSA 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_HG2_KR1   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x226 ; break; } /* TSC_SPEED_25G_XFI_HG2 or TSC_SPEED_25G_KR1_HG2_MSA 27.34375G VCO : 1 OS */

    case sc_x4_control_sc_S_50G_HG2_X2    : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x226 ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_50G_HG2_CR2   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x226 ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS or TSC_SPEED_50G_CR2_MSA 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_50G_HG2_KR2   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x226 ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS or TSC_SPEED_50G_KR2_MSA 25.78125G VCO : 1 OS */

    case sc_x4_control_sc_S_100G_HG2_X4   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x226 ; break; } /* TSC_SPEED_106G_HG2_MLD 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_100G_HG2_CR4  : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x226 ; break; } /* TSC_SPEED_106G_HG2_MLD 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_100G_HG2_KR4  : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffe ; *rx_fixed_frac = 0x226 ; break; } /* TSC_SPEED_106G_HG2_MLD 27.34375G VCO : 1 OS */


    case sc_x4_control_sc_S_50G_HG2_X4    : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffd ; *rx_fixed_frac = 0x04c ; break; } /* TSC_SPEED_53G_HG2_MLD 27.34375G VCO : 2 OS */
    case sc_x4_control_sc_S_50G_HG2_CR4   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffd ; *rx_fixed_frac = 0x04c ; break; } /* TSC_SPEED_53G_HG2_MLD 27.34375G VCO : 2 OS */
    case sc_x4_control_sc_S_50G_HG2_KR4   : { *tx_fixed_int = 0x00 ; *tx_fixed_frac = 0x000 ;  *rx_fixed_int = 0xffd ; *rx_fixed_frac = 0x04c ; break; } /* TSC_SPEED_53G_HG2_MLD 27.34375G VCO : 2 OS */

    default                               : { return PHYMOD_E_FAIL ;                                                                            break; }
  }

  return PHYMOD_E_NONE;
}

int tefmod16_gen3_ui_values_get(int speed, uint16_t *ui_msb, uint16_t *ui_lsb)
{
  switch(speed) {
    case sc_x4_control_sc_S_20G_X1           : { *ui_msb = 0x0c69 ; *ui_lsb = 0x101 ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO : 1 OS */
    case sc_x4_control_sc_S_20G_CR1          : { *ui_msb = 0x0c69 ; *ui_lsb = 0x101 ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO : 1 OS */
    case sc_x4_control_sc_S_20G_KR1          : { *ui_msb = 0x0c69 ; *ui_lsb = 0x101 ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO : 1 OS */

    case sc_x4_control_sc_S_40G_X2           : { *ui_msb = 0x0c69 ; *ui_lsb = 0x101 ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO : 1 OS */
    case sc_x4_control_sc_S_40G_CR2          : { *ui_msb = 0x0c69 ; *ui_lsb = 0x101 ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO : 1 OS */
    case sc_x4_control_sc_S_40G_KR2          : { *ui_msb = 0x0c69 ; *ui_lsb = 0x101 ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO : 1 OS */


    case sc_x4_control_sc_S_10G_X1           : { *ui_msb = 0x18d3 ; *ui_lsb = 0x003 ; break; } /* TSC_SPEED_10G_XFI 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_10G_CR1          : { *ui_msb = 0x18d3 ; *ui_lsb = 0x003 ; break; } /* TSC_SPEED_10G_XFI 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_10G_KR1          : { *ui_msb = 0x18d3 ; *ui_lsb = 0x003 ; break; } /* TSC_SPEED_10G_XFI 20.625G VCO : 2 OS */

    case sc_x4_control_sc_S_20G_X2           : { *ui_msb = 0x18d3 ; *ui_lsb = 0x003 ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_20G_CR2          : { *ui_msb = 0x18d3 ; *ui_lsb = 0x003 ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_20G_KR2          : { *ui_msb = 0x18d3 ; *ui_lsb = 0x003 ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO : 2 OS */

    case sc_x4_control_sc_S_40G_X4           : { *ui_msb = 0x18d3 ; *ui_lsb = 0x003 ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_40G_CR4          : { *ui_msb = 0x18d3 ; *ui_lsb = 0x003 ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_40G_KR4          : { *ui_msb = 0x18d3 ; *ui_lsb = 0x003 ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO : 2 OS */


    case sc_x4_control_sc_S_5G_KR1           : { *ui_msb = 0x31a6 ; *ui_lsb = 0x006 ; break; } /* TSC_SPEED_5G_XFI 20.625G VCO : 4 OS */


    case sc_x4_control_sc_S_CL36_20GVCO      : { *ui_msb = 0xcccc ; *ui_lsb = 0x199 ; break; } /* TSC_SPEED_1G 20.625G VCO : 16.5 OS */
    case sc_x4_control_sc_S_CL73_20GVCO      : { *ui_msb = 0xcccc ; *ui_lsb = 0x199 ; break; } /* TSC_SPEED_1G_AN 20.625G VCO : 16.5 OS */


    case sc_x4_control_sc_S_20G_HG2_X1       : { *ui_msb = 0x0bb3 ; *ui_lsb = 0x1dc ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO : 1 OS */
    case sc_x4_control_sc_S_20G_HG2_CR1      : { *ui_msb = 0x0bb3 ; *ui_lsb = 0x1dc ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO : 1 OS */
    case sc_x4_control_sc_S_20G_HG2_KR1      : { *ui_msb = 0x0bb3 ; *ui_lsb = 0x1dc ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO : 1 OS */

    case sc_x4_control_sc_S_40G_HG2_X2       : { *ui_msb = 0x0bb3 ; *ui_lsb = 0x1dc ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO : 1 OS */
    case sc_x4_control_sc_S_40G_HG2_CR2      : { *ui_msb = 0x0bb3 ; *ui_lsb = 0x1dc ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO : 1 OS */
    case sc_x4_control_sc_S_40G_HG2_KR2      : { *ui_msb = 0x0bb3 ; *ui_lsb = 0x1dc ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO : 1 OS */


    case sc_x4_control_sc_S_10G_HG2_CR1      : { *ui_msb = 0x1767 ; *ui_lsb = 0x1b9 ; break; } /* TSC_SPEED_10P606G_XFI 21.875G VCO : 2 OS */
    case sc_x4_control_sc_S_10G_HG2_KR1      : { *ui_msb = 0x1767 ; *ui_lsb = 0x1b9 ; break; } /* TSC_SPEED_10P606G_XFI 21.875G VCO : 2 OS */
    case sc_x4_control_sc_S_10G_HG2_X1       : { *ui_msb = 0x1767 ; *ui_lsb = 0x1b9 ; break; } /* TSC_SPEED_10P606G_XFI 21.875G VCO : 2 OS */

    case sc_x4_control_sc_S_20G_HG2_X2       : { *ui_msb = 0x1767 ; *ui_lsb = 0x1b9 ; break; } /* TSC_SPEED_21G_HG2_MLD_DXGXS 21.875G VCO : 2 OS */

    case sc_x4_control_sc_S_40G_HG2_X4       : { *ui_msb = 0x1767 ; *ui_lsb = 0x1b9 ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO : 2 OS */
    case sc_x4_control_sc_S_40G_HG2_CR4      : { *ui_msb = 0x1767 ; *ui_lsb = 0x1b9 ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO : 2 OS */
    case sc_x4_control_sc_S_40G_HG2_KR4      : { *ui_msb = 0x1767 ; *ui_lsb = 0x1b9 ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO : 2 OS */


    case sc_x4_control_sc_S_25G_X1           : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_CR1          : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_KR1          : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_CR_IEEE      : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_25G_CR_IEEE 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_CRS_IEEE     : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_25G_CRS_IEEE 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_KR_IEEE      : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_25G_KR_IEEE 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_KRS_IEEE     : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_25G_KRS_IEEE 25.78125G VCO : 1 OS */

    case sc_x4_control_sc_S_50G_X2           : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_50G_MLD_DXGXS 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_50G_CR2          : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_50G_MLD_DXGXS or TSC_SPEED_50G_CR2_MSA 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_50G_KR2          : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_50G_MLD_DXGXS or TSC_SPEED_50G_KR2_MSA 25.78125G VCO : 1 OS */

    case sc_x4_control_sc_S_100G_X4          : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_100G_CR4         : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_100G_KR4         : { *ui_msb = 0x09ee ; *ui_lsb = 0x001 ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO : 1 OS */

    case sc_x4_control_sc_S_50G_X4           : { *ui_msb = 0x13dc ; *ui_lsb = 0x002 ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO : 2 OS */
    case sc_x4_control_sc_S_50G_CR4          : { *ui_msb = 0x13dc ; *ui_lsb = 0x002 ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO : 2 OS */
    case sc_x4_control_sc_S_50G_KR4          : { *ui_msb = 0x13dc ; *ui_lsb = 0x002 ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO : 2 OS */

    case sc_x4_control_sc_S_2P5G_KX1         : { *ui_msb = 0x51eb ; *ui_lsb = 0x10a ; break; } /* TSC_SPEED_2P5G 25.78125G VCO : 8.25 OS */


    case sc_x4_control_sc_S_CL36_25GVCO      : { *ui_msb = 0xcccc ; *ui_lsb = 0x199 ; break; } /* TSC_SPEED_1G 25.78125G VCO : 20.625 OS */
    case sc_x4_control_sc_S_CL73_25GVCO      : { *ui_msb = 0xcccc ; *ui_lsb = 0x199 ; break; } /* TSC_SPEED_1G_AN 25.78125G VCO : 20.625 OS */


    case sc_x4_control_sc_S_25G_HG2_X1       : { *ui_msb = 0x095c ; *ui_lsb = 0x17d ; break; } /* TSC_SPEED_25G_XFI_HG2 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_HG2_CR1      : { *ui_msb = 0x095c ; *ui_lsb = 0x17d ; break; } /* TSC_SPEED_25G_XFI_HG2 or TSC_SPEED_25G_CR1_HG2_MSA 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_HG2_KR1      : { *ui_msb = 0x095c ; *ui_lsb = 0x17d ; break; } /* TSC_SPEED_25G_XFI_HG2 or TSC_SPEED_25G_KR1_HG2_MSA 27.34375G VCO : 1 OS */

    case sc_x4_control_sc_S_50G_HG2_X2       : { *ui_msb = 0x095c ; *ui_lsb = 0x17d ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_50G_HG2_CR2      : { *ui_msb = 0x095c ; *ui_lsb = 0x17d ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS or TSC_SPEED_50G_CR2_MSA 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_50G_HG2_KR2      : { *ui_msb = 0x095c ; *ui_lsb = 0x17d ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS or TSC_SPEED_50G_KR2_MSA 27.34375G VCO : 1 OS */


    case sc_x4_control_sc_S_100G_HG2_X4      : { *ui_msb = 0x095c ; *ui_lsb = 0x17d ; break; } /* TSC_SPEED_106G_HG2_MLD 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_100G_HG2_CR4     : { *ui_msb = 0x095c ; *ui_lsb = 0x17d ; break; } /* TSC_SPEED_106G_HG2_MLD 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_100G_HG2_KR4     : { *ui_msb = 0x095c ; *ui_lsb = 0x17d ; break; } /* TSC_SPEED_106G_HG2_MLD 27.34375G VCO : 1 OS */

    case sc_x4_control_sc_S_50G_HG2_X4       : { *ui_msb = 0x12b9 ; *ui_lsb = 0x0fb ; break; } /* TSC_SPEED_53G_HG2_MLD 27.34375G VCO : 2 OS */
    case sc_x4_control_sc_S_50G_HG2_CR4      : { *ui_msb = 0x12b9 ; *ui_lsb = 0x0fb ; break; } /* TSC_SPEED_53G_HG2_MLD 27.34375G VCO : 2 OS */
    case sc_x4_control_sc_S_50G_HG2_KR4      : { *ui_msb = 0x12b9 ; *ui_lsb = 0x0fb ; break; } /* TSC_SPEED_53G_HG2_MLD 27.34375G VCO : 2 OS */

    default                                  : { return PHYMOD_E_FAIL ;               break; }
  }
  return PHYMOD_E_NONE;

}

int tefmod16_gen3_vl_bml_get(int speed, uint16_t *vl, uint16_t *bml)
{

  switch(speed) {
    case sc_x4_control_sc_S_20G_X2           : { *vl=2; *bml=1; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO */
    case sc_x4_control_sc_S_20G_CR2          : { *vl=2; *bml=1; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO */
    case sc_x4_control_sc_S_20G_KR2          : { *vl=2; *bml=1; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO */
    case sc_x4_control_sc_S_20G_HG2_X2       : { *vl=2; *bml=1; break; } /* TSC_SPEED_21G_HG2_MLD_DXGXS 21.875G VCO */
    case sc_x4_control_sc_S_40G_X2           : { *vl=4; *bml=2; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO */
    case sc_x4_control_sc_S_40G_CR2          : { *vl=4; *bml=2; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO */
    case sc_x4_control_sc_S_40G_KR2          : { *vl=4; *bml=2; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO */
    case sc_x4_control_sc_S_40G_HG2_X2       : { *vl=4; *bml=2; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO */
    case sc_x4_control_sc_S_40G_HG2_CR2      : { *vl=4; *bml=2; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO */
    case sc_x4_control_sc_S_40G_HG2_KR2      : { *vl=4; *bml=2; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO */
    case sc_x4_control_sc_S_50G_X2           : { *vl=4; *bml=2; break; } /* TSC_SPEED_50G_MLD_DXGXS 25.78125G VCO */
    case sc_x4_control_sc_S_50G_CR2          : { *vl=4; *bml=2; break; } /* TSC_SPEED_50G_MLD_DXGXS or TSC_SPEED_50G_CR2_MSA 25.78125G VCO */
    case sc_x4_control_sc_S_50G_KR2          : { *vl=4; *bml=2; break; } /* TSC_SPEED_50G_MLD_DXGXS or TSC_SPEED_50G_KR2_MSA 25.78125G VCO */
    case sc_x4_control_sc_S_50G_HG2_X2       : { *vl=4; *bml=2; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS 27.34375G VCO */
    case sc_x4_control_sc_S_50G_HG2_CR2      : { *vl=4; *bml=2; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS or TSC_SPEED_50G_CR2_MSA 25.78125G VCO */
    case sc_x4_control_sc_S_50G_HG2_KR2      : { *vl=4; *bml=2; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS or TSC_SPEED_50G_KR2_MSA 25.78125G VCO */
    case sc_x4_control_sc_S_40G_X4           : { *vl=4; *bml=1; break; } /* TSC_SPEED_40G_MLD 20.625G VCO */
    case sc_x4_control_sc_S_40G_CR4          : { *vl=4; *bml=1; break; } /* TSC_SPEED_40G_MLD 20.625G VCO */
    case sc_x4_control_sc_S_40G_KR4          : { *vl=4; *bml=1; break; } /* TSC_SPEED_40G_MLD 20.625G VCO */
    case sc_x4_control_sc_S_40G_HG2_X4       : { *vl=4; *bml=1; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO */
    case sc_x4_control_sc_S_40G_HG2_CR4      : { *vl=4; *bml=1; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO */
    case sc_x4_control_sc_S_40G_HG2_KR4      : { *vl=4; *bml=1; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO */
    case sc_x4_control_sc_S_50G_X4           : { *vl=4; *bml=1; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO */
    case sc_x4_control_sc_S_50G_CR4          : { *vl=4; *bml=1; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO */
    case sc_x4_control_sc_S_50G_KR4          : { *vl=4; *bml=1; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO */
    case sc_x4_control_sc_S_50G_HG2_X4       : { *vl=4; *bml=1; break; } /* TSC_SPEED_53G_HG2_MLD 27.343755G VCO */
    case sc_x4_control_sc_S_50G_HG2_CR4      : { *vl=4; *bml=1; break; } /* TSC_SPEED_53G_HG2_MLD 27.343755G VCO */
    case sc_x4_control_sc_S_50G_HG2_KR4      : { *vl=4; *bml=1; break; } /* TSC_SPEED_53G_HG2_MLD 27.343755G VCO */
    case sc_x4_control_sc_S_100G_X4          : { *vl=20; *bml=5; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO */
    case sc_x4_control_sc_S_100G_CR4         : { *vl=20; *bml=5; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO */
    case sc_x4_control_sc_S_100G_KR4         : { *vl=20; *bml=5; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO */
    case sc_x4_control_sc_S_100G_HG2_X4      : { *vl=20; *bml=5; break; } /* TSC_SPEED_106G_HG2_MLD 27.343755G VCO */
    case sc_x4_control_sc_S_100G_HG2_CR4     : { *vl=20; *bml=5; break; } /* TSC_SPEED_106G_HG2_MLD 27.343755G VCO */
    case sc_x4_control_sc_S_100G_HG2_KR4     : { *vl=20; *bml=5; break; } /* TSC_SPEED_106G_HG2_MLD 27.343755G VCO */
    default                                  : { return PHYMOD_E_FAIL  ; break; }
  }

  return PHYMOD_E_NONE;
}

int tefmod16_gen3_mapped_frac_ns_get(int speed, uint16_t *frac_ns)
{
  switch(speed) {
     case sc_x4_control_sc_S_CL36_20GVCO      : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_1G 20.625G VCO */
     case sc_x4_control_sc_S_CL73_20GVCO      : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_1G_AN 20.625G VCO */

     case sc_x4_control_sc_S_5G_KR1           : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_5G_XFI 20.625G VCO */

     case sc_x4_control_sc_S_10G_X1           : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_10G_XFI 25.78125G VCO */
     case sc_x4_control_sc_S_10G_CR1          : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_10G_XFI 25.78125G VCO */
     case sc_x4_control_sc_S_10G_KR1          : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_10G_XFI 25.78125G VCO */

     case sc_x4_control_sc_S_20G_X1           : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO */
     case sc_x4_control_sc_S_20G_CR1          : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO */
     case sc_x4_control_sc_S_20G_KR1          : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO */

     case sc_x4_control_sc_S_20G_X2           : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO */
     case sc_x4_control_sc_S_20G_CR2          : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO */
     case sc_x4_control_sc_S_20G_KR2          : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO */

     case sc_x4_control_sc_S_40G_X2           : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO */
     case sc_x4_control_sc_S_40G_CR2          : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO */
     case sc_x4_control_sc_S_40G_KR2          : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO */

     case sc_x4_control_sc_S_40G_X4           : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO */
     case sc_x4_control_sc_S_40G_CR4          : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO */
     case sc_x4_control_sc_S_40G_KR4          : { *frac_ns =  0x634c ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO */


     case sc_x4_control_sc_S_10G_HG2_CR1      : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_10P606G_XFI 27.34375G VCO */
     case sc_x4_control_sc_S_10G_HG2_KR1      : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_10P606G_XFI 27.34375G VCO */
     case sc_x4_control_sc_S_10G_HG2_X1       : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_10P606G_XFI 27.34375G VCO */

     case sc_x4_control_sc_S_20G_HG2_X1       : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO */
     case sc_x4_control_sc_S_20G_HG2_CR1      : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO */
     case sc_x4_control_sc_S_20G_HG2_KR1      : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO */

     case sc_x4_control_sc_S_20G_HG2_X2       : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_21G_HG2_MLD_DXGXS 21.875G VCO */

     case sc_x4_control_sc_S_40G_HG2_X2       : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO */
     case sc_x4_control_sc_S_40G_HG2_CR2      : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO */
     case sc_x4_control_sc_S_40G_HG2_KR2      : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO */

     case sc_x4_control_sc_S_40G_HG2_X4       : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO */
     case sc_x4_control_sc_S_40G_HG2_CR4      : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO */
     case sc_x4_control_sc_S_40G_HG2_KR4      : { *frac_ns =  0x5d9f ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO */


     case sc_x4_control_sc_S_CL36_25GVCO      : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_1G 25.78125G VCO */
     case sc_x4_control_sc_S_CL73_25GVCO      : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_1G_AN 25.78125G VCO */

     case sc_x4_control_sc_S_2P5G_KX1          : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_2P5G 25.78125G VCO */

     case sc_x4_control_sc_S_25G_X1           : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO */
     case sc_x4_control_sc_S_25G_CR1          : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO */
     case sc_x4_control_sc_S_25G_KR1          : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO */
     case sc_x4_control_sc_S_25G_CR_IEEE      : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_25G_CR_IEEE 25.78125G VCO */
     case sc_x4_control_sc_S_25G_CRS_IEEE     : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_25G_CRS_IEEE 25.78125G VCO */
     case sc_x4_control_sc_S_25G_KR_IEEE      : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_25G_KR_IEEE 25.78125G VCO */
     case sc_x4_control_sc_S_25G_KRS_IEEE     : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_25G_KRS_IEEE 25.78125G VCO */

     case sc_x4_control_sc_S_50G_X2           : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_50G_MLD_DXGXS 25.78125G VCO */
     case sc_x4_control_sc_S_50G_CR2          : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_50G_MLD_DXGXS or TSC_SPEED_50G_CR2_MSA 25.78125G VCO */
     case sc_x4_control_sc_S_50G_KR2          : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_50G_MLD_DXGXS or TSC_SPEED_50G_KR2_MSA 25.78125G VCO */

     case sc_x4_control_sc_S_50G_X4           : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO */
     case sc_x4_control_sc_S_50G_CR4          : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO */
     case sc_x4_control_sc_S_50G_KR4          : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO */

     case sc_x4_control_sc_S_100G_X4          : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO */
     case sc_x4_control_sc_S_100G_CR4         : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO */
     case sc_x4_control_sc_S_100G_KR4         : { *frac_ns =  0x4f70 ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO */


     case sc_x4_control_sc_S_25G_HG2_X1       : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_25G_XFI_HG2 27.34375G VCO */
     case sc_x4_control_sc_S_25G_HG2_CR1      : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_25G_XFI_HG2 or TSC_SPEED_25G_CR1_HG2_MSA 27.34375G VCO */
     case sc_x4_control_sc_S_25G_HG2_KR1      : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_25G_XFI_HG2 or TSC_SPEED_25G_KR1_HG2_MSA 27.34375G VCO */

     case sc_x4_control_sc_S_50G_HG2_X2       : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS 27.34375G VCO */
     case sc_x4_control_sc_S_50G_HG2_CR2      : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS or TSC_SPEED_50G_CR2_MSA 27.34375G VCO */
     case sc_x4_control_sc_S_50G_HG2_KR2      : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS or TSC_SPEED_50G_KR2_MSA 27.34375G VCO */

     case sc_x4_control_sc_S_50G_HG2_X4       : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_53G_HG2_MLD 27.34375G VCO */
     case sc_x4_control_sc_S_50G_HG2_CR4      : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_53G_HG2_MLD 27.34375G VCO */
     case sc_x4_control_sc_S_50G_HG2_KR4      : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_53G_HG2_MLD 27.34375G VCO */

     case sc_x4_control_sc_S_100G_HG2_X4      : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_106G_HG2_MLD 27.34375G VCO */
     case sc_x4_control_sc_S_100G_HG2_CR4     : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_106G_HG2_MLD 27.34375G VCO */
     case sc_x4_control_sc_S_100G_HG2_KR4     : { *frac_ns =  0x4ae5 ; break; } /* TSC_SPEED_106G_HG2_MLD 27.34375G VCO */

     default                                  : { return PHYMOD_E_FAIL;break; }
  }

  return PHYMOD_E_NONE;
}

int tefmod16_gen3_txrx_fixed_latency_get(int speed, uint16_t *tx_fixed_int, uint16_t *tx_fixed_frac, uint16_t *rx_fixed_int, uint16_t *rx_fixed_frac)
{
  switch(speed) {
    case sc_x4_control_sc_S_CL36_20GVCO      : { *tx_fixed_int = 0x08 ; *tx_fixed_frac = 0x34a ;  *rx_fixed_int = 0xfee ; *rx_fixed_frac = 0x0f7 ; break; } /* TSC_SPEED_1G 20.625G VCO : 16.5 OS */
    case sc_x4_control_sc_S_CL73_20GVCO      : { *tx_fixed_int = 0x08 ; *tx_fixed_frac = 0x34a ;  *rx_fixed_int = 0xfee ; *rx_fixed_frac = 0x0f7 ; break; } /* TSC_SPEED_1G_AN 20.625G VCO : 16.5 OS */

    case sc_x4_control_sc_S_CL36_25GVCO      : { *tx_fixed_int = 0x07 ; *tx_fixed_frac = 0x16f ;  *rx_fixed_int = 0xff0 ; *rx_fixed_frac = 0x0d0 ; break; } /* TSC_SPEED_1G 25.78125G VCO : 20.625 OS */
    case sc_x4_control_sc_S_CL73_25GVCO      : { *tx_fixed_int = 0x07 ; *tx_fixed_frac = 0x16f ;  *rx_fixed_int = 0xff0 ; *rx_fixed_frac = 0x0d0 ; break; } /* TSC_SPEED_1G_AN 25.78125G VCO : 20.625 OS */


    case sc_x4_control_sc_S_20G_X1           : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x198 ;  *rx_fixed_int = 0xff9 ; *rx_fixed_frac = 0x08d ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO : 1 OS */
    case sc_x4_control_sc_S_20G_CR1          : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x198 ;  *rx_fixed_int = 0xff9 ; *rx_fixed_frac = 0x08d ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO : 1 OS */
    case sc_x4_control_sc_S_20G_KR1          : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x198 ;  *rx_fixed_int = 0xff9 ; *rx_fixed_frac = 0x08d ; break; } /* TSC_SPEED_20G_XFI 20.625G VCO : 1 OS */

    case sc_x4_control_sc_S_40G_X2           : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x198 ;  *rx_fixed_int = 0xff9 ; *rx_fixed_frac = 0x08d ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO : 1 OS */
    case sc_x4_control_sc_S_40G_CR2          : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x198 ;  *rx_fixed_int = 0xff9 ; *rx_fixed_frac = 0x08d ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO : 1 OS */
    case sc_x4_control_sc_S_40G_KR2          : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x198 ;  *rx_fixed_int = 0xff9 ; *rx_fixed_frac = 0x08d ; break; } /* TSC_SPEED_40G_MLD_DXGXS 20.625G VCO : 1 OS */


    case sc_x4_control_sc_S_20G_X2           : { *tx_fixed_int = 0x0c ; *tx_fixed_frac = 0x2ce ;  *rx_fixed_int = 0xff2 ; *rx_fixed_frac = 0x11c ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_20G_CR2          : { *tx_fixed_int = 0x0c ; *tx_fixed_frac = 0x2ce ;  *rx_fixed_int = 0xff2 ; *rx_fixed_frac = 0x11c ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_20G_KR2          : { *tx_fixed_int = 0x0c ; *tx_fixed_frac = 0x2ce ;  *rx_fixed_int = 0xff2 ; *rx_fixed_frac = 0x11c ; break; } /* TSC_SPEED_20G_MLD_DXGXS 20.625G VCO : 2 OS */

    case sc_x4_control_sc_S_40G_X4           : { *tx_fixed_int = 0x0c ; *tx_fixed_frac = 0x2ce ;  *rx_fixed_int = 0xff2 ; *rx_fixed_frac = 0x11c ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_40G_CR4          : { *tx_fixed_int = 0x0c ; *tx_fixed_frac = 0x2ce ;  *rx_fixed_int = 0xff2 ; *rx_fixed_frac = 0x11c ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO : 2 OS */
    case sc_x4_control_sc_S_40G_KR4          : { *tx_fixed_int = 0x0c ; *tx_fixed_frac = 0x2ce ;  *rx_fixed_int = 0xff2 ; *rx_fixed_frac = 0x11c ; break; } /* TSC_SPEED_40G_MLD 20.625G VCO : 2 OS */


    case sc_x4_control_sc_S_5G_KR1           : { *tx_fixed_int = 0x19 ; *tx_fixed_frac = 0x19e ;  *rx_fixed_int = 0xfe4 ; *rx_fixed_frac = 0x239 ; break; } /* TSC_SPEED_5G_XFI 20.625G VCO : 4 OS */


    case sc_x4_control_sc_S_20G_HG2_X1       : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x022 ;  *rx_fixed_int = 0xff9 ; *rx_fixed_frac = 0x21e ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO : 1 OS */
    case sc_x4_control_sc_S_20G_HG2_CR1      : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x022 ;  *rx_fixed_int = 0xff9 ; *rx_fixed_frac = 0x21e ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO : 1 OS */
    case sc_x4_control_sc_S_20G_HG2_KR1      : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x022 ;  *rx_fixed_int = 0xff9 ; *rx_fixed_frac = 0x21e ; break; } /* TSC_SPEED_20G_XFI_HG2 21.875G VCO : 1 OS */

    case sc_x4_control_sc_S_40G_HG2_X2       : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x022 ;  *rx_fixed_int = 0xff9 ; *rx_fixed_frac = 0x21e ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO : 1 OS */
    case sc_x4_control_sc_S_40G_HG2_CR2      : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x022 ;  *rx_fixed_int = 0xff9 ; *rx_fixed_frac = 0x21e ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO : 1 OS */
    case sc_x4_control_sc_S_40G_HG2_KR2      : { *tx_fixed_int = 0x06 ; *tx_fixed_frac = 0x022 ;  *rx_fixed_int = 0xff9 ; *rx_fixed_frac = 0x21e ; break; } /* TSC_SPEED_42G_HG2_MLD_DXGXS 21.875G VCO : 1 OS */


    case sc_x4_control_sc_S_20G_HG2_X2       : { *tx_fixed_int = 0x0b ; *tx_fixed_frac = 0x3e7 ;  *rx_fixed_int = 0xff3 ; *rx_fixed_frac = 0x03f ; break; } /* TSC_SPEED_21G_HG2_MLD_DXGXS 21.875G VCO : 2 OS */

    case sc_x4_control_sc_S_40G_HG2_X4       : { *tx_fixed_int = 0x0b ; *tx_fixed_frac = 0x3e7 ;  *rx_fixed_int = 0xff3 ; *rx_fixed_frac = 0x03f ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO : 2 OS */
    case sc_x4_control_sc_S_40G_HG2_CR4      : { *tx_fixed_int = 0x0b ; *tx_fixed_frac = 0x3e7 ;  *rx_fixed_int = 0xff3 ; *rx_fixed_frac = 0x03f ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO : 2 OS */
    case sc_x4_control_sc_S_40G_HG2_KR4      : { *tx_fixed_int = 0x0b ; *tx_fixed_frac = 0x3e7 ;  *rx_fixed_int = 0xff3 ; *rx_fixed_frac = 0x03f ; break; } /* TSC_SPEED_42G_HG2_MLD 21.875G VCO : 2 OS */


    case sc_x4_control_sc_S_25G_X1           : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_CR1          : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_KR1          : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_25G_XFI 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_CR_IEEE      : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_25G_CR_IEEE 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_CRS_IEEE     : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_25G_CRS_IEEE 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_KR_IEEE      : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_25G_KR_IEEE 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_KRS_IEEE     : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_25G_KRS_IEEE 25.78125G VCO : 1 OS */

    case sc_x4_control_sc_S_50G_X2           : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_50G_MLD_DXGXS 25.78125G VCO */
    case sc_x4_control_sc_S_50G_CR2          : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_50G_MLD_DXGXS or TSC_SPEED_50G_CR2_MSA 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_50G_KR2          : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_50G_MLD_DXGXS or TSC_SPEED_50G_KR2_MSA 25.78125G VCO : 1 OS */

    case sc_x4_control_sc_S_100G_X4          : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_100G_CR4         : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_100G_KR4         : { *tx_fixed_int = 0x05 ; *tx_fixed_frac = 0x07a ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x20a ; break; } /* TSC_SPEED_100G_MLD 25.78125G VCO : 1 OS */


    case sc_x4_control_sc_S_50G_X4           : { *tx_fixed_int = 0x0a ; *tx_fixed_frac = 0x0a5 ;  *rx_fixed_int = 0xff5 ; *rx_fixed_frac = 0x017 ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO : 2 OS */
    case sc_x4_control_sc_S_50G_CR4          : { *tx_fixed_int = 0x0a ; *tx_fixed_frac = 0x0a5 ;  *rx_fixed_int = 0xff5 ; *rx_fixed_frac = 0x017 ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO : 2 OS */
    case sc_x4_control_sc_S_50G_KR4          : { *tx_fixed_int = 0x0a ; *tx_fixed_frac = 0x0a5 ;  *rx_fixed_int = 0xff5 ; *rx_fixed_frac = 0x017 ; break; } /* TSC_SPEED_50G_MLD 25.78125G VCO : 2 OS */


    case sc_x4_control_sc_S_10G_X1           : { *tx_fixed_int = 0x0c ; *tx_fixed_frac = 0x208 ;  *rx_fixed_int = 0xff2 ; *rx_fixed_frac = 0x11c ; break; } /* TSC_SPEED_10G_XFI 25.78125G VCO : 2.5 OS */
    case sc_x4_control_sc_S_10G_CR1          : { *tx_fixed_int = 0x0c ; *tx_fixed_frac = 0x208 ;  *rx_fixed_int = 0xff2 ; *rx_fixed_frac = 0x11c ; break; } /* TSC_SPEED_10G_XFI 25.78125G VCO : 2.5 OS */
    case sc_x4_control_sc_S_10G_KR1          : { *tx_fixed_int = 0x0c ; *tx_fixed_frac = 0x208 ;  *rx_fixed_int = 0xff2 ; *rx_fixed_frac = 0x11c ; break; } /* TSC_SPEED_10G_XFI 25.78125G VCO : 2.5 OS */

    case sc_x4_control_sc_S_2P5G_KX1          : { *tx_fixed_int = 0x07 ; *tx_fixed_frac = 0x102 ;  *rx_fixed_int = 0xff6 ; *rx_fixed_frac = 0x2ad ; break; } /* TSC_SPEED_2P5G 25.78125G VCO : 8.25 OS */


    case sc_x4_control_sc_S_25G_HG2_X1       : { *tx_fixed_int = 0x04 ; *tx_fixed_frac = 0x34e ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x34b ; break; } /* TSC_SPEED_25G_XFI_HG2 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_HG2_CR1      : { *tx_fixed_int = 0x04 ; *tx_fixed_frac = 0x34e ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x34b ; break; } /* TSC_SPEED_25G_XFI_HG2 or TSC_SPEED_25G_CR1_HG2_MSA 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_25G_HG2_KR1      : { *tx_fixed_int = 0x04 ; *tx_fixed_frac = 0x34e ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x34b ; break; } /* TSC_SPEED_25G_XFI_HG2 or TSC_SPEED_25G_KR1_HG2_MSA 27.34375G VCO : 1 OS */

    case sc_x4_control_sc_S_50G_HG2_X2       : { *tx_fixed_int = 0x04 ; *tx_fixed_frac = 0x34e ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x34b ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_50G_HG2_CR2      : { *tx_fixed_int = 0x04 ; *tx_fixed_frac = 0x34e ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x34b ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS or TSC_SPEED_50G_CR2_MSA 25.78125G VCO : 1 OS */
    case sc_x4_control_sc_S_50G_HG2_KR2      : { *tx_fixed_int = 0x04 ; *tx_fixed_frac = 0x34e ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x34b ; break; } /* TSC_SPEED_53G_HG2_MLD_DXGXS or TSC_SPEED_50G_KR2_MSA 25.78125G VCO : 1 OS */

    case sc_x4_control_sc_S_100G_HG2_X4      : { *tx_fixed_int = 0x04 ; *tx_fixed_frac = 0x34e ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x34b ; break; } /* TSC_SPEED_106G_HG2_MLD 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_100G_HG2_CR4     : { *tx_fixed_int = 0x04 ; *tx_fixed_frac = 0x34e ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x34b ; break; } /* TSC_SPEED_106G_HG2_MLD 27.34375G VCO : 1 OS */
    case sc_x4_control_sc_S_100G_HG2_KR4     : { *tx_fixed_int = 0x04 ; *tx_fixed_frac = 0x34e ;  *rx_fixed_int = 0xffa ; *rx_fixed_frac = 0x34b ; break; } /* TSC_SPEED_106G_HG2_MLD 27.34375G VCO : 1 OS */


    case sc_x4_control_sc_S_50G_HG2_X4       : { *tx_fixed_int = 0x09 ; *tx_fixed_frac = 0x252 ;  *rx_fixed_int = 0xff5 ; *rx_fixed_frac = 0x298 ; break; } /* TSC_SPEED_53G_HG2_MLD 27.34375G VCO : 2 OS */
    case sc_x4_control_sc_S_50G_HG2_CR4      : { *tx_fixed_int = 0x09 ; *tx_fixed_frac = 0x252 ;  *rx_fixed_int = 0xff5 ; *rx_fixed_frac = 0x298 ; break; } /* TSC_SPEED_53G_HG2_MLD 27.34375G VCO : 2 OS */
    case sc_x4_control_sc_S_50G_HG2_KR4      : { *tx_fixed_int = 0x09 ; *tx_fixed_frac = 0x252 ;  *rx_fixed_int = 0xff5 ; *rx_fixed_frac = 0x298 ; break; } /* TSC_SPEED_53G_HG2_MLD 27.34375G VCO : 2 OS */


    case sc_x4_control_sc_S_10G_HG2_CR1      : { *tx_fixed_int = 0x0b ; *tx_fixed_frac = 0x32c ;  *rx_fixed_int = 0xff3 ; *rx_fixed_frac = 0x03f ; break; } /* TSC_SPEED_10P606G_XFI 27.34375G VCO : 2.5 OS */
    case sc_x4_control_sc_S_10G_HG2_KR1      : { *tx_fixed_int = 0x0b ; *tx_fixed_frac = 0x32c ;  *rx_fixed_int = 0xff3 ; *rx_fixed_frac = 0x03f ; break; } /* TSC_SPEED_10P606G_XFI 27.34375G VCO : 2.5 OS */
    case sc_x4_control_sc_S_10G_HG2_X1       : { *tx_fixed_int = 0x0b ; *tx_fixed_frac = 0x32c ;  *rx_fixed_int = 0xff3 ; *rx_fixed_frac = 0x03f ; break; } /* TSC_SPEED_10P606G_XFI 27.34375G VCO : 2.5 OS */

    default                                  : { return PHYMOD_E_FAIL  ;                                                                           break; }
  }

  return PHYMOD_E_NONE;
}
