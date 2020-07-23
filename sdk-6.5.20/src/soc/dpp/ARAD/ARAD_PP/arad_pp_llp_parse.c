/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LLP



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_parse.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_bmact.h>
#include <soc/dpp/drv.h>





#define ARAD_PP_LLP_PARSE_TPID_PROFILE_NDX_MAX                   (7)
#define ARAD_PP_LLP_PARSE_PORT_PROFILE_NDX_MAX                   (7)
#define ARAD_PP_LLP_PARSE_TPID_PROFILE_ID_MAX                    (3)
#define ARAD_PP_LLP_PARSE_INDEX_MAX                              (3)
#define ARAD_PP_LLP_PARSE_TYPE_MAX                               (SOC_SAND_PP_NOF_VLAN_TAG_TYPES-1)
#define ARAD_PP_LLP_PARSE_OUTER_TPID_MAX                         (SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS-1)
#define ARAD_PP_LLP_PARSE_INNER_TPID_MAX                         (SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS-1)
#define ARAD_PP_LLP_PARSE_TAG_FORMAT_MAX                         (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)

#define ARAD_PP_LLP_PARSE_PRIORITY_TAG_TYPE_MAX                  (SOC_SAND_PP_NOF_VLAN_TAG_TYPES-1)
#define ARAD_PP_LLP_PARSE_DLFT_EDIT_COMMAND_ID_MAX               (63)
#define ARAD_PP_LLP_PARSE_DFLT_EDIT_PCP_PROFILE_MAX              (1)

#define ARAD_PP_LLP_PARSE_MIM_BTAG_TPID                         (0x81a8)

#define ARAD_PP_LLP_PARSE_ADVANCE_MODE(__unit)  (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED((__unit)))
#define ARAD_PP_LLP_PARSE_IS_TAGGED(__tag_format)  ((__tag_format)!=0)













CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_llp_parse[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_VALUES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_VALUES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_VALUES_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_VALUES_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_VALUES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_VALUES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_VALUES_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_VALUES_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_PARSE_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_PARSE_GET_ERRS_PTR),
  



  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_llp_parse[] =
{
  
  {
    ARAD_PP_LLP_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port_profile_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_TPID_PROFILE_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_TPID_PROFILE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile_id' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_VLAN_TAG_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_OUTER_TPID_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_OUTER_TPID_OUT_OF_RANGE_ERR",
    "The parameter 'outer_tpid' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_INNER_TPID_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_INNER_TPID_OUT_OF_RANGE_ERR",
    "The parameter 'inner_tpid' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_TAG_FORMAT_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_TAG_FORMAT_OUT_OF_RANGE_ERR",
    "The parameter 'tag_format' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_PRIORITY_TAG_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_PRIORITY_TAG_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'priority_tag_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_VLAN_TAG_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_DLFT_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_DLFT_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR",
    "The parameter 'dlft_edit_command_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_DFLT_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_DFLT_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'dflt_edit_pcp_profile' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  {
    ARAD_PP_LLP_PARSE_TYPE_ILLEGAL_ERR,
    "ARAD_PP_LLP_PARSE_TYPE_ILLEGAL_ERR",
    "User should not specify VLAN_TAG type for TPID profile. \n\r "
    "SOC_PPC_LLP_PARSE_TPID_INFO.type must be SOC_SAND_PP_VLAN_TAG_TYPE_ANY.\n\r "
    "this is the default value set by clear function.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_TRAP_CODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code' is out of range. \n\r "
    "The range is: SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP - .\n\r "
    "SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_PRIO_TAG_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_PARSE_PRIO_TAG_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'priority_tag_type' is out of range. \n\r "
    "The range is: SOC_SAND_PP_VLAN_TAG_TYPE_CTAG or  .\n\r "
    "SOC_SAND_PP_VLAN_TAG_TYPE_STAG.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_PARSE_PRIORITY_TYPE_ILLEGAL_ERR,
    "ARAD_PP_LLP_PARSE_PRIORITY_TYPE_ILLEGAL_ERR",
    "The parameter 'priority_tag_type' is illegal. \n\r "
    "The range is: when is_outer_prio is FALSE \n\r "
    "then has to be SOC_SAND_PP_VLAN_TAG_TYPE_NONE.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};







uint32
  arad_pp_llp_parse_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  ARAD_PP_IHP_LLR_LLVP_TBL_DATA
    ihp_llr_llvp_tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);



  res = arad_pp_ihp_llr_llvp_tbl_get_unsafe(
          unit,
          0,
          &ihp_llr_llvp_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
  ihp_llr_llvp_tbl_data.acceptable_frame_type_action = 0x1;
  
  ihp_llr_llvp_tbl_data.outer_comp_index = ARAD_PP_LLP_PARSE_VLAN_COMPRESSION_OUTER_COMP_NDX;
  ihp_llr_llvp_tbl_data.inner_comp_index = ARAD_PP_LLP_PARSE_VLAN_COMPRESSION_INNER_COMP_NDX;


  res = arad_pp_ihp_llr_llvp_tbl_fill_unsafe(
          unit,
          0,
          &ihp_llr_llvp_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_init_unsafe()", 0, 0);
}






STATIC uint8
  arad_pp_llp_parse_is_outer_s_tag(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO     *packet_format
  )
{
  
  if (
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG ||
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG ||
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG
     )
  {
    return TRUE;
  }
  
  if (
      (packet_format->priority_tag_type == SOC_SAND_PP_VLAN_TAG_TYPE_STAG) &&
       (packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG ||
        packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG ||
        packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_S_TAG)
     )
  {
    return TRUE;
  }
  return FALSE;
}


STATIC uint8
  arad_pp_llp_parse_is_outer_c_tag(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO     *packet_format
  )
{
  
  if (
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG ||
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_S_TAG ||
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG
     )
  {
    return TRUE;
  }
  
  if (
      (packet_format->priority_tag_type == SOC_SAND_PP_VLAN_TAG_TYPE_CTAG) &&
       (packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG ||
        packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG ||
        packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_S_TAG)
     )
  {
    return TRUE;
  }
  return FALSE;
}


STATIC uint8
  arad_pp_llp_parse_is_inner_c_tag(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO     *packet_format
  )
{
  
  if (
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG ||
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG ||
      packet_format->tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG
     )
  {
    return TRUE;
  }
  return FALSE;
}


uint32
  arad_pp_llp_parse_tpid_values_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_VALUES               *tpid_vals
  )
{ 
  uint32
    res;
  uint64 reg_64, field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_PARSE_TPID_VALUES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tpid_vals);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_2r, REG_PORT_ANY, 0, TPID_0f,  tpid_vals->tpid_vals[0]));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_3r, REG_PORT_ANY, 0, TPID_2f,  tpid_vals->tpid_vals[2]));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_2r, REG_PORT_ANY, 0, TPID_1f,  tpid_vals->tpid_vals[1]));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_3r, REG_PORT_ANY, 0, TPID_3f,  tpid_vals->tpid_vals[3]));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_5r, REG_PORT_ANY, 0, ITAG_TPIDf,  tpid_vals->tpid_vals[4]));

  
  COMPILER_64_ZERO(reg_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_OAMP_CCM_TPID_MAPr(unit, &reg_64));
  COMPILER_64_SET(field64, 0, tpid_vals->tpid_vals[0]);
  soc_reg64_field_set(unit, OAMP_CCM_TPID_MAPr, &reg_64, CCM_TPID_0f, field64);
  COMPILER_64_SET(field64, 0, tpid_vals->tpid_vals[1]);
  soc_reg64_field_set(unit, OAMP_CCM_TPID_MAPr, &reg_64, CCM_TPID_1f, field64);
  COMPILER_64_SET(field64, 0, tpid_vals->tpid_vals[2]);
  soc_reg64_field_set(unit, OAMP_CCM_TPID_MAPr, &reg_64, CCM_TPID_2f, field64);
  COMPILER_64_SET(field64, 0, tpid_vals->tpid_vals[3]);
  soc_reg64_field_set(unit, OAMP_CCM_TPID_MAPr, &reg_64, CCM_TPID_3f, field64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_OAMP_CCM_TPID_MAPr(unit, reg_64));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_tpid_values_set_unsafe()", 0, 0);
}

uint32
  arad_pp_llp_parse_tpid_values_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_VALUES               *tpid_vals
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_PARSE_TPID_VALUES_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_PARSE_TPID_VALUES, tpid_vals, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_tpid_values_set_verify()", 0, 0);
}

uint32
  arad_pp_llp_parse_tpid_values_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_PARSE_TPID_VALUES_GET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_tpid_values_get_verify()", 0, 0);
}


uint32
  arad_pp_llp_parse_tpid_values_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_TPID_VALUES               *tpid_vals
  )
{ 
  uint32
    res,
    tmp;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_PARSE_TPID_VALUES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tpid_vals);

  SOC_PPC_LLP_PARSE_TPID_VALUES_clear(tpid_vals);
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_PP_2r, REG_PORT_ANY, 0, TPID_0f, &tmp));
  tpid_vals->tpid_vals[0] = (SOC_SAND_PP_TPID)tmp;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_PP_2r, REG_PORT_ANY, 0, TPID_1f, &tmp));
  tpid_vals->tpid_vals[1] = (SOC_SAND_PP_TPID)tmp;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_PP_3r, REG_PORT_ANY, 0, TPID_2f, &tmp));
  tpid_vals->tpid_vals[2] = (SOC_SAND_PP_TPID)tmp;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_PP_3r, REG_PORT_ANY, 0, TPID_3f, &tmp));
  tpid_vals->tpid_vals[3] = (SOC_SAND_PP_TPID)tmp;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_PP_5r, REG_PORT_ANY, 0, ITAG_TPIDf, &tmp));
  tpid_vals->tpid_vals[4] = (SOC_SAND_PP_TPID)tmp;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_tpid_values_get_unsafe()", 0, 0);
}


uint32
  arad_pp_llp_parse_tpid_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  )
{ 
  uint32
    tmp,
    lsb,
    msb,
    reg_val,
    mim_tpid;
  uint32
    res = SOC_SAND_OK;
  uint8
    mac_in_mac_enabled;
  SOC_PPC_LLP_PARSE_TPID_VALUES               
    tpid_vals;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tpid_profile_info);
  
  
  if (tpid_profile_ndx <= ARAD_PP_LLP_PARSE_TPID_PROFILE_ID_MAX)
  {
    lsb = tpid_profile_ndx*2;
    msb = tpid_profile_ndx*2 + 1;
    
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_PP_4r, REG_PORT_ANY, 0, TPID_PROFILE_OUTER_INDEXf, &tmp));
    res = soc_sand_set_field(&tmp, msb, lsb, tpid_profile_info->tpid1.index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_4r, REG_PORT_ANY, 0, TPID_PROFILE_OUTER_INDEXf,  tmp));

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_PP_4r, REG_PORT_ANY, 0, TPID_PROFILE_INNER_INDEXf, &tmp));
    res = soc_sand_set_field(&tmp, msb, lsb, tpid_profile_info->tpid2.index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_4r, REG_PORT_ANY, 0, TPID_PROFILE_INNER_INDEXf,  tmp));

    res = arad_pp_is_mac_in_mac_enabled(
      unit,
      &mac_in_mac_enabled
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = READ_EPNI_TPID_PROFILESr(unit, REG_PORT_ANY, &reg_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    mim_tpid = soc_reg_field_get(unit, EPNI_TPID_PROFILESr, reg_val, TPID_PROFILE_MAC_IN_MACf);

    if (mac_in_mac_enabled && (mim_tpid == 0)) {
        
        res = arad_pp_llp_parse_tpid_values_get_unsafe(
            unit, 
            &tpid_vals
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

        if (tpid_vals.tpid_vals[tpid_profile_info->tpid1.index] == ARAD_PP_LLP_PARSE_MIM_BTAG_TPID) { 
            
            soc_reg_field_set(unit, EPNI_TPID_PROFILESr, &reg_val, TPID_PROFILE_MAC_IN_MACf, tpid_profile_ndx);

            res = WRITE_EPNI_TPID_PROFILESr(unit, REG_PORT_ANY, reg_val);
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
        }
    }
  }
  

    
    
    
    
    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_tpid_profile_info_set_unsafe()", tpid_profile_ndx, 0);
}

uint32
  arad_pp_llp_parse_tpid_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tpid_profile_ndx, ARAD_PP_LLP_PARSE_TPID_PROFILE_NDX_MAX, ARAD_PP_LLP_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO, tpid_profile_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_tpid_profile_info_set_verify()", tpid_profile_ndx, 0);
}

uint32
  arad_pp_llp_parse_tpid_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tpid_profile_ndx, ARAD_PP_LLP_PARSE_TPID_PROFILE_NDX_MAX, ARAD_PP_LLP_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_tpid_profile_info_get_verify()", tpid_profile_ndx, 0);
}


uint32
  arad_pp_llp_parse_tpid_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  )
{
  uint32
    res = SOC_SAND_OK,
    tmp,
    lsb,
    msb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tpid_profile_info);

  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_clear(tpid_profile_info);
  
  
  if (tpid_profile_ndx <= ARAD_PP_LLP_PARSE_TPID_PROFILE_ID_MAX)
  {
    lsb = tpid_profile_ndx*2;
    msb = tpid_profile_ndx*2 + 1;
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_PP_4r, REG_PORT_ANY, 0, TPID_PROFILE_OUTER_INDEXf, &tmp));
    tpid_profile_info->tpid1.index = (uint8)SOC_SAND_GET_BITS_RANGE(tmp, msb, lsb);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_PP_4r, REG_PORT_ANY, 0, TPID_PROFILE_INNER_INDEXf, &tmp));
    tpid_profile_info->tpid2.index = (uint8)SOC_SAND_GET_BITS_RANGE(tmp, msb, lsb);
  }
  

    
    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_tpid_profile_info_get_unsafe()", tpid_profile_ndx, 0);
}


uint32
  arad_pp_llp_parse_port_profile_to_tpid_profile_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint32                                  tpid_profile_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_port_profile_to_tpid_profile_map_set_unsafe()", port_profile_ndx, 0);
}

uint32
  arad_pp_llp_parse_port_profile_to_tpid_profile_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint32                                  tpid_profile_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, ARAD_PP_LLP_PARSE_PORT_PROFILE_NDX_MAX, ARAD_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tpid_profile_id, ARAD_PP_LLP_PARSE_TPID_PROFILE_ID_MAX, ARAD_PP_LLP_PARSE_TPID_PROFILE_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_port_profile_to_tpid_profile_map_set_verify()", port_profile_ndx, 0);
}

uint32
  arad_pp_llp_parse_port_profile_to_tpid_profile_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, ARAD_PP_LLP_PARSE_PORT_PROFILE_NDX_MAX, ARAD_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_port_profile_to_tpid_profile_map_get_verify()", port_profile_ndx, 0);
}


uint32
  arad_pp_llp_parse_port_profile_to_tpid_profile_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_OUT uint32                                  *tpid_profile_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tpid_profile_id);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_port_profile_to_tpid_profile_map_get_unsafe()", port_profile_ndx, 0);
}


uint32
  arad_pp_llp_parse_packet_format_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  )
{
  ARAD_PP_IHP_LLR_LLVP_TBL_DATA
    llr_llvp_tbl;
  ARAD_PP_IHP_VTT_LLVP_TBL_DATA
    vtt_llvp_tbl;
  uint32
    entry_offset, parser_leaf_context;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(parse_key);
  SOC_SAND_CHECK_NULL_INPUT(format_info);

  
  entry_offset = ARAD_PP_TBL_IHP_LLVP_KEY_ENTRY_OFFSET(
                   port_profile_ndx,
                   parse_key->outer_tpid,
                   parse_key->is_outer_prio,
                   parse_key->inner_tpid
                 );

  res = arad_pp_ihp_llr_llvp_tbl_get_unsafe(
          unit,
          entry_offset,
          &llr_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  llr_llvp_tbl.acceptable_frame_type_action = (format_info->action_trap_code - SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP);

  
  llr_llvp_tbl.incoming_tag_exist = (format_info->tag_format != SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE);

  
  llr_llvp_tbl.incoming_vid_exist = (llr_llvp_tbl.incoming_tag_exist && !parse_key->is_outer_prio);
  
  if (!ARAD_PP_LLP_PARSE_ADVANCE_MODE(unit)) {
      llr_llvp_tbl.incoming_s_tag_exist = (llr_llvp_tbl.incoming_tag_exist && arad_pp_llp_parse_is_outer_s_tag(format_info));
  }
  else{
      
      llr_llvp_tbl.incoming_s_tag_exist = ARAD_PP_LLP_PARSE_IS_TAGGED(format_info->tag_format)  && 
          ((format_info->flags & SOC_PPC_LLP_PARSE_FLAGS_OUTER_C)==0);
  }

  
  if (format_info->initial_c_tag && llr_llvp_tbl.incoming_s_tag_exist) 
  {
    llr_llvp_tbl.incoming_vid_exist = 0;
  }

  res = arad_pp_ihp_llr_llvp_tbl_set_unsafe(
          unit,
          entry_offset,
          &llr_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  for (parser_leaf_context = 0; parser_leaf_context < ARAD_PARSER_PLC_NOF; parser_leaf_context++)
  {
    res = arad_pp_ihp_vtt_llvp_tbl_get_unsafe(
          unit,
          ARAD_PP_TBL_IHP_VTT_LLVP_ENTRY_OFFSET(entry_offset,parser_leaf_context),
          &vtt_llvp_tbl
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    vtt_llvp_tbl.incoming_tag_structure = format_info->tag_format;
    vtt_llvp_tbl.ivec = format_info->dlft_edit_command_id;
    vtt_llvp_tbl.pcp_dei_profile = format_info->dflt_edit_pcp_profile;

    
      res = arad_pp_ihp_vtt_llvp_tbl_set_unsafe(
              unit,
              ARAD_PP_TBL_IHP_VTT_LLVP_ENTRY_OFFSET(entry_offset,parser_leaf_context),
              &vtt_llvp_tbl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_packet_format_info_set_unsafe()", port_profile_ndx, 0);
}

soc_error_t
  arad_pp_llp_parse_packet_format_eg_info_set(
   SOC_SAND_IN   int                                 unit,
   SOC_SAND_IN   uint32                                  port_profile_ndx,
   SOC_SAND_IN   SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_INOUT  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  )
{
  ARAD_PP_EPNI_LLVP_TABLE_TBL_DATA
    epni_llvp_tbl;
  uint8
    is_outer_ctag,
    is_inner_ctag,
    is_outer_stag;
  uint32
    entry_offset;
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(parse_key);
  SOC_SAND_CHECK_NULL_INPUT(format_info);
  
  
  entry_offset = ARAD_TBL_EPNI_LLVP_KEY_ENTRY_OFFSET(
      port_profile_ndx,
      parse_key->outer_tpid,
      parse_key->is_outer_prio,
      parse_key->inner_tpid
      );
 
  if (!ARAD_PP_LLP_PARSE_ADVANCE_MODE(unit)) {
     is_outer_ctag = arad_pp_llp_parse_is_outer_c_tag(format_info);
     is_inner_ctag = arad_pp_llp_parse_is_inner_c_tag(format_info);
     is_outer_stag = arad_pp_llp_parse_is_outer_s_tag(format_info);
  }
  else{
     
     is_outer_ctag = (format_info->flags & SOC_PPC_LLP_PARSE_FLAGS_OUTER_C)?1:0;
     is_inner_ctag = (format_info->flags & SOC_PPC_LLP_PARSE_FLAGS_INNER_C)?1:0;
     
     is_outer_stag = ARAD_PP_LLP_PARSE_IS_TAGGED(format_info->tag_format)  && ((format_info->flags & SOC_PPC_LLP_PARSE_FLAGS_OUTER_C)==0);
  }

  
  ARAD_CLEAR(&epni_llvp_tbl, ARAD_PP_EPNI_LLVP_TABLE_TBL_DATA, 1);
  epni_llvp_tbl.incoming_tag_format = format_info->tag_format;

  if (is_outer_ctag)
  {
     epni_llvp_tbl.c_tag_offset = 0;
     
     epni_llvp_tbl.packet_has_c_tag = TRUE;
     epni_llvp_tbl.packet_has_up = TRUE;
  }
  else if (is_inner_ctag)
  {
    epni_llvp_tbl.c_tag_offset = 1;
    
    epni_llvp_tbl.packet_has_c_tag = TRUE;
  }

  
  if (is_outer_stag)
  {
    epni_llvp_tbl.packet_has_pcp_dei = TRUE;
  }

  res = arad_pp_epni_llvp_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &epni_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_packet_format_eg_info_set()", port_profile_ndx, 0);
}

uint32
  arad_pp_llp_parse_packet_format_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, ARAD_PP_LLP_PARSE_PORT_PROFILE_NDX_MAX, ARAD_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_PARSE_INFO, parse_key, 20, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO, format_info, 30, exit);
  
  
  if(!ARAD_PP_LLP_PARSE_ADVANCE_MODE(unit)){
      if (parse_key->is_outer_prio)
      {
        
        SOC_SAND_ERR_IF_OUT_OF_RANGE(
          format_info->priority_tag_type,
          SOC_SAND_PP_VLAN_TAG_TYPE_CTAG,
          SOC_SAND_PP_VLAN_TAG_TYPE_STAG,
          ARAD_PP_LLP_PARSE_PRIO_TAG_TYPE_OUT_OF_RANGE_ERR,
          40,
          exit
        );
      }
      else
      {
        
        if (format_info->priority_tag_type != SOC_SAND_PP_VLAN_TAG_TYPE_NONE)
        {
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_LLP_PARSE_PRIORITY_TYPE_ILLEGAL_ERR, 50, exit);
        }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_packet_format_info_set_verify()", port_profile_ndx, 0);
}

uint32
  arad_pp_llp_parse_packet_format_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, ARAD_PP_LLP_PARSE_PORT_PROFILE_NDX_MAX, ARAD_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_PARSE_INFO, parse_key, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_packet_format_info_get_verify()", port_profile_ndx, 0);
}


uint32
  arad_pp_llp_parse_packet_format_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  )
{
  ARAD_PP_IHP_LLR_LLVP_TBL_DATA
    llr_llvp_tbl;
  ARAD_PP_IHP_VTT_LLVP_TBL_DATA
    vtt_llvp_tbl;
  uint32
    entry_offset;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(parse_key);
  SOC_SAND_CHECK_NULL_INPUT(format_info);

  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_clear(format_info);

  
  entry_offset = ARAD_PP_TBL_IHP_LLVP_KEY_ENTRY_OFFSET(
                   port_profile_ndx,
                   parse_key->outer_tpid,
                   parse_key->is_outer_prio,
                   parse_key->inner_tpid
                 );

  res = arad_pp_ihp_llr_llvp_tbl_get_unsafe(
          unit,
          entry_offset,
          &llr_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  format_info->action_trap_code = (SOC_PPC_TRAP_CODE)(llr_llvp_tbl.acceptable_frame_type_action + SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP);

  
  res = arad_pp_ihp_vtt_llvp_tbl_get_unsafe(
          unit,
          ARAD_PP_TBL_IHP_VTT_LLVP_ENTRY_OFFSET(entry_offset,ARAD_PARSER_PLC_MAX),
          &vtt_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  format_info->tag_format = vtt_llvp_tbl.incoming_tag_structure;
  format_info->dlft_edit_command_id = vtt_llvp_tbl.ivec;
  format_info->dflt_edit_pcp_profile = vtt_llvp_tbl.pcp_dei_profile;

  format_info->priority_tag_type = SOC_SAND_PP_VLAN_TAG_TYPE_CTAG;

  
  if (!parse_key->is_outer_prio)
  {
    format_info->priority_tag_type = SOC_SAND_PP_VLAN_TAG_TYPE_NONE;
  }
  else
  {
    
    if (arad_pp_llp_parse_is_outer_s_tag(format_info))
    {
      format_info->priority_tag_type = SOC_SAND_PP_VLAN_TAG_TYPE_STAG;
    }
  }

  
  if ((!llr_llvp_tbl.incoming_tag_exist) && llr_llvp_tbl.incoming_s_tag_exist) 
  {
    format_info->initial_c_tag = TRUE;
  }
  
  
  if (ARAD_PP_LLP_PARSE_IS_TAGGED(format_info->tag_format)) {
      if (!llr_llvp_tbl.incoming_s_tag_exist) {
           format_info->flags |= SOC_PPC_LLP_PARSE_FLAGS_OUTER_C;
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_packet_format_info_get_unsafe()", port_profile_ndx, 0);
}


soc_error_t
  arad_pp_llp_parse_packet_format_eg_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  )
{
  ARAD_PP_EPNI_LLVP_TABLE_TBL_DATA
    epni_llvp_tbl;
  uint32
    entry_offset;
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(parse_key);
  SOC_SAND_CHECK_NULL_INPUT(format_info);
  
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_clear(format_info);
  
  
  entry_offset = ARAD_TBL_EPNI_LLVP_KEY_ENTRY_OFFSET(
                   port_profile_ndx,
                   parse_key->outer_tpid,
                   parse_key->is_outer_prio,
                   parse_key->inner_tpid
                 );

  
  format_info->flags = 0;
  if (ARAD_PP_LLP_PARSE_ADVANCE_MODE(unit)) {
    res = arad_pp_epni_llvp_table_tbl_get_unsafe(
               unit,
               entry_offset,
               &epni_llvp_tbl
               );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    
    
    if (epni_llvp_tbl.packet_has_c_tag == TRUE) {
       format_info->flags |= ((epni_llvp_tbl.c_tag_offset == 0) ?
       SOC_PPC_LLP_PARSE_FLAGS_OUTER_C : SOC_PPC_LLP_PARSE_FLAGS_INNER_C);
    }
    format_info->tag_format= epni_llvp_tbl.incoming_tag_format;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_parse_packet_format_eg_info_get()", port_profile_ndx, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_parse_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_llp_parse;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_parse_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_llp_parse;
}
uint32
  SOC_PPC_LLP_PARSE_TPID_VALUES_verify(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_VALUES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_PARSE_TPID_VALUES_verify()",0,0);
}

uint32
  SOC_PPC_LLP_PARSE_TPID_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->index, ARAD_PP_LLP_PARSE_INDEX_MAX, ARAD_PP_LLP_PARSE_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, ARAD_PP_LLP_PARSE_TYPE_MAX, ARAD_PP_LLP_PARSE_TYPE_OUT_OF_RANGE_ERR, 11, exit);
  if (info->type != SOC_SAND_PP_VLAN_TAG_TYPE_ANY)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LLP_PARSE_TYPE_ILLEGAL_ERR, 12, exit);
  }
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_PARSE_TPID_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_PARSE_TPID_INFO, &(info->tpid1), 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_PARSE_TPID_INFO, &(info->tpid2), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_PARSE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->outer_tpid, ARAD_PP_LLP_PARSE_OUTER_TPID_MAX, ARAD_PP_LLP_PARSE_OUTER_TPID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->inner_tpid, ARAD_PP_LLP_PARSE_INNER_TPID_MAX, ARAD_PP_LLP_PARSE_INNER_TPID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_PARSE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tag_format, ARAD_PP_LLP_PARSE_TAG_FORMAT_MAX, ARAD_PP_LLP_PARSE_TAG_FORMAT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->priority_tag_type, ARAD_PP_LLP_PARSE_PRIORITY_TAG_TYPE_MAX, ARAD_PP_LLP_PARSE_PRIORITY_TAG_TYPE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dlft_edit_command_id, ARAD_PP_LLP_PARSE_DLFT_EDIT_COMMAND_ID_MAX, ARAD_PP_LLP_PARSE_DLFT_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dflt_edit_pcp_profile, ARAD_PP_LLP_PARSE_DFLT_EDIT_PCP_PROFILE_MAX, ARAD_PP_LLP_PARSE_DFLT_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR, 14, exit);
  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    info->action_trap_code,
    SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP,
    SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2,
    ARAD_PP_LLP_PARSE_TRAP_CODE_OUT_OF_RANGE_ERR,
    20,
    exit
  );

  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    info->priority_tag_type,
    SOC_SAND_PP_VLAN_TAG_TYPE_NONE,
    SOC_SAND_PP_VLAN_TAG_TYPE_STAG,
    ARAD_PP_LLP_PARSE_PRIO_TAG_TYPE_OUT_OF_RANGE_ERR,
    30,
    exit
  );

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

