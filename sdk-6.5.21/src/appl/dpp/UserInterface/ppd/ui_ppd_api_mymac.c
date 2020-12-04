/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

 
/* 
 * Utilities include file. 
 */ 
#include <shared/bsl.h> 
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 

  
#include <soc/dpp/PPD/ppd_api_mymac.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_mymac.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_MYMAC
/********************************************************************
*  Function handler: msb_set (section mymac)
 */
int
ui_ppd_api_mymac_msb_set(
                         CURRENT_LINE *current_line
                         )
{
  uint32
    ret;
  SOC_SAND_PP_MAC_ADDRESS
    prm_my_mac_msb;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_mymac_msb_set";

  unit = soc_ppd_get_default_unit();
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_my_mac_msb);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_mymac_msb_get(
    unit,
    &prm_my_mac_msb
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_msb_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_msb_get");
    goto exit;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_MYMAC_MSB_SET_MSB_SET_MY_MAC_MSB_ID,1,
    &param_val,VAL_TEXT,err_msg))

  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_my_mac_msb));
  }

  /* Call function */
  ret = soc_ppd_mymac_msb_set(
    unit,
    &prm_my_mac_msb
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_msb_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_msb_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: msb_get (section mymac)
 */
int
ui_ppd_api_mymac_msb_get(
                         CURRENT_LINE *current_line
                         )
{
  uint32
    ret;
  SOC_SAND_PP_MAC_ADDRESS
    prm_my_mac_msb;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_mymac_msb_get";

  unit = soc_ppd_get_default_unit();
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_my_mac_msb);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_mymac_msb_get(
    unit,
    &prm_my_mac_msb
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_msb_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_msb_get");
    goto exit;
  }

  send_string_to_screen("--> my_mac_msb:", TRUE);
  soc_sand_SAND_PP_MAC_ADDRESS_print(&prm_my_mac_msb);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: vsi_lsb_set (section mymac)
 */
int
ui_ppd_api_mymac_vsi_lsb_set(
                             CURRENT_LINE *current_line
                             )
{
  uint32
    ret;
  SOC_PPC_VSI_ID   
    prm_vsi_ndx;
  SOC_SAND_PP_MAC_ADDRESS
    prm_my_mac_lsb;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_mymac_vsi_lsb_set";

  unit = soc_ppd_get_default_unit();
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_my_mac_lsb);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VSI_LSB_SET_VSI_LSB_SET_VSI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MYMAC_VSI_LSB_SET_VSI_LSB_SET_VSI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsi_ndx after vsi_lsb_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_mymac_vsi_lsb_get(
    unit,
    prm_vsi_ndx,
    &prm_my_mac_lsb
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_vsi_lsb_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_vsi_lsb_get");
    goto exit;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_MYMAC_VSI_LSB_SET_VSI_LSB_SET_MY_MAC_LSB_ID,1,
    &param_val,VAL_TEXT,err_msg))

  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_my_mac_lsb));
  }

  /* Call function */
  ret = soc_ppd_mymac_vsi_lsb_set(
    unit,
    prm_vsi_ndx,
    &prm_my_mac_lsb
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_vsi_lsb_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_vsi_lsb_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: vsi_lsb_get (section mymac)
 */
int
ui_ppd_api_mymac_vsi_lsb_get(
                             CURRENT_LINE *current_line
                             )
{
  uint32
    ret;
  SOC_PPC_VSI_ID   
    prm_vsi_ndx;
  SOC_SAND_PP_MAC_ADDRESS
    prm_my_mac_lsb;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_mymac_vsi_lsb_get";

  unit = soc_ppd_get_default_unit();
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_my_mac_lsb);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VSI_LSB_GET_VSI_LSB_GET_VSI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MYMAC_VSI_LSB_GET_VSI_LSB_GET_VSI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsi_ndx after vsi_lsb_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_mymac_vsi_lsb_get(
    unit,
    prm_vsi_ndx,
    &prm_my_mac_lsb
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_vsi_lsb_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_vsi_lsb_get");
    goto exit;
  }

  send_string_to_screen("--> my_mac_lsb:", TRUE);
  soc_sand_SAND_PP_MAC_ADDRESS_print(&prm_my_mac_lsb);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: vrrp_info_set (section mymac)
 */
int
ui_ppd_api_mymac_vrrp_info_set(
                               CURRENT_LINE *current_line
                               )
{
  uint32
    ret;
  SOC_PPC_MYMAC_VRRP_INFO
    prm_vrrp_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_mymac_vrrp_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_MYMAC_VRRP_INFO_clear(&prm_vrrp_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_mymac_vrrp_info_get(
    unit,
    &prm_vrrp_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_vrrp_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_vrrp_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VRRP_INFO_SET_VRRP_INFO_SET_VRRP_INFO_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MYMAC_VRRP_INFO_SET_VRRP_INFO_SET_VRRP_INFO_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vrrp_info.mode = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VRRP_INFO_SET_VRRP_INFO_SET_VRRP_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MYMAC_VRRP_INFO_SET_VRRP_INFO_SET_VRRP_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vrrp_info.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VRRP_INFO_SET_VRRP_INFO_SET_VRRP_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MYMAC_VRRP_INFO_SET_VRRP_INFO_SET_VRRP_INFO_IPV6_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vrrp_info.ipv6_enable = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_MYMAC_VRRP_INFO_SET_VRRP_INFO_SET_VRRP_INFO_VRID_MY_MAC_MSB_ID,1,
    &param_val,VAL_TEXT,err_msg))

  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_vrrp_info.vrid_my_mac_msb));
  }

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_MYMAC_VRRP_INFO_SET_VRRP_INFO_SET_VRRP_INFO_IPV6_VRID_MY_MAC_MSB_ID,1,
    &param_val,VAL_TEXT,err_msg))

  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_vrrp_info.ipv6_vrid_my_mac_msb));
  }

  /* Call function */
  ret = soc_ppd_mymac_vrrp_info_set(
    unit,
    &prm_vrrp_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_vrrp_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_vrrp_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: vrrp_info_get (section mymac)
 */
int
ui_ppd_api_mymac_vrrp_info_get(
                               CURRENT_LINE *current_line
                               )
{
  uint32
    ret;
  SOC_PPC_MYMAC_VRRP_INFO
    prm_vrrp_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_mymac_vrrp_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_MYMAC_VRRP_INFO_clear(&prm_vrrp_info);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_mymac_vrrp_info_get(
    unit,
    &prm_vrrp_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_vrrp_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_vrrp_info_get");
    goto exit;
  }

  send_string_to_screen("--> vrrp_info:", TRUE);
  SOC_PPC_MYMAC_VRRP_INFO_print(&prm_vrrp_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: vrrp_mac_set (section mymac)
 */
int
ui_ppd_api_mymac_vrrp_mac_set(
                              CURRENT_LINE *current_line
                              )
{
  uint32
    ret;
  uint32
    prm_vrrp_id_ndx;
  SOC_SAND_PP_MAC_ADDRESS   
    prm_vrrp_mac_lsb_key;
  uint8   
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_mymac_vrrp_mac_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VRRP_MAC_SET_VRRP_MAC_SET_VRRP_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MYMAC_VRRP_MAC_SET_VRRP_MAC_SET_VRRP_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vrrp_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vrrp_id_ndx after vrrp_mac_set***", TRUE);
    goto exit;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_MYMAC_VRRP_MAC_SET_VRRP_MAC_SET_VRRP_MAC_LSB_KEY_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_vrrp_mac_lsb_key));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrrp_mac_lsb_key after vrrp_mac_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mymac_vrrp_mac_get(
          unit,
          prm_vrrp_id_ndx,
          &prm_vrrp_mac_lsb_key,
          &prm_enable
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_vrrp_mac_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_vrrp_mac_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VRRP_MAC_SET_VRRP_MAC_SET_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MYMAC_VRRP_MAC_SET_VRRP_MAC_SET_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_mymac_vrrp_mac_set(
    unit,
          prm_vrrp_id_ndx,
          &prm_vrrp_mac_lsb_key,
          prm_enable
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_vrrp_mac_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_vrrp_mac_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: vrrp_mac_get (section mymac)
 */
int
ui_ppd_api_mymac_vrrp_mac_get(
                              CURRENT_LINE *current_line
                              )
{
  uint32
    ret;
  uint32
    prm_vrrp_id_ndx;
  SOC_SAND_PP_MAC_ADDRESS   
    prm_vrrp_mac_lsb_key;
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac"); 
  soc_sand_proc_name = "soc_ppd_mymac_vrrp_mac_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VRRP_MAC_GET_VRRP_MAC_GET_VRRP_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MYMAC_VRRP_MAC_GET_VRRP_MAC_GET_VRRP_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrrp_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrrp_id_ndx after vrrp_mac_get***", TRUE); 
    goto exit; 
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_MYMAC_VRRP_MAC_GET_VRRP_MAC_GET_VRRP_MAC_LSB_KEY_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_vrrp_mac_lsb_key));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrrp_mac_lsb_key after vrrp_mac_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_mymac_vrrp_mac_get(
          unit,
          prm_vrrp_id_ndx,
          &prm_vrrp_mac_lsb_key,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  {
    send_string_to_screen(" *** soc_ppd_mymac_vrrp_mac_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_vrrp_mac_get");
    goto exit;
  }

  cli_out("enable: %u\n\r",prm_enable);
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: trill_info_set (section mymac)
 */
int
ui_ppd_api_mymac_trill_info_set(
                                CURRENT_LINE *current_line
                                )
{
  uint32
    ret;
  SOC_PPC_MYMAC_TRILL_INFO
    prm_trill_info;
  uint32
    virtual_nick_name_index = -1;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_mymac_trill_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_MYMAC_TRILL_INFO_clear(&prm_trill_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_mymac_trill_info_get(
    unit,
    &prm_trill_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_trill_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_trill_info_get");
    goto exit;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_MYMAC_TRILL_INFO_SET_TRILL_INFO_SET_TRILL_INFO_ALL_ESADI_RBRIDGES_ID,1,
    &param_val,VAL_TEXT,err_msg))

  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_trill_info.all_esadi_rbridges));
  }
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_MYMAC_TRILL_INFO_SET_TRILL_INFO_SET_TRILL_INFO_ALL_RBRIDGES_MAC_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_trill_info.all_rbridges_mac));
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_TRILL_INFO_SET_TRILL_INFO_SET_TRILL_INFO_MY_NICK_NAME_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MYMAC_TRILL_INFO_SET_TRILL_INFO_SET_TRILL_INFO_MY_NICK_NAME_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_info.my_nick_name = (uint32)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_TRILL_INFO_SET_TRILL_INFO_SET_TRILL_INFO_VIRTUAL_NICK_NAME_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MYMAC_TRILL_INFO_SET_TRILL_INFO_SET_TRILL_INFO_VIRTUAL_NICK_NAME_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    virtual_nick_name_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_TRILL_INFO_SET_TRILL_INFO_SET_TRILL_INFO_VIRTUAL_NICK_NAME_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MYMAC_TRILL_INFO_SET_TRILL_INFO_SET_TRILL_INFO_VIRTUAL_NICK_NAME_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

    if (virtual_nick_name_index == -1) {
      send_string_to_screen(" *** SW error - expecting parameter virtual_nick_name_index before virtual_nick_name value***", TRUE); 
      goto exit; 
    }
    prm_trill_info.virtual_nick_names[virtual_nick_name_index] = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_mymac_trill_info_set(
    unit,
    &prm_trill_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_trill_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_trill_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: trill_info_get (section mymac)
 */
int
ui_ppd_api_mymac_trill_info_get(
                                CURRENT_LINE *current_line
                                )
{
  uint32
    ret;
  SOC_PPC_MYMAC_TRILL_INFO
    prm_trill_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_mymac_trill_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_MYMAC_TRILL_INFO_clear(&prm_trill_info);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_mymac_trill_info_get(
    unit,
    &prm_trill_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_mymac_trill_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_mymac_trill_info_get");
    goto exit;
  }

  send_string_to_screen("--> trill_info:", TRUE);
  SOC_PPC_MYMAC_TRILL_INFO_print(&prm_trill_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: lif_my_bmac_msb_set (section mymac)
 */
int
ui_ppd_api_mymac_lif_my_bmac_msb_set(
                                     CURRENT_LINE *current_line
                                     )
{
  uint32
    ret;
  SOC_SAND_PP_MAC_ADDRESS
    prm_my_bmac_msb;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_lif_my_bmac_msb_set";

  unit = soc_ppd_get_default_unit();
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_my_bmac_msb);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_my_bmac_msb_get(
    unit,
    &prm_my_bmac_msb
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_my_bmac_msb_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_my_bmac_msb_get");
    goto exit;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_LIF_MY_BMAC_MSB_SET_LIF_MY_BMAC_MSB_SET_MY_BMAC_MSB_ID,1,
    &param_val,VAL_TEXT,err_msg))

  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_my_bmac_msb));
  }

  /* Call function */
  ret = soc_ppd_lif_my_bmac_msb_set(
    unit,
    &prm_my_bmac_msb
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_my_bmac_msb_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_my_bmac_msb_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: lif_my_bmac_msb_get (section mymac)
 */
int
ui_ppd_api_mymac_lif_my_bmac_msb_get(
                                     CURRENT_LINE *current_line
                                     )
{
  uint32
    ret;
  SOC_SAND_PP_MAC_ADDRESS
    prm_my_bmac_msb;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_lif_my_bmac_msb_get";

  unit = soc_ppd_get_default_unit();
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_my_bmac_msb);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_lif_my_bmac_msb_get(
    unit,
    &prm_my_bmac_msb
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_my_bmac_msb_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_my_bmac_msb_get");
    goto exit;
  }

  send_string_to_screen("--> my_bmac_msb:", TRUE);
  soc_sand_SAND_PP_MAC_ADDRESS_print(&prm_my_bmac_msb);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: lif_my_bmac_port_lsb_set (section mymac)
 */
int
ui_ppd_api_mymac_lif_my_bmac_port_lsb_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYS_PORT_ID
    prm_src_sys_port_ndx;
  SOC_SAND_PP_MAC_ADDRESS
    prm_my_mac_lsb;
  uint8
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_lif_my_bmac_port_lsb_set";

  unit = soc_ppd_get_default_unit();
  soc_sand_SAND_PP_SYS_PORT_ID_clear(&prm_src_sys_port_ndx);
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_my_mac_lsb);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_SET_LIF_MY_BMAC_PORT_LSB_SET_SRC_SYS_PORT_NDX_SYS_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_SET_LIF_MY_BMAC_PORT_LSB_SET_SRC_SYS_PORT_NDX_SYS_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_src_sys_port_ndx.sys_id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_SET_LIF_MY_BMAC_PORT_LSB_SET_SRC_SYS_PORT_NDX_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_SET_LIF_MY_BMAC_PORT_LSB_SET_SRC_SYS_PORT_NDX_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_src_sys_port_ndx.sys_port_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter src_sys_port_ndx after lif_my_bmac_port_lsb_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_my_bmac_port_lsb_get(
    unit,
    &prm_src_sys_port_ndx,
    &prm_my_mac_lsb,
    &prm_enable
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_my_bmac_port_lsb_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_my_bmac_port_lsb_get");
    goto exit;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_SET_LIF_MY_BMAC_PORT_LSB_SET_MY_MAC_LSB_ID,1,
    &param_val,VAL_TEXT,err_msg))

  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_my_mac_lsb));
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_SET_LIF_MY_BMAC_PORT_LSB_SET_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_SET_LIF_MY_BMAC_PORT_LSB_SET_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_enable = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_my_bmac_port_lsb_set(
    unit,
    &prm_src_sys_port_ndx,
    &prm_my_mac_lsb,
    prm_enable
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_my_bmac_port_lsb_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_my_bmac_port_lsb_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: lif_my_bmac_port_lsb_get (section mymac)
 */
int
ui_ppd_api_mymac_lif_my_bmac_port_lsb_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYS_PORT_ID
    prm_src_sys_port_ndx;
  SOC_SAND_PP_MAC_ADDRESS
    prm_my_mac_lsb;
  uint8
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");
  soc_sand_proc_name = "soc_ppd_lif_my_bmac_port_lsb_get";

  unit = soc_ppd_get_default_unit();
  soc_sand_SAND_PP_SYS_PORT_ID_clear(&prm_src_sys_port_ndx);
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_my_mac_lsb);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_GET_LIF_MY_BMAC_PORT_LSB_GET_SRC_SYS_PORT_NDX_SYS_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_GET_LIF_MY_BMAC_PORT_LSB_GET_SRC_SYS_PORT_NDX_SYS_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_src_sys_port_ndx.sys_id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_GET_LIF_MY_BMAC_PORT_LSB_GET_SRC_SYS_PORT_NDX_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_GET_LIF_MY_BMAC_PORT_LSB_GET_SRC_SYS_PORT_NDX_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_src_sys_port_ndx.sys_port_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter src_sys_port_ndx after lif_my_bmac_port_lsb_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_my_bmac_port_lsb_get(
    unit,
    &prm_src_sys_port_ndx,
    &prm_my_mac_lsb,
    &prm_enable
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_my_bmac_port_lsb_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_my_bmac_port_lsb_get");
    goto exit;
  }

  soc_sand_SAND_PP_MAC_ADDRESS_print(&prm_my_mac_lsb);

  cli_out("enable: %u\n",prm_enable);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Section handler: mymac
 */
int
ui_ppd_api_mymac(
                 CURRENT_LINE *current_line
                 )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mymac");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_MSB_SET_MSB_SET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_msb_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_MSB_GET_MSB_GET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_msb_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VSI_LSB_SET_VSI_LSB_SET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_vsi_lsb_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VSI_LSB_GET_VSI_LSB_GET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_vsi_lsb_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VRRP_INFO_SET_VRRP_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_vrrp_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VRRP_INFO_GET_VRRP_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_vrrp_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VRRP_MAC_SET_VRRP_MAC_SET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_vrrp_mac_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_VRRP_MAC_GET_VRRP_MAC_GET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_vrrp_mac_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_TRILL_INFO_SET_TRILL_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_trill_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MYMAC_TRILL_INFO_GET_TRILL_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_trill_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_MY_BMAC_MSB_SET_LIF_MY_BMAC_MSB_SET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_lif_my_bmac_msb_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_MY_BMAC_MSB_GET_LIF_MY_BMAC_MSB_GET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_lif_my_bmac_msb_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_SET_LIF_MY_BMAC_PORT_LSB_SET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_lif_my_bmac_port_lsb_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_MY_BMAC_PORT_LSB_GET_LIF_MY_BMAC_PORT_LSB_GET_ID,1))
  {
    ui_ret = ui_ppd_api_mymac_lif_my_bmac_port_lsb_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mymac***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}


#endif
#endif /* LINK_PPD_LIBRARIES */ 

