/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        utils_arad_card.h
 * Purpose:     arad card Utilities Header.
 */

#ifndef __UTILITIES_UTILS_ARAD_CARD_H_INCLUDED__
/* { */
#define __UTILITIES_UTILS_ARAD_CARD_H_INCLUDED__

/*************                                                      
 * INCLUDES  *                                                      
 */                                                     
/* { */

#include <sal/types.h>
#include <appl/diag/dcmn/bsp_cards_consts.h>

/* } */
/*************
 * DEFINES   *
 */
/* { */

/* IO_AGENT_FPGA { */
#define ARAD_CARD_IO_AGENT_FPGA_FILE              "arad_card_io_agent.rbf"

/* I2C Adress */
#define ARAD_CARD_I2C_IO_AGENT_DEVICE_ADDR    0x41
#define ARAD_CARD_I2C_GPIO_DEVICE_ADDR             0x20

/*************
 * TYPE DEFS *
 */
/* { */

typedef enum
{
  ARAD_CARD_SYNT_FABRIC = 0,
  ARAD_CARD_SYNT_NIF = 1,
  ARAD_CARD_SYNT_DRAM = 2,
  ARAD_CARD_SYNT_CORE = 3,
  ARAD_CARD_NOF_SYNT_TYPES
} ARAD_CARD_SYNT_TYPE;

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

/* 
 * Board functions 
 */
uint32
    arad_card_fpga_io_regs_init(void);

uint32
  arad_card_init_host_board(
    const  SOC_BSP_CARDS_DEFINES          card_type
  );

uint32 
  arad_BOARD_SPECIFICATIONS_clear( 
    const SOC_BSP_CARDS_DEFINES  card_type 
  );

uint32 
  jericho_BOARD_SPECIFICATIONS_clear( 
    const SOC_BSP_CARDS_DEFINES  card_type 
  );

uint32 
  qmx_BOARD_SPECIFICATIONS_clear( 
    const SOC_BSP_CARDS_DEFINES  card_type 
  );

/* 
 * Board Utility Functions 
 */
uint32 card_power_monitor(int unit, int eq_sel);

/* 
 * Synt Functions 
 */
uint32
  arad_card_board_synt_set(
   const ARAD_CARD_SYNT_TYPE targetsynt,
   const uint32               targetfreq,
   const unsigned char         silent
  );

uint32 
  arad_card_board_synt_nominal_freq_get( 
    const ARAD_CARD_SYNT_TYPE   targetsynt 
  );
/* } */

#endif /*  __UTILITIES_UTILS_LINE_GFA_PETRA_H_INCLUDED__ */
