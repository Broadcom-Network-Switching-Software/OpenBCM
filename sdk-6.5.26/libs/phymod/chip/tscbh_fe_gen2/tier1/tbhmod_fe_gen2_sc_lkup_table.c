/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
#include "tbhmod_fe_gen2.h"
#include <phymod/phymod.h>

#include "tbhmod_fe_gen2_sc_lkup_table.h"

const tscbh_fe_gen2_sc_pmd_entry_st tscbh_fe_gen2_sc_pmd_entry[] = {
  /* SPD_10G_IEEE_KR1: 0*/                        { TBHMOD_FE_GEN2_OS_MODE_2, TBHMOD_FE_GEN2_PLL_MODE_DIV_132},
  /* SPD_25G_IEEE_KS1_CS1: 1*/                    { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_25G_IEEE_KR1_CR1: 2*/                    { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_40G_IEEE_KR4: 3*/                        { TBHMOD_FE_GEN2_OS_MODE_2, TBHMOD_FE_GEN2_PLL_MODE_DIV_132},
  /* SPD_40G_IEEE_CR4: 4*/                        { TBHMOD_FE_GEN2_OS_MODE_2, TBHMOD_FE_GEN2_PLL_MODE_DIV_132},
  /* SPD_50G_IEEE_KR1_CR1: 5*/                    { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_100G_IEEE_KR4: 6*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_100G_IEEE_CR4: 7*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_100G_IEEE_KR2_CR2: 8*/                   { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_200G_IEEE_KR4_CR4: 9*/                   { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_400G_IEEE_X8: 10*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_CL73_IEEE_20GVCO: 11*/                   { TBHMOD_FE_GEN2_OS_MODE_16p5, TBHMOD_FE_GEN2_PLL_MODE_DIV_132},
  /* SPD_CL73_IEEE_25GVCO: 12*/                   { TBHMOD_FE_GEN2_OS_MODE_20p625, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_CL73_IEEE_26GVCO: 13*/                   { TBHMOD_FE_GEN2_OS_MODE_21p25, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_20G_BRCM_CR1: 14*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_132},
  /* SPD_20G_BRCM_KR1: 15*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_132},
  /* SPD_25G_BRCM_CR1: 16*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_25G_BRCM_KR1: 17*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_40G_BRCM_CR2: 18*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_132},
  /* SPD_40G_BRCM_KR2: 19*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_132},
  /* SPD_50G_BRCM_CR2_KR2_NO_FEC: 20*/            { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_50G_BRCM_CR2_KR2_RS_FEC: 21*/            { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_50G_BRCM_CR4: 22*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_50G_BRCM_KR4: 23*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_50G_BRCM_FEC_544_CR2_KR2: 24*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* NULL ENTRY: 25*/                             {         0, 0},
  /* SPD_50G_BRCM_FEC_544_CR1_KR1: 26*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_50G_BRCM_NO_FEC_CR1_KR1: 27*/            { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_50G_BRCM_FEC_528_CR1_KR1: 28*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_400G_BRCM_NO_FEC_KR8_CR8: 29*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_400G_BRCM_FEC_KR8_CR8: 30*/              { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_200G_BRCM_NO_FEC_KR4_CR4: 31*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_200G_BRCM_KR4_CR4: 32*/                  { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_100G_BRCM_KR4_CR4: 33*/                  { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* NULL ENTRY: 34*/                             {         0, 0},
  /* SPD_100G_BRCM_FEC_528_KR2_CR2: 35*/          { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_100G_BRCM_NO_FEC_KR2_CR2: 36*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_100G_BRCM_NO_FEC_X4: 37*/                { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_50G_BRCM_FEC_272_CR1_KR1: 38*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_100G_BRCM_FEC_272_CR2_KR2: 39*/          { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_200G_BRCM_FEC_272_CR4_KR4: 40*/          { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_25G_BRCM_NO_FEC_KR1_CR1: 41*/            { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_25G_BRCM_FEC_BASE_R_KR1_CR1: 42*/        { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_25G_BRCM_FEC_528_KR1_CR1 :43*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_50G_BRCM_CR2_KR2_RS_FEC_FLEXE: 44*/      { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_50G_BRCM_FEC_544_CR2_KR2_FLEXE: 45*/     { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* NULL ENTRY: 46*/                             {         0, 0},
  /* NULL ENTRY: 47*/                             {         0, 0},
  /* NULL ENTRY: 48*/                             {         0, 0},
  /* NULL ENTRY: 49*/                             {         0, 0},
  /* NULL ENTRY: 50*/                             {         0, 0},
  /* NULL ENTRY: 51*/                             {         0, 0},
  /* NULL ENTRY: 52*/                             {         0, 0},
  /* NULL ENTRY: 53*/                             {         0, 0},
  /* NULL ENTRY: 54*/                             {         0, 0},
  /* NULL ENTRY: 55*/                             {         0, 0},
  /* NULL ENTRY: 56*/                             {         0, 0},
  /* NULL ENTRY: 57*/                             {         0, 0},
  /* NULL ENTRY: 58*/                             {         0, 0},
  /* NULL ENTRY: 59*/                             {         0, 0},
  /* NULL ENTRY: 60*/                             {         0, 0},
  /* NULL ENTRY: 61*/                             {         0, 0},
  /* NULL ENTRY: 62*/                             {         0, 0},
  /* NULL ENTRY: 63*/                             {         0, 0},
  /* SPD_10G_IEEE_FEC_BASE_R_N1: 64*/             { TBHMOD_FE_GEN2_OS_MODE_2, TBHMOD_FE_GEN2_PLL_MODE_DIV_132},
  /* SPD_20G_BRCM_FEC_BASE_R_N1: 65*/             { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_132},
  /* SPD_40G_IEEE_FEC_BASE_R_N4: 66*/             { TBHMOD_FE_GEN2_OS_MODE_2, TBHMOD_FE_GEN2_PLL_MODE_DIV_132},
  /* SPD_150G_BRCM_N3: 67*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_200G_BRCM_FEC_272_N4: 68*/               { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_300G_BRCM_N6: 69*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_350G_BRCM_N7: 70*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_400G_BRCM_FEC_272_N8: 71*/               { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_200G_BRCM_FEC_544_CR8_KR8: 72*/          { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170},
  /* SPD_12P12G_BRCM_KR1: 73*/                    { TBHMOD_FE_GEN2_OS_MODE_2, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_50G_BRCM_CR2_KR2_RS_FEC_FLEXE: 74*/      { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_165},
  /* SPD_50G_BRCM_FEC_544_CR2_KR2_FLEXE: 75*/     { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_170}

};

const tscbh_fe_gen2_sc_pmd_entry_st tscbh_fe_gen2_sc_pmd_entry_312M_ref[] = {
  /* SPD_10G_IEEE_KR1: 0*/                        { TBHMOD_FE_GEN2_OS_MODE_2, TBHMOD_FE_GEN2_PLL_MODE_DIV_66},
  /* SPD_25G_IEEE_KS1_CS1: 1*/                    { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_25G_IEEE_KR1_CR1: 2*/                    { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_40G_IEEE_KR4: 3*/                        { TBHMOD_FE_GEN2_OS_MODE_2, TBHMOD_FE_GEN2_PLL_MODE_DIV_66},
  /* SPD_40G_IEEE_CR4: 4*/                        { TBHMOD_FE_GEN2_OS_MODE_2, TBHMOD_FE_GEN2_PLL_MODE_DIV_66},
  /* SPD_50G_IEEE_KR1_CR1: 5*/                    { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_100G_IEEE_KR4: 6*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_100G_IEEE_CR4: 7*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_100G_IEEE_KR2_CR2: 8*/                   { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_200G_IEEE_KR4_CR4: 9*/                   { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_400G_IEEE_X8: 10*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_CL73_IEEE_20GVCO: 11*/                   { TBHMOD_FE_GEN2_OS_MODE_16p5, TBHMOD_FE_GEN2_PLL_MODE_DIV_66},
  /* SPD_CL73_IEEE_25GVCO: 12*/                   { TBHMOD_FE_GEN2_OS_MODE_20p625, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_CL73_IEEE_26GVCO: 13*/                   { TBHMOD_FE_GEN2_OS_MODE_21p25, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_20G_BRCM_CR1: 14*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_66},
  /* SPD_20G_BRCM_KR1: 15*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_66},
  /* SPD_25G_BRCM_CR1: 16*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_25G_BRCM_KR1: 17*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_40G_BRCM_CR2: 18*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_66},
  /* SPD_40G_BRCM_KR2: 19*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_66},
  /* SPD_50G_BRCM_CR2_KR2_NO_FEC: 20*/            { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_50G_BRCM_CR2_KR2_RS_FEC: 21*/            { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_50G_BRCM_CR4: 22*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_50G_BRCM_KR4: 23*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_50G_BRCM_FEC_544_CR2_KR2: 24*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* NULL ENTRY: 25*/                             {         0, 0},
  /* SPD_50G_BRCM_FEC_544_CR1_KR1: 26*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_50G_BRCM_NO_FEC_CR1_KR1: 27*/            { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_50G_BRCM_FEC_528_CR1_KR1: 28*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_400G_BRCM_NO_FEC_KR8_CR8: 29*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_400G_BRCM_FEC_KR8_CR8: 30*/              { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_200G_BRCM_NO_FEC_KR4_CR4: 31*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_200G_BRCM_KR4_CR4: 32*/                  { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_100G_BRCM_KR4_CR4: 33*/                  { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* NULL ENTRY: 34*/                             {         0, 0},
  /* SPD_100G_BRCM_FEC_528_KR2_CR2: 35*/          { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_100G_BRCM_NO_FEC_KR2_CR2: 36*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_100G_BRCM_NO_FEC_X4: 37*/                { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_50G_BRCM_FEC_272_CR1_KR1: 38*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_100G_BRCM_FEC_272_CR2_KR2: 39*/          { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_200G_BRCM_FEC_272_CR4_KR4: 40*/          { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_25G_BRCM_NO_FEC_KR1_CR1: 41*/            { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_25G_BRCM_FEC_BASE_R_KR1_CR1: 42*/        { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_25G_BRCM_FEC_528_KR1_CR1 :43*/           { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_50G_BRCM_CR2_KR2_RS_FEC_FLEXE: 44*/      { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_50G_BRCM_FEC_544_CR2_KR2_FLEXE: 45*/     { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* NULL ENTRY: 46*/                             {         0, 0},
  /* NULL ENTRY: 47*/                             {         0, 0},
  /* NULL ENTRY: 48*/                             {         0, 0},
  /* NULL ENTRY: 49*/                             {         0, 0},
  /* NULL ENTRY: 50*/                             {         0, 0},
  /* NULL ENTRY: 51*/                             {         0, 0},
  /* NULL ENTRY: 52*/                             {         0, 0},
  /* NULL ENTRY: 53*/                             {         0, 0},
  /* NULL ENTRY: 54*/                             {         0, 0},
  /* NULL ENTRY: 55*/                             {         0, 0},
  /* NULL ENTRY: 56*/                             {         0, 0},
  /* NULL ENTRY: 57*/                             {         0, 0},
  /* NULL ENTRY: 58*/                             {         0, 0},
  /* NULL ENTRY: 59*/                             {         0, 0},
  /* NULL ENTRY: 60*/                             {         0, 0},
  /* NULL ENTRY: 61*/                             {         0, 0},
  /* NULL ENTRY: 62*/                             {         0, 0},
  /* NULL ENTRY: 63*/                             {         0, 0},
  /* SPD_10G_IEEE_FEC_BASE_R_N1: 64*/             { TBHMOD_FE_GEN2_OS_MODE_2, TBHMOD_FE_GEN2_PLL_MODE_DIV_66},
  /* SPD_20G_BRCM_FEC_BASE_R_N1: 65*/             { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_66},
  /* SPD_40G_IEEE_FEC_BASE_R_N4: 66*/             { TBHMOD_FE_GEN2_OS_MODE_2, TBHMOD_FE_GEN2_PLL_MODE_DIV_66},
  /* SPD_150G_BRCM_N3: 67*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_200G_BRCM_FEC_272_N4: 68*/               { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_300G_BRCM_N6: 69*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_350G_BRCM_N7: 70*/                       { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_400G_BRCM_FEC_272_N8: 71*/               { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_200G_BRCM_FEC_544_CR8_KR8: 72*/          { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85},
  /* SPD_12P12G_BRCM_KR1: 73*/                    { TBHMOD_FE_GEN2_OS_MODE_2, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_50G_BRCM_CR2_KR2_RS_FEC_FLEXE: 74*/      { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5},
  /* SPD_50G_BRCM_FEC_544_CR2_KR2_FLEXE: 75*/     { TBHMOD_FE_GEN2_OS_MODE_1, TBHMOD_FE_GEN2_PLL_MODE_DIV_85}

};

/* the following speed id table, am_table and um table are based off
 * Speed_Control_3VCOs_Unified.xlsx, AM_Table_Unified.xlsx and UM_Table_Unified.xlsx. */

static uint32_t tscbh_fe_gen2_spd_id_entry[TSCBH_FE_GEN2_SPEED_ID_TABLE_SIZE][TSCBH_FE_GEN2_SPEED_ID_ENTRY_SIZE] = {
    { 0x00020000, 0x00000000, 0x00104400, 0x01842304, 0x00010804 },
    { 0x00040000, 0x00000000, 0x00105800, 0x02402304, 0x00110804 },
    { 0x34352802, 0x0001f800, 0x00105804, 0x02402304, 0x001a940a },
    { 0x00040003, 0x1d400000, 0x00105800, 0x01042a04, 0x00010810 },
    { 0x00040003, 0x1d400000, 0x00105800, 0x01042a04, 0x00010810 },
    { 0x1d912905, 0x0009d003, 0x0010383d, 0x0300ea04, 0x001aa80a },
    { 0x34312886, 0x00406800, 0x0010583d, 0x01002a04, 0x0018a50a },
    { 0x34312886, 0x00406800, 0x0010583d, 0x01002a04, 0x0018a50a },
    { 0x1d912908, 0x0028c003, 0x0010383d, 0x0200ea04, 0x0019540a },
    { 0xb7113749, 0x004800c2, 0x0010393d, 0x0100ec04, 0x00395414 },
    { 0xb711374a, 0x00680042, 0x0010393d, 0x0100ec04, 0x0038aa14 },
    { 0x00000000, 0x00000000, 0x00108000, 0x01600002, 0x00000000 },
    { 0x00000000, 0x00000000, 0x0050a000, 0x01700008, 0x00000000 },
    { 0x00000000, 0x00000000, 0x0050a000, 0x01500004, 0x00000000 },
    { 0x00020000, 0x00000000, 0x00104000, 0x01402304, 0x00110804 },
    { 0x00020000, 0x00000000, 0x00104000, 0x01402304, 0x00110804 },
    { 0x34352810, 0x0001f800, 0x00105804, 0x02402304, 0x001a940a },
    { 0x34352810, 0x0001f800, 0x00105804, 0x02402304, 0x001a940a },
    { 0x00000003, 0x1d280000, 0x00105800, 0x01002a04, 0x00121010 },
    { 0x00000003, 0x1d280000, 0x00105800, 0x01002a04, 0x00121010 },
    { 0x00000014, 0x24280000, 0x00105800, 0x01002a04, 0x00121010 },
    { 0x34312895, 0x0020f000, 0x0010583d, 0x01402a04, 0x00194a0a },
    { 0x00000000, 0x00000000, 0x00102000, 0x00000008, 0x00100000 },
    { 0x00000000, 0x00000000, 0x00102000, 0x00000008, 0x00100000 },
    { 0x1d912905, 0x0020f003, 0x0010583d, 0x01402a02, 0x0019540a },
    { 0x00000019, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x00102000, 0x00000004, 0x00080000 },
    { 0x00000014, 0x24100000, 0x00103800, 0x0180ea08, 0x00121008 },
    { 0x34312895, 0x0009e000, 0x0010383d, 0x0300ea08, 0x001a940a },
    { 0x00000000, 0x00000000, 0x02100000, 0x00000020, 0x00000000 },
    { 0xb711374a, 0x00680042, 0x0010393d, 0x0100ec04, 0x0038aa14 },
    { 0x0000001f, 0x47480000, 0x00103800, 0x0100ea08, 0x00310810 },
    { 0xb7113509, 0x00484002, 0x0010393d, 0x0100ec04, 0x0038aa0a },
    { 0x1d912908, 0x00406003, 0x0010583d, 0x01002a02, 0x0018aa0a },
    { 0x00000022, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x34312886, 0x0028d000, 0x0010383d, 0x0200ea08, 0x00194a0a },
    { 0x00000024, 0x47280000, 0x00103800, 0x0100ea08, 0x00121010 },
    { 0x00000025, 0x0e580000, 0x00105800, 0x01002a04, 0x00114a14 },
    { 0x8ed128c5, 0x00090001, 0x0010383d, 0x0300ea04, 0x00195405 },
    { 0x5b9128c8, 0x00287001, 0x0010383d, 0x0200ea04, 0x0018aa05 },
    { 0x5b913789, 0x00480071, 0x0010393d, 0x0100ec04, 0x0038aa0a },
    { 0x00000000, 0x00000000, 0x00105800, 0x02402304, 0x00110804 },
    { 0x00040040, 0x00000000, 0x00105800, 0x02402304, 0x00110804 },
    { 0x34312890, 0x0001f800, 0x00105804, 0x02402304, 0x001a940a },
    { 0x34312895, 0x0020f000, 0x0010583d, 0x01402a04, 0x00194a0a },
    { 0x1d912905, 0x0020f003, 0x0010583d, 0x01402a02, 0x0019540a },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x40000000, 0x00000000, 0x00000000 },
    { 0x00020040, 0x00000000, 0x00104400, 0x01842304, 0x00010804 },
    { 0x00020040, 0x00000000, 0x00104000, 0x01402304, 0x00110804 },
    { 0x00040043, 0x1d400000, 0x00105800, 0x01442a04, 0x00010810 },
    { 0xb711377d, 0x00880122, 0x0050393d, 0x0140cc03, 0x00380000 },
    { 0x5b9134c9, 0x00483001, 0x0010393d, 0x0100ec04, 0x00385505 },
    { 0xb711377e, 0x00a800c2, 0x0050393d, 0x0100cc03, 0x00380000 },
    { 0xb711377f, 0x00c800b2, 0x8030393d, 0x0100cc03, 0x00380000 },
    { 0x5b91378a, 0x00680031, 0x0010393d, 0x0100ec04, 0x0038550a },
    { 0xb711377c, 0x00600072, 0x0010393d, 0x01002c04, 0x0038aa14 },
    { 0x00020000, 0x00000000, 0x00104400, 0x01842304, 0x00010804 },
    { 0x34312895, 0x0020f000, 0x0010583d, 0x01402a04, 0x00194a0a },
    { 0x1d912905, 0x0020f003, 0x0010583d, 0x01402a02, 0x0019540a }
};


uint32_t tscbh_fe_gen2_spd_id_entry_100g_4lane_no_fec[TSCBH_FE_GEN2_SPEED_ID_ENTRY_SIZE] = {
     0x34312825, 0x0e586800, 0x0010583d, 0x01002a02, 0x0010a50a
};

static uint32_t tscbh_fe_gen2_am_table_entry[TSCBH_FE_GEN2_AM_TABLE_SIZE][TSCBH_FE_GEN2_AM_ENTRY_SIZE] = {
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x8c1006c8, 0x00421216, 0x00000014 },
    { 0x0000000f, 0x20002000, 0x00000010 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000009, 0x00223000, 0x00000014 },
    { 0x0000002a, 0x00425000, 0x00000050 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x0000002a, 0x00445000, 0x00000050 },
    { 0x00000023, 0x00226000, 0x00000050 },
    { 0x00000044, 0x00227000, 0x000000a0 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x8c1006c8, 0x00421216, 0x00000014 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x0000000f, 0x20003000, 0x00000010 },
    { 0x00000009, 0x00213000, 0x00000014 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000017, 0x20006000, 0x00000020 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x0000000f, 0x20005000, 0x00000010 },
    { 0x0000002f, 0x20005000, 0x00000050 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000007, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000000, 0x00001000, 0x00000000 },
    { 0x00000023, 0x00226000, 0x00000050 },
    { 0x0000001b, 0x00226000, 0x0000003c },
    { 0x00000034, 0x00227000, 0x00000078 },
    { 0x0000003c, 0x00227000, 0x0000008c }
};

static uint32_t tscbh_fe_gen2_um_table_entry[TSCBH_FE_GEN2_UM_TABLE_SIZE][TSCBH_FE_GEN2_UM_ENTRY_SIZE] = {
    { 0x33e6c4f0, 0x00000133 },
    { 0x009b65c5, 0x00000033 },
    { 0x003d79a2, 0x00000033 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000 }
};

uint32_t tscbh_fe_gen2_speed_priority_mapping_table[TSCBH_FE_GEN2_SPEED_PRIORITY_MAPPING_TABLE_SIZE][TSCBH_FE_GEN2_SPEED_PRIORITY_MAPPING_ENTRY_SIZE] = {

    {0x1f809662, 0x48e11872, 0x71b60596, 0x930c4554, 0xf4110424, 0xffef4038, 0x00000000, 0x00000000, 0x00000000}
};

int tbhmod_fe_gen2_get_mapped_speed(tbhmod_fe_gen2_spd_intfc_type_t spd_intf, int *speed)
{
    *speed = -1;
    /* speeds covered in Forced-speed */
    if (spd_intf == TBHMOD_FE_GEN2_SPD_10G_IEEE_KR1)                        *speed = 0;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_10G_IEEE_FEC_BASE_R_N1)              *speed = 64;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_20G_BRCM_CR1)                        *speed = 14;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_20G_BRCM_KR1)                        *speed = 15;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_20G_BRCM_FEC_BASE_R_N1)              *speed = 65;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_25G_IEEE_KS1_CS1)                    *speed = 1;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_25G_IEEE_KR1_CR1)                    *speed = 2;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_25G_BRCM_CR1)                        *speed = 16;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_25G_BRCM_KR1)                        *speed = 17;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_40G_IEEE_FEC_BASE_R_N4)              *speed = 66;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_40G_IEEE_KR4)                        *speed = 3;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_40G_IEEE_CR4)                        *speed = 4;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_40G_BRCM_CR2)                        *speed = 18;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_40G_BRCM_KR2)                        *speed = 19;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_25G_BRCM_NO_FEC_KR1_CR1)             *speed = 41;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_25G_BRCM_FEC_BASE_R_KR1_CR1)         *speed = 42;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_25G_BRCM_FEC_528_KR1_CR1)            *speed = 43;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_CL73_IEEE_20GVCO)                    *speed = 11;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_CL73_IEEE_25GVCO)                    *speed = 12;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_CL73_IEEE_26GVCO)                    *speed = 13;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_50G_BRCM_CR2_KR2_NO_FEC)             *speed = 20;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_50G_BRCM_CR2_KR2_RS_FEC)             *speed = 21;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_544_CR2_KR2)            *speed = 24;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_50G_BRCM_NO_FEC_CR1_KR1)             *speed = 27;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_272_CR1_KR1)            *speed = 38;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_528_CR1_KR1)            *speed = 28;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_544_CR1_KR1)            *speed = 26;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_50G_IEEE_KR1_CR1)                    *speed = 5;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_50G_BRCM_CR2_KR2_RS_FEC_FLEXE)       *speed = 44;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_544_CR2_KR2_FLEXE)      *speed = 45;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_100G_BRCM_NO_FEC_X4)                 *speed = 37;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_100G_IEEE_KR4)                       *speed = 6;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_100G_IEEE_CR4)                       *speed = 7;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_100G_BRCM_KR4_CR4)                   *speed = 33;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_100G_BRCM_NO_FEC_KR2_CR2)            *speed = 36;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_100G_BRCM_FEC_272_CR2_KR2)           *speed = 39;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_100G_BRCM_FEC_528_KR2_CR2)           *speed = 35;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_100G_IEEE_KR2_CR2)                   *speed = 8;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_200G_BRCM_NO_FEC_KR4_CR4)            *speed = 31;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_200G_BRCM_KR4_CR4)                   *speed = 32;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_200G_IEEE_KR4_CR4)                   *speed = 9;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_200G_BRCM_FEC_272_CR4_KR4)           *speed = 40;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_200G_BRCM_FEC_272_N4)                *speed = 68;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_400G_BRCM_FEC_KR8_CR8)               *speed = 30;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_400G_BRCM_FEC_272_N8)                *speed = 71;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_150G_BRCM_N3)                        *speed = 67;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_300G_BRCM_N6)                        *speed = 69;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_350G_BRCM_N7)                        *speed = 70;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_12P12G_BRCM_KR1)                     *speed = 73;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_200G_BRCM_FEC_544_CR8_KR8)           *speed = 72;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_50G_BRCM_CR4)                        *speed = 22;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_50G_BRCM_KR4)                        *speed = 23;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_56)                     *speed = 56;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_57)                     *speed = 57;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_58)                     *speed = 58;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_59)                     *speed = 59;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_60)                     *speed = 60;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_61)                     *speed = 61;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_62)                     *speed = 62;
    if (spd_intf == TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_63)                     *speed = 63;
    if (*speed == -1) {
        return PHYMOD_E_UNAVAIL;
    }

    return PHYMOD_E_NONE;
}

uint32_t* tscbh_fe_gen2_spd_id_entry_get(void)
{
    return &tscbh_fe_gen2_spd_id_entry[0][0];
}

uint32_t* tscbh_fe_gen2_am_table_entry_get(void)
{
    return &tscbh_fe_gen2_am_table_entry[0][0];
}

uint32_t* tscbh_fe_gen2_um_table_entry_get(void)
{
    return &tscbh_fe_gen2_um_table_entry[0][0];
}

uint32_t* tscbh_fe_gen2_speed_priority_mapping_table_get(void)
{
    return &tscbh_fe_gen2_speed_priority_mapping_table[0][0];
}

uint32_t* tscbh_fe_gen2_spd_id_entry_100g_4lane_no_fec_get(void)
{
    return &tscbh_fe_gen2_spd_id_entry_100g_4lane_no_fec[0];
}
