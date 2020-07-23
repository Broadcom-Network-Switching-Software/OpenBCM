
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_LIF



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_ing_vlan_edit.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_api_framework.h>




#define ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_MAX                (63)
#define ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX            (15)
#define ARAD_PP_LIF_ING_VLAN_EDIT_VID_SOURCE_MAX                 (SOC_PPC_NOF_LIF_ING_VLAN_EDIT_TAG_VID_SRCS_ARAD-1)
#define ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SOURCE_MAX             (SOC_PPC_NOF_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRCS-1)
#define ARAD_PP_LIF_ING_VLAN_EDIT_TAG_FORMAT_MAX                 (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)
#define ARAD_PP_LIF_ING_VLAN_EDIT_EDIT_PROFILE_MAX               (7)
#define ARAD_PP_LIF_ING_VLAN_EDIT_TAGS_TO_REMOVE_MAX             (2)
#define ARAD_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_MAX               (3)
#define ARAD_PP_LIF_ING_PCP_DEI_MAX				(15)
#define ARAD_PP_DP_PROFILE_MAX      				(3)
#define ARAD_PP_DP_PROFILE_MASK						(3)





#define ARAD_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_NONE 0
#define ARAD_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_EMPTY 0
#define ARAD_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_NEW 2
#define ARAD_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_AC_EDIT_INFO 2
#define ARAD_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_AC_EDIT_INFO_2 3
#define ARAD_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_OUTER_TAG 4
#define ARAD_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_INNER_TAG 5
        

#define ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, src_suffix) \
  case SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_##src_suffix: \
    fld_val = ARAD_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_##src_suffix; \
    break;

#define ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD(src, fld_val) \
  switch (src) \
  { \
    ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, NONE) \
    ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, OUTER_TAG) \
    ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, INNER_TAG) \
    ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, NEW) \
    default: \
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR, 555, exit); \
  }

#define ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, src_suffix) \
  case ARAD_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_##src_suffix: \
    src = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_##src_suffix; \
    break;

#define ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC(fld_val, src) \
  switch (fld_val) \
  { \
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, NONE) \
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, OUTER_TAG) \
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, INNER_TAG) \
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, NEW) \
  default: \
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR, 555, exit); \
  }





#define ARAD_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, src_suffix) \
  case SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_##src_suffix: \
    fld_val = ARAD_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_##src_suffix; \
    break;

#define ARAD_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD(src, fld_val) \
  switch (src) \
  { \
  ARAD_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, EMPTY) \
  ARAD_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, OUTER_TAG) \
  ARAD_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, INNER_TAG) \
  ARAD_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, AC_EDIT_INFO) \
  ARAD_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, AC_EDIT_INFO_2) \
  default: \
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR, 555, exit); \
  }

#define ARAD_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, src_suffix) \
  case ARAD_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_##src_suffix: \
    src = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_##src_suffix; \
    break;

#define ARAD_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC(fld_val, src) \
  switch (fld_val) \
  { \
  ARAD_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, EMPTY) \
  ARAD_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, OUTER_TAG) \
  ARAD_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, INNER_TAG) \
  ARAD_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, AC_EDIT_INFO) \
  ARAD_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, AC_EDIT_INFO_2) \
  default: \
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR, 555, exit); \
  }





#define ARAD_PP_EPNI_IN_LIF_PROFILE_MAP_OUTER_DP_OFFET(profile_ndx) \
	((profile_ndx*4) + 2)

#define ARAD_PP_EPNI_IN_LIF_PROFILE_MAP_INNER_DP_OFFET(profile_ndx) \
	(profile_ndx*4)
	









CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_lif_ing_vlan_edit[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_GET_ERRS_PTR),
  

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_TBL_GET),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_lif_ing_vlan_edit[] =
{
  
  {
    ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR",
    "The parameter 'command_id' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'command_ndx' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'pcp_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ING_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ING_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR",
    "The parameter 'vid_source' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_LIF_ING_VLAN_EDIT_TAG_VID_SRCS_ARAD-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR",
    "The parameter 'pcp_dei_source' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRCS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ING_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ING_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR",
    "The parameter 'tag_format' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ING_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ING_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'edit_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ING_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ING_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR",
    "The parameter 'tags_to_remove' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  {
    ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR",
    "pcp-dei/vid source is out of range. \n\r "
    "The range is: 0 to 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ING_VLAN_EDIT_INCONSISTANT_TBLS_ERR,
    "ARAD_PP_LIF_ING_VLAN_EDIT_INCONSISTANT_TBLS_ERR",
    "Relevant data should be the same in two tables. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





uint32
  arad_pp_lif_ing_vlan_edit_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_init_unsafe()", 0, 0);
}

static uint8 conn_to_np_enable;
static int conn_to_np_enable_read_soc = 0;


uint32
  arad_pp_lif_ing_vlan_edit_command_id_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY           *command_key,
    SOC_SAND_IN  uint32                                      command_id
  )
{
  uint32
    res = SOC_SAND_OK,
    val,
    entry_offset;
  ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA
    ingress_vlan_edit_command_table_tbl_data;
  soc_reg_above_64_val_t fhei_ive_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(command_key);

  if (conn_to_np_enable_read_soc == 0) {
        conn_to_np_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "conn_to_np_enable", 0);
        conn_to_np_enable_read_soc = 1;
  }

  
  if ((conn_to_np_enable == 1) && (command_id >= 0x30)) {
        val = 1;
        SOC_REG_ABOVE_64_CLEAR(fhei_ive_size);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, READ_IHB_PPH_FHEI_IVE_SIZEr(unit, REG_PORT_ANY, fhei_ive_size));
        SHR_BITCOPY_RANGE(fhei_ive_size, command_id * 2, &val, 0, 2);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 6, exit, WRITE_IHB_PPH_FHEI_IVE_SIZEr(unit, REG_PORT_ANY, fhei_ive_size));
  }

  entry_offset =
    ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_KEY_ENTRY_OFFSET(
      FALSE,
      command_key->tag_format,
      command_key->edit_profile);

  ingress_vlan_edit_command_table_tbl_data.ivec = command_id;

  res = arad_pp_ihp_ingress_vlan_edit_command_table_tbl_set_unsafe(
    unit,
    entry_offset,
    &ingress_vlan_edit_command_table_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_command_id_set_unsafe()", 0, 0);
}

uint32
  arad_pp_lif_ing_vlan_edit_command_id_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY           *command_key,
    SOC_SAND_IN  uint32                                      command_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY, command_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(command_id, ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_command_id_set_verify()", 0, 0);
}

uint32
  arad_pp_lif_ing_vlan_edit_command_id_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY           *command_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY, command_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_command_id_get_verify()", 0, 0);
}


uint32
  arad_pp_lif_ing_vlan_edit_command_id_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY           *command_key,
    SOC_SAND_OUT uint32                                      *command_id
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA
    ingress_vlan_edit_command_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(command_key);
  SOC_SAND_CHECK_NULL_INPUT(command_id);

  entry_offset =
    ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_KEY_ENTRY_OFFSET(
    FALSE,
    command_key->tag_format,
    command_key->edit_profile);

  res = arad_pp_ihp_ingress_vlan_edit_command_table_tbl_get_unsafe(
    unit,
    entry_offset,
    &ingress_vlan_edit_command_table_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *command_id = ingress_vlan_edit_command_table_tbl_data.ivec;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_command_id_get_unsafe()", 0, 0);
}


uint32
  arad_pp_lif_ing_vlan_edit_command_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      command_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO          *command_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    epni_tbl_data;
  ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    egq_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(command_info);

  res = arad_pp_egq_ingress_vlan_edit_command_map_tbl_get_unsafe(
          unit,
          command_ndx,
          &egq_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  epni_tbl_data.tpid_profile_link =
    command_info->tpid_profile;
  epni_tbl_data.inner_tpid_ndx =
    command_info->inner_tag.tpid_index;
  epni_tbl_data.outer_tpid_ndx =
    command_info->outer_tag.tpid_index;

  epni_tbl_data.edit_command_bytes_to_remove =
    command_info->tags_to_remove;

  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD(
    command_info->inner_tag.pcp_dei_source,
    epni_tbl_data.edit_command_inner_pcp_dei_source
    );
  
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD(
    command_info->outer_tag.pcp_dei_source,
    epni_tbl_data.edit_command_outer_pcp_dei_source
    );

  ARAD_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD(
    command_info->inner_tag.vid_source, 
    epni_tbl_data.edit_command_inner_vid_source 
    );
  
  ARAD_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD(
    command_info->outer_tag.vid_source,
    epni_tbl_data.edit_command_outer_vid_source
    );

  
  egq_tbl_data.edit_command_bytes_to_remove =
    epni_tbl_data.edit_command_bytes_to_remove;
  egq_tbl_data.edit_command_inner_pcp_dei_source =
    epni_tbl_data.edit_command_inner_pcp_dei_source;
  egq_tbl_data.edit_command_outer_pcp_dei_source =
    epni_tbl_data.edit_command_outer_pcp_dei_source;
  egq_tbl_data.edit_command_inner_vid_source =
    epni_tbl_data.edit_command_inner_vid_source;
  egq_tbl_data.edit_command_outer_vid_source =
    epni_tbl_data.edit_command_outer_vid_source;
  egq_tbl_data.tpid_profile_link =
    epni_tbl_data.tpid_profile_link;
  egq_tbl_data.inner_tpid_ndx=
    epni_tbl_data.inner_tpid_ndx;
  egq_tbl_data.outer_tpid_ndx =
    epni_tbl_data.outer_tpid_ndx;

  res = arad_pp_epni_ingress_vlan_edit_command_map_tbl_set_unsafe(
          unit,
          command_ndx,
          &epni_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_egq_ingress_vlan_edit_command_map_tbl_set_unsafe(
          unit,
          command_ndx,
          &egq_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_command_info_set_unsafe()", command_ndx, 0);
}

uint32
  arad_pp_lif_ing_vlan_edit_command_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      command_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO          *command_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(command_ndx, ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO, command_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_command_info_set_verify()", command_ndx, 0);
}

uint32
  arad_pp_lif_ing_vlan_edit_command_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      command_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(command_ndx, ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_command_info_get_verify()", command_ndx, 0);
}


uint32
  arad_pp_lif_ing_vlan_edit_command_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      command_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO          *command_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    epni_tbl_data;
  ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    egq_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(command_info);

  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(command_info);

  res = arad_pp_epni_ingress_vlan_edit_command_map_tbl_get_unsafe(
    unit,
    command_ndx,
    &epni_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_egq_ingress_vlan_edit_command_map_tbl_get_unsafe(
    unit,
    command_ndx,
    &egq_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if ((egq_tbl_data.edit_command_bytes_to_remove != epni_tbl_data.edit_command_bytes_to_remove) ||
      (egq_tbl_data.edit_command_inner_pcp_dei_source != epni_tbl_data.edit_command_inner_pcp_dei_source) ||
      (egq_tbl_data.edit_command_outer_pcp_dei_source != epni_tbl_data.edit_command_outer_pcp_dei_source) ||
      (egq_tbl_data.edit_command_inner_vid_source != epni_tbl_data.edit_command_inner_vid_source) ||
      (egq_tbl_data.edit_command_outer_vid_source != epni_tbl_data.edit_command_outer_vid_source) ||
      (egq_tbl_data.tpid_profile_link != epni_tbl_data.tpid_profile_link) ||
      (egq_tbl_data.inner_tpid_ndx != epni_tbl_data.inner_tpid_ndx) ||
      (egq_tbl_data.outer_tpid_ndx != epni_tbl_data.outer_tpid_ndx))
  {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_ING_VLAN_EDIT_INCONSISTANT_TBLS_ERR, 30, exit);
  }

  command_info->tpid_profile =
    epni_tbl_data.tpid_profile_link;
  command_info->inner_tag.tpid_index =
    epni_tbl_data.inner_tpid_ndx;
  command_info->outer_tag.tpid_index =
    epni_tbl_data.outer_tpid_ndx;

  command_info->tags_to_remove =
    (uint8)epni_tbl_data.edit_command_bytes_to_remove;

  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC(
    epni_tbl_data.edit_command_inner_pcp_dei_source,
    command_info->inner_tag.pcp_dei_source
    );

  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC(
    epni_tbl_data.edit_command_outer_pcp_dei_source,
    command_info->outer_tag.pcp_dei_source
    );

  ARAD_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC(
    epni_tbl_data.edit_command_inner_vid_source,
    command_info->inner_tag.vid_source
    );

  ARAD_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC(
    epni_tbl_data.edit_command_outer_vid_source,
    command_info->outer_tag.vid_source
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_command_info_get_unsafe()", command_ndx, 0);
}


STATIC uint32
  arad_pp_lif_ing_vlan_edit_pcp_dei_tbl_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      ihp_entry_offset,
    SOC_SAND_IN  uint8                                       out_pcp,
    SOC_SAND_IN  uint8                                     out_dei
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA
    ihp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_TBL_SET);

  ihp_tbl_data.pcp_dei = ARAD_PP_PCP_DEI_TO_FLD_VAL(out_pcp, out_dei);

  res = arad_pp_ihp_vlan_edit_pcp_dei_map_tbl_set_unsafe(
    unit,
    ihp_entry_offset,
    &ihp_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_dei_tbl_set_unsafe()", ihp_entry_offset, 0);
}


STATIC uint32
  arad_pp_lif_ing_vlan_edit_pcp_dei_tbl_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      ihp_entry_offset,
    SOC_SAND_OUT uint8                                       *out_pcp,
    SOC_SAND_OUT uint8                                     *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA
    ihp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_TBL_GET);

  res = arad_pp_ihp_vlan_edit_pcp_dei_map_tbl_get_unsafe(
    unit,
    ihp_entry_offset,
    &ihp_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_PP_PCP_DEI_FROM_FLD_VAL(ihp_tbl_data.pcp_dei, *out_pcp, *out_dei);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_dei_tbl_get_unsafe()", ihp_entry_offset, 0);
}


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_stag_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  uint8                                       tag_pcp_ndx,
    SOC_SAND_IN  uint8                                     tag_dei_ndx,
    SOC_SAND_IN  uint8                                       out_pcp,
    SOC_SAND_IN  uint8                                     out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_UNSAFE);

  res = arad_pp_lif_ing_vlan_edit_pcp_dei_tbl_set_unsafe(
    unit,
    ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_STAG(pcp_profile_ndx, tag_pcp_ndx, tag_dei_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_stag_set_unsafe()", pcp_profile_ndx, tag_pcp_ndx);
}

uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_stag_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_pcp_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_dei_ndx, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_stag_set_verify()", pcp_profile_ndx, tag_pcp_ndx);
}



uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_stag_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_pcp_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_dei_ndx, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_stag_get_verify()", pcp_profile_ndx, tag_pcp_ndx);
}


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_stag_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = arad_pp_lif_ing_vlan_edit_pcp_dei_tbl_get_unsafe(
    unit,
    ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_STAG(pcp_profile_ndx, tag_pcp_ndx, tag_dei_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_stag_get_unsafe()", pcp_profile_ndx, tag_pcp_ndx);
}


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_ctag_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_UNSAFE);

  res = arad_pp_lif_ing_vlan_edit_pcp_dei_tbl_set_unsafe(
    unit,
    ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_CTAG(pcp_profile_ndx, tag_up_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_ctag_set_unsafe()", pcp_profile_ndx, tag_up_ndx);
}

uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_ctag_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_up_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_ctag_set_verify()", pcp_profile_ndx, tag_up_ndx);
}

uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_ctag_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_up_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_ctag_get_verify()", pcp_profile_ndx, tag_up_ndx);
}


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_ctag_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = arad_pp_lif_ing_vlan_edit_pcp_dei_tbl_get_unsafe(
    unit,
    ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_CTAG(pcp_profile_ndx, tag_up_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_ctag_get_unsafe()", pcp_profile_ndx, tag_up_ndx);
}


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_untagged_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_UNSAFE);

  res = arad_pp_lif_ing_vlan_edit_pcp_dei_tbl_set_unsafe(
    unit,
    ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_UNTAGGED(pcp_profile_ndx, tc_ndx, dp_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_untagged_set_unsafe()", pcp_profile_ndx, tc_ndx);
}

uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_untagged_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 50, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_untagged_set_verify()", pcp_profile_ndx, tc_ndx);
}

uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_untagged_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_untagged_get_verify()", pcp_profile_ndx, tc_ndx);
}


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_untagged_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = arad_pp_lif_ing_vlan_edit_pcp_dei_tbl_get_unsafe(
    unit,
    ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_UNTAGGED(pcp_profile_ndx, tc_ndx, dp_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_untagged_get_unsafe()", pcp_profile_ndx, tc_ndx);
}



uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_dp_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      dp_profile_ndx,
    SOC_SAND_IN  uint32                           dp_ndx,
    SOC_SAND_IN  uint32                           sel_dei_ndx,
    SOC_SAND_IN  uint8                            out_pcp,
    SOC_SAND_IN  uint8                           out_dei
  )
{
  uint32
  res = SOC_SAND_OK;
  ARAD_PP_EPNI_DP_MAP_TBL_DATA   epni_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(dp_profile_ndx, ARAD_PP_DP_PROFILE_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_DP_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(sel_dei_ndx, ARAD_PP_LIF_ING_PCP_DEI_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 50, exit);

  epni_tbl_data.pcp_dei = ARAD_PP_PCP_DEI_TO_FLD_VAL(out_pcp, out_dei);

  res = arad_pp_epni_dp_map_tbl_set_unsafe(
    unit,
    ARAD_PP_EPNI_VLAN_EDIT_PCP_DEI_MAP_DP_TBL_INDX(dp_profile_ndx, dp_ndx, sel_dei_ndx),
    &epni_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_dp_set_unsafe()", dp_profile_ndx, dp_ndx);
}


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_dp_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      dp_profile_ndx,
    SOC_SAND_IN  uint32                           dp_ndx,
    SOC_SAND_IN  uint32                           sel_dei_ndx,
    SOC_SAND_OUT  uint8                            *out_pcp,
    SOC_SAND_OUT  uint8                           *out_dei
  )
{
  uint32
  res = SOC_SAND_OK;
  ARAD_PP_EPNI_DP_MAP_TBL_DATA   epni_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(dp_profile_ndx, ARAD_PP_DP_PROFILE_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_DP_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(sel_dei_ndx, ARAD_PP_LIF_ING_PCP_DEI_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = arad_pp_epni_dp_map_tbl_get_unsafe(
    unit,
    ARAD_PP_EPNI_VLAN_EDIT_PCP_DEI_MAP_DP_TBL_INDX(dp_profile_ndx, dp_ndx, sel_dei_ndx),
    &epni_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_PP_PCP_DEI_FROM_FLD_VAL(epni_tbl_data.pcp_dei, *out_pcp, *out_dei);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_dp_get_unsafe()", dp_profile_ndx, dp_ndx);
}





uint32
  arad_pp_lif_ing_vlan_edit_inner_global_info_set(
    SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 in_lif_profile,
   SOC_SAND_IN int value
  )
{
    uint32 profile_map;
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_JERICHO_PLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on QAX")));
    }

    SOC_SAND_ERR_IF_ABOVE_MAX(in_lif_profile, ARAD_PP_DP_PROFILE_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_DP_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(value, ARAD_PP_DP_PROFILE_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 20, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,	exit, ARAD_REG_ACCESS_ERR, READ_EPNI_IN_LIF_DP_PROFILE_MAPr(unit,  &profile_map));
    profile_map &=  ~(ARAD_PP_DP_PROFILE_MASK << ARAD_PP_EPNI_IN_LIF_PROFILE_MAP_INNER_DP_OFFET(in_lif_profile));
    profile_map |= (value << ARAD_PP_EPNI_IN_LIF_PROFILE_MAP_INNER_DP_OFFET(in_lif_profile));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,	exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_IN_LIF_DP_PROFILE_MAPr(unit,	profile_map));
  	
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_dp_set_unsafe()", in_lif_profile, value);
}

uint32
  arad_pp_lif_ing_vlan_edit_outer_global_info_set(
    SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 in_lif_profile,
   SOC_SAND_IN int value
  )
{
    uint32 profile_map;
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_JERICHO_PLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on QAX and above")));
    }

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,	exit, ARAD_REG_ACCESS_ERR, READ_EPNI_IN_LIF_DP_PROFILE_MAPr(unit,  &profile_map));
    profile_map &=  ~(ARAD_PP_DP_PROFILE_MASK << ARAD_PP_EPNI_IN_LIF_PROFILE_MAP_OUTER_DP_OFFET(in_lif_profile));
    profile_map |= (value << ARAD_PP_EPNI_IN_LIF_PROFILE_MAP_OUTER_DP_OFFET(in_lif_profile));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,	exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_IN_LIF_DP_PROFILE_MAPr(unit,	profile_map));
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_dp_set_unsafe()", in_lif_profile, value);
}


uint32
  arad_pp_lif_ing_vlan_edit_inner_global_info_get(
    SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 in_lif_profile,
   SOC_SAND_OUT int *value
  )
{
	uint32 res = SOC_SAND_OK;
	uint32 profile_map;

	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

	if (!SOC_IS_JERICHO_PLUS(unit)) {
		SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on QAX and above")));
	}

	SOC_SAND_ERR_IF_ABOVE_MAX(in_lif_profile, ARAD_PP_DP_PROFILE_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_DP_PROFILE_OUT_OF_RANGE_ERR, 10, exit);

	SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR, READ_EPNI_IN_LIF_DP_PROFILE_MAPr(unit,  &profile_map));
	*value = (profile_map >> ARAD_PP_EPNI_IN_LIF_PROFILE_MAP_INNER_DP_OFFET(in_lif_profile) )& ARAD_PP_DP_PROFILE_MASK;


	SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_dp_get_unsafe()", in_lif_profile, 0);
}

uint32
  arad_pp_lif_ing_vlan_edit_outer_global_info_get(
    SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 in_lif_profile,
   SOC_SAND_OUT int *value
  )
{
	uint32 res = SOC_SAND_OK;
	uint32 profile_map;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

	if (!SOC_IS_JERICHO_PLUS(unit)) {
		SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on QAX and above")));
	}

	SOC_SAND_ERR_IF_ABOVE_MAX(in_lif_profile, ARAD_PP_DP_PROFILE_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_DP_PROFILE_OUT_OF_RANGE_ERR, 10, exit);

	SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR, READ_EPNI_IN_LIF_DP_PROFILE_MAPr(unit,  &profile_map));
	*value = (profile_map >> ARAD_PP_EPNI_IN_LIF_PROFILE_MAP_OUTER_DP_OFFET(in_lif_profile) )& ARAD_PP_DP_PROFILE_MASK;


	SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_ing_vlan_edit_pcp_map_dp_get_unsafe()", in_lif_profile, 0);
}



CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_lif_ing_vlan_edit_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_lif_ing_vlan_edit;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_lif_ing_vlan_edit_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_lif_ing_vlan_edit;
}

uint32
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_index, ARAD_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_OUT_OF_RANGE_ERR, 9, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid_source, ARAD_PP_LIF_ING_VLAN_EDIT_VID_SOURCE_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pcp_dei_source, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SOURCE_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY_verify(
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tag_format, ARAD_PP_LIF_ING_VLAN_EDIT_TAG_FORMAT_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->edit_profile, ARAD_PP_LIF_ING_VLAN_EDIT_EDIT_PROFILE_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY_verify()",0,0);
}

uint32
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tags_to_remove, ARAD_PP_LIF_ING_VLAN_EDIT_TAGS_TO_REMOVE_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile, ARAD_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_OUT_OF_RANGE_ERR, 11, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO, &(info->inner_tag), 12, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO, &(info->outer_tag), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_verify()",0,0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 


