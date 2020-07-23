#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#include <soc/mem.h>
#define _ERR_MSG_MODULE_NAME BSL_SOC_LLP



#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_filter.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_parse.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>





#define ARAD_PP_LLP_FILTER_PORT_PROFILE_NDX_MAX                  (7)
#define ARAD_PP_LLP_FILTER_VLAN_FORMAT_NDX_MAX                   (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)
#define ARAD_PP_LLP_FILTER_VID_MAX                            (4095)


#define ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_MIN_NOT_NULL (1)

#define ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL         (0)













CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_llp_filter[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_FILTER_GET_ERRS_PTR),
  



  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_llp_filter[] =
{
  
  {
    ARAD_PP_LLP_FILTER_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_FILTER_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port_profile_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_format_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_FILTER_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_FILTER_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_FILTER_ACCEPT_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_FILTER_ACCEPT_OUT_OF_RANGE_ERR",
    "The parameter 'accept' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  {
    SOC_PPC_PORTS_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORTS_OUT_OF_RANGE_ERR",
    "The parameter 'ports' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_UNEXPECTED_TAG_FORMAT_VLAN_FORMAT_ERR,
    "ARAD_PP_LLP_UNEXPECTED_TAG_FORMAT_VLAN_FORMAT_ERR",
    "The parameter 'vlan_format_ndx' was not found in llvp table. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FILTER_VID_NOT_DESIGNATED_ERR,
    "ARAD_PP_FILTER_VID_NOT_DESIGNATED_ERR",
    "The parameter 'vid' should be configured as designated when \n\r "
    "parameter 'accept' is false",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





uint32
  arad_pp_llp_filter_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res;
  uint32
      vlan_membership[5];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  soc_sand_os_memset(vlan_membership, 0x0, sizeof(vlan_membership));
  res = arad_fill_partial_table_with_entry(unit,IHP_VSI_PORT_MEMBERSHIPm,0,1,MEM_BLOCK_ANY,0,ARAD_PP_LLP_FILTER_VID_MAX, &vlan_membership);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_init_unsafe()", 0, 0);
}


uint32
  arad_pp_llp_filter_ingress_vlan_membership_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN   int                                    core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                               vid_ndx,
    SOC_SAND_IN  uint32                                      ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_UNSAFE);

  soc_sand_os_memcpy(&tbl_data.vlan_port_member_line, ports, sizeof(tbl_data.vlan_port_member_line));

  res = arad_pp_ihp_vlan_port_membership_tbl_set_unsafe(
          unit,
          core_id,
          vid_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_ingress_vlan_membership_set_unsafe()", vid_ndx, 0);
}

uint32
  arad_pp_llp_filter_ingress_vlan_membership_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_ingress_vlan_membership_set_verify()", vid_ndx, 0);
}

uint32
  arad_pp_llp_filter_ingress_vlan_membership_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_ingress_vlan_membership_get_verify()", vid_ndx, 0);
}


uint32
  arad_pp_llp_filter_ingress_vlan_membership_get_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                             vid_ndx,
    SOC_SAND_OUT SOC_PPC_PORT                                ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_UNSAFE);

  res = arad_pp_ihp_vlan_port_membership_tbl_get_unsafe(
    unit,
    core_id,
    vid_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  soc_sand_os_memset(ports, 0, sizeof(SOC_PPC_PORT) * SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE); 
  soc_sand_os_memcpy(ports, &tbl_data.vlan_port_member_line, sizeof(uint32) * 8); 
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_ingress_vlan_membership_get_unsafe()", vid_ndx, 0);
}


uint32
  arad_pp_llp_filter_ingress_vlan_membership_port_add_unsafe(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_UNSAFE);

  res = arad_pp_ihp_vlan_port_membership_tbl_get_unsafe(
    unit,
	core_id,
    vid_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_sand_bitstream_set_bit((uint32*)&tbl_data.vlan_port_member_line, local_port);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  res = arad_pp_ihp_vlan_port_membership_tbl_set_unsafe(
    unit,
    core_id,
    vid_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_ingress_vlan_membership_port_add_unsafe()", vid_ndx, 0);
}

uint32
  arad_pp_llp_filter_ingress_vlan_membership_port_add_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(local_port, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_ingress_vlan_membership_port_add_verify()", vid_ndx, 0);
}


uint32
  arad_pp_llp_filter_ingress_vlan_membership_port_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_UNSAFE);

  res = arad_pp_ihp_vlan_port_membership_tbl_get_unsafe(
    unit,
	core_id,
    vid_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_sand_bitstream_reset_bit((uint32*)&tbl_data.vlan_port_member_line, local_port);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  res = arad_pp_ihp_vlan_port_membership_tbl_set_unsafe(
    unit,
    core_id,
    vid_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_ingress_vlan_membership_port_remove_unsafe()", vid_ndx, 0);
}

uint32
  arad_pp_llp_filter_ingress_vlan_membership_port_remove_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(local_port, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_ingress_vlan_membership_port_remove_verify()", vid_ndx, 0);
}


uint32
  arad_pp_llp_filter_ingress_acceptable_frames_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT            vlan_format_ndx,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                        *action_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_profile);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_ingress_acceptable_frames_set_unsafe()", port_profile_ndx, 0);
}

uint32
  arad_pp_llp_filter_ingress_acceptable_frames_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT            vlan_format_ndx,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                          *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, ARAD_PP_LLP_FILTER_PORT_PROFILE_NDX_MAX, ARAD_PP_LLP_FILTER_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vlan_format_ndx, ARAD_PP_LLP_FILTER_VLAN_FORMAT_NDX_MAX, ARAD_PP_LLP_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR, 20, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_ACTION_PROFILE, action_profile, 30, exit);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_ingress_acceptable_frames_set_verify()", port_profile_ndx, 0);
}

uint32
  arad_pp_llp_filter_ingress_acceptable_frames_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT            vlan_format_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, ARAD_PP_LLP_FILTER_PORT_PROFILE_NDX_MAX, ARAD_PP_LLP_FILTER_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vlan_format_ndx, ARAD_PP_LLP_FILTER_VLAN_FORMAT_NDX_MAX, ARAD_PP_LLP_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_ingress_acceptable_frames_get_verify()", port_profile_ndx, 0);
}


uint32
  arad_pp_llp_filter_ingress_acceptable_frames_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT            vlan_format_ndx,
    SOC_SAND_OUT SOC_PPC_ACTION_PROFILE                        *action_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_ingress_acceptable_frames_get_unsafe()", port_profile_ndx, 0);
}


uint32
  arad_pp_llp_filter_designated_vlan_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                  local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                               vid,
    SOC_SAND_IN  uint8                                     accept,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset_i,
    offset_free,
    offset_inc,
    offset_dec;
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_in_pp_port_config_tbl_data;
  ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA
    designated_vlan_table_tbl_data;
  ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_REF_COUNT
    ref_count;
  soc_error_t
    rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);

  rv = sw_state_access[unit].dpp.soc.arad.pp.llp_filter.ref_count.get(unit, &ref_count);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 12, exit);

  res = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &vtt_in_pp_port_config_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  res = arad_pp_ihp_designated_vlan_table_tbl_get_unsafe(
    unit,
    vtt_in_pp_port_config_tbl_data.designated_vlan_index,
    &designated_vlan_table_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  offset_inc = offset_dec = offset_free = ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL;

  if (accept)
  {
    if (designated_vlan_table_tbl_data.designated_vlan == vid)
    {
      
      *success = SOC_SAND_SUCCESS;
      goto exit;
    }

    if (vtt_in_pp_port_config_tbl_data.designated_vlan_index !=
        ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL)
    {
      
      if (--(ref_count.arr[vtt_in_pp_port_config_tbl_data.designated_vlan_index]) == 0)
      {
        designated_vlan_table_tbl_data.designated_vlan = 0;

        res = arad_pp_ihp_designated_vlan_table_tbl_set_unsafe(
          unit,
          vtt_in_pp_port_config_tbl_data.designated_vlan_index,
          &designated_vlan_table_tbl_data
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
      }
    }

    for (offset_i = ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_MIN_NOT_NULL;
         offset_i < ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_SIZE;
         ++offset_i)
    {
      if (ref_count.arr[offset_i] == 0)
      {
        
        if (offset_free == ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL)
        {
          offset_free = offset_i;
        }
      }
      else
      {
        
        res = arad_pp_ihp_designated_vlan_table_tbl_get_unsafe(
          unit,
          offset_i,
          &designated_vlan_table_tbl_data
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

        if (designated_vlan_table_tbl_data.designated_vlan == vid)
        {
          
          offset_inc = offset_i;
          break;
        }
      }
    }

    if (offset_inc == ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL)
    {
      

      if (offset_free == ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL)
      {
        
        *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
        goto exit;
      }
      else
      {
        
        designated_vlan_table_tbl_data.designated_vlan = vid;

        res = arad_pp_ihp_designated_vlan_table_tbl_set_unsafe(
          unit,
          offset_free,
          &designated_vlan_table_tbl_data
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
        
        offset_inc = offset_free;
      }
    }
  }
  else 
  {
    if (vtt_in_pp_port_config_tbl_data.designated_vlan_index ==
        ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL)
    {
      
    }
    else
    {
      
      if (designated_vlan_table_tbl_data.designated_vlan != vid)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FILTER_VID_NOT_DESIGNATED_ERR, 20, exit);
      }

      
      if (--(ref_count.arr[vtt_in_pp_port_config_tbl_data.designated_vlan_index]) == 0)
      {
        designated_vlan_table_tbl_data.designated_vlan = 0;

        res = arad_pp_ihp_designated_vlan_table_tbl_set_unsafe(
          unit,
          vtt_in_pp_port_config_tbl_data.designated_vlan_index,
          &designated_vlan_table_tbl_data
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
      }

      offset_inc = ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL;
    }
  }
    
  vtt_in_pp_port_config_tbl_data.designated_vlan_index = offset_inc;
  ++(ref_count.arr[offset_inc]);

  res = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &vtt_in_pp_port_config_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  rv = sw_state_access[unit].dpp.soc.arad.pp.llp_filter.ref_count.set(unit, &ref_count);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 52, exit);

  *success = SOC_SAND_SUCCESS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_designated_vlan_set_unsafe()", local_port_ndx, 0);
}

uint32
  arad_pp_llp_filter_designated_vlan_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid,
    SOC_SAND_IN  uint8                                     accept
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_designated_vlan_set_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_llp_filter_designated_vlan_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_designated_vlan_get_verify()", local_port_ndx, 0);
}


uint32
  arad_pp_llp_filter_designated_vlan_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_SAND_PP_VLAN_ID                           *vid,
    SOC_SAND_OUT uint8                                     *accept
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_in_pp_port_config_tbl_data;
  ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA
    designated_vlan_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vid);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  res = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &vtt_in_pp_port_config_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  if (vtt_in_pp_port_config_tbl_data.designated_vlan_index ==
      ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL)
  {
    *vid = 0;
    *accept = FALSE;
  }
  else
  {
    res = arad_pp_ihp_designated_vlan_table_tbl_get_unsafe(
      unit,
      vtt_in_pp_port_config_tbl_data.designated_vlan_index,
      &designated_vlan_table_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

    *vid = designated_vlan_table_tbl_data.designated_vlan;
    *accept = (vtt_in_pp_port_config_tbl_data.designated_vlan_index == 0 ? FALSE : TRUE);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_filter_designated_vlan_get_unsafe()", local_port_ndx, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_filter_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_llp_filter;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_filter_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_llp_filter;
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


