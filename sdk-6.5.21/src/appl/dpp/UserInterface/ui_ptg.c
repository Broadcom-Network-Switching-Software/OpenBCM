/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <appl/diag/dpp/ref_sys.h>
#if !DUNE_BCM
#include <bcm_app/dpp/../H/usrApp.h>
#endif

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_ptg.h>

#include <appl/diag/dpp/utils_line_PTG.h>

SOC_SAND_RET utils_ptg_fpga_io_regs_init();

int
  ui_ptg_mdio_get(
    CURRENT_LINE *current_line
  )
{
  uint8
    phy_ndx = 0;
  uint32
    address = 0;
  uint16
    data;

  UI_MACROS_INIT_FUNCTION("ui_ptg_mdio_get");
  soc_sand_proc_name = "ui_ptg_mdio_get";

#ifdef WIN32
  utils_ptg_fpga_io_regs_init();
#endif

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PTG_MDIO_GET_PHY_NDX,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PTG_MDIO_GET_PHY_NDX);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    phy_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen("*** ptg command with unknown parameters'.\r\n    Syntax error/sw error...\r\n", TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PTG_MDIO_GET_ADDRESS,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PTG_MDIO_GET_ADDRESS);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    address = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen("*** ptg command with unknown parameters'.\r\n    Syntax error/sw error...\r\n", TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

  data = utils_ptg_mdio_get(phy_ndx, address);
  soc_sand_os_printf( "mdio_get phy_ndx %d address 0x%lx: 0x%x\n", phy_ndx, address, data);
  goto exit;

exit:
  return ui_ret;
}

int
  ui_ptg_mdio_set(
    CURRENT_LINE *current_line
  )
{
  uint8
    phy_ndx;
  uint32
    address;
  uint16
    data = 0;

  UI_MACROS_INIT_FUNCTION("ui_ptg_mdio_set");
  soc_sand_proc_name = "ui_ptg_mdio_set";

#ifdef WIN32
  utils_ptg_fpga_io_regs_init();
#endif

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PTG_MDIO_SET_PHY_NDX,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PTG_MDIO_SET_PHY_NDX);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    phy_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen("*** ptg command with unknown parameters'.\r\n    Syntax error/sw error...\r\n", TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PTG_MDIO_SET_ADDRESS,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PTG_MDIO_SET_ADDRESS);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    address = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen("*** ptg command with unknown parameters'.\r\n    Syntax error/sw error...\r\n", TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PTG_MDIO_SET_DATA,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PTG_MDIO_SET_DATA);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data = (uint16)param_val->value.ulong_value;
  }

  utils_ptg_mdio_set(phy_ndx, address, data);
  data = utils_ptg_mdio_get(phy_ndx, address);
  soc_sand_os_printf( "mdio_set phy_ndx %d address 0x%lx: 0x%x\n", phy_ndx, address, data);
  goto exit;

exit:
  return ui_ret;
}

int
  ui_ptg_ptg_init(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_ptg_ptg_init");
  soc_sand_proc_name = "ui_ptg_ptg_init";

#ifdef WIN32
  utils_ptg_fpga_io_regs_init();
#endif

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PTG_PTG_INIT_START_SERVER_ID,1))
  {
    utils_ptg_ptg_init(TRUE);
  }
  else
  {
    utils_ptg_ptg_init(FALSE);
  }
  goto exit;

exit:
  return ui_ret;
}

int
  ui_ptg_phy_init(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_ptg_phy_init");
  soc_sand_proc_name = "ui_ptg_phy_init";

#ifdef WIN32
  utils_ptg_fpga_io_regs_init();
#endif

  utils_ptg_phy_init();
  goto exit;

exit:
  return ui_ret;
}

int
  subject_ptg(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  UI_MACROS_INIT_FUNCTION("subject_ptg");
  soc_sand_proc_name = "subject_ptg";

  proc_name = "subject_ptg" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_ptg()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_ptg').
   */

  send_array_to_screen("\r\n",2) ;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PTG_MDIO_SET,1))
  {
    ui_ret = ui_ptg_mdio_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PTG_MDIO_GET,1))
  {
    ui_ret = ui_ptg_mdio_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PTG_PHY_INIT_ID,1))
  {
    ui_ret = ui_ptg_phy_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PTG_PTG_INIT_ID,1))
  {
    ui_ret = ui_ptg_ptg_init(current_line);
  }
  else
  {
    send_string_to_screen(
      "\r\n"
      "*** ptg command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

