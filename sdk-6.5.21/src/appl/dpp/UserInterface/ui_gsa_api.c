/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#if !DUNE_BCM

/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
#include <bcm_app/dpp/../H/usrApp.h>

/*
 * Utilities include file.
 */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>

#include <appl/dpp/UserInterface/ui_pure_defi_gsa_api.h>
#include <appl/dpp/UserInterface/ui_rom_defi_gsa_api.h>


#include <DuneApp/gsa_api_vlan.h>
#include <DuneApp/gsa_api_mact.h>
#include <DuneApp/gsa_api_fec.h>
#include <DuneApp/gsa_api_ipv4_uc.h>
#include <DuneApp/gsa_api_ipv4_mc.h>
#include <DuneApp/gsa_api_ipv6.h>
#include <DuneApp/gsa_api_acl.h>

#define UI_VLAN
#define UI_MACT
#define UI_FEC
#define UI_IPV4_MC
#define UI_IPV4_UC
#define UI_ACL

#define UI_GSA_BLOCK_GET_SIZE 20
#define UI_GSA_BLOCK_GET_SIZE_SMALL 10
#define UI_ACL_MASK_SET_ALL 0xFFFFFFFF

static
  uint32
    Default_unit = 0;

void
  gsa_set_default_unit(uint32 dev_id)
{
  Default_unit = dev_id;
}

uint32
  gsa_get_default_unit()
{
  return Default_unit;
}
#ifdef UI_VLAN
/********************************************************************
 *  Function handler: set (section vlan)
 */
int
  ui_gsa_api_vlan_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vlan_ndx,
    up_to_vlan;
  GSA_VLAN_INFO
    prm_vlan_info,
    old_vlan_info;
  uint32
    indx,
    prm_nof_ports;
   uint8
     topology_changed=FALSE,
     valid_changed=FALSE,
     members_changed=FALSE,
     fld_changed=FALSE;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_vlan");
  soc_sand_proc_name = "gsa_vlan_set";

  unit = gsa_get_default_unit();
  gsa_GSA_VLAN_INFO_clear(&prm_vlan_info);
  gsa_GSA_VLAN_INFO_clear(&old_vlan_info);


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_SET_SET_VLAN_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_VLAN_SET_SET_VLAN_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_ndx = (uint32)param_val->value.ulong_value;
    up_to_vlan = prm_vlan_ndx;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_ndx after set***", TRUE);
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_SET_SET_UP_TO_VLAN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_VLAN_SET_SET_UP_TO_VLAN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    up_to_vlan = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_SET_SET_VLAN_INFO_ENABLE_FLOODING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_VLAN_SET_SET_VLAN_INFO_ENABLE_FLOODING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_info.enable_flooding = (uint8)param_val->value.ulong_value;
    fld_changed = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_SET_SET_VLAN_INFO_TOPOLOGY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_VLAN_SET_SET_VLAN_INFO_TOPOLOGY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_info.topology = (uint32)param_val->value.ulong_value;
    topology_changed = TRUE;
  }



  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_SET_SET_VLAN_INFO_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_VLAN_SET_SET_VLAN_INFO_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_info.valid = (uint8)param_val->value.ulong_value;
    valid_changed = TRUE;
  }

  prm_nof_ports = 0;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_SET_SET_VLAN_INFO_MEMBERSHIP_TAGGED_PORTS_ID,1))
  {
    for (indx = 0; indx < 80; ++prm_nof_ports, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_VLAN_SET_SET_VLAN_INFO_MEMBERSHIP_TAGGED_PORTS_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_vlan_info.membership.member_ports[prm_nof_ports] = (uint32)param_val->value.ulong_value;
      prm_vlan_info.membership.tagged[prm_nof_ports] = TRUE;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_SET_SET_VLAN_INFO_MEMBERSHIP_UNTAGGED_PORTS_ID,1))
  {
    for (indx = 0 ;prm_nof_ports < 100; ++prm_nof_ports, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_VLAN_SET_SET_VLAN_INFO_MEMBERSHIP_UNTAGGED_PORTS_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_vlan_info.membership.member_ports[prm_nof_ports] = (uint32)param_val->value.ulong_value;
      prm_vlan_info.membership.tagged[prm_nof_ports] = FALSE;
    }
  }
 /*
  * if the list of ports not touched then the VLAN membership will not be changed.
  */
  if (prm_nof_ports != 0)
  {
    members_changed = TRUE;
    prm_vlan_info.membership.nof_members = prm_nof_ports;
  }

  for (indx = prm_vlan_ndx; indx <= up_to_vlan; ++indx)
  {
    /* Call function */
    ret = gsa_vlan_get(
            indx,
            &old_vlan_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_vlan_set - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_vlan_set");
      goto exit;
    }

    if (fld_changed)
    {
      old_vlan_info.enable_flooding = prm_vlan_info.enable_flooding;
    }
    if (topology_changed)
    {
      old_vlan_info.topology = prm_vlan_info.topology;
    }

    if (valid_changed)
    {
      old_vlan_info.valid = prm_vlan_info.valid;
    }

    if (members_changed)
    {
      sal_memcpy(
        &(old_vlan_info.membership),
        &(prm_vlan_info.membership),
        sizeof(GSA_VLAN_MEMBERSHIP)
      );
    }
    /* Call function */
    ret = gsa_vlan_set(
            indx,
            &old_vlan_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_vlan_set - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_vlan_set");
      goto exit;
    }
  }

  send_string_to_screen(" ---> gsa_vlan_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: get (section vlan)
 */
int
  ui_gsa_api_vlan_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vlan_ndx;
  GSA_VLAN_INFO
    prm_vlan_info;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_vlan");
  soc_sand_proc_name = "gsa_vlan_get";

  unit = gsa_get_default_unit();
  gsa_GSA_VLAN_INFO_clear(&prm_vlan_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_GET_GET_VLAN_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_VLAN_GET_GET_VLAN_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_ndx after get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = gsa_vlan_get(
          prm_vlan_ndx,
          &prm_vlan_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_vlan_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_vlan_get");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_vlan_get - SUCCEEDED", TRUE);
  gsa_GSA_VLAN_INFO_print(prm_vlan_ndx, &prm_vlan_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: member_add (section vlan)
 */
int
  ui_gsa_api_vlan_member_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vlan_ndx;
  uint32
    prm_sys_port=0;
  uint8
    prm_tagged;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_vlan");
  soc_sand_proc_name = "gsa_vlan_member_add";
  prm_tagged = FALSE;

  unit = gsa_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_MEMBER_ADD_MEMBER_ADD_VLAN_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_VLAN_MEMBER_ADD_MEMBER_ADD_VLAN_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_ndx after member_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_MEMBER_ADD_MEMBER_ADD_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_VLAN_MEMBER_ADD_MEMBER_ADD_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_port = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_port after member_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_MEMBER_ADD_MEMBER_ADD_TAGGED_ID,1))
  {
    prm_tagged = TRUE;
  }


  /* Call function */
  ret = gsa_vlan_member_add(
          prm_vlan_ndx,
          prm_sys_port,
          prm_tagged
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_vlan_member_add - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_vlan_member_add");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_vlan_member_add - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: member_remove (section vlan)
 */
int
  ui_gsa_api_vlan_member_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vlan_ndx;
  uint32
    prm_sys_port=0;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_vlan");
  soc_sand_proc_name = "gsa_vlan_member_remove";

  unit = gsa_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_VLAN_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_VLAN_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_ndx after member_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_port = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_port after member_remove***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = gsa_vlan_member_remove(
          prm_vlan_ndx,
          prm_sys_port
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_vlan_member_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_vlan_member_remove");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_vlan_member_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_info_set (section edit)
 */
int
  ui_gsa_api_edit_port_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_port_ndx;
  uint32
    prm_vid_ndx;
  uint8
    prm_tagged;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_edit");
  soc_sand_proc_name = "gsa_edit_port_info_set";

  unit = soc_petra_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_EDIT_PORT_INFO_SET_PORT_INFO_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_EDIT_PORT_INFO_SET_PORT_INFO_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after port_info_set***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_EDIT_PORT_INFO_SET_PORT_INFO_SET_VID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_EDIT_PORT_INFO_SET_PORT_INFO_SET_VID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after port_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = gsa_vlan_port_tagging_get(
          prm_port_ndx,
          prm_vid_ndx,
          &prm_tagged
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_edit_port_info_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_edit_port_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_EDIT_PORT_INFO_SET_PORT_INFO_SET_TAGGED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_EDIT_PORT_INFO_SET_PORT_INFO_SET_TAGGED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tagged = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = gsa_vlan_port_tagging_set(
          prm_port_ndx,
          prm_vid_ndx,
          prm_tagged
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_edit_port_info_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_edit_port_info_set");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_edit_port_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_info_get (section edit)
 */
int
  ui_gsa_api_edit_port_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_port_ndx;
  uint32
    prm_vid_ndx;
  uint8
    prm_tagged;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_edit");
  soc_sand_proc_name = "gsa_edit_port_info_get";

  unit = soc_petra_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_EDIT_PORT_INFO_GET_PORT_INFO_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_EDIT_PORT_INFO_GET_PORT_INFO_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after port_info_get***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_EDIT_PORT_INFO_GET_PORT_INFO_GET_VID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_EDIT_PORT_INFO_GET_PORT_INFO_GET_VID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after port_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = gsa_vlan_port_tagging_get(
          prm_vid_ndx,
          prm_port_ndx,
          &prm_tagged
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_edit_port_info_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_edit_port_info_get");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_edit_port_info_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> tagged:", TRUE);
  soc_sand_os_printf( "tagged: %u\n\r",prm_tagged);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_info_set (section edit)
 */
int
  ui_gsa_api_edit_port_pvid_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_port_ndx;
  uint32
    prm_pvid;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_edit");
  soc_sand_proc_name = "gsa_edit_port_info_set";

  unit = soc_petra_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_EDIT_PORT_INFO_SET_PORT_PVID_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_EDIT_PORT_INFO_SET_PORT_PVID_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after port_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = gsa_vlan_port_pvid_get(
          prm_port_ndx,
          &prm_pvid
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_edit_port_info_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_edit_port_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_EDIT_PORT_INFO_SET_PORT_PVID_SET_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_EDIT_PORT_INFO_SET_PORT_PVID_SET_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pvid = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = gsa_vlan_port_pvid_set(
          prm_port_ndx,
          prm_pvid
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_edit_port_info_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_edit_port_info_set");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_edit_port_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_info_get (section edit)
 */
int
  ui_gsa_api_edit_port_pvid_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_port_ndx;
  uint32
    prm_pvid;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_edit");
  soc_sand_proc_name = "gsa_edit_port_vid_get";

  unit = soc_petra_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_EDIT_PORT_INFO_GET_PORT_PVID_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_EDIT_PORT_INFO_GET_PORT_PVID_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after port_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = gsa_vlan_port_pvid_get(
          prm_port_ndx,
          &prm_pvid
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_edit_port_info_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_edit_port_info_get");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_edit_port_info_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> pvid:", TRUE);
  soc_sand_os_printf( "pvid: %u\n\r",prm_pvid);


  goto exit;
exit:
  return ui_ret;
}
#endif

#ifdef UI_MACT
/********************************************************************
 *  Function handler: entry_add (section mact)
 */
int
  ui_gsa_api_mact_entry_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_MACT_KEY
    prm_key;
  GSA_MACT_KEY
    prm_up_to_key;
  GSA_MACT_VALUE
    prm_value;
  uint32
    prm_nof_sys_ports = 0,
    indx;
  uint8
    prm_up_to=FALSE;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_mact");
  soc_sand_proc_name = "gsa_mact_entry_add";

  unit = gsa_get_default_unit();
  gsa_GSA_MACT_KEY_clear(&prm_key);
  gsa_GSA_MACT_KEY_clear(&prm_up_to_key);
  gsa_GSA_MACT_VALUE_clear(&prm_value);
  prm_value.frwrd_action = GSA_MACT_FRWRD_ACTION_TYPE_BRIDGE;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_KEY_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_KEY_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_key.vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_KEY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_key.mac));
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_UP_TO_MAC_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_up_to_key.mac));
    prm_up_to = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_SA_DROP_ID,1))
  {
    prm_value.sa_drop = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_IS_DYNAMIC_ID,1))
  {
    prm_value.is_dynamic = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_DESTS_DESTS_PHY_PORTS_ID_ID,1))
  {
    for (indx = 0; indx < 80; ++prm_nof_sys_ports, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_DESTS_DESTS_PHY_PORTS_ID_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_value.dests.dests_ports[prm_nof_sys_ports].id = (uint32)param_val->value.ulong_value;
      prm_value.dests.dests_ports[prm_nof_sys_ports].type = SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
    }
  }

  /* lags */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_DESTS_DESTS_LAG_PORTS_ID_ID,1))
  {
    for (indx = 0; indx < 80; ++prm_nof_sys_ports, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_DESTS_DESTS_LAG_PORTS_ID_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_value.dests.dests_ports[prm_nof_sys_ports].id = (uint32)param_val->value.ulong_value;
      prm_value.dests.dests_ports[prm_nof_sys_ports].type = SOC_PETRA_DEST_SYS_PORT_TYPE_LAG;
    }
  }

  prm_value.dests.nof_ports = prm_nof_sys_ports;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_FRWRD_ACTION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_FRWRD_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_value.frwrd_action = (GSA_MACT_FRWRD_ACTION_TYPE)param_val->numeric_equivalent;
  }


  if (!prm_up_to)
  {
    /* Call function */
    ret = gsa_mact_entry_add(
            &prm_key,
            &prm_value
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_mact_entry_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_mact_entry_add");
      goto exit;
    }

    send_string_to_screen(" ---> gsa_mact_entry_add - SUCCEEDED", TRUE);
  }
  else
  {
        /* Call function */
    ret = gsa_mact_range_add(
            &prm_key,
            &(prm_up_to_key.mac),
            &prm_value
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_mact_range_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_mact_range_add");
      goto exit;
    }

    send_string_to_screen(" ---> gsa_mact_range_add - SUCCEEDED", TRUE);
  }

exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: range_add (section mact)
 */
int
  ui_gsa_api_mact_range_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_MACT_KEY
    prm_key;
  SOC_SAND_PP_MAC_ADDRESS
    prm_up_to_mac;
  GSA_MACT_VALUE
    prm_value;
  uint32
    prm_nof_sys_ports = 0,
    indx;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_mact");
  soc_sand_proc_name = "gsa_mact_range_add";

  unit = gsa_get_default_unit();
  gsa_GSA_MACT_KEY_clear(&prm_key);
  gsa_GSA_MACT_VALUE_clear(&prm_value);
  prm_value.frwrd_action = GSA_MACT_FRWRD_ACTION_TYPE_BRIDGE;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_KEY_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_KEY_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_key.vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_KEY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_key.mac));
  }
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_UP_TO_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_up_to_mac));
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_VALUE_SA_DROP_ID,1))
  {
    prm_value.sa_drop = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_VALUE_IS_DYNAMIC_ID,1))
  {
    prm_value.is_dynamic = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_VALUE_DESTS_DESTS_PORTS_ID,1))
  {
    for (indx = 0; indx < 80; ++prm_nof_sys_ports, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_VALUE_DESTS_DESTS_PORTS_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_value.dests.dests_ports[prm_nof_sys_ports].id = (uint32)param_val->value.ulong_value;
      prm_value.dests.dests_ports[prm_nof_sys_ports].type = SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
    }
  }

  /* lags */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_VALUE_DESTS_DESTS_LAGS_ID,1))
  {
    for (indx = 0; indx < 80; ++prm_nof_sys_ports, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_VALUE_DESTS_DESTS_LAGS_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_value.dests.dests_ports[prm_nof_sys_ports].id = (uint32)param_val->value.ulong_value;
      prm_value.dests.dests_ports[prm_nof_sys_ports].type = SOC_PETRA_DEST_SYS_PORT_TYPE_LAG;
    }
  }
  prm_value.dests.nof_ports = prm_nof_sys_ports;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_VALUE_FRWRD_ACTION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_VALUE_FRWRD_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_value.frwrd_action = (GSA_MACT_FRWRD_ACTION_TYPE)param_val->numeric_equivalent;
  }

  /* Call function */
  ret = gsa_mact_range_add(
          &prm_key,
          &prm_up_to_mac,
          &prm_value
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_mact_range_add - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_mact_range_add");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_mact_range_add - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: entry_remove (section mact)
 */
int
  ui_gsa_api_mact_entry_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_MACT_KEY
    prm_key;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_mact");
  soc_sand_proc_name = "gsa_mact_entry_remove";

  unit = gsa_get_default_unit();
  gsa_GSA_MACT_KEY_clear(&prm_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_ENTRY_REMOVE_ENTRY_REMOVE_KEY_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_ENTRY_REMOVE_ENTRY_REMOVE_KEY_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_key.vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_MACT_ENTRY_REMOVE_ENTRY_REMOVE_KEY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_key.mac));
  }

  /* Call function */
  ret = gsa_mact_entry_remove(
          &prm_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_mact_entry_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_mact_entry_remove");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_mact_entry_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: range_remove (section mact)
 */
int
  ui_gsa_api_mact_range_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_MACT_KEY
    prm_key;
  SOC_SAND_PP_MAC_ADDRESS
    prm_up_to_mac;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_mact");
  soc_sand_proc_name = "gsa_mact_range_remove";

  unit = gsa_get_default_unit();
  gsa_GSA_MACT_KEY_clear(&prm_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_RANGE_REMOVE_RANGE_REMOVE_KEY_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_RANGE_REMOVE_RANGE_REMOVE_KEY_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_key.vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_MACT_RANGE_REMOVE_RANGE_REMOVE_KEY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_key.mac));
  }
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_MACT_RANGE_REMOVE_RANGE_REMOVE_UP_TO_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_up_to_mac));
  }

  /* Call function */
  ret = gsa_mact_range_remove(
          &prm_key,
          &prm_up_to_mac
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_mact_range_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_mact_range_remove");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_mact_range_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: flush_block (section mact)
 */
int
  ui_gsa_api_mact_flush_block(
    CURRENT_LINE *current_line,
    SOC_SAND_IN BOOL clear
  )
{
  uint32
    ret;
  GSA_MACT_FLUSH_INFO
    prm_flush_info;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_mact");
  soc_sand_proc_name = "gsa_mact_flush_block";

  unit = gsa_get_default_unit();
  gsa_GSA_MACT_FLUSH_INFO_clear(&prm_flush_info);
  prm_flush_info.new_dest.dest_type = SOC_SAND_PP_DEST_SINGLE_PORT;
  prm_flush_info.rule.dest.dest_type = SOC_SAND_PP_DEST_SINGLE_PORT;
  if (clear)
  {
    prm_flush_info.rule.including_dynamic = TRUE;
    prm_flush_info.rule.including_static = FALSE;
    prm_flush_info.mode = SOC_PETRA_PP_MACT_FLUSH_MODE_CLEAR;
  }
  else
  {
    prm_flush_info.rule.including_dynamic = TRUE;
    prm_flush_info.rule.including_static = TRUE;
    prm_flush_info.mode = SOC_PETRA_PP_MACT_FLUSH_MODE_TRANSPLANT;
  }

#ifdef PETRA_PP_MINIMAL
  prm_flush_info.rule.including_static = TRUE;
#endif



  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_NEW_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_NEW_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.new_dest.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_NEW_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_NEW_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.new_dest.dest_type = (SOC_SAND_PP_DEST_TYPE)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_INCLUDING_DYNAMIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_INCLUDING_DYNAMIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.including_dynamic = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_INCLUDING_STATIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_INCLUDING_STATIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.including_static = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.dest.dest_val = (uint32)param_val->value.ulong_value;
    prm_flush_info.rule.compare_dest = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.dest.dest_type = (SOC_SAND_PP_DEST_TYPE)param_val->value.ulong_value;
    prm_flush_info.rule.compare_dest = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.fid = (uint32)param_val->value.ulong_value;
    prm_flush_info.rule.compare_fid = TRUE;
  }



  /* Call function */
  ret = gsa_mact_flush_block(
          &prm_flush_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_mact_flush_block - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_mact_flush_block");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_mact_flush_block - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: get_block (section mact)
 */
int
  ui_gsa_api_mact_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_MACT_ENTRIES_RANGE
    prm_range_info;
  GSA_MACT_MATCH_RULE
    prm_rule;
  GSA_MACT_KEY
    prm_keys[UI_GSA_BLOCK_GET_SIZE];
  GSA_MACT_VALUE
    prm_values[UI_GSA_BLOCK_GET_SIZE];
  uint32
    mc_group_ids[UI_GSA_BLOCK_GET_SIZE];
  uint32
    prm_nof_entries,
    prm_total_entries;
  uint32
    total_entries,
    flavor;
  uint32
    prm_fap_id = 0;
  char
    user_msg[5] = "";
  uint32
    con;
  uint8
    prm_print_minimal=FALSE;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_mact");
  soc_sand_proc_name = "gsa_mact_get_block";

  unit = gsa_get_default_unit();
  gsa_GSA_MACT_ENTRIES_RANGE_clear(&prm_range_info);
  gsa_GSA_MACT_MATCH_RULE_clear(&prm_rule);
  prm_range_info.entries_to_act = UI_GSA_BLOCK_GET_SIZE;
  prm_range_info.entries_to_scan = 40000;
  prm_nof_entries = 0;
  total_entries = 0;
  flavor = 1;
  prm_rule.including_dynamic = TRUE;
  prm_rule.including_static = TRUE;
  prm_rule.dest.dest_type = SOC_SAND_PP_DEST_SINGLE_PORT;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_GET_BLOCK_GET_FAP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_GET_BLOCK_GET_FAP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fap_id = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_GET_BLOCK_GET_PRINT_MINIMAL_ID,1))
  {
    prm_print_minimal = GSA_MACT_PRINT_TABLE_MINIMAL;
  }

#ifdef PETRA_PP_MINIMAL
  prm_print_minimal = GSA_MACT_PRINT_TABLE_MINIMAL;
#endif

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_INCLUDING_DYNAMIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_INCLUDING_DYNAMIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.including_dynamic = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_INCLUDING_STATIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_INCLUDING_STATIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.including_static = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.dest.dest_val = (uint32)param_val->value.ulong_value;
    prm_rule.compare_dest = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.compare_dest = TRUE;
    prm_rule.dest.dest_type = (SOC_SAND_PP_DEST_TYPE)param_val->numeric_equivalent;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.fid = (uint32)param_val->value.ulong_value;
    prm_rule.compare_fid = TRUE;
  }


  while (prm_range_info.iter < SOC_PETRA_PP_MACT_END && prm_range_info.entries_to_act && prm_range_info.entries_to_scan)
  {
    /* Call function */
    ret = gsa_mact_get_block(
            prm_fap_id,
            &prm_range_info,
            &prm_rule,
            prm_keys,
            prm_values,
            mc_group_ids,
            &prm_nof_entries,
            &prm_total_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_mact_get_block - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_mact_get_block");
      goto exit;
    }

    if (prm_nof_entries == 0)
    {
      break;
    }

    ret = gsa_mact_print_block(
            prm_keys,
            prm_values,
            mc_group_ids,
            prm_nof_entries,
            prm_print_minimal
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_mact_get_block - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_mact_get_block");
      goto exit;
    }
    soc_sand_os_printf( "prm_total_entries: %u\n\r",prm_total_entries);
    if (prm_range_info.iter < SOC_PETRA_PP_MACT_END && prm_nof_entries >= prm_range_info.entries_to_act)
    {
      soc_sand_os_printf( "Hit Space to continue/Any Key to Abort.\n\r");
      con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
      if (!con)
      {
        break;
      }
    }
  }

exit:
  return ui_ret;
}



/********************************************************************
 *  Function handler: aging_info_set (section mact)
 */
int
  ui_gsa_api_mact_aging_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_MACT_AGING_INFO
    prm_aging_info;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_mact");
  soc_sand_proc_name = "gsa_mact_aging_info_set";

  unit = gsa_get_default_unit();
  gsa_GSA_MACT_AGING_INFO_clear(&prm_aging_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = gsa_mact_aging_info_get(
          &prm_aging_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_mact_aging_info_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_mact_aging_info_get");
    goto exit;
  }

  prm_aging_info.meta_cycle.nano_sec = 0;
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_META_CYCLE_NANO_SEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_META_CYCLE_NANO_SEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_aging_info.meta_cycle.nano_sec = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_META_CYCLE_SEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_META_CYCLE_SEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_aging_info.meta_cycle.sec = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_ENABLE_AGING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_ENABLE_AGING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_aging_info.enable_aging = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = gsa_mact_aging_info_set(
          &prm_aging_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_mact_aging_info_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_mact_aging_info_set");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_mact_aging_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: aging_info_get (section mact)
 */
int
  ui_gsa_api_mact_aging_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_MACT_AGING_INFO
    prm_aging_info;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_mact");
  soc_sand_proc_name = "gsa_mact_aging_info_get";

  unit = gsa_get_default_unit();
  gsa_GSA_MACT_AGING_INFO_clear(&prm_aging_info);

  /* Get parameters */

  /* Call function */
  ret = gsa_mact_aging_info_get(
          &prm_aging_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_mact_aging_info_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_mact_aging_info_get");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_mact_aging_info_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> aging_info:", TRUE);
  gsa_GSA_MACT_AGING_INFO_print(&prm_aging_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: my_mac_add (section mact)
 */
int
  ui_gsa_api_mact_my_mac_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_MACT_KEY
    prm_key;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_mact");
  soc_sand_proc_name = "gsa_mact_my_mac_add";

  unit = gsa_get_default_unit();
  gsa_GSA_MACT_KEY_clear(&prm_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_MY_MAC_ADD_MY_MAC_ADD_KEY_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_MY_MAC_ADD_MY_MAC_ADD_KEY_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_key.vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_MACT_MY_MAC_ADD_MY_MAC_ADD_KEY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_key.mac));
  }

  /* Call function */
  ret = gsa_mact_my_mac_add(
          &prm_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_mact_my_mac_add - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_mact_my_mac_add");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_mact_my_mac_add - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: my_mac_remove (section mact)
 */
int
  ui_gsa_api_mact_my_mac_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_MACT_KEY
    prm_key;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_mact");
  soc_sand_proc_name = "gsa_mact_my_mac_remove";

  unit = gsa_get_default_unit();
  gsa_GSA_MACT_KEY_clear(&prm_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_MY_MAC_REMOVE_MY_MAC_REMOVE_KEY_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_MACT_MY_MAC_REMOVE_MY_MAC_REMOVE_KEY_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_key.vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_MACT_MY_MAC_REMOVE_MY_MAC_REMOVE_KEY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_key.mac));
  }

  /* Call function */
  ret = gsa_mact_my_mac_remove(
          &prm_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_mact_my_mac_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_mact_my_mac_remove");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_mact_my_mac_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_FEC
/********************************************************************
 *  Function handler: uc_ecmp_add (section fec)
 */
int
  ui_gsa_api_fec_uc_ecmp_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SWP_P_PP_FEC_ADD_TYPE
    prm_add_type = SWP_P_PP_FEC_ADD_TYPE_CONCAT;
  GSA_FEC_UC_ENTRY
    prm_uc_fec_array[SOC_PETRA_PP_ECMP_MAX_NOF_FECS];
  uint32
    prm_nof_entries,
    static_cntr_indx = 0,
    dest_val_indx = 0,
    cpu_code_indx = 0,
    out_vid_indx = 0,
    indx;
  uint32
    tmp,
    prm_up_to_sys_fec_ndx;
  GSA_FEC_UC_RANGE_INFO
    prm_fec_uc_range_info;
  uint8
    prm_success=0;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_uc_ecmp_add";

  unit = gsa_get_default_unit();
  gsa_GSA_FEC_UC_ENTRY_clear(prm_uc_fec_array);
  gsa_GSA_FEC_UC_RANGE_INFO_clear(&prm_fec_uc_range_info);



  prm_uc_fec_array[0].action = SOC_SAND_PP_FEC_COMMAND_TYPE_ROUTE;
  prm_uc_fec_array[0].dest_sys_port.type = SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
  prm_uc_fec_array[0].enable_rpf = FALSE;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after uc_ecmp_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_ADD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_ADD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_add_type = param_val->numeric_equivalent;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_MAC_ADDR_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_uc_fec_array[0].mac_addr));
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_ENABLE_RPF_ID,1))
  {
    prm_uc_fec_array[0].enable_rpf = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_DEST_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_DEST_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[0].dest_sys_port.type = (SOC_PETRA_DEST_SYS_PORT_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_ACTION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[0].action = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_DEST_SYS_PORT_ID_ID,1))
  {
    for (indx = 0; indx < 80; ++dest_val_indx, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_DEST_SYS_PORT_ID_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_uc_fec_array[dest_val_indx].dest_sys_port.id = (uint32)param_val->value.ulong_value;
    }
  }

  dest_val_indx = SOC_SAND_MAX(1, dest_val_indx);

  for (indx = 1; indx < dest_val_indx; ++indx)
  {
    tmp = prm_uc_fec_array[indx].dest_sys_port.id;
    sal_memcpy(
      &(prm_uc_fec_array[indx]),
      &(prm_uc_fec_array[0]),
      sizeof(GSA_FEC_UC_ENTRY)
    );
    prm_uc_fec_array[indx].dest_sys_port.id = tmp;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_STATISTICS_COUNTER_ID,1))
  {
    for (indx = 0; indx < 80; ++static_cntr_indx, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_STATISTICS_COUNTER_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_uc_fec_array[static_cntr_indx].statistics_counter = (uint32)param_val->value.ulong_value;
    }
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_CPU_CODE_ID,1))
  {
    for (indx = 0; indx < 80; ++cpu_code_indx, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_CPU_CODE_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_uc_fec_array[cpu_code_indx].cpu_code = (uint32)param_val->value.ulong_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_OUT_VID_ID,1))
  {
    for (indx = 0; indx < 80; ++out_vid_indx, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_OUT_VID_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_uc_fec_array[out_vid_indx].out_vid = (uint32)param_val->value.ulong_value;
    }
  }


  prm_nof_entries = dest_val_indx;

  for (indx = 0; indx < prm_nof_entries; ++indx )
  {
    if (cpu_code_indx > 0 && cpu_code_indx <= indx)
    {
      prm_uc_fec_array[indx].cpu_code = prm_uc_fec_array[cpu_code_indx - 1].cpu_code;
    }
    if (static_cntr_indx > 0 && static_cntr_indx <= indx)
    {
      prm_uc_fec_array[indx].statistics_counter = prm_uc_fec_array[static_cntr_indx - 1].statistics_counter;
    }
    if (out_vid_indx > 0 && out_vid_indx <= indx)
    {
      prm_uc_fec_array[indx].out_vid = prm_uc_fec_array[out_vid_indx - 1].out_vid;
    }
  }


 /*
  * for range
  */
  prm_up_to_sys_fec_ndx = prm_sys_fec_ndx;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_UP_TO_SYS_FEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_UP_TO_SYS_FEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_MAC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_MAC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_uc_range_info.inc_mac = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_uc_range_info.inc_cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_uc_range_info.inc_vid = (uint32)param_val->value.ulong_value;
  }

  if (prm_up_to_sys_fec_ndx == prm_sys_fec_ndx)
  {
    /* Call function */
    ret = gsa_fec_uc_ecmp_add(
            prm_sys_fec_ndx,
            prm_add_type,
            prm_uc_fec_array,
            prm_nof_entries,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_fec_uc_ecmp_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_fec_uc_ecmp_add");
      goto exit;
    }
  }
  else
  {
    ret = gsa_fec_uc_ecmp_range_add(
            prm_sys_fec_ndx,
            prm_up_to_sys_fec_ndx,
            prm_add_type,
            prm_uc_fec_array,
            prm_nof_entries,
            &prm_fec_uc_range_info,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_fec_uc_ecmp_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_fec_uc_ecmp_add");
      goto exit;
    }

  }

  send_string_to_screen(" ---> gsa_fec_uc_ecmp_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
 *  Function handler: uc_ecmp_update_entry (section fec)
 */
int
  ui_gsa_api_fec_uc_ecmp_update_entry(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  uint32
    prm_entry_ndx;
  GSA_FEC_UC_ENTRY
    prm_uc_fec_entry;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_uc_ecmp_update_entry";

  unit = gsa_get_default_unit();
  gsa_GSA_FEC_UC_ENTRY_clear(&prm_uc_fec_entry);
  prm_uc_fec_entry.enable_rpf = FALSE;
  prm_uc_fec_entry.action = SOC_SAND_PP_FEC_COMMAND_TYPE_ROUTE;
  prm_uc_fec_entry.dest_sys_port.type = GSA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after uc_ecmp_update_entry***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after uc_ecmp_update_entry***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_STATISTICS_COUNTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_STATISTICS_COUNTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_entry.statistics_counter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_ENABLE_RPF_ID,1))
  {
    prm_uc_fec_entry.enable_rpf = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_entry.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_OUT_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_OUT_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_entry.out_vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_MAC_ADDR_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_uc_fec_entry.mac_addr));
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_DEST_SYS_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_DEST_SYS_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_entry.dest_sys_port.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_DEST_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_DEST_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_entry.dest_sys_port.type = (SOC_PETRA_DEST_SYS_PORT_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_ACTION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_entry.action = (SOC_SAND_PP_FEC_COMMAND_TYPE)param_val->numeric_equivalent;
  }

  /* Call function */
  ret = gsa_fec_uc_ecmp_update_entry(
          prm_sys_fec_ndx,
          prm_entry_ndx,
          &prm_uc_fec_entry
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_fec_uc_ecmp_update_entry - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_fec_uc_ecmp_update_entry");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_fec_uc_ecmp_update_entry - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mc_fec_add (section fec)
 */
int
  ui_gsa_api_fec_mc_fec_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SWP_P_PP_FEC_ADD_TYPE
    prm_add_type;
  GSA_FEC_MC_ENTRY
    prm_mc_fec_entry;
  uint32
    prm_nof_sys_ports=0,
    prm_nof_vids=0,
    indx;
  uint32
    prm_up_to_sys_fec_ndx;
  uint8
    prm_success=0;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_mc_fec_add";

  unit = gsa_get_default_unit();
  gsa_GSA_FEC_MC_ENTRY_clear(&prm_mc_fec_entry);
  prm_mc_fec_entry.action = SOC_SAND_PP_FEC_COMMAND_TYPE_ROUTE;
  prm_add_type = SWP_P_PP_FEC_ADD_TYPE_NEW_OVERRIDER;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
    prm_up_to_sys_fec_ndx = prm_sys_fec_ndx;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after mc_fec_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_ADD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_ADD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_add_type = (SWP_P_PP_FEC_ADD_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_STATISTICS_COUNTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_STATISTICS_COUNTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.statistics_counter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.exp_in_port.sys_id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.exp_in_port.sys_port_type = (SOC_SAND_PP_SYS_PORT_TYPE)param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.exp_in_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_RPF_CHECK_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_RPF_CHECK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.rpf_check = (SOC_PETRA_PP_FEC_RPF_CHECK)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_DESTS_DESTS_PHY_PORTS_ID_ID,1))
  {
    for (indx = 0; indx < 80; ++prm_nof_sys_ports, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_DESTS_DESTS_PHY_PORTS_ID_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_mc_fec_entry.dests.dests_ports[prm_nof_sys_ports].id = (uint32)param_val->value.ulong_value;
      prm_mc_fec_entry.dests.dests_ports[prm_nof_sys_ports].type = SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_PORTS_VIDS_ID,1))
  {
    for (indx = 0; indx < 80; ++prm_nof_vids, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_PORTS_VIDS_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_mc_fec_entry.vids[prm_nof_vids] = (uint32)param_val->value.ulong_value;
    }
  }

  if (prm_nof_vids < prm_nof_sys_ports)
  {
    soc_sand_os_printf( "Error: Vids list for System ports is shorter than System port list! \n\r" );
    goto exit;
  }

  if (prm_nof_vids > prm_nof_sys_ports)
  {
    soc_sand_os_printf( "Error: Vids list for System ports is longer than System port list! \n\r" );
    goto exit;
  }

  /* lags */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_DESTS_DESTS_LAGS_ID_ID,1))
  {
    for (indx = 0; indx < 80; ++prm_nof_sys_ports, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_DESTS_DESTS_LAGS_ID_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_mc_fec_entry.dests.dests_ports[prm_nof_sys_ports].id = (uint32)param_val->value.ulong_value;
      prm_mc_fec_entry.dests.dests_ports[prm_nof_sys_ports].type = SOC_PETRA_DEST_SYS_PORT_TYPE_LAG;
    }
  }
    /* lags */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_LAGS_VIDS_ID,1))
  {
    for (indx = 0; indx < 80; ++prm_nof_vids, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_LAGS_VIDS_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_mc_fec_entry.vids[prm_nof_vids] = (uint32)param_val->value.ulong_value;
    }
  }

  if (prm_nof_vids < prm_nof_sys_ports)
  {
    soc_sand_os_printf( "Error: Vids list for LAG is shorter than LAG list! \n\r" );
    goto exit;
  }

  if (prm_nof_vids > prm_nof_sys_ports)
  {
    soc_sand_os_printf( "Error: Vids list for LAG is longer than LAG list! \n\r" );
    goto exit;
  }
  prm_mc_fec_entry.dests.nof_ports = prm_nof_sys_ports;


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_ACTION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.action = (SOC_SAND_PP_FEC_COMMAND_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_RANGE_UP_TO_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_RANGE_UP_TO_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_sys_fec_ndx = (SOC_SAND_PP_FEC_COMMAND_TYPE)param_val->value.ulong_value;
  }


  if (prm_up_to_sys_fec_ndx == prm_sys_fec_ndx)
  {
    /* Call function */
    ret = gsa_fec_mc_fec_add(
            prm_sys_fec_ndx,
            prm_add_type,
            &prm_mc_fec_entry,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_fec_mc_fec_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_fec_mc_fec_add");
      goto exit;
    }
  }
  else
  {
    /* Call function */
    ret = gsa_fec_mc_fec_range_add(
            prm_sys_fec_ndx,
            prm_up_to_sys_fec_ndx,
            &prm_mc_fec_entry,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_fec_mc_fec_range_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_fec_mc_fec_range_add");
      goto exit;
    }
  }

  send_string_to_screen(" ---> gsa_fec_mc_fec_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: uc_emcp_remove_entry (section fec)
 */
int
  ui_gsa_api_fec_uc_emcp_remove_entry(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  uint32
    prm_fec_entry_ndx;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_uc_emcp_remove_entry";

  unit = gsa_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after uc_emcp_remove_entry***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_FEC_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_FEC_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter fec_entry_ndx after uc_emcp_remove_entry***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = gsa_fec_uc_emcp_remove_entry(
          prm_sys_fec_ndx,
          prm_fec_entry_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_fec_uc_emcp_remove_entry - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_fec_uc_emcp_remove_entry");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_fec_uc_emcp_remove_entry - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: remove (section fec)
 */
int
  ui_gsa_api_fec_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_up_to_sys_fec_ndx;


  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_remove";

  unit = gsa_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_REMOVE_REMOVE_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_REMOVE_REMOVE_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
    prm_up_to_sys_fec_ndx = prm_sys_fec_ndx;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after remove***", TRUE);
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_REMOVE_UP_TO_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_REMOVE_UP_TO_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }

  if (prm_up_to_sys_fec_ndx == prm_sys_fec_ndx)
  {

    /* Call function */
    ret = gsa_fec_remove(
            prm_sys_fec_ndx
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_fec_remove - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_fec_remove");
      goto exit;
    }
  }
  else
  {
    /* Call function */
    ret = gsa_fec_range_remove(
            prm_sys_fec_ndx,
            prm_up_to_sys_fec_ndx
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_fec_range_remove - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_fec_range_remove");
      goto exit;
    }
  }
  send_string_to_screen(" ---> gsa_fec_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: uc_get (section fec)
 */
int
  ui_gsa_api_fec_uc_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  GSA_FEC_UC_ENTRY
    prm_uc_fec_array[SOC_PETRA_PP_ECMP_MAX_NOF_FECS];
  uint32
    prm_nof_entries;
  uint8
    prm_is_first[SOC_PETRA_PP_ECMP_MAX_NOF_FECS];


  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_uc_get";

  unit = gsa_get_default_unit();
  gsa_GSA_FEC_UC_ENTRY_clear(prm_uc_fec_array);
  sal_memset(prm_is_first, 0x0, sizeof(uint8) * SOC_PETRA_PP_ECMP_MAX_NOF_FECS);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_GET_UC_GET_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_GET_UC_GET_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after uc_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = gsa_fec_uc_get(
          prm_sys_fec_ndx,
          prm_uc_fec_array,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_fec_uc_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_fec_uc_get");
    goto exit;
  }
  send_string_to_screen(" ---> gsa_fec_uc_get - SUCCEEDED", TRUE);

  if (prm_nof_entries == 0)
  {
    soc_sand_os_printf( "sys-fec-id %lu \n\r is not allocated", prm_sys_fec_ndx);
    goto exit;
  }

  prm_is_first[0] = TRUE;

  gsa_fec_table_uc_print_block(
    &prm_sys_fec_ndx,
    prm_uc_fec_array,
    prm_is_first,
    prm_nof_entries
  );

exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mc_get (section fec)
 */
int
  ui_gsa_api_fec_mc_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  GSA_FEC_MC_ENTRY
    prm_mc_fec;
  uint8
    prm_associated;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_mc_get";

  unit = gsa_get_default_unit();
  gsa_GSA_FEC_MC_ENTRY_clear(&prm_mc_fec);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_GET_MC_GET_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_GET_MC_GET_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after mc_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = gsa_fec_mc_get(
          prm_sys_fec_ndx,
          &prm_mc_fec,
          &prm_associated
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_fec_mc_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_fec_mc_get");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_fec_mc_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> mc_fec:", TRUE);
  gsa_GSA_FEC_MC_ENTRY_print(prm_sys_fec_ndx, &prm_mc_fec);

  send_string_to_screen("--> associated:", TRUE);
  soc_sand_os_printf( "associated: %u\n\r",prm_associated);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: table_uc_get_block (section fec)
 */
int
  ui_gsa_api_fec_table_uc_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_BLOCK_RANGE
    prm_range;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ids[UI_GSA_BLOCK_GET_SIZE];
  GSA_FEC_UC_ENTRY
    prm_uc_fec_table[UI_GSA_BLOCK_GET_SIZE];
  uint8
    prm_is_first[UI_GSA_BLOCK_GET_SIZE];
  uint32
    prm_nof_entries;
  uint8
    print_entries;
  uint32
    total_entries;
  char
    user_msg[5] = "";
  uint32
    con;


  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_table_uc_get_block";

  unit = gsa_get_default_unit();
  gsa_GSA_BLOCK_RANGE_clear(&prm_range);
  prm_range.entries_to_act = UI_GSA_BLOCK_GET_SIZE;
  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_TABLE_UC_GET_BLOCK_TABLE_UC_GET_BLOCK_START_FROM_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_TABLE_UC_GET_BLOCK_TABLE_UC_GET_BLOCK_START_FROM_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range.iter = (uint32)param_val->value.ulong_value;
  }

  total_entries = 0;
  print_entries = TRUE;

  /* Call function */
  while (prm_range.iter < SOC_PETRA_PP_FEC_TABLE_SIZE && prm_range.entries_to_act && prm_range.entries_to_scan)
  {

    ret = gsa_fec_table_uc_get_block(
            &prm_range,
            prm_sys_fec_ids,
            prm_uc_fec_table,
            prm_is_first,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_fec_table_uc_get_block - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_fec_table_uc_get_block");
      goto exit;
    }

    total_entries += prm_nof_entries;
    if (print_entries == FALSE)
    {
      continue;
    }
    gsa_fec_table_uc_print_block(
      prm_sys_fec_ids,
      prm_uc_fec_table,
      prm_is_first,
      prm_nof_entries
    );
    if (prm_range.iter < SOC_PETRA_PP_FEC_TABLE_SIZE)
    {
      soc_sand_os_printf( "Hit Space to continue/Any Key to Abort.\n\r");
      con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
      if (!con)
      {
        print_entries = FALSE;
      }
    }
  }
  soc_sand_os_printf( "nof_entries: %lu\n\r",total_entries);

exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: table_mc_get_block (section fec)
 */
int
  ui_gsa_api_fec_table_mc_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_BLOCK_RANGE
    prm_range;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ids[UI_GSA_BLOCK_GET_SIZE];
  GSA_FEC_MC_ENTRY
    prm_mc_fec_table[UI_GSA_BLOCK_GET_SIZE];
  uint8
    prm_is_first[UI_GSA_BLOCK_GET_SIZE];
  uint32
    prm_nof_entries;
  uint8
    print_entries;
  uint32
    total_entries;
  char
    user_msg[5] = "";
  uint32
    con;


  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_table_mc_get_block";

  unit = gsa_get_default_unit();
  gsa_GSA_BLOCK_RANGE_clear(&prm_range);
  prm_range.entries_to_act = UI_GSA_BLOCK_GET_SIZE;

  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_TABLE_MC_GET_BLOCK_TABLE_MC_GET_BLOCK_START_FROM_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_TABLE_MC_GET_BLOCK_TABLE_MC_GET_BLOCK_START_FROM_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range.iter = (uint32)param_val->value.ulong_value;
  }

  total_entries = 0;
  print_entries = TRUE;

  /* Call function */
  while (prm_range.iter < SOC_PETRA_PP_FEC_TABLE_SIZE && prm_range.entries_to_act && prm_range.entries_to_scan)
  {
    /* Call function */
    ret = gsa_fec_table_mc_get_block(
            &prm_range,
            prm_sys_fec_ids,
            prm_mc_fec_table,
            prm_is_first,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_fec_table_mc_get_block - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_fec_table_mc_get_block");
      goto exit;
    }

    total_entries += prm_nof_entries;
    if (print_entries == FALSE)
    {
      continue;
    }

    gsa_fec_table_mc_print_block(
      prm_sys_fec_ids,
      prm_mc_fec_table,
      prm_is_first,
      prm_nof_entries
    );

    if (prm_range.iter < SOC_PETRA_PP_FEC_TABLE_SIZE)
    {
      soc_sand_os_printf( "Hit Space to continue/Any Key to Abort.\n\r");
      con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
      if (!con)
      {
        print_entries = FALSE;
      }
    }
  }
  soc_sand_os_printf( "nof_entries: %lu\n\r",total_entries);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: uc_ecmp_range_add (section fec)
 */
int
  ui_gsa_api_fec_uc_ecmp_range_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_up_to_sys_fec_ndx;
  uint32
    prm_uc_fec_array_index = 0xFFFFFFFF;
  GSA_FEC_UC_ENTRY
    prm_uc_fec_array[SOC_PETRA_PP_ECMP_MAX_NOF_FECS];
  uint32
    prm_nof_entries;
  GSA_FEC_UC_RANGE_INFO
    prm_range_info;
  SWP_P_PP_FEC_ADD_TYPE
    prm_add_type = SWP_P_PP_FEC_ADD_TYPE_CONCAT;
  uint8
    prm_success=0;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_uc_ecmp_range_add";

  unit = gsa_get_default_unit();
  gsa_GSA_FEC_UC_ENTRY_clear(prm_uc_fec_array);
  gsa_GSA_FEC_UC_RANGE_INFO_clear(&prm_range_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after uc_ecmp_range_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UP_TO_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UP_TO_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_sys_fec_ndx = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter up_to_sys_fec_ndx after uc_ecmp_range_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_uc_fec_array_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_STATISTICS_COUNTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_STATISTICS_COUNTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].statistics_counter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_ENABLE_RPF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_ENABLE_RPF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].enable_rpf = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_OUT_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_OUT_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].out_vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_MAC_ADDR_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_uc_fec_array[ prm_uc_fec_array_index].mac_addr));
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_DEST_SYS_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_DEST_SYS_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].dest_sys_port.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_DEST_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_DEST_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].dest_sys_port.type = (SOC_PETRA_DEST_SYS_PORT_TYPE)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_ACTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].action = (SOC_SAND_PP_FEC_COMMAND_TYPE)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_NOF_ENTRIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_NOF_ENTRIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_entries = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_MAC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_MAC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_mac = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = gsa_fec_uc_ecmp_range_add(
          prm_sys_fec_ndx,
          prm_up_to_sys_fec_ndx,
          prm_add_type,
          prm_uc_fec_array,
          prm_nof_entries,
          &prm_range_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_fec_uc_ecmp_range_add - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_fec_uc_ecmp_range_add");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_fec_uc_ecmp_range_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mc_fec_range_add (section fec)
 */
int
  ui_gsa_api_fec_mc_fec_range_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_up_to_sys_fec_ndx;
  uint32
    prm_vids_index = 0xFFFFFFFF;
  uint32
    prm_dests_ports_index = 0xFFFFFFFF;
  GSA_FEC_MC_ENTRY
    prm_mc_fec_entry;
  uint8
    prm_success=0;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_mc_fec_range_add";

  unit = gsa_get_default_unit();
  gsa_GSA_FEC_MC_ENTRY_clear(&prm_mc_fec_entry);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after mc_fec_range_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_UP_TO_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_UP_TO_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_sys_fec_ndx = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter up_to_sys_fec_ndx after mc_fec_range_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_STATISTICS_COUNTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_STATISTICS_COUNTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.statistics_counter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.exp_in_port.sys_id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.exp_in_port.sys_port_type = (SOC_SAND_PP_SYS_PORT_TYPE)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.exp_in_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_RPF_CHECK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_RPF_CHECK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.rpf_check = (SOC_PETRA_PP_FEC_RPF_CHECK)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_VIDS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_VIDS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vids_index = (uint32)param_val->value.ulong_value;
  }
  if(prm_vids_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_VIDS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_VIDS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.vids[ prm_vids_index] = (uint32)param_val->value.ulong_value;
  }
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DESTS_NOF_PORTS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DESTS_NOF_PORTS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.dests.nof_ports = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DESTS_DESTS_PORTS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DESTS_DESTS_PORTS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dests_ports_index = (uint32)param_val->value.ulong_value;
  }
  if(prm_dests_ports_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DESTS_DESTS_PORTS_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DESTS_DESTS_PORTS_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.dests.dests_ports[ prm_dests_ports_index].id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DESTS_DESTS_PORTS_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DESTS_DESTS_PORTS_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.dests.dests_ports[ prm_dests_ports_index].type = (SOC_PETRA_DEST_SYS_PORT_TYPE)param_val->value.ulong_value;
  }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_ACTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.action = (SOC_SAND_PP_FEC_COMMAND_TYPE)param_val->value.ulong_value;
  }




  /* Call function */
  ret = gsa_fec_mc_fec_range_add(
          prm_sys_fec_ndx,
          prm_up_to_sys_fec_ndx,
          &prm_mc_fec_entry,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_fec_mc_fec_range_add - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_fec_mc_fec_range_add");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_fec_mc_fec_range_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: range_remove (section fec)
 */
int
  ui_gsa_api_fec_range_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_up_to_sys_fec_ndx;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_range_remove";

  unit = gsa_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_RANGE_REMOVE_RANGE_REMOVE_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_RANGE_REMOVE_RANGE_REMOVE_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after range_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_RANGE_REMOVE_RANGE_REMOVE_UP_TO_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_FEC_RANGE_REMOVE_RANGE_REMOVE_UP_TO_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_sys_fec_ndx = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter up_to_sys_fec_ndx after range_remove***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = gsa_fec_range_remove(
          prm_sys_fec_ndx,
          prm_up_to_sys_fec_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_fec_range_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_fec_range_remove");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_fec_range_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: table_clear (section fec)
 */
int
  ui_gsa_api_fec_table_clear(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");
  soc_sand_proc_name = "gsa_fec_table_clear";

  unit = gsa_get_default_unit();



  /* Call function */
  ret = gsa_fec_table_clear(
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_fec_table_clear - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_fec_table_clear");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_fec_table_clear - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_IPV4_UC
/********************************************************************
 *  Function handler: route_add (section ipv4_uc)
 */
int
  ui_gsa_api_ipv4_uc_route_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV4_SUBNET
    prm_route_key;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_id=0;
  uint8
    prm_override=0;
  uint8
    prm_success=0;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_uc");
  soc_sand_proc_name = "gsa_ipv4_uc_route_add";

  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_route_key);

  unit = gsa_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_ROUTE_KEY_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_ROUTE_KEY_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_ROUTE_KEY_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_SYS_FEC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_SYS_FEC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_id = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_OVERRIDE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_OVERRIDE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_override = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = gsa_ipv4_uc_route_add(
          &prm_route_key,
          prm_sys_fec_id,
          prm_override,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv4_uc_route_add - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv4_uc_route_add");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv4_uc_route_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: route_get (section ipv4_uc)
 */
int
  ui_gsa_api_ipv4_uc_route_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV4_SUBNET
    prm_route_key;
  uint8
    prm_exact_match=1;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_id;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_uc");
  soc_sand_proc_name = "gsa_ipv4_uc_route_get";

  unit = gsa_get_default_unit();
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_route_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_ROUTE_GET_ROUTE_GET_ROUTE_KEY_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_UC_ROUTE_GET_ROUTE_GET_ROUTE_KEY_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_UC_ROUTE_GET_ROUTE_GET_ROUTE_KEY_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_ROUTE_GET_ROUTE_GET_EXACT_MATCH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_UC_ROUTE_GET_ROUTE_GET_EXACT_MATCH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_exact_match = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = gsa_ipv4_uc_route_get(
          &prm_route_key,
          prm_exact_match,
          &prm_sys_fec_id,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv4_uc_route_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv4_uc_route_get");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv4_uc_route_get - SUCCEEDED", TRUE);
  if (prm_found)
  {
    send_string_to_screen("--> Found:", TRUE);
    soc_sand_os_printf( "sys_fec_id: %lu\n\r",prm_sys_fec_id);
  }
  else
  {
    send_string_to_screen("--> Not Found:", TRUE);
  }



  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: route_lpm_get (section ipv4_uc)
 */
int
  ui_gsa_api_ipv4_uc_route_lpm_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV4_SUBNET
    prm_route_key;
  SOC_SAND_PP_IPV4_SUBNET
    prm_longest_match;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_uc");
  soc_sand_proc_name = "gsa_ipv4_uc_route_lpm_get";

  unit = gsa_get_default_unit();
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_route_key);
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_longest_match);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_ROUTE_LPM_GET_ROUTE_LPM_GET_ROUTE_KEY_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_UC_ROUTE_LPM_GET_ROUTE_LPM_GET_ROUTE_KEY_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_UC_ROUTE_LPM_GET_ROUTE_LPM_GET_ROUTE_KEY_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.ip_address = param_val->value.ip_value;
  }

  /* Call function */
  ret = gsa_ipv4_uc_route_lpm_get(
          &prm_route_key,
          &prm_longest_match,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv4_uc_route_lpm_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv4_uc_route_lpm_get");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv4_uc_route_lpm_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> longest_match:", TRUE);
  if (prm_found)
  {
    send_string_to_screen("--> Found:", TRUE);
    soc_sand_SAND_PP_IPV4_SUBNET_print(&prm_longest_match);
  }
  else
  {
    send_string_to_screen("--> Not Found:", TRUE);
  }

exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: get_block (section ipv4_uc)
 */
int
  ui_gsa_api_ipv4_uc_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_IPV4_UC_ROUTE_TABLE_RANGE
    prm_range;
  GSA_IPV4_UC_ROUTE_ENTRY
    prm_route_table[UI_GSA_BLOCK_GET_SIZE];
  uint32
    prm_nof_entries;
  uint32
    flavors = SOC_PETRA_PP_IPV4_UC_PRINT_NEXT_HOP_INFO;
  uint32
    total_entries;
  char
    user_msg[5] = "";
  uint32
    con;


  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_uc");
  soc_sand_proc_name = "gsa_ipv4_uc_get_block";

  unit = gsa_get_default_unit();
  gsa_GSA_IPV4_UC_ROUTE_TABLE_RANGE_clear(&prm_range);
  prm_range.entries_to_act = UI_GSA_BLOCK_GET_SIZE;
  prm_range.entries_to_scan = 100000;
  prm_range.start.type = SOC_PETRA_PP_L3_TABLE_ITER_TYPE_FAST;
  prm_nof_entries = 0;
  total_entries = 0;


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_GET_BLOCK_GET_BLOCK_SHOW_FEC_INFO_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_UC_GET_BLOCK_GET_BLOCK_SHOW_FEC_INFO_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    flavors = (param_val->value.ulong_value == 0)?0:SOC_PETRA_PP_IPV4_UC_PRINT_NEXT_HOP_INFO;
  }

  while (!SOC_PETRA_PP_L3_TABLE_ITER_IS_END(&prm_range.start.payload) && prm_range.entries_to_act && prm_range.entries_to_scan)
  {

    /* Call function */
    ret = gsa_ipv4_uc_get_block(
            &prm_range,
            prm_route_table,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_ipv4_uc_get_block - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv4_uc_get_block");
      goto exit;
    }


    ret = gsa_ipv4_uc_print_block(
            prm_route_table,
            prm_nof_entries,
            flavors
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_ipv4_uc_get_block - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv4_uc_get_block");
      goto exit;
    }
    if (!SOC_PETRA_PP_L3_TABLE_ITER_IS_END(&(prm_range.start.payload)))
    {
      soc_sand_os_printf( "Hit Space to continue/Any Key to Abort.\n\r");
      con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
      if (!con)
      {
        break;
      }
    }
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: print_block (section ipv4_uc)
 */
int
  ui_gsa_api_ipv4_uc_print_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_IPV4_UC_ROUTE_ENTRY
    prm_route_table;
  uint32
    prm_nof_entries;
  uint32
    prm_flavors;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_uc");
  soc_sand_proc_name = "gsa_ipv4_uc_print_block";

  unit = gsa_get_default_unit();
  gsa_GSA_IPV4_UC_ROUTE_ENTRY_clear(&prm_route_table);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_ROUTE_TABLE_SYS_FEC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_ROUTE_TABLE_SYS_FEC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_table.sys_fec_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_ROUTE_TABLE_KEY_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_ROUTE_TABLE_KEY_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_table.key.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_ROUTE_TABLE_KEY_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_table.key.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_NOF_ENTRIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_NOF_ENTRIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_entries = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_FLAVORS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_FLAVORS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flavors = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = gsa_ipv4_uc_print_block(
          &prm_route_table,
          prm_nof_entries,
          prm_flavors
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv4_uc_print_block - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv4_uc_print_block");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv4_uc_print_block - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: route_remove (section ipv4_uc)
 */
int
  ui_gsa_api_ipv4_uc_route_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV4_SUBNET
    prm_route_key;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_uc");
  soc_sand_proc_name = "gsa_ipv4_uc_route_remove";

  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_route_key);

  unit = gsa_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_ROUTE_KEY_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_ROUTE_KEY_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_ROUTE_KEY_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.ip_address = param_val->value.ip_value;
  }

  /* Call function */
  ret = gsa_ipv4_uc_route_remove(
          &prm_route_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv4_uc_route_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv4_uc_route_remove");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv4_uc_route_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: routes_clear (section ipv4_uc)
 */
int
  ui_gsa_api_ipv4_uc_routes_clear(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_uc");
  soc_sand_proc_name = "gsa_ipv4_uc_routes_clear";

  unit = gsa_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = gsa_ipv4_uc_routes_clear(
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv4_uc_routes_clear - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv4_uc_routes_clear");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv4_uc_routes_clear - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_IPV4_MC


/********************************************************************
 *  Function handler: ipv4_mc_route_add (section ipv4_mc)
 */
int
  ui_gsa_api_ipv4_mc_ipv4_mc_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_IPV4_MC_VRF_INFO
    prm_init_info;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_mc");
  soc_sand_proc_name = "gsa_ipv4_mc_init";


  unit = gsa_get_default_unit();
  gsa_GSA_IPV4_MC_VRF_INFO_clear(&prm_init_info);




  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_INIT_USE_PORT_ID,1))
  {
    prm_init_info.use_mapped_port = TRUE;
  }

    /* Call function */
    ret = gsa_ipv4_mc_vrf_info_set(
            &prm_init_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_ipv4_mc_route_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv4_mc_route_add");
      goto exit;
    }

  send_string_to_screen(" ---> gsa_ipv4_mc_init - SUCCEEDED", TRUE);


  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
 *  Function handler: ipv4_mc_route_add (section ipv4_mc)
 */
int
  ui_gsa_api_ipv4_mc_ipv4_mc_route_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    mask_len,
    from;
  SOC_SAND_PP_IPV4_MC_ROUTE_KEY
    prm_route_key;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  uint32
    prm_count,
    indx;
  uint8
    prm_override = FALSE,
    inc_sys_fec = FALSE;
  uint8
    prm_success=0;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_mc");
  soc_sand_proc_name = "gsa_ipv4_mc_route_add";


  unit = gsa_get_default_unit();
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(&prm_route_key);


  prm_count = 1;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after ipv4_mc_route_add***", TRUE);
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_count = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_INC_SYS_FEC_NDX_ID,1))
  {
    inc_sys_fec = TRUE;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.group.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.group.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.source.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.source.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_OVERRIDE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_OVERRIDE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_override = (uint8)param_val->value.ulong_value;
  }

  for (indx = 0; indx < prm_count; ++indx )
  {
    /* Call function */
    ret = gsa_ipv4_mc_route_add(
            &prm_route_key,
            prm_sys_fec_ndx,
            prm_override,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_ipv4_mc_route_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv4_mc_route_add");
      goto exit;
    }
    mask_len = 32 - prm_route_key.group.prefix_len;
    from = prm_route_key.group.ip_address >> mask_len;
    from += 1;
    from <<= mask_len;
    prm_route_key.group.ip_address = from;
    if (inc_sys_fec)
    {
      ++prm_sys_fec_ndx;
    }
  }

  send_string_to_screen(" ---> gsa_ipv4_mc_route_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv4_mc_route_get (section ipv4_mc)
 */
int
  ui_gsa_api_ipv4_mc_ipv4_mc_route_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV4_MC_ROUTE_KEY
    prm_route_key;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_mc");
  soc_sand_proc_name = "gsa_ipv4_mc_route_get";

  prm_route_key.vid.mask = 0;


  unit = gsa_get_default_unit();
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(&prm_route_key);

  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.group.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.group.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.source.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.source.ip_address = param_val->value.ip_value;
  }


  /* Call function */
  ret = gsa_ipv4_mc_route_get(
          &prm_route_key,
          &prm_sys_fec_ndx,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv4_mc_route_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv4_mc_route_get");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv4_mc_route_get - SUCCEEDED", TRUE);
  if (prm_found)
  {
    soc_sand_os_printf( "Found \n\r",prm_found);
    soc_sand_os_printf( "sys_fec_ndx: %lu\n\r",prm_sys_fec_ndx);
  }
  else
  {
    soc_sand_os_printf( "Not Found \n\r");
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv4_mc_route_lpm_get (section ipv4_mc)
 */
int
  ui_gsa_api_ipv4_mc_ipv4_mc_route_lpm_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV4_MC_ROUTE_KEY
    prm_route_key;
  SOC_SAND_PP_IPV4_MC_ROUTE_KEY
    prm_longest_match;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_mc");
  soc_sand_proc_name = "gsa_ipv4_mc_route_lpm_get";

  unit = gsa_get_default_unit();
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(&prm_route_key);
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(&prm_longest_match);

  /* Get parameters */


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.group.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.group.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.source.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.source.ip_address = param_val->value.ip_value;
  }

  /* Call function */
  ret = gsa_ipv4_mc_route_lpm_get(
          &prm_route_key,
          &prm_longest_match,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv4_mc_route_lpm_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv4_mc_route_lpm_get");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv4_mc_route_lpm_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> longest_match:", TRUE);
  if (prm_found)
  {
    send_string_to_screen("--> Found:", TRUE);
    soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_print(&prm_longest_match);
  }
  else
  {
    send_string_to_screen("--> Not Found:", TRUE);
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv4_mc_route_get_block (section ipv4_mc)
 */
int
  ui_gsa_api_ipv4_mc_ipv4_mc_route_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_BLOCK_RANGE
    prm_block_range;
  GSA_IPV4_MC_ROUTE_ENTRY
    prm_route_table[UI_GSA_BLOCK_GET_SIZE];
  uint32
    prm_nof_entries,
    flavors;
  char
    user_msg[5] = "";
  uint32
    con;
  uint32
    total_entries;
  uint8
    print_entries;



  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_mc");
  soc_sand_proc_name = "gsa_ipv4_mc_route_get_block";

  unit = gsa_get_default_unit();
  prm_block_range.entries_to_act = UI_GSA_BLOCK_GET_SIZE;
  prm_block_range.entries_to_scan = 10000;
  prm_block_range.iter = SOC_PETRA_PP_TCAM_LIST_ITER_BEGIN(0);

  flavors = 0;



  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_GET_BLOCK_IPV4_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_GET_BLOCK_IPV4_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    flavors = (uint32)param_val->value.ulong_value;
  }

  total_entries = 0;
  print_entries = TRUE;

  while (prm_block_range.iter !=  SOC_PETRA_PP_TCAM_LIST_ITER_END(0) && prm_block_range.entries_to_act && prm_block_range.entries_to_scan)
  {

    /* Call function */
    ret = gsa_ipv4_mc_route_get_block(
            &prm_block_range,
            prm_route_table,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_ipv4_mc_route_get_block - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv4_mc_route_get_block");
      goto exit;
    }

    if (prm_nof_entries == 0)
    {
      break;
    }

    total_entries += prm_nof_entries;
    if (print_entries == FALSE)
    {
      continue;
    }



    ret = gsa_ipv4_mc_print_block(
            prm_route_table,
            prm_nof_entries,
            flavors
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_ipv4_mc_print_block - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv4_mc_print_block");
      goto exit;
    }


    if (prm_block_range.iter !=  SOC_PETRA_PP_TCAM_LIST_ITER_END(0) && prm_nof_entries >= prm_block_range.entries_to_act)
    {
      soc_sand_os_printf( "Hit Space to continue/Any Key to Abort.\n\r");
      con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
      if (!con)
      {
        print_entries = FALSE;
      }
    }

  }

  soc_sand_os_printf( "total_entries: %lu\n\r",total_entries);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv4_mc_route_remove (section ipv4_mc)
 */
int
  ui_gsa_api_ipv4_mc_ipv4_mc_route_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV4_MC_ROUTE_KEY
    prm_route_key;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_mc");
  soc_sand_proc_name = "gsa_ipv4_mc_route_remove";

  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(&prm_route_key);


  unit = gsa_get_default_unit();
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(&prm_route_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.group.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.group.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.source.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.source.ip_address = param_val->value.ip_value;
  }

  /* Call function */
  ret = gsa_ipv4_mc_route_remove(
          &prm_route_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv4_mc_route_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv4_mc_route_remove");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv4_mc_route_remove - SUCCEEDED", TRUE);


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: ipv4_mc_route_remove_all (section ipv4_mc)
 */
int
  ui_gsa_api_ipv4_mc_ipv4_mc_route_remove_all(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_mc");
  soc_sand_proc_name = "gsa_ipv4_mc_route_remove_all";


  unit = gsa_get_default_unit();


  /* Call function */
  ret = gsa_ipv4_mc_route_remove_all(
          FALSE
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv4_mc_route_remove_all - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv4_mc_route_remove_all");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv4_mc_route_remove_all - SUCCEEDED", TRUE);


  goto exit;
exit:
  return ui_ret;
}

#endif



#ifdef UI_IPV6

/********************************************************************
 *  Function handler: ipv6_route_add (section ipv6)
 */
int
  ui_gsa_api_ipv6_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_IPV6_VRF_INFO
    prm_init_info;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv6");
  soc_sand_proc_name = "gsa_ipv6_init";


  unit = gsa_get_default_unit();
  gsa_GSA_IPV6_VRF_INFO_clear(&prm_init_info);


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_INIT_USE_PORT_ID,1))
  {
    prm_init_info.use_mapped_port = TRUE;
  }

    /* Call function */
    ret = gsa_ipv6_vrf_info_set(
            &prm_init_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_ipv6_route_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv6_route_add");
      goto exit;
    }

  send_string_to_screen(" ---> gsa_ipv6_init - SUCCEEDED", TRUE);


  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
 *  Function handler: ipv6_route_add (section ipv6)
 */
int
  ui_gsa_api_ipv6_route_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    mask_len,
    from;
  uint32
    prm_count = 1,
    indx;
  SOC_SAND_PP_IPV6_ROUTE_KEY
    prm_route_key;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  uint8
    prm_override = FALSE,
    inc_sys_fec = FALSE;
  uint8
    prm_success=0;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv6");
  soc_sand_proc_name = "gsa_ipv6_route_add";


  unit = gsa_get_default_unit();
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_clear(&prm_route_key);

  prm_route_key.vid.mask = 0;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after ipv6_route_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_INC_SYS_FEC_NDX_ID,1))
  {
    inc_sys_fec = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_count = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.dest.prefix_len = (uint8)param_val->value.ulong_value;
  }


  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    ret = soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.dest.ipv6_address));
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_pp_ipv6_address_string_parse - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv6_route_add");
      goto exit;
    }
 }



  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_OVERRIDE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_OVERRIDE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_override = (uint8)param_val->value.ulong_value;
  }
  for (indx = 0; indx < prm_count; ++indx )
  {
    /* Call function */
    ret = gsa_ipv6_route_add(
            &prm_route_key,
            prm_sys_fec_ndx,
            prm_override,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_ipv6_route_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv6_route_add");
      goto exit;
    }
    if (inc_sys_fec)
    {
      ++prm_sys_fec_ndx;
    }
    mask_len = 32 - (prm_route_key.dest.prefix_len % 32);
    from = prm_route_key.dest.ipv6_address.address[(128 - prm_route_key.dest.prefix_len) /32] >> mask_len;
    from += 1;
    from <<= mask_len;
    prm_route_key.dest.ipv6_address.address[(128 - prm_route_key.dest.prefix_len) /32] = from;
    ++prm_sys_fec_ndx;

  }

  /*prm_route_key.dest.ipv6_address*/

  send_string_to_screen(" ---> gsa_ipv6_route_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv6_route_get (section ipv6)
 */
int
  ui_gsa_api_ipv6_route_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV6_ROUTE_KEY
    prm_route_key;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv6");
  soc_sand_proc_name = "gsa_ipv6_route_get";

  prm_route_key.vid.mask = 0;


  unit = gsa_get_default_unit();
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_clear(&prm_route_key);

  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.dest.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    ret = soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.dest.ipv6_address));
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_pp_ipv6_address_string_parse - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv6_route_get");
      goto exit;
    }
  }


  /* Call function */
  ret = gsa_ipv6_route_get(
          &prm_route_key,
          &prm_sys_fec_ndx,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv6_route_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv6_route_get");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv6_route_get - SUCCEEDED", TRUE);
  if (prm_found)
  {
    send_string_to_screen("--> Found:", TRUE);
    soc_sand_os_printf( "sys_fec_id: %lu\n\r",prm_sys_fec_ndx);
  }
  else
  {
    send_string_to_screen("--> Not Found:", TRUE);
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv6_route_lpm_get (section ipv6)
 */
int
  ui_gsa_api_ipv6_route_lpm_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV6_ROUTE_KEY
    prm_route_key;
  SOC_SAND_PP_IPV6_ROUTE_KEY
    prm_longest_match;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv6");
  soc_sand_proc_name = "gsa_ipv6_route_lpm_get";

  unit = gsa_get_default_unit();
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_clear(&prm_route_key);
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_clear(&prm_longest_match);


  /* Get parameters */


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.dest.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    ret = soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.dest.ipv6_address));
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_pp_ipv6_address_string_parse - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv6_route_lpm_get");
      goto exit;
    }
}


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }




  /* Call function */
  ret = gsa_ipv6_route_lpm_get(
          &prm_route_key,
          &prm_longest_match,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv6_route_lpm_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv6_route_lpm_get");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv6_route_lpm_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> longest_match:", TRUE);
  if (prm_found)
  {
    send_string_to_screen("--> Found:", TRUE);
    soc_sand_SAND_PP_IPV6_ROUTE_KEY_print(&prm_longest_match);
  }
  else
  {
    send_string_to_screen("--> Not Found:", TRUE);
  }



  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv6_route_get_block (section ipv6)
 */
int
  ui_gsa_api_ipv6_route_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_BLOCK_RANGE
    prm_block_range;
  GSA_IPV6_ROUTE_ENTRY
    prm_route_table[UI_GSA_BLOCK_GET_SIZE];
  uint32
    prm_nof_entries,
    flavors;
  char
    user_msg[5] = "";
  uint32
    con;
  uint32
    total_entries;
  uint8
    print_entries;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv6");
  soc_sand_proc_name = "gsa_ipv6_route_get_block";

  unit = gsa_get_default_unit();
  prm_block_range.entries_to_act = UI_GSA_BLOCK_GET_SIZE;
  prm_block_range.entries_to_scan = 10000;
  prm_block_range.iter = SOC_PETRA_PP_TCAM_LIST_ITER_BEGIN(0);
  flavors = 0;


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_GET_BLOCK_IPV6_ROUTE_GET_BLOCK_BLOCK_RANGE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_GET_BLOCK_IPV6_ROUTE_GET_BLOCK_BLOCK_RANGE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_block_range.iter = param_val->value.ulong_value;
  }

  total_entries = 0;
  print_entries = TRUE;

  while (prm_block_range.iter !=  SOC_PETRA_PP_TCAM_LIST_ITER_END(0) && prm_block_range.entries_to_act && prm_block_range.entries_to_scan)
  {

    /* Call function */
    ret = gsa_ipv6_route_get_block(
            &prm_block_range,
            prm_route_table,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_ipv6_route_get_block - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv6_route_get_block");
      goto exit;
    }

    if (prm_nof_entries == 0)
    {
      break;
    }

    total_entries += prm_nof_entries;
    if (print_entries == FALSE)
    {
      continue;
    }
    ret = gsa_ipv6_route_print_block(
            prm_route_table,
            prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_ipv6_route_print_block - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv6_route_print_block");
      goto exit;
    }


    if (prm_block_range.iter !=  SOC_PETRA_PP_TCAM_LIST_ITER_END(0) && prm_nof_entries >= prm_block_range.entries_to_act)
    {
      soc_sand_os_printf( "Hit Space to continue/Any Key to Abort.\n\r");
      con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
      if (!con)
      {
        print_entries = FALSE;
      }
    }
  }

  soc_sand_os_printf( "total_entries: %lu\n\r",total_entries);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv6_route_remove (section ipv6)
 */
int
  ui_gsa_api_ipv6_route_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV6_ROUTE_KEY
    prm_route_key;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv6");
  soc_sand_proc_name = "gsa_ipv6_route_remove";

  prm_route_key.vid.mask = 0;


  unit = gsa_get_default_unit();
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_clear(&prm_route_key);

  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.dest.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    ret = soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.dest.ipv6_address));
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_pp_ipv6_address_string_parse - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_ipv6_route_remove");
      goto exit;
    }
  }

  /* Call function */
  ret = gsa_ipv6_route_remove(
          &prm_route_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_ipv6_route_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_ipv6_route_remove");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_ipv6_route_remove - SUCCEEDED", TRUE);


  goto exit;
exit:
  return ui_ret;
}



#endif

#ifdef UI_ACL
/********************************************************************
 *  Function handler: ace_add (section acl)
 */
int
  ui_gsa_api_acl_ace_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx;
  SOC_PETRA_PP_ACL_ACE_INFO
    prm_ace;
  /*GSA_ACL_RANGE
    prm_range_info;*/
  GSA_ACL_ACTION_INFO
    prm_action_info;
  uint8
    prm_override = FALSE;
  uint8
    prm_success=0;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_acl");
  soc_sand_proc_name = "gsa_acl_ace_add";

  unit = gsa_get_default_unit();
  soc_petra_pp_PETRA_PP_ACL_ACE_INFO_clear(&prm_ace);
  prm_ace.val.l2_info.c_tag.vid.val = 0;
  prm_ace.val.l2_info.s_tag.vid.val = 0;
  gsa_GSA_ACL_ACTION_INFO_clear(&prm_action_info);
  /*gsa_GSA_ACL_RANGE_clear(&prm_range_info);*/

  /*prm_range_info.inc_sys_fec_id = FALSE;*/
  /*prm_range_info.inc_vid = FALSE;*/

  prm_ace.val.l2_info.in_port_type = SOC_SAND_PP_PORT_L2_TYPE_VBP;
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after ace_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_DEST_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_DEST_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range.max_dest_port = (uint16)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.port_range_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_DEST_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_DEST_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range.min_dest_port = (uint16)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.port_range_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_SRC_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_SRC_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range.max_src_port = (uint16)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.port_range_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_SRC_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_SRC_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range.min_src_port = (uint16)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.port_range_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.dest_port.val = (uint16)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.dest_port.mask = (uint16)UI_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.dest_port.mask = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.src_port.val = (uint16)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.src_port.mask = (uint16)UI_ACL_MASK_SET_ALL;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.src_port.mask = (uint16)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.mask = (uint8)UI_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.mask = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_IP_ADDRESS_ID,1,
    &param_val,VAL_IP,err_msg))
  {
    prm_ace.val.l3_l4_info.l3_info.dest_ip.ip_address = param_val->value.ip_value;
    prm_ace.val.l3_l4_info.l3_info.dest_ip.prefix_len = 32;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.dest_ip.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_ace.val.l3_l4_info.l3_info.src_ip.ip_address = param_val->value.ip_value;
    prm_ace.val.l3_l4_info.l3_info.src_ip.prefix_len = 32;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.src_ip.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.protocol_code.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l3_info.protocol_code.mask = (uint8)UI_ACL_MASK_SET_ALL;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.protocol_code.mask = (uint8)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.tos.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l3_info.tos.mask = (uint8)UI_ACL_MASK_SET_ALL;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.tos.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.in_vid.val = (uint32)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l3_info.in_vid.mask = (uint32)UI_ACL_MASK_SET_ALL;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.in_vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.in_local_port_id.val = (uint32)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l3_info.in_local_port_id.mask = (uint32)UI_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.in_local_port_id.mask = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_DEST_MAC_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_ace.val.l2_info.dest_mac));
    soc_sand_pp_mac_address_string_parse("FFFFFFFF", &(prm_ace.val.l2_info.dest_mac_mask));
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_DEST_MAC_MASK_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_ace.val.l2_info.dest_mac_mask));
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_SRC_MAC_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_ace.val.l2_info.src_mac));
    soc_sand_pp_mac_address_string_parse("FFFFFFFF", &(prm_ace.val.l2_info.src_mac_mask));
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_SRC_MAC_MASK_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_ace.val.l2_info.src_mac_mask));
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.ether_type.val = (uint16)param_val->value.ulong_value;
    prm_ace.val.l2_info.ether_type.mask = (uint16)UI_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.ether_type.mask = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_DSAP_ID,1))
  {
    prm_ace.val.l2_info.ether_type_dsap_ssap = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.dei.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l2_info.s_tag.dei.mask = (uint8)UI_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.dei.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.pcp.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l2_info.s_tag.pcp.mask = (uint8)UI_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.pcp.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.vid.val = (uint32)param_val->value.ulong_value;
    prm_ace.val.l2_info.s_tag.vid.mask = (uint32)UI_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.tag_exist.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l2_info.s_tag.tag_exist.mask = (uint8)UI_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.tag_exist.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.dei.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l2_info.c_tag.dei.mask = (uint8)UI_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.dei.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.pcp.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l2_info.c_tag.pcp.mask = (uint8)UI_ACL_MASK_SET_ALL;

  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.pcp.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.vid.val = (uint32)param_val->value.ulong_value;
    prm_ace.val.l2_info.c_tag.vid.mask = (uint32)UI_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.tag_exist.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l2_info.c_tag.tag_exist.mask = (uint8)UI_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.tag_exist.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.in_local_port_id.val = (uint32)param_val->value.ulong_value;
    prm_ace.val.l2_info.in_local_port_id.mask = (uint32)UI_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.in_local_port_id.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.in_port_type = param_val->numeric_equivalent;
    prm_ace.val.l2_info.in_port_type_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.in_port_type_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_L2_ID,1))
  {
    prm_ace.type = SOC_PETRA_PP_ACL_ACE_TYPE_L2;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_L3A_ID,1))
  {
    prm_ace.type = SOC_PETRA_PP_ACL_ACE_TYPE_L3A;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_L3B_ID,1))
  {
    prm_ace.type = SOC_PETRA_PP_ACL_ACE_TYPE_L3B;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.priority = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ace_ndx after acl_ndx***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.id = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ace_ndx after acl_ndx***", TRUE);
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_UPDATE_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_UPDATE_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.update_type = param_val->numeric_equivalent;
  }

  prm_action_info.action_mask = 0;
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_ACTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.cos_action = (uint8)param_val->value.ulong_value;
    prm_action_info.action_mask |= SOC_PETRA_PP_ACL_ACTION_MASK_COS;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_SYS_FEC_ID_ID,1))
  {
    prm_action_info.update_type = SOC_PETRA_PP_ACL_ACTION_TYPE_UPDATE_L3;
  }
  else
  {
    prm_action_info.update_type = SOC_PETRA_PP_ACL_ACTION_TYPE_UPDATE_L2;
  }


  if (prm_action_info.update_type == SOC_PETRA_PP_ACL_ACTION_TYPE_UPDATE_L2)
  {

    if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_VID_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_VID_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_action_info.l2_info.vid = (uint32)param_val->value.ulong_value;
      prm_action_info.action_mask |= SOC_PETRA_PP_ACL_ACTION_MASK_VID;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_DESTINATION_DEST_VAL_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_DESTINATION_DEST_VAL_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_action_info.l2_info.destination.dest_val = (uint32)param_val->value.ulong_value;
      prm_action_info.action_mask |= SOC_PETRA_PP_ACL_ACTION_MASK_DEST;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_DESTINATION_DEST_TYPE_ID,1))
    {
      UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_DESTINATION_DEST_TYPE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_action_info.l2_info.destination.dest_type = param_val->numeric_equivalent;
      prm_action_info.action_mask |= SOC_PETRA_PP_ACL_ACTION_MASK_DEST;
    }

  }
  else
  {
    prm_action_info.action_mask |= SOC_PETRA_PP_ACL_ACTION_UPDATE_FEC;
    if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_SYS_FEC_ID_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_SYS_FEC_ID_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_action_info.l3_info.sys_fec_id = (uint32)param_val->value.ulong_value;
    }
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_OVERRIDE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_ADD_ACE_ADD_OVERRIDE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_override = (uint8)param_val->value.ulong_value;
  }

  /*if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_VID_ID,1))
  {
    prm_range_info.inc_vid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_SYS_FEC_ID_ID,1))
  {
    prm_range_info.inc_sys_fec_id = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.step = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.count = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_FIELD_TO_INC_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_FIELD_TO_INC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.field_to_inc = param_val->numeric_equivalent;
  } */

/*  if (prm_range_info.count <= 1)
  {*/
    /* Call function */
    ret = gsa_acl_ace_add(
            prm_acl_ndx,
            &prm_ace,
            &prm_action_info,
            prm_override,
            &prm_success
          );
  /*}
  else
  {
     Call function
    ret = gsa_acl_range_add(
            prm_acl_ndx,
            &prm_ace,
            &prm_range_info,
            &prm_action_info,
            &prm_success
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_acl_ace_add - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_acl_ace_add");
    goto exit;
  }      */

  send_string_to_screen(" ---> gsa_acl_ace_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ace_remove (section acl)
 */
int
  ui_gsa_api_acl_ace_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx;
  uint32
    prm_ace_id_ndx;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_acl");
  soc_sand_proc_name = "gsa_acl_ace_remove";

  unit = gsa_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_REMOVE_ACE_REMOVE_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_REMOVE_ACE_REMOVE_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after ace_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_REMOVE_ACE_REMOVE_ACE_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_REMOVE_ACE_REMOVE_ACE_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ace_id_ndx after ace_remove***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = gsa_acl_ace_remove(
          prm_acl_ndx,
          prm_ace_id_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_acl_ace_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_acl_ace_remove");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_acl_ace_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ace_get (section acl)
 */
int
  ui_gsa_api_acl_ace_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx;
  uint32
    prm_ace_id_ndx;
  uint8
    prm_valid_ace;
  SOC_PETRA_PP_ACL_ACE_INFO
    prm_ace;
  GSA_ACL_ACTION_INFO
    prm_action_info;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_acl");
  soc_sand_proc_name = "gsa_acl_ace_get";

  unit = gsa_get_default_unit();
  soc_petra_pp_PETRA_PP_ACL_ACE_INFO_clear(&prm_ace);
  gsa_GSA_ACL_ACTION_INFO_clear(&prm_action_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_GET_ACE_GET_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_GET_ACE_GET_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after ace_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_GET_ACE_GET_ACE_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_GET_ACE_GET_ACE_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ace_id_ndx after ace_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = gsa_acl_ace_get(
          prm_acl_ndx,
          prm_ace_id_ndx,
          &prm_valid_ace,
          &prm_ace,
          &prm_action_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_acl_ace_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_acl_ace_get");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_acl_ace_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> valid_ace:", TRUE);
  soc_sand_os_printf( "valid_ace: %u\n\r",prm_valid_ace);

  if (prm_valid_ace)
  {
    send_string_to_screen("--> ace:", TRUE);
    soc_petra_pp_PETRA_PP_ACL_ACE_INFO_print(&prm_ace);

    send_string_to_screen("--> action_info:", TRUE);
    gsa_GSA_ACL_ACTION_INFO_print(&prm_action_info);
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ace_get_block (section acl)
 */
int
  ui_gsa_api_acl_ace_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx;
  SOC_PETRA_PP_BLOCK_RANGE
    prm_range;
  SOC_PETRA_PP_ACL_ACE_INFO
    prm_aces[UI_GSA_BLOCK_GET_SIZE_SMALL];
  GSA_ACL_ACTION_INFO
    prm_actions[UI_GSA_BLOCK_GET_SIZE_SMALL];
  uint32
    prm_nof_entries,
    total_entries;
  uint8
    print_entries=TRUE;
  char
    user_msg[5] = "";
  uint32
    con;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_acl");
  soc_sand_proc_name = "gsa_acl_ace_get_block";

  unit = gsa_get_default_unit();


  prm_range.entries_to_act = UI_GSA_BLOCK_GET_SIZE_SMALL;
  prm_range.entries_to_scan = 20000;
  prm_range.iter = SOC_PETRA_PP_TCAM_LIST_ITER_BEGIN(SOC_PETRA_PP_TCAM_ENTRY_USE_ACL_L2);
  prm_nof_entries = 0;
  total_entries = 0;


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after ace_get_block***", TRUE);
    goto exit;
  }

  while (prm_range.iter != SOC_PETRA_PP_TCAM_LIST_ITER_END(0) && prm_range.entries_to_act && prm_range.entries_to_scan)
  {
    /* Call function */
    ret = gsa_acl_ace_get_block(
            prm_acl_ndx,
            &prm_range,
            prm_aces,
            prm_actions,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_acl_ace_get_block - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_acl_ace_get_block");
      goto exit;
    }
    total_entries += prm_nof_entries;
    if (prm_nof_entries == 0)
    {
      break;
    }
    if (print_entries == FALSE)
    {
      continue;
    }

    /* Call function */
    ret = gsa_acl_ace_print_block(
            prm_acl_ndx,
            prm_aces,
            prm_actions,
            prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_acl_ace_get_block - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_acl_ace_get_block");
      goto exit;
    }
    if (prm_range.iter != SOC_PETRA_PP_TCAM_LIST_ITER_END(0) && prm_nof_entries >= prm_range.entries_to_act)
    {
      soc_sand_os_printf( "Hit Space to continue/Any Key to Abort.\n\r");
      con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
      if (!con)
      {
        print_entries = FALSE;
      }
    }

  }

  send_string_to_screen(" ---> gsa_acl_ace_get_block - SUCCEEDED", TRUE);
  send_string_to_screen("--> nof_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %lu\n\r",total_entries);


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: range_remove (section acl)
 */
int
  ui_gsa_api_acl_range_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx;
  uint32
    prm_from_ace_ndx;
  uint32
    prm_to_ace_ndx;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_acl");
  soc_sand_proc_name = "gsa_acl_range_remove";

  unit = gsa_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_RANGE_REMOVE_RANGE_REMOVE_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_RANGE_REMOVE_RANGE_REMOVE_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after range_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_RANGE_REMOVE_RANGE_REMOVE_FROM_ACE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_RANGE_REMOVE_RANGE_REMOVE_FROM_ACE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_from_ace_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter from_ace_ndx after range_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_RANGE_REMOVE_RANGE_REMOVE_TO_ACE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_ACL_RANGE_REMOVE_RANGE_REMOVE_TO_ACE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_to_ace_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter to_ace_ndx after range_remove***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = gsa_acl_ace_range_remove(
          prm_acl_ndx,
          prm_from_ace_ndx,
          prm_to_ace_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_acl_ace_range_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "gsa_acl_ace_range_remove");
    goto exit;
  }

  send_string_to_screen(" ---> gsa_acl_ace_range_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif


#ifdef UI_VLAN/* { vlan*/
/********************************************************************
 *  Section handler: vlan
 */
int
  ui_gsa_api_vlan(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_gsa_api_vlan");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_SET_SET_ID,1))
  {
    ui_ret = ui_gsa_api_vlan_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_SET_RANGE_SET_ID,1))
  {
    ui_ret = ui_gsa_api_vlan_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_GET_GET_ID,1))
  {
    ui_ret = ui_gsa_api_vlan_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_MEMBER_ADD_MEMBER_ADD_ID,1))
  {
    ui_ret = ui_gsa_api_vlan_member_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_ID,1))
  {
    ui_ret = ui_gsa_api_vlan_member_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_EDIT_PORT_INFO_SET_PORT_INFO_SET_ID,1))
  {
    ui_ret = ui_gsa_api_edit_port_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_EDIT_PORT_INFO_GET_PORT_INFO_GET_ID,1))
  {
    ui_ret = ui_gsa_api_edit_port_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_EDIT_PORT_INFO_SET_PORT_PVID_SET_ID,1))
  {
    ui_ret = ui_gsa_api_edit_port_pvid_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_EDIT_PORT_INFO_GET_PORT_PVID_GET_ID,1))
  {
    ui_ret = ui_gsa_api_edit_port_pvid_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after vlan***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */vlan

#ifdef UI_MACT/* { mact*/
/********************************************************************
 *  Section handler: mact
 */
int
  ui_gsa_api_mact(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_gsa_api_mact");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_ID,1))
  {
    ui_ret = ui_gsa_api_mact_entry_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_ID,1))
  {
    ui_ret = ui_gsa_api_mact_entry_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_ENTRY_REMOVE_ENTRY_REMOVE_ID,1))
  {
    ui_ret = ui_gsa_api_mact_entry_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_RANGE_REMOVE_RANGE_REMOVE_ID,1))
  {
    ui_ret = ui_gsa_api_mact_range_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_ID,1))
  {
    ui_ret = ui_gsa_api_mact_flush_block(current_line,TRUE);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_FLUSH_BLOCK_TRANSPLANT_BLOCK_ID,1))
  {
    ui_ret = ui_gsa_api_mact_flush_block(current_line,FALSE);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_ID,1))
  {
    ui_ret = ui_gsa_api_mact_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_AGING_INFO_SET_AGING_INFO_SET_ID,1))
  {
    ui_ret = ui_gsa_api_mact_aging_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_AGING_INFO_GET_AGING_INFO_GET_ID,1))
  {
    ui_ret = ui_gsa_api_mact_aging_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_MY_MAC_ADD_MY_MAC_ADD_ID,1))
  {
    ui_ret = ui_gsa_api_mact_my_mac_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_MACT_MY_MAC_REMOVE_MY_MAC_REMOVE_ID,1))
  {
    ui_ret = ui_gsa_api_mact_my_mac_remove(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mact***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */mact

#ifdef UI_FEC/* { fec*/
/********************************************************************
 *  Section handler: fec
 */
int
  ui_gsa_api_fec(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_gsa_api_fec");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_ID,1))
  {
    ui_ret = ui_gsa_api_fec_uc_ecmp_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_ID,1))
  {
    ui_ret = ui_gsa_api_fec_uc_ecmp_update_entry(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_ID,1))
  {
    ui_ret = ui_gsa_api_fec_mc_fec_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_ID,1))
  {
    ui_ret = ui_gsa_api_fec_uc_emcp_remove_entry(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_REMOVE_REMOVE_ID,1))
  {
    ui_ret = ui_gsa_api_fec_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_GET_UC_GET_ID,1))
  {
    ui_ret = ui_gsa_api_fec_uc_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_GET_MC_GET_ID,1))
  {
    ui_ret = ui_gsa_api_fec_mc_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_TABLE_UC_GET_BLOCK_TABLE_UC_GET_BLOCK_ID,1))
  {
    ui_ret = ui_gsa_api_fec_table_uc_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_TABLE_MC_GET_BLOCK_TABLE_MC_GET_BLOCK_ID,1))
  {
    ui_ret = ui_gsa_api_fec_table_mc_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_ID,1))
  {
    ui_ret = ui_gsa_api_fec_uc_ecmp_range_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_ID,1))
  {
    ui_ret = ui_gsa_api_fec_mc_fec_range_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_RANGE_REMOVE_RANGE_REMOVE_ID,1))
  {
    ui_ret = ui_gsa_api_fec_range_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_FEC_TABLE_CLEAR_TABLE_CLEAR_ID,1))
  {
    ui_ret = ui_gsa_api_fec_table_clear(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fec***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */fec

#ifdef UI_IPV4_UC/* { ipv4_uc*/
/********************************************************************
 *  Section handler: ipv4_uc
 */
int
  ui_gsa_api_ipv4_uc(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_gsa_api_ipv4_uc");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_uc_route_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_ROUTE_GET_ROUTE_GET_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_uc_route_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_ROUTE_LPM_GET_ROUTE_LPM_GET_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_uc_route_lpm_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_GET_BLOCK_GET_BLOCK_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_uc_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_uc_print_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_uc_route_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_UC_ROUTES_CLEAR_ROUTES_CLEAR_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_uc_routes_clear(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ipv4_uc***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ipv4_uc

#ifdef UI_IPV4_MC/* { ipv4_mc*/
/********************************************************************
 *  Section handler: ipv4_mc
 */
int
  ui_gsa_api_ipv4_mc(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_petra_pp_api_ipv4_mc");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_INIT_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_mc_ipv4_mc_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_mc_ipv4_mc_route_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_mc_ipv4_mc_route_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_mc_ipv4_mc_route_lpm_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_GET_BLOCK_IPV4_MC_ROUTE_GET_BLOCK_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_mc_ipv4_mc_route_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_mc_ipv4_mc_route_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ALL_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_mc_ipv4_mc_route_remove_all(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ipv4_mc***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { ipv4_mc*/

#ifdef UI_IPV6/* { ipv6*/
/********************************************************************
 *  Section handler: ipv6
 */
int
  ui_gsa_api_ipv6(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_petra_pp_api_ipv6");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_INIT_ID,1))
  {
    ui_ret = ui_gsa_api_ipv6_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ID,1))
  {
    ui_ret = ui_gsa_api_ipv6_route_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ID,1))
  {
    ui_ret = ui_gsa_api_ipv6_route_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ID,1))
  {
    ui_ret = ui_gsa_api_ipv6_route_lpm_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_GET_BLOCK_IPV6_ROUTE_GET_BLOCK_ID,1))
  {
    ui_ret = ui_gsa_api_ipv6_route_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ID,1))
  {
    ui_ret = ui_gsa_api_ipv6_route_remove(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ipv6***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { ipv6*/

#ifdef UI_ACL/* { acl*/
/********************************************************************
 *  Section handler: acl
 */
int
  ui_gsa_api_acl(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_gsa_api_acl");


  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ID,1))
  {
    ui_ret = ui_gsa_api_acl_ace_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_REMOVE_ACE_REMOVE_ID,1))
  {
    ui_ret = ui_gsa_api_acl_ace_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_GET_ACE_GET_ID,1))
  {
    ui_ret = ui_gsa_api_acl_ace_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ID,1))
  {
    ui_ret = ui_gsa_api_acl_ace_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_ACL_RANGE_REMOVE_RANGE_REMOVE_ID,1))
  {
    ui_ret = ui_gsa_api_acl_range_remove(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after acl***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */acl
/*****************************************************
*NAME
*  subject_gsa_api
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_gsa_api' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_gsa_api(current_line,current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int
  subject_gsa_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  unsigned int
    match_index;
  int
    ui_ret;
  char
    err_msg[80*8] = "" ;
  unsigned int
    ;
  char
    *proc_name ;


  proc_name = "subject_gsa_api" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n\r",FALSE) ;
    send_string_to_screen("'subject_gsa_api()' function was called with no parameters.\r\n\r",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_gsa_api').
   */

  send_array_to_screen("\r\n\r",2) ;



  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_API_VLAN_ID,1))
  {
    ui_ret = ui_gsa_api_vlan(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_API_MACT_ID,1))
  {
    ui_ret = ui_gsa_api_mact(current_line);
  }
#ifndef PETRA_PP_MINIMAL
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_API_FEC_ID,1))
  {
    ui_ret = ui_gsa_api_fec(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_API_IPV4_UC_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_uc(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_IPV4_MC_ID,1))
  {
    ui_ret = ui_gsa_api_ipv4_mc(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_API_IPV6_ID,1))
  {
    ui_ret = ui_gsa_api_ipv6(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_API_ACL_ID,1))
  {
    ui_ret = ui_gsa_api_acl(current_line);
  }
#endif
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\r\n\r"
      "*** gsa_api command with unknown parameters'.\r\n\r"
      "    Syntax error/sw error...\r\n\r",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

#else
int ui_gsa_api = 0;
#endif

