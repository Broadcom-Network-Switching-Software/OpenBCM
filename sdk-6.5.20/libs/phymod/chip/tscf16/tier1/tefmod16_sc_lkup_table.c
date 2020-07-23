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
 *  Description: define enumerators  
 *----------------------------------------------------------------------*/
#ifndef _SDK_TEFMOD16_
#ifndef _DV_TB_
 #define _SDK_TEFMOD16_ 1
#endif
#endif
 
#ifdef _SDK_TEFMOD16_
#include "tefmod16_enum_defines.h"
#include "tefmod16.h"
#include <phymod/phymod.h>
#endif

#ifdef _SDK_TEFMOD16_
#define PHYMOD_ST const phymod_access_t
#else
#define PHYMOD_ST tefmod16_st
#endif

#include "tefmod16_sc_defines.h"
#include "tf16PCSRegEnums.h"

#include "tefmod16_sc_lkup_table.h"

const sc_pmd16_entry_st sc_pmd16_entry[] = {
  /*`SPEED_10G_CR1: 0*/             { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_132},
  /*`SPEED_10G_KR1: 1*/             { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_132},
  /*`SPEED_10G_X1: 2*/              { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_132},
  /*`NULL ENTRY: 3*/                {         0, 0},
  /*`SPEED_10G_HG2_CR1: 4*/         { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_140},
  /*`SPEED_10G_HG2_KR1: 5*/         { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_140},
  /*`SPEED_10G_HG2_X1: 6*/          { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_140},
  /*`NULL ENTRY: 7*/                {         0, 0},
  /*`SPEED_20G_CR1: 8*/             { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_132},
  /*`SPEED_20G_KR1: 9 */            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_132},
  /*`SPEED_20G_X1: 10*/             { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_132},
  /*`NULL ENTRY: 11*/               {         0, 0},
  /*`SPEED_20G_HG2_CR1: 12*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_140},
  /*`SPEED_20G_HG2_KR1: 13*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_140},
  /*`SPEED_20G_HG2_X1: 14*/         { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_140},
  /*`NULL ENTRY: 15*/               {         0, 0},
  /*`SPEED_25G_CR1: 16*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_25G_KR1:  17*/           { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_25G_X1: 18*/             { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 19*/               {         0, 0},
  /*`SPEED_25G_HG2_CR1: 20*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_25G_HG2_KR1: 21*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_25G_HG2_X1: 22*/         { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 23*/               {         0, 0},
  /*`SPEED_20G_CR2:  24*/           { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_132},
  /*`SPEED_20G_KR2: 25*/            { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_132},
  /*`SPEED_20G_X2: 26*/             { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_132},
  /*`NULL ENTRY: 27*/               {         0, 0},
  /*`SPEED_20G_HG2_CR2: 28*/        { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_140},
  /*`SPEED_20G_HG2_KR2: 29*/        { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_140},
  /*`SPEED_20G_HG2_X2: 30*/         { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_140},
  /*`NULL ENTRY: 31*/               {         0, 0},
  /*`SPEED_40G_CR2: 32*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_132},
  /*`SPEED_40G_KR2: 33*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_132},
  /*`SPEED_40G_X2: 34*/             { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_132},
  /*`NULL ENTRY: 35*/               {         0, 0},
  /*`SPEED_40G_HG2_CR2: 36*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_140},
  /*`SPEED_40G_HG2_KR2: 37*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_140},
  /*`SPEED_40G_HG2_X2: 38*/         { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_140},
  /*`NULL ENTRY: 39*/               {         0, 0},
  /*`SPEED_40G_CR4:  40*/           { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_132},
  /*`SPEED_40G_KR4:  41*/           { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_132},
  /*`SPEED_40G_X4: 42*/             { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_132},
  /*`NULL ENTRY: 43*/               {         0, 0},
  /*`SPEED_40G_HG2_CR4: 44*/        { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_140},
  /*`SPEED_40G_HG2_KR4: 45*/        { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_140},
  /*`SPEED_40G_HG2_X4: 46*/         { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_140},
  /*`NULL ENTRY: 47*/               {         0, 0},
  /*`SPEED_50G_CR2: 48*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_50G_KR2: 49*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_50G_X2: 50*/             { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 51*/               {         0, 0},
  /*`SPEED_50G_HG2_CR2: 52*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_50G_HG2_KR2: 53*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_50G_HG2_X2: 54*/         { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 55*/               {         0, 0},
  /*`SPEED_50G_CR4: 56*/            { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_50G_KR4:  57*/           { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_50G_X4: 58*/             { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 59*/               {         0, 0},
  /*`SPEED_50G_HG2_CR4: 60*/        { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_50G_HG2_KR4: 61*/        { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_50G_HG2_X4: 62*/         { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 63*/               {         0, 0},
  /*`SPEED_100G_CR4: 64*/           { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_100G_KR4: 65*/           { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_100G_X4: 66*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 67*/               {         0, 0},
  /*`SPEED_100G_HG2_CR4: 68*/       { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_100G_HG2_KR4: 69*/       { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_100G_HG2_X4: 70*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 71*/               {         0, 0},
  /*`SPEED_CL73_20GVCO: 72*/        { OS_MODE_16p5, TEFMOD16_PLL_MODE_DIV_140},
  /*`NULL ENTRY: 73*/               {         0, 0},
  /*`NULL ENTRY: 74*/               {         0, 0},
  /*`NULL ENTRY: 75*/               {         0, 0},
  /*`NULL ENTRY: 76*/               {         0, 0},
  /*`NULL ENTRY: 77*/               {         0, 0},
  /*`NULL ENTRY: 78*/               {         0, 0},
  /*`NULL ENTRY: 79*/               {         0, 0},
  /*`SPEED_CL73_25GVCO: 80*/        { OS_MODE_20p625, TEFMOD16_PLL_MODE_DIV_140},
  /*`NULL ENTRY: 81*/               {         0, 0},
  /*`NULL ENTRY: 82*/               {         0, 0},
  /*`NULL ENTRY: 83*/               {         0, 0},
  /*`NULL ENTRY: 84*/               {         0, 0},
  /*`NULL ENTRY: 84*/               {         0, 0},
  /*`NULL ENTRY: 86*/               {         0, 0},
  /*`NULL ENTRY: 87*/               {         0, 0},
  /*`SPEED_1G_20GVCO: 88*/          { OS_MODE_16p5, TEFMOD16_PLL_MODE_DIV_132},
  /*`NULL ENTRY: 89*/               {         0, 0},
  /*`NULL ENTRY: 90*/               {         0, 0},
  /*`NULL ENTRY: 91*/               {         0, 0},
  /*`NULL ENTRY: 92*/               {         0, 0},
  /*`NULL ENTRY: 93*/               {         0, 0},
  /*`NULL ENTRY: 94*/               {         0, 0},
  /*`NULL ENTRY: 95*/               {         0, 0},
  /*`SPEED_1G_25GVCO: 96*/          { OS_MODE_20p625, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 97*/               {         0, 0},
  /*`NULL ENTRY: 98*/               {         0, 0},
  /*`NULL ENTRY: 99*/               {         0, 0},
  /*`NULL ENTRY: 100*/              {         0, 0},
  /*`NULL ENTRY: 101*/              {         0, 0},
  /*`NULL ENTRY: 102*/              {         0, 0},
  /*`NULL ENTRY: 103*/              {         0, 0},
  /*`NULL ENTRY: 104*/              {         0, 0},
  /*`NULL ENTRY: 105*/              {         0, 0},
  /*`NULL ENTRY: 106*/              {         0, 0},
  /*`NULL ENTRY: 107*/              {         0, 0},
  /*`NULL ENTRY: 108*/              {         0, 0},
  /*`NULL ENTRY: 109*/              {         0, 0},
  /*`NULL ENTRY: 110*/              {         0, 0},
  /*`NULL ENTRY: 111*/              {         0, 0},
  /*`SPEED_25G_CR: 112*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_25G_CRS: 113*/           { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_25G_KR: 114*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_25G_KRS: 115*/           { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
        
};

const sc_pmd16_entry_st sc_pmd16_entry_125M_ref[] = {
  /*`SPEED_10G_CR1: 0*/             { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_10G_KR1: 1*/             { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_10G_X1: 2*/              { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 3*/                {         0, 0},
  /*`SPEED_10G_HG2_CR1: 4*/         { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_10G_HG2_KR1: 5*/         { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_10G_HG2_X1: 6*/          { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 7*/                {         0, 0},
  /*`SPEED_20G_CR1: 8*/             { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_20G_KR1: 9 */            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_20G_X1: 10*/             { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 11*/               {         0, 0},
  /*`SPEED_20G_HG2_CR1: 12*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_20G_HG2_KR1: 13*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_20G_HG2_X1: 14*/         { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 15*/               {         0, 0},
  /*`SPEED_25G_CR1: 16*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_25G_KR1:  17*/           { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_25G_X1: 18*/             { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 19*/               {         0, 0},
  /*`SPEED_25G_HG2_CR1: 20*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_25G_HG2_KR1: 21*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_25G_HG2_X1: 22*/         { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 23*/               {         0, 0},
  /*`SPEED_20G_CR2:  24*/           { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_20G_KR2: 25*/            { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_20G_X2: 26*/             { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 27*/               {         0, 0},
  /*`SPEED_20G_HG2_CR2: 28*/        { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_20G_HG2_KR2: 29*/        { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_20G_HG2_X2: 30*/         { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 31*/               {         0, 0},
  /*`SPEED_40G_CR2: 32*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_40G_KR2: 33*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_40G_X2: 34*/             { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 35*/               {         0, 0},
  /*`SPEED_40G_HG2_CR2: 36*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_40G_HG2_KR2: 37*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_40G_HG2_X2: 38*/         { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 39*/               {         0, 0},
  /*`SPEED_40G_CR4:  40*/           { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_40G_KR4:  41*/           { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_40G_X4: 42*/             { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 43*/               {         0, 0},
  /*`SPEED_40G_HG2_CR4: 44*/        { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_40G_HG2_KR4: 45*/        { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_40G_HG2_X4: 46*/         { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 47*/               {         0, 0},
  /*`SPEED_50G_CR2: 48*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_50G_KR2: 49*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_50G_X2: 50*/             { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 51*/               {         0, 0},
  /*`SPEED_50G_HG2_CR2: 52*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_50G_HG2_KR2: 53*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_50G_HG2_X2: 54*/         { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 55*/               {         0, 0},
  /*`SPEED_50G_CR4: 56*/            { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_50G_KR4:  57*/           { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_50G_X4: 58*/             { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 59*/               {         0, 0},
  /*`SPEED_50G_HG2_CR4: 60*/        { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_50G_HG2_KR4: 61*/        { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_50G_HG2_X4: 62*/         { OS_MODE_2, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 63*/               {         0, 0},
  /*`SPEED_100G_CR4: 64*/           { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_100G_KR4: 65*/           { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`SPEED_100G_X4: 66*/            { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 67*/               {         0, 0},
  /*`SPEED_100G_HG2_CR4: 68*/       { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_100G_HG2_KR4: 69*/       { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`SPEED_100G_HG2_X4: 70*/        { OS_MODE_1, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 71*/               {         0, 0},
  /*`SPEED_CL73_20GVCO: 72*/        { OS_MODE_16p5, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 73*/               {         0, 0},
  /*`NULL ENTRY: 74*/               {         0, 0},
  /*`NULL ENTRY: 75*/               {         0, 0},
  /*`NULL ENTRY: 76*/               {         0, 0},
  /*`NULL ENTRY: 77*/               {         0, 0},
  /*`NULL ENTRY: 78*/               {         0, 0},
  /*`NULL ENTRY: 79*/               {         0, 0},
  /*`SPEED_CL73_25GVCO: 80*/        { OS_MODE_20p625, TEFMOD16_PLL_MODE_DIV_175},
  /*`NULL ENTRY: 81*/               {         0, 0},
  /*`NULL ENTRY: 82*/               {         0, 0},
  /*`NULL ENTRY: 83*/               {         0, 0},
  /*`NULL ENTRY: 84*/               {         0, 0},
  /*`NULL ENTRY: 84*/               {         0, 0},
  /*`NULL ENTRY: 86*/               {         0, 0},
  /*`NULL ENTRY: 87*/               {         0, 0},
  /*`SPEED_1G_20GVCO: 88*/          { OS_MODE_16p5, TEFMOD16_PLL_MODE_DIV_165},
  /*`NULL ENTRY: 89*/               {         0, 0},
  /*`NULL ENTRY: 90*/               {         0, 0},
  /*`NULL ENTRY: 91*/               {         0, 0},
  /*`NULL ENTRY: 92*/               {         0, 0},
  /*`NULL ENTRY: 93*/               {         0, 0},
  /*`NULL ENTRY: 94*/               {         0, 0},
  /*`NULL ENTRY: 95*/               {         0, 0},
  /*`SPEED_1G_25GVCO: 96*/          { OS_MODE_20p625, TEFMOD16_PLL_MODE_DIV_165}
};


int tefmod16_get_mapped_speed(tefmod16_spd_intfc_type_t spd_intf, int *speed) 
{
  /* speeds covered in Forced-speed */
  if(spd_intf == TEFMOD16_SPD_10000_XFI)           *speed = sc_x4_control_sc_S_10G_X1;
  if(spd_intf == TEFMOD16_SPD_10600_XFI_HG)        *speed = sc_x4_control_sc_S_10G_HG2_X1;
  if(spd_intf == TEFMOD16_SPD_20000_XFI)           *speed = sc_x4_control_sc_S_20G_X1;
  if(spd_intf == TEFMOD16_SPD_21200_XFI_HG)        *speed = sc_x4_control_sc_S_20G_HG2_X1;
  if(spd_intf == TEFMOD16_SPD_25000_XFI)           *speed = sc_x4_control_sc_S_25G_X1;
  if(spd_intf == TEFMOD16_SPD_26500_XFI_HG)        *speed = sc_x4_control_sc_S_25G_HG2_X1;
  if(spd_intf == TEFMOD16_SPD_20G_MLD_X2)          *speed = sc_x4_control_sc_S_20G_X2;
  if(spd_intf == TEFMOD16_SPD_21G_MLD_HG_X2)       *speed = sc_x4_control_sc_S_20G_HG2_X2;
  if(spd_intf == TEFMOD16_SPD_40G_MLD_X2)          *speed = sc_x4_control_sc_S_40G_X2;
  if(spd_intf == TEFMOD16_SPD_42G_MLD_HG_X2)       *speed = sc_x4_control_sc_S_40G_HG2_X2;
  if(spd_intf == TEFMOD16_SPD_40G_MLD_X4)          *speed = sc_x4_control_sc_S_40G_X4;
  if(spd_intf == TEFMOD16_SPD_42G_MLD_HG_X4)       *speed = sc_x4_control_sc_S_40G_HG2_X4;
  if(spd_intf == TEFMOD16_SPD_50G_MLD_X2)          *speed = sc_x4_control_sc_S_50G_X2;
  if(spd_intf == TEFMOD16_SPD_53G_MLD_HG_X2)       *speed = sc_x4_control_sc_S_50G_HG2_X2;
  if(spd_intf == TEFMOD16_SPD_50G_MLD_X4)          *speed = sc_x4_control_sc_S_50G_X4;
  if(spd_intf == TEFMOD16_SPD_53G_MLD_HG_X4)       *speed = sc_x4_control_sc_S_50G_HG2_X4;
  if(spd_intf == TEFMOD16_SPD_100G_MLD_X4)         *speed = sc_x4_control_sc_S_100G_X4;
  if(spd_intf == TEFMOD16_SPD_106G_MLD_HG_X4)      *speed = sc_x4_control_sc_S_100G_HG2_X4;
  /*  These are the speeds that are not in forced mode or AN mode */
  if(spd_intf == TEFMOD16_SPD_10000_XFI_CR1)       *speed = sc_x4_control_sc_S_10G_CR1;
  if(spd_intf == TEFMOD16_SPD_10600_XFI_HG_CR1)    *speed = sc_x4_control_sc_S_10G_HG2_CR1;
  if(spd_intf == TEFMOD16_SPD_CL73_20G)            *speed = sc_x4_control_sc_S_CL73_20GVCO;
  if(spd_intf == TEFMOD16_SPD_CL73_25G)            *speed = sc_x4_control_sc_S_CL73_25GVCO;
  if(spd_intf == TEFMOD16_SPD_1G_20G)              *speed = sc_x4_control_sc_S_CL36_20GVCO;
  if(spd_intf == TEFMOD16_SPD_1G_25G)              *speed = sc_x4_control_sc_S_CL36_25GVCO;
  if(spd_intf == TEFMOD16_SPD_10G_25G)             *speed = sc_x4_control_sc_S_10G_25GVCO;
  if(spd_intf == TEFMOD16_SPD_2P5G_KX1)            *speed = 98;  /* to modify after getting DV's code */
  if(spd_intf == TEFMOD16_SPD_5G_KR1)              *speed = 99;

  return PHYMOD_E_NONE;
}

#ifdef _DV_TB_
int tefmod16_get_ht_entries(PHYMOD_ST* pc) 
{

  int or_en, or_val;

  or_en  = pc->per_field_override_en & 0xffff0000 ;
  or_val = pc->per_field_override_en & 0x0000ffff ;


  printf("SC_Info_Beg : pc->per_field_override_en %08x\n", pc->per_field_override_en);

  if(or_en == 0x00010000) {
    pc->num_lanes = or_val;
    printf("SC_Info_OVR : pc->num_lanes = or_val = %04x\n", pc->num_lanes);
  } else {
    pc->num_lanes = sc_ht_pcs[pc->speed].num_lanes;
  }

  if(or_en == 0x00020000) {
    pc->os_mode = or_val;
    printf("SC_Info_OVR : pc->os_mode = or_val = %04x\n", pc->os_mode);
  } else {
    pc->os_mode = sc_ht_pcs[pc->speed].os_mode;
  }

  if(or_en == 0x00040000) {
    pc->t_fifo_mode = or_val;
    printf("SC_Info_OVR : pc->t_fifo_mode = or_val = %04x\n", pc->t_fifo_mode);
  } else {
    pc->t_fifo_mode = sc_ht_pcs[pc->speed].t_fifo_mode;
  }

  if(or_en == 0x00080000) {
    pc->t_enc_mode = or_val;
    printf("SC_Info_OVR : pc->t_enc_mode = or_val = %04x\n", pc->t_enc_mode);
  } else {
    pc->t_enc_mode = sc_ht_pcs[pc->speed].t_enc_mode;
  }

  if(or_en == 0x00100000) {
    pc->t_HG2_ENABLE = or_val;
    printf("SC_Info_OVR : pc->t_HG2_ENABLE = or_val = %04x\n", pc->t_HG2_ENABLE);
  } else {
    pc->t_HG2_ENABLE = sc_ht_pcs[pc->speed].t_HG2_ENABLE;
  }

  if(or_en == 0x00200000) {
    pc->t_pma_btmx_mode = or_val;
    printf("SC_Info_OVR : pc->t_pma_btmx_mode = or_val = %04x\n", pc->t_pma_btmx_mode);
  } else {
    pc->t_pma_btmx_mode = sc_ht_pcs[pc->speed].t_pma_btmx_mode;
    if(pc->cl91_en == 1 && (cl91_valid_speed(pc) || pc->fc_mode == 1)) {
      pc->t_pma_btmx_mode = 0;
    }
  }

  if(or_en == 0x00400000) {
    pc->scr_mode = or_val;
    printf("SC_Info_OVR : pc->scr_mode = or_val = %04x\n", pc->scr_mode);
  } else {
    pc->scr_mode = sc_ht_pcs[pc->speed].scr_mode;
  }

  if(or_en == 0x01000000) {
    pc->descr_mode = or_val;
    printf("SC_Info_OVR : pc->descr_mode = or_val = %04x\n", pc->descr_mode);
  } else {
    pc->descr_mode = sc_ht_pcs[pc->speed].descr_mode;
  }

  if(or_en == 0x02000000) {
    pc->dec_tl_mode = or_val;
    printf("SC_Info_OVR : pc->dec_tl_mode = or_val = %04x\n", pc->dec_tl_mode);
  } else {
    pc->dec_tl_mode = sc_ht_pcs[pc->speed].dec_tl_mode;
  }

  if(or_en == 0x04000000) {
    pc->deskew_mode = or_val;
    printf("SC_Info_OVR : pc->deskew_mode = or_val = %04x\n", pc->deskew_mode);
  } else {
    pc->deskew_mode = sc_ht_pcs[pc->speed].deskew_mode;
    if((pc->speed == sc_x1_speed_override0_sc_S_100G_CR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_KR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_X4     ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_CR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_KR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_X4 )) {
      if((pc->cl91_en == 1) || ((pc->an_en == 1) && (pc->an_fec_sel_override == 0 || pc->an_fec_sel_override == 3)) ) {
        pc->deskew_mode = R_DESKEW_MODE_CL91;
      }
    }
  }

  if(or_en == 0x08000000) {
    pc->dec_fsm_mode = or_val;
    printf("SC_Info_OVR : pc->dec_fsm_mode = or_val = %04x\n", pc->dec_fsm_mode);
  } else {
    pc->dec_fsm_mode = sc_ht_pcs[pc->speed].dec_fsm_mode;
  }

  pc->r_HG2_ENABLE = sc_ht_pcs[pc->speed].r_HG2_ENABLE;

  if(or_en == 0x10000000) {
    pc->bs_sm_sync_mode = or_val;
    printf("SC_Info_OVR : pc->bs_sm_sync_mode = or_val = %04x\n", pc->bs_sm_sync_mode);
  } else {
    pc->bs_sm_sync_mode = sc_ht_pcs[pc->speed].bs_sm_sync_mode;
  }

  if(or_en  == 0x10010000) {
    pc->bs_sync_en = or_val;
    printf("SC_Info_OVR : pc->bs_sync_en = or_val = %04x\n", pc->bs_sync_en);
  } else {

    pc->bs_sync_en = sc_ht_pcs[pc->speed].bs_sync_en;
    if((pc->cl91_en == 1 && cl91_valid_speed(pc)) || pc->an_hcd_fec || pc->t_fec_enable) {
      pc->bs_sync_en = 0;   
    }
     
  }

  if(or_en  == 0x10020000) {
    pc->bs_dist_mode = or_val;
    printf("SC_Info_OVR : pc->bs_dist_mode = or_val = %04x\n", pc->bs_dist_mode);
  } else {
    pc->bs_dist_mode = sc_ht_pcs[pc->speed].bs_dist_mode;
    if((pc->speed == sc_x1_speed_override0_sc_S_100G_CR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_KR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_X4     ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_CR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_KR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_X4 )) {
      if((pc->cl91_en == 1) || ((pc->an_en == 1) && (pc->an_fec_sel_override == 0 || pc->an_fec_sel_override == 3)) ) {
        pc->bs_dist_mode = 0;
      }
    }
  }

  if(or_en == 0x10040000) {
    pc->bs_btmx_mode = or_val;
    printf("SC_Info_OVR : pc->bs_btmx_mode = or_val = %04x\n", pc->bs_btmx_mode);
  } else {
    pc->bs_btmx_mode = sc_ht_pcs[pc->speed].bs_btmx_mode;
    if((pc->speed == sc_x1_speed_override0_sc_S_100G_CR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_KR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_X4     ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_CR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_KR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_X4 )) {
      if((pc->cl91_en == 1) || ((pc->an_en == 1) && (pc->an_fec_sel_override == 0 || pc->an_fec_sel_override == 3)) ) {
        pc->bs_btmx_mode = 0;
      }
    }
  }

  if(pc->per_field_override_en == 0x1008 && pc->cl72_en==1) {
    pc->cl72_en = 1; 
    printf("SC_Info_OVR : #1 pc->cl72_en = %04x\n", pc->cl72_en);
    pc->per_field_override_en = or_en;
  } else if(pc->per_field_override_en == 0x1008 && pc->cl72_en==0) {
    pc->cl72_en = 0; 
    printf("SC_Info_OVR : #2 pc->cl72_en = %04x\n", pc->cl72_en);
  } else {
    pc->cl72_en = sc_ht_pcs[pc->speed].cl72_en;
  }

  if(or_en == 0x20000000) {
    pc->clkcnt0 = or_val;
    printf("SC_Info_OVR : pc->clkcnt0 = or_val = %04x\n", pc->clkcnt0);
  } else {
    if(pc->tsc_clk_freq_pll_by_48 == 0) {
      pc->clkcnt0 =	sc_ht_pcs[pc->speed].clkcnt0;
    } else {
      pc->clkcnt0 =	sc_ht_pcs[pc->speed].clkcnt0_by48;
    }
  }

  if(or_en == 0x20010000) {
    pc->clkcnt1 = or_val;
    printf("SC_Info_OVR : pc->clkcnt1 = or_val = %04x\n", pc->clkcnt1);
  } else {
    if(pc->tsc_clk_freq_pll_by_48 == 0) {
      pc->clkcnt1 =	sc_ht_pcs[pc->speed].clkcnt1;
    } else {
      pc->clkcnt1 =	sc_ht_pcs[pc->speed].clkcnt1_by48;
    }

    if(pc->credit_sel) {
      if(pc->fc_tx_spd_intf == TEFMOD16_SPD_16G_FC && pc->fc_mode == 1) { 
        pc->clkcnt1 = 10;
      }

      if(pc->fc_mode == 0) {
        if (pc->speed == 0x0 || pc->speed == 0x1 || pc->speed == 0x2 || pc->speed == 0x4 || pc->speed == 0x5 || pc->speed == 0x6) {
          pc->clkcnt1 = 10;
        }
      }
    } 
  }

  if(or_en == 0x20020000) {
    pc->lpcnt0 = or_val;
    printf("SC_Info_OVR : pc->lpcnt0 = or_val = %04x\n", pc->lpcnt0);
  } else {
    if(pc->tsc_clk_freq_pll_by_48 == 0) {
      pc->lpcnt0 = sc_ht_pcs[pc->speed].lpcnt0;
    } else {
      pc->lpcnt0 = sc_ht_pcs[pc->speed].lpcnt0_by48;
    }
  }

  if(or_en == 0x20040000) {
    pc->lpcnt1 = or_val;
    printf("SC_Info_OVR : pc->lpcnt1 = or_val = %04x\n", pc->lpcnt1);
  } else {
    if(pc->tsc_clk_freq_pll_by_48 == 0) {
      pc->lpcnt1 = sc_ht_pcs[pc->speed].lpcnt1;
    } else {
      pc->lpcnt1 = sc_ht_pcs[pc->speed].lpcnt1_by48;
    }

    if(pc->credit_sel) {
      if(pc->fc_tx_spd_intf == TEFMOD16_SPD_16G_FC && pc->fc_mode == 1) { 
        pc->lpcnt1 = 9;
      }

      if(pc->fc_mode == 0) {
        if (pc->speed == 0x0 || pc->speed == 0x1 || pc->speed == 0x2 || pc->speed == 0x4 || pc->speed == 0x5 || pc->speed == 0x6) {
          pc->lpcnt1 = 9;
        }
      }
    } 
  }

  if(or_en == 0x20080000) {
    pc->cgc = or_val;
    printf("SC_Info_OVR : pc->cgc = or_val = %04x\n", pc->cgc);
  } else {
    if(pc->tsc_clk_freq_pll_by_48 == 0) {
      pc->cgc =	sc_ht_pcs[pc->speed].cgc;
    } else {
      pc->cgc =	sc_ht_pcs[pc->speed].cgc_by48;
    }

    if(pc->credit_sel) {
      if(pc->fc_tx_spd_intf == TEFMOD16_SPD_16G_FC && pc->fc_mode == 1) { 
        pc->cgc = 6;  
      }

      if(pc->fc_mode == 0) {
        if (pc->speed == 0x0 || pc->speed == 0x1 || pc->speed == 0x2 || pc->speed == 0x4 || pc->speed == 0x5 || pc->speed == 0x6) {
          pc->cgc = 6; 
        }
        if(pc->speed == 0x18 || pc->speed == 0x19 || pc->speed == 0x1a || pc->speed == 0x1c || pc->speed == 0x1d || pc->speed == 0x1e) {
          pc->cgc = 4;
        }
        if((pc->speed == 0x28 || pc->speed == 0x29 || pc->speed == 0x2a || pc->speed == 0x2c || pc->speed == 0x2d || pc->speed == 0x2e) ||
           (pc->speed == 0x38 || pc->speed == 0x39 || pc->speed == 0x3a || pc->speed == 0x3c || pc->speed == 0x3d || pc->speed == 0x3e)) {
          pc->cgc = 2;
        }
      }
    } 
  }

  printf("SC_Info_End : pc->per_field_override_en %08x\n", pc->per_field_override_en);

  return PHYMOD_E_NONE;
}

#endif

int print_tefmod16_sc_lkup_table(PHYMOD_ST* pc) 
{
  #ifdef _DV_TB_
  printf("PRINTING THE SC TABLE .....\n");
  printf("NUM_LANES is %d\n", pc->num_lanes);
  printf("OS_MODE is %d\n", pc->os_mode);
  printf("T_FIFO_MODE is %d\n", pc->t_fifo_mode);
  printf("T_ENC_MODE is %d\n", pc->t_enc_mode);
  printf("T_HG2_ENABLE is %d\n", pc->t_HG2_ENABLE);
  printf("T_PMA_BTMX_MODE is %d\n", pc->t_pma_btmx_mode);
  printf("SCR_MODE is %d\n", pc->scr_mode);
  printf("DESCR_MODE is %d\n", pc->descr_mode);
  printf("DEC_TL_MODE is %d\n", pc->dec_tl_mode);
  printf("DESKEW_MODE is %d\n", pc->deskew_mode);
  printf("DEC_FSM_MODE is %d\n", pc->dec_fsm_mode);
  printf("R_HG2_ENABLE is %d\n", pc->r_HG2_ENABLE);
  printf("BS_SM_SYNC_MODE is %d\n", pc->bs_sm_sync_mode);
  printf("BS_SYNC_EN is %d\n", pc->bs_sync_en);
  printf("BS_DIST_MODE is %d\n", pc->bs_dist_mode);
  printf("BS_BTMX_MODE is %d\n", pc->bs_btmx_mode);
  printf("CL72_EN is %d\n", pc->cl72_en);
  printf("CLKCNT0 is %d\n", pc->clkcnt0);
  printf("CLKCNT1 is %d\n", pc->clkcnt1);
  printf("LOOPCNT0 is %d\n", pc->lpcnt0);
  printf("LOOPCNT1 is %d\n", pc->lpcnt1);
  printf("CGC is %d\n", pc->cgc);
  printf("USE_100G_AM0 is %d\n", pc->USE_100G_AM0);
  printf("USE_100G_AM123 is %d\n", pc->USE_100G_AM123);
  printf("cl74_shcorrupt is %d\n", pc->cl74_shcorrupt);
  printf("corrupt_6th_group is %d\n", pc->corrupt_6th_group);
  printf("corrupt_2nd_group is %d\n", pc->corrupt_2nd_group);
  #endif

  return PHYMOD_E_NONE;
}


#ifdef _DV_TB_
int tefmod16_sc_lkup_table(PHYMOD_ST* pc)
{
  int mapped_speed;
  tefmod16_get_mapped_speed(pc->spd_intf, &mapped_speed);
  tefmod16_get_ht_entries(pc);
  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
int tefmod16_fill_lkup_exp(PHYMOD_ST* pc)
{
     tefmod16_sc_lkup_table(pc);

 return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
int cl91_valid_speed(PHYMOD_ST* pc)
{

  if(pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_CR1     || 
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_KR1     ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_X1      ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_HG2_CR1 ||  
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_HG2_KR1 ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_HG2_X1  ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_50G_CR2     || 
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_50G_KR2     ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_50G_X2      ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_50G_HG2_CR2 ||  
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_50G_HG2_KR2 ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_50G_HG2_X2  ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_CR4     || 
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_KR4     ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_X4      ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_HG2_CR4 ||  
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_HG2_KR4 ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_100G_HG2_X4  ||
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_CR_IEEE  ||        
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_KR_IEEE  || 
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_CRS_IEEE ||        
     pc->speed == SC_X4_CONTROL_CONTROL_SPEED_S_25G_KRS_IEEE   
    ) {
      return 1;
    }
    else {
      return 0;
    }
}
#endif
