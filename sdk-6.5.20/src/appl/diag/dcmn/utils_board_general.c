/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#include <shared/bsl.h>

#include <sal/types.h>
#include <soc/drv.h>

#include <soc/dcmn/dcmn_utils_eeprom.h>

#include <appl/diag/dcmn/utils_board_general.h>
#include <appl/diag/dcmn/bsp_cards_consts.h>

static D_BOARD_CHIP_TYPE Board_chip_type=D_BOARD_CHIP_TYPE_LAST;

#if (defined(LINUX) || defined(UNIX)) || defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(BCM_PETRA_SUPPORT)
int
  host_board_type_from_nv(
    SOC_BSP_CARDS_DEFINES  *board_board_type_ptr
    )
{
    int ret = 0;

#ifdef PLISIM
    *board_board_type_ptr = LINE_CARD_GFA_PETRA_B_INTERLAKEN;
#else
#if defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__)
    ret = eeprom_read(NEGEV_CHASSIS_CARD_TYPE, NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_TYPE, NEGEV_CHASSIS_EEPROM_BYTE_SIZE_BOARD_TYPE, (int *)board_board_type_ptr);
    if (ret != 0) {
      cli_out("Error in %s(): eeprom_read(). FAILED !!!\n", FUNCTION_NAME());
    }
#endif
#endif

    return ret;
}
#endif

void
  board_set_device_type(
    D_BOARD_CHIP_TYPE board_chip_type
  )
{
  Board_chip_type = board_chip_type;
  return;
}

void
  board_get_device_type(
    D_BOARD_CHIP_TYPE *board_chip_type
  )
{
  *board_chip_type = Board_chip_type;
  return;
}


const char*
  utils_D_BOARD_CHIP_TYPE_to_string(
    D_BOARD_CHIP_TYPE board_chip_type
  )
{
  const char* str = 0;
  switch(board_chip_type)
  {
  case D_BOARD_CHIP_TYPE_FE200:
    str = "D_BOARD_CHIP_TYPE_FE200";
    break;
  case D_BOARD_CHIP_TYPE_FAP10M_A:
    str = "D_BOARD_CHIP_TYPE_FAP10M_A";
    break;
  case D_BOARD_CHIP_TYPE_FAP10M_B:
    str = "D_BOARD_CHIP_TYPE_FAP10M_B";
    break;
  case D_BOARD_CHIP_TYPE_FAP20V_A:
    str = "D_BOARD_CHIP_TYPE_FAP20V_A";
    break;
  case D_BOARD_CHIP_TYPE_FAP20V_B:
    str = "D_BOARD_CHIP_TYPE_FAP20V_B";
    break;
  case D_BOARD_CHIP_TYPE_FAP21V:
    str = "D_BOARD_CHIP_TYPE_FAP21V";
    break;
  case D_BOARD_CHIP_TYPE_PETRA:
    str = "D_BOARD_CHIP_TYPE_PETRA";
    break;
  case D_BOARD_CHIP_TYPE_PB:
    str = "D_BOARD_CHIP_TYPE_PB";
    break;
  case D_BOARD_CHIP_TYPE_LAST:
    str = "D_BOARD_CHIP_TYPE_LAST";
    break;
  default:
    str = "***utils_D_BOARD_CHIP_TYPE_to_string got error in input***";
  }
  return str;
}

int
  utils_print_board_info(
    void
  )
{
  D_BOARD_CHIP_TYPE
    board_chip_type;

  board_get_device_type(&board_chip_type);
/*  cli_out("Devices on board %s"); TODO!!*/
  return 0;
}
