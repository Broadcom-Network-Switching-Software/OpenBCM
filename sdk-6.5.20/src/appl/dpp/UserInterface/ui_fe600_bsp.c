/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#if !DUNE_BCM

#include <appl/diag/dpp/ref_sys.h>
#include <bcm_app/dpp/../H/usrApp.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_fe600_bsp.h>

#include <appl/diag/dpp/utils_defi.h>
#include <appl/diag/dpp/utils_fe600_card.h>

extern
  uint32
    Default_unit;

STATIC uint32
  get_default_unit()
{
  return Default_unit;
}

int
  ui_fe600_bsp_gen_i2c(
    CURRENT_LINE *current_line
  )
{
  int32
    ret;
  uint8
    address,
    data[FE600_BSP_I2C_BUFFER_SIZE];
  unsigned short
    nof_bytes;

  UI_MACROS_INIT_FUNCTION("ui_fe600_bsp_gen_i2c");
  soc_sand_proc_name = "ui_fe600_bsp_gen_i2c";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_BYTE_0,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_BYTE_0);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data[0] = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_BYTE_1,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_BYTE_1);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data[1] = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_BYTE_2,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_BYTE_2);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data[2] = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_BYTE_3,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_BYTE_3);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data[3] = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_BYTE_4,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_BYTE_4);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data[4] = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_BYTE_5,1))
  {
    data[5] = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_BYTE_5,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_BYTE_5);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data[6] = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_BYTE_6,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_BYTE_6);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data[7] = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_BYTE_7,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_BYTE_7);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data[8] = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_BYTE_8,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_BYTE_8);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data[9] = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_GEN_READ,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_GEN_READ);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    address = (uint8)param_val->value.ulong_value;
    ret = fe600_bsp_i2c_read_gen(
      address,data,&nof_bytes
      );
    if (ret != SOC_SAND_OK)
    {
      send_string_to_screen(" *** ui_fe600_bsp_gen_i2c - FAIL", TRUE);
      goto exit;
    }
    else
    {
      soc_sand_os_printf( 
        "read %d bytes from 0x%x: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n\r",
        nof_bytes,
        address,
        data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9]
        );
    }
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_GEN_WRITE,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_GEN_WRITE);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    address = (uint8)param_val->value.ulong_value;

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_NOF_BYTES,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_NOF_BYTES);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      nof_bytes = (uint8)param_val->value.ulong_value;
    }
    else
    {
      send_string_to_screen(" *** need nof bytes to perform I2C write - FAIL", TRUE);
      goto exit;
    }

    ret = fe600_bsp_i2c_write_gen(
      address,data,nof_bytes
      );
    if (ret != SOC_SAND_OK)
    {
      send_string_to_screen(" *** ui_fe600_bsp_gen_i2c - FAIL", TRUE);
      goto exit;
    }
    else
    {
      soc_sand_os_printf( 
        "Wrote %d bytes to 0x%x: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n\r",
        nof_bytes,
        address,
        data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9]
        );
    }
  }

  send_string_to_screen(" *** ui_fe600_bsp_gen_i2c - SUCCEEDED", TRUE);

exit:
  return ui_ret;
}

int
  ui_fe600_bsp_fe600_i2c(
    CURRENT_LINE *current_line
  )
{
  int32
    ret;
  unsigned short
    address;
  unsigned long
    data;

  UI_MACROS_INIT_FUNCTION("ui_fe600_bsp_fe600_i2c");
  soc_sand_proc_name = "ui_fe600_bsp_fe600_i2c";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_FE600_READ,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_FE600_READ);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    address = (unsigned short)param_val->value.ulong_value;
    ret = fe600_bsp_i2c_read_fe600(
      address, &data
      );
    if (ret != SOC_SAND_OK)
    {
      send_string_to_screen(" *** fe600_bsp_i2c_read_fe600 - FAIL", TRUE);
      goto exit;
    }
    else
    {
      soc_sand_os_printf( 
        "Read SOC_SAND_FE600 0x%x: 0x%x\n\r",
        address,
        data
      );
    }
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_FE600_WRITE,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_FE600_WRITE);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    address = (unsigned short)param_val->value.ulong_value;

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_VAL,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_I2C_VAL);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      data = (unsigned long)param_val->value.ulong_value;
    }
    else
    {
      send_string_to_screen(" *** need data to write - FAIL", TRUE);
      goto exit;
    }

    ret = fe600_bsp_i2c_write_fe600(
      address,data
      );
    if (ret != SOC_SAND_OK)
    {
      send_string_to_screen(" *** ui_fe600_bsp_gen_i2c - FAIL", TRUE);
      goto exit;
    }
    else
    {
      soc_sand_os_printf( 
        "Wrote SOC_SAND_FE600 0x%x: 0x%x\n\r",
        address,
        data
        );
    }
  }
  send_string_to_screen(" *** ui_fe600_bsp_fe600_i2c - SUCCEEDED", TRUE);

exit:
  return ui_ret;
}

int
  ui_fe600_bsp_reset_device(
    CURRENT_LINE *current_line
  )
{
  int32
    ret;
  uint32
    remain_reset;

  UI_MACROS_INIT_FUNCTION("ui_fe600_bsp_reset_device");
  soc_sand_proc_name = "ui_fe600_bsp_reset_device";


  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_RESET_DEVICE_REMAIN_OFF,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_RESET_DEVICE_REMAIN_OFF);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    remain_reset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** need remain_reset - FAIL", TRUE);
    goto exit;
  }

  ret = fe600_bsp_fe600_reset(
    remain_reset
    );
  if (ret != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_fe600_bsp_gen_i2c - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** ui_fe600_bsp_reset_device - SUCCEEDED", TRUE);

exit:
  return ui_ret;
}
int
  ui_fe600_synt_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_TG_SYNT_TYPE
    syntsizer;
  uint32
    targetfreq;

  UI_MACROS_INIT_FUNCTION("ui_fe600_synt_set");
  soc_sand_proc_name = "fe600_synt_set";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_SYNT_SET_SYN_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_BSP_SYNT_SET_SYN_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    syntsizer = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter freq_val after synt_type***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_SYNT_SET_FREQ_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_BSP_SYNT_SET_FREQ_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    targetfreq = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter freq_val after synt_type***", TRUE);
    goto exit;
  }

  ret = fe600_bsp_synt_set(
          syntsizer,
          targetfreq,
          FALSE
        );
  if (ret)
  {
    send_string_to_screen(" *** fe600_bsp_synt_set - FAIL", TRUE);
  }
exit:
  return ui_ret;
}

int
  ui_fe600_bsp_init_host_board(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  unsigned short
    card_type;

  UI_MACROS_INIT_FUNCTION("ui_fe600_bsp_init_host_board");
  soc_sand_proc_name = "fe600_bsp_init_host_board";

  host_board_type_from_nv(&card_type); 
  ret = fe600_bsp_init_host_board(card_type);
  if (ret)
  {
    send_string_to_screen(" *** fe600_bsp_init_host_board - FAIL", TRUE);
  }

  return ui_ret;
}

int
  subject_fe600_bsp(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  UI_MACROS_INIT_FUNCTION("subject_fe600_bsp");
  soc_sand_proc_name = "subject_fe600_bsp";

  proc_name = "subject_fe600_bsp" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_fe600_bsp()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_fap21v_api').
   */

  send_array_to_screen("\r\n",2) ;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_GEN_READ,1) ||
     !search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_GEN_WRITE,1)
    )
  {
    ui_ret = ui_fe600_bsp_gen_i2c(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_FE600_READ,1) ||
    !search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_I2C_FE600_WRITE,1)
    )
  {
    ui_ret = ui_fe600_bsp_fe600_i2c(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_RESET_DEVICE,1))
  {
    ui_ret = ui_fe600_bsp_reset_device(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_INIT_HOST_BOARD,1))
  {
    ui_ret = ui_fe600_bsp_init_host_board(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_BSP_SYNT_SET_ID,1))
  {
    ui_ret = ui_fe600_synt_set(current_line);
  }
  else
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

#else
int ui_fe600_bsp = 0;
#endif
