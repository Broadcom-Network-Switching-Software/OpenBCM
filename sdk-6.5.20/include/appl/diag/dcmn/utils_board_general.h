/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UTILS_BOARD_GEN_H_INCLUDED__
/* { */
#define __UTILS_BOARD_GEN_H_INCLUDED__

#include <appl/diag/dcmn/bsp_cards_consts.h> 

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * Sometimes, the application need to know the device type,
 * before the driver is initialized.
 * D_BOARD_CHIP_TYPE holds the expected device type, according
 * to the board information,
 */
typedef enum
{
  D_BOARD_CHIP_TYPE_FE200,
  D_BOARD_CHIP_TYPE_FAP10M_A,
  D_BOARD_CHIP_TYPE_FAP10M_B,
  D_BOARD_CHIP_TYPE_FAP20V_A,
  D_BOARD_CHIP_TYPE_FAP20V_B,
  D_BOARD_CHIP_TYPE_FAP20M,
  D_BOARD_CHIP_TYPE_FAP21V,
  D_BOARD_CHIP_TYPE_PETRA,
  D_BOARD_CHIP_TYPE_FE600,
  D_BOARD_CHIP_TYPE_T20E,
  D_BOARD_CHIP_TYPE_PB,
  D_BOARD_CHIP_TYPE_LAST

} D_BOARD_CHIP_TYPE;

void
  board_set_device_type(
    D_BOARD_CHIP_TYPE board_chip_type
  );
void
  board_get_device_type(
    D_BOARD_CHIP_TYPE *board_chip_type
  );
const char*
  utils_D_BOARD_CHIP_TYPE_to_string(
    D_BOARD_CHIP_TYPE board_chip_type
  );
int
  utils_print_board_info(
    void
  );

int
  host_board_type_from_nv(
    SOC_BSP_CARDS_DEFINES  *board_board_type_ptr
    ) ;

#ifdef  __cplusplus
}
#endif


/* } __UTILS_BOARD_GEN_H_INCLUDED__*/
#endif

