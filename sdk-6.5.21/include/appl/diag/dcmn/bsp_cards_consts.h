/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __APPL_DCMN_BSP_CARDS_CONSTS_H_INCLUDED__
/* { */
#define __APPL_DCMN_BSP_CARDS_CONSTS_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

/*************
 * INCLUDES  *
 */
/* { */

/* } */

/*************
 * DEFINES   *
 */
/* { */

/*
 * Number of devices on each of the cards.
 * INDEX_OF_BRD0012A_LINE_CARD
 */

/*
 * Maximal number of FAP devices on any DUNE board
 */

/*
 * Number of devices on each of the cards.
 * INDEX_OF_BRD002A_FABRIC_CARD
 */
#define BSP_NUM_FES_ON_BRD002A            2

/*
 * Maximal number of FE devices on any DUNE board
 */
#define SOC_BSP_MAX_NUM_FES_ON_BRD            BSP_NUM_FES_ON_BRD002A

/*
 * Maximal number of Dune devices on a Dune board.
 */

/*
 * Number of devices on each GFA line-card.
 */

/*
 * Number of devices on each GFA SOC_SAND_FAP21V line-card.
 */

/*
 * Maximal supported number of fabric cards for each GFA line-card.
 */



/*
 * The mezzanine board gets to reside on top of
 * plenty of hosts. The host updates its type
 * on its serial eeprom. These types should be
 * set there (by the nvram command in the application)
 */
typedef enum
{
  SOC_UNKNOWN_CARD                       = 0,
  STANDALONE_MEZZANINE               = 1,
  FABRIC_CARD_01                     = 1000,
  LINE_CARD_01                       = 2000,
  LINE_CARD_FAP10M_B                 = 2050,
  LINE_CARD_GFA_FAP20V               = 2100,
  LINE_CARD_GFA_MB_FAP20V            = 2101,
  LINE_CARD_GFA_FAP21V               = 2102,
  LINE_CARD_GFA_MB_FAP21V            = 2103,
  LINE_CARD_GFA_PETRA_DDR3           = 2104,
  LINE_CARD_GFA_PETRA_DDR3_STREAMING = 2105,
  LINE_CARD_GFA_PETRA_DDR2           = 2106,
  LINE_CARD_GFA_T20E                 = 2107,
  LINE_CARD_GFA_PETRA_B_DDR3                        = 2108,
  LINE_CARD_GFA_PETRA_B_DDR3_STREAMING              = 2109,
  LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3           = 2110,
  LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING = 2111,
  FRONT_END_FTG                      = 2200,
  FRONT_END_TGS                      = 2300,
  SOC_FRONT_END_PTG                      = 2350,
  FAP_MEZ_GDDR1                      = 2400,
  FAP_MEZ_DDR2                       = 2500,
  FRONT_END_TEVB                     = 2600,
  LINE_CARD_TEVB                     = 2700,
  FAP_MEZ_FAP21V                     = 2800,
  FABRIC_CARD_FE600_01               = 2900,
  LOAD_BOARD_FE600                   = 3000,
  LOAD_BOARD_PB                      = 3050,
  LINE_CARD_GFA_PETRA_B_INTERLAKEN   = 3100,
  LINE_CARD_GFA_PETRA_B_INTERLAKEN_2   = 3200,
  FABRIC_CARD_FE1600   = 3300,
  FABRIC_CARD_FE1600_BCM88754   = 3350,
  NEGEV2_MNG_CARD   = 3400,
  LINE_CARD_ARAD   = 3500,
  LINE_CARD_ARAD_DVT   = 3600,
  LINE_CARD_ARAD_NOACP  = 3700,
  LINE_LOOPBACK_CARD = 3800, /* 0xed8 */
  LINE_MESH_CARD = 3900, /* 0x73c */
  FABRIC_LOOPBACK_CARD = 4000, /* 0xfa0 */
  CHASSIS_BACKPLANE_TYPE = 4100, /* 0x1004 */
  FABRIC_CARD_FE3200 = 4400, /* 0x1130 */
  LINE_CARD_JERICHO   = 4500, /* 0x1194 */
  
 /*
  *    Generic identifier for Negev/Negev-2 family.
  *  Does not representh the specific GFA linecard or Facbric card, but the family of Negev cards.
  */
    NEGEV_CHASSIS_CARD_TYPE              = 0xfffb, 
 /*
  *    Generic identifier for Soc_petra A GFA-linecards family.
  *  Does not representh the specific GFA linecard, but the family of Soc_petra A linecards.
  */
  LINE_CARD_GFA_PETRA_A              = 0xfffc, 
 /*
  *    Generic identifier for Soc_petra B GFA-linecards family (including load board)
  *  Does not representh the specific GFA linecard, but the family of Soc_petra B linecards.
  */
  LINE_CARD_GFA_PETRA_B              = 0xfffd, 
 /*
  *    Generic identifier for any Soc_petra A or Soc_petra B GFA-linecards family.
  *  Does not representh the specific GFA linecard.
  */
  LINE_CARD_GFA_PETRA_X              = 0xfffe,
  SOC_INVALID_CARD                       = 0xFFFF /*LAST ONE - DO NOT TOUCH*/
} SOC_BSP_CARDS_DEFINES; /*This is up to 16 bits number - That is ansi-C SHORT*/




/* } */

/*************
 * TYPEDEFS  *
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

/*See comments in bsp_cards_defines.c */
unsigned int
  bsp_card_is_same_family(
    const SOC_BSP_CARDS_DEFINES card_x,
    const SOC_BSP_CARDS_DEFINES card_y
  );

const char *
  bsp_card_enum_to_str(
    const SOC_BSP_CARDS_DEFINES card_type
  );

/* } */


#ifdef  __cplusplus
}
#endif


/* } __APPL_DCMN_BSP_CARDS_CONSTS_H_INCLUDED__*/
#endif
