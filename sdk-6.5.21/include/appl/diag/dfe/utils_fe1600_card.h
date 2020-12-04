/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        utils_fe1600_card.h
 * Purpose:     fe1600 card Utilities Header.
 */

#ifndef __UTILITIES_UTILS_FE1600_CARD_H_INCLUDED__
/* { */
#define __UTILITIES_UTILS_FE1600_CARD_H_INCLUDED__

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
#define FE1600_CARD_IO_AGENT_FPGA_FILE              "fe1600_card_io_agent.rbf"

/* I2C Adress */
#define FE1600_CARD_I2C_IO_AGENT_DEVICE_ADDR    0x40
#define FE1600_CARD_I2C_FE1600_DEVICE_ADDR          0x44
#define FE1600_CARD_I2C_GPIO_DEVICE_ADDR             0x20

/*************
 * TYPE DEFS *
 */
/* { */

typedef enum
{
  FE1600_CARD_SYNT_SERDES_0 =   0,
  FE1600_CARD_SYNT_SERDES_1 =   1,
  FE1600_CARD_SYNT_SERDES_2 =   2,
  FE1600_CARD_SYNT_SERDES_3 =   3,
  FE1600_CARD_SYNT_CORE     =   4,
  FE1600_CARD_NOF_SYNT_TYPES
} FE1600_CARD_SYNT_TYPE;

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
    fe1600_card_fpga_io_regs_init(void);

uint32
  fe1600_card_init_host_board(
    const  SOC_BSP_CARDS_DEFINES          card_type
  );

uint32 
  fe1600_BOARD_SPECIFICATIONS_clear( 
    const SOC_BSP_CARDS_DEFINES  card_type 
  );

/* 
 * Board Utility Functions 
 */
uint32
    fe1600_card_power_monitor(
        int eq_sel
    );

uint32
    fe3200_card_power_monitor(
        int eq_sel
    );

/* 
 * Synt Functions 
 */
uint32
  fe1600_card_board_synt_set(
   const FE1600_CARD_SYNT_TYPE targetsynt,
   const uint32               targetfreq,
   const unsigned char         silent
  );

uint32 
  fe1600_card_board_synt_nominal_freq_get( 
    const FE1600_CARD_SYNT_TYPE   targetsynt 
  );

uint32 fe3200_card_board_synt_set(uint32 targetfreq, uint32 nominalfreq);
/* } */

#endif /*  __UTILITIES_UTILS_LINE_GFA_PETRA_H_INCLUDED__ */
