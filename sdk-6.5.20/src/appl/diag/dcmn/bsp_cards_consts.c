/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


/*************
 * INCLUDES  *
 */
/* { */

#if !DUNE_BCM
#include "bsp_cards_consts.h"
#else
#include <appl/diag/dcmn/bsp_cards_consts.h>
#endif

/* } */

/*************
 * DEFINES   *
 */
/* { */

/* } */

/*************
 * TYPED EFS *
 */
/* { */

/* } */

/*************
 * GLOBALS   *
 */
/* { */

/* } */

/*************
 * FUNCTIONS *
 */
/* { */


/*****************************************************
*NAME
*
*  bsp_card_is_same_family
*FUNCTION:
*  Returns:
*    1 -- IFF the 2 card types are the same family.
*    0 -- IFF the 2 card types are NOT the same family.
*INPUT:
*  SOC_SAND_DIRECT:
*    const SOC_BSP_CARDS_DEFINES card_x -
*    const SOC_BSP_CARDS_DEFINES card_y -
*  SOC_SAND_INDIRECT:
*REMARKS:
*  None.
*SEE ALSO:
 */
unsigned int
  bsp_card_is_same_family(
    const SOC_BSP_CARDS_DEFINES card_x,
    const SOC_BSP_CARDS_DEFINES card_y
  )
{
  unsigned int
    same_familiy=0;

  switch(card_x)
  {
  case LINE_CARD_01:
  case LINE_CARD_FAP10M_B:
    if (
          (card_y == LINE_CARD_01) ||
          (card_y == LINE_CARD_FAP10M_B)
       )
    {
      same_familiy = 1;
    }
    else
    {
      same_familiy = 0;
    }
    break;

  case LINE_CARD_GFA_FAP20V:
  case LINE_CARD_GFA_MB_FAP20V:
    if (
          (card_y == LINE_CARD_GFA_FAP20V) ||
          (card_y == LINE_CARD_GFA_MB_FAP20V)
       )
    {
      same_familiy = 1;
    }
    else
    {
      same_familiy = 0;
    }
    break;

  case LINE_CARD_GFA_FAP21V:
  case LINE_CARD_GFA_MB_FAP21V:
    if (
          (card_y == LINE_CARD_GFA_FAP21V) ||
          (card_y == LINE_CARD_GFA_MB_FAP21V)
       )
    {
      same_familiy = 1;
    }
    else
    {
      same_familiy = 0;
    }
    break;

  case LINE_CARD_GFA_PETRA_DDR2:
  case LINE_CARD_GFA_PETRA_DDR3:
  case LINE_CARD_GFA_PETRA_DDR3_STREAMING:
  case LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3:
  case LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING:
    if (
      (card_y == LINE_CARD_GFA_PETRA_DDR2) ||
      (card_y == LINE_CARD_GFA_PETRA_DDR3) ||
      (card_y == LINE_CARD_GFA_PETRA_DDR3_STREAMING) ||
      (card_y == LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3) ||
      (card_y == LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING) ||
      (card_y == LOAD_BOARD_PB) ||
      (card_y == LINE_CARD_GFA_PETRA_A) ||
      (card_y == LINE_CARD_GFA_PETRA_X)
      )
    {
      same_familiy = 1;
    }
    else
    {
      same_familiy = 0;
    }
    break;
  case LINE_CARD_GFA_PETRA_B_DDR3:
  case LINE_CARD_GFA_PETRA_B_DDR3_STREAMING:
    if (
      (card_y == LINE_CARD_GFA_PETRA_DDR2) ||
      (card_y == LINE_CARD_GFA_PETRA_DDR3) ||
      (card_y == LINE_CARD_GFA_PETRA_DDR3_STREAMING) ||
      (card_y == LINE_CARD_GFA_PETRA_B_DDR3) ||
      (card_y == LINE_CARD_GFA_PETRA_B_DDR3_STREAMING) ||
      (card_y == LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3) ||
      (card_y == LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING) ||
      (card_y == LOAD_BOARD_PB) ||
      (card_y == LINE_CARD_GFA_PETRA_B) ||
      (card_y == LINE_CARD_GFA_PETRA_X)
      )
    {
      same_familiy = 1;
    }
    else
    {
      same_familiy = 0;
    }
    break;
  case LINE_CARD_GFA_PETRA_X:
  case LOAD_BOARD_PB:
    if (
        (card_y == LINE_CARD_GFA_PETRA_DDR2) ||
        (card_y == LINE_CARD_GFA_PETRA_DDR3) ||
        (card_y == LINE_CARD_GFA_PETRA_DDR3_STREAMING) ||
        (card_y == LINE_CARD_GFA_PETRA_B_DDR3) ||
        (card_y == LINE_CARD_GFA_PETRA_B_DDR3_STREAMING) ||
        (card_y == LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3) ||
        (card_y == LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING) ||
        (card_y == LOAD_BOARD_PB) ||
        (card_y == LINE_CARD_GFA_PETRA_A) ||
        (card_y == LINE_CARD_GFA_PETRA_B) ||
        (card_y == LINE_CARD_GFA_PETRA_X)
       )
    {
      same_familiy = 1;
    }
    else
    {
      same_familiy = 0;
    }
    break;
  case LINE_CARD_GFA_PETRA_A:
    if (
      (card_y == LINE_CARD_GFA_PETRA_DDR2) ||
      (card_y == LINE_CARD_GFA_PETRA_DDR3) ||
      (card_y == LINE_CARD_GFA_PETRA_DDR3_STREAMING) ||
      (card_y == LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3) ||
      (card_y == LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING) ||
      (card_y == LOAD_BOARD_PB) ||
      (card_y == LINE_CARD_GFA_PETRA_A) ||
      (card_y == LINE_CARD_GFA_PETRA_X)
      )
    {
      same_familiy = 1;
    }
    else
    {
      same_familiy = 0;
    }
    break;
  case LINE_CARD_GFA_PETRA_B:
    if (
      (card_y == LINE_CARD_GFA_PETRA_B_DDR3) ||
      (card_y == LINE_CARD_GFA_PETRA_B_DDR3_STREAMING) ||
      (card_y == LOAD_BOARD_PB) ||
      (card_y == LINE_CARD_GFA_PETRA_B) ||
      (card_y == LINE_CARD_GFA_PETRA_X)
      )
    {
      same_familiy = 1;
    }
    else
    {
      same_familiy = 0;
    }
    break;
  case LINE_CARD_TEVB:
  case FRONT_END_TEVB:
    if (
      (card_y == LINE_CARD_TEVB) ||
      (card_y == FRONT_END_TEVB)
      )
    {
      same_familiy = 1;
    }
    else
    {
      same_familiy = 0;
    }
    break;

  case FABRIC_CARD_FE600_01:
  case LOAD_BOARD_FE600:
    if (
      (card_y == FABRIC_CARD_FE600_01) ||
      (card_y == LOAD_BOARD_FE600)
      )
    {
      same_familiy = 1;
    }
    else
    {
      same_familiy = 0;
    }
    break;

  default:
    /*general case - only equality is considered same family*/
    if (card_x == card_y)
    {
      same_familiy = 1;
    }
    else
    {
      same_familiy = 0;
    }
  }

  return same_familiy;
}


/*****************************************************
*NAME
*   bsp_card_enum_to_str
*FUNCTION:
*  Convert SOC_BSP_CARDS_DEFINES to STR.
*INPUT:
*  SOC_SAND_DIRECT:
*    const SOC_BSP_CARDS_DEFINES card_type -
 */
const char *
  bsp_card_enum_to_str(
    const SOC_BSP_CARDS_DEFINES card_type
  )
{
  const char
    *str = "";

  switch(card_type)
  {
    case(STANDALONE_MEZZANINE):
      str = "Stand alone";
      break;
    case(FABRIC_CARD_01):
      str = "Fabric card";
      break;
    case(LINE_CARD_01):
      str = "Line card";
      break;
    case(FABRIC_CARD_FE600_01):
      str = "SOC_SAND_FE600 card";
      break;
    case(LINE_CARD_FAP10M_B):
      str = "Line card 10M-B";
      break;
    case(LINE_CARD_GFA_FAP20V):
      str = "Line card GFA SOC_SAND_FAP20V";
      break;
    case(LINE_CARD_GFA_FAP21V):
      str = "Line card GFA SOC_SAND_FAP21V";
      break;
    case(LINE_CARD_GFA_PETRA_DDR3):
      str = "Line card GFA FAP80 DDR3";
      break;
    case(LINE_CARD_GFA_PETRA_DDR2):
      str = "Line card GFA FAP80 DDR2";
      break;
    case(LINE_CARD_GFA_PETRA_DDR3_STREAMING):
      str = "Line card GFA FAP80 DDR3 Streaming";
      break;
    case(LINE_CARD_GFA_PETRA_B_DDR3):
      str = "Line card GFA FAP100 DDR3";
      break;
    case(LINE_CARD_GFA_PETRA_B_DDR3_STREAMING):
      str = "Line card GFA FAP100 DDR3 Streaming";
      break;
    case(LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3):
      str = "Line card GFA FAP100 DDR3 with FAP80 assembly";
      break;
    case(LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING):
      str = "Line card GFA FAP100 DDR Streaming with FAP80 assembly";
      break;
    case(LINE_CARD_GFA_MB_FAP20V):
      str = "Line card GFA MB";
      break;
    case(LINE_CARD_GFA_MB_FAP21V):
      str = "Line card GFA MB";
      break;
    case(FRONT_END_FTG):
      str = "Front End FTG";
      break;
    case(FRONT_END_TGS):
      str = "Front End TGS";
      break;
    case(SOC_FRONT_END_PTG):
      str = "Front End PTG";
      break;
    case(FAP_MEZ_GDDR1):
      str = "Fap Mezzanine GDDR1";
      break;
    case(FAP_MEZ_DDR2):
      str = "Fap Mezzanine DDR2";
      break;
    case(FRONT_END_TEVB):
      str = "Front End Timna Evalution Board";
      break;
    case(LINE_CARD_TEVB):
      str = "Line Card Timna Evalution Board";
      break;
    case(FAP_MEZ_FAP21V):
      str = "Fap21v Mezzanine";
      break;
    case(LOAD_BOARD_FE600):
      str = "Load Board for FE 600";
      break;
    case(LOAD_BOARD_PB):
      str = "Load Board for Soc_petra B";
      break;
    case(LINE_CARD_GFA_PETRA_A):
      str = "Soc_petra A line card familiy";
      break;
    case(LINE_CARD_GFA_PETRA_B):
      str = "Soc_petra B line card familiy";
      break;
    case(LINE_CARD_GFA_PETRA_X):
      str = "Soc_petra A or Soc_petra B line card familiy";
      break;
    case(LINE_CARD_GFA_PETRA_B_INTERLAKEN):
      str = "Line card GFA BI";
      break;
    case(LINE_CARD_GFA_PETRA_B_INTERLAKEN_2):
      str = "Line card GFA BI 2";
      break;
    case(FABRIC_CARD_FE1600):
      str = "Fabric card FE1600";
      break;
    case(FABRIC_CARD_FE1600_BCM88754):
      str = "Fabric card FE1600 BCM88754";
      break;
    case(NEGEV2_MNG_CARD):
      str = "Negev-2 Management Card";
      break;
    case(LINE_CARD_ARAD):
      str = "Line Card ARAD";
      break;
    case(LINE_CARD_ARAD_DVT):
      str = "Line Card ARAD DVT";
      break;
    case(LINE_CARD_ARAD_NOACP):
      str = "Line Card ARAD without ACP";
      break;
    case(LINE_LOOPBACK_CARD):
      str = "Line Card LoopBack";
      break;
    case(LINE_MESH_CARD):
      str = "Line Card Mesh";
      break;
    case(FABRIC_LOOPBACK_CARD):
      str = "Fabric Card LoopBack";
      break;
    case(CHASSIS_BACKPLANE_TYPE):
      str = "Negev-2 Chassis BackPlane";
      break;
    case(FABRIC_CARD_FE3200):
      str = "Fabric card FE200";
      break;
    case(SOC_UNKNOWN_CARD):
    default:
      str = "Unknown Card";
      break;
  }

  return str;
}

/* } */

