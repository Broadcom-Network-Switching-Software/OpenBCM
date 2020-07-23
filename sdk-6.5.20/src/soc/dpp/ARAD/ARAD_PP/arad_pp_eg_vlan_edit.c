
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/mem.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PPC/ppc_api_eg_vlan_edit.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_vlan_edit.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/drv.h>




#define ARAD_PP_EG_VLAN_EDIT_NOF_PCP_PROFILE_NDXS                (16)
#define ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX                 (ARAD_PP_EG_VLAN_EDIT_NOF_PCP_PROFILE_NDXS-1)
#define ARAD_PP_EG_VLAN_EDIT_KEY_MAPPING_MAX                     (SOC_PPC_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEYS_ARAD-1)
#define ARAD_PP_EG_VLAN_EDIT_PEP_EDIT_PROFILE_MAX                (3)
#define ARAD_PP_EG_VLAN_EDIT_TAG_FORMAT_MAX                      (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)
#define ARAD_PP_EG_VLAN_EDIT_EDIT_PROFILE_MAX(unit)              (SOC_PPC_NOF_EGRESS_VLAN_EDIT_ACTION_IDS(unit) - 1)
#define ARAD_PP_EG_VLAN_EDIT_TPID_INDEX_MAX                      (SOC_SAND_U32_MAX)
#define ARAD_PP_EG_VLAN_EDIT_VID_SOURCE_MAX                      (SOC_PPC_NOF_EG_VLAN_EDIT_TAG_VID_SRCS_ARAD-1)
#define ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SOURCE_MAX                  (SOC_PPC_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_SRCS-1)
#define ARAD_PP_EG_VLAN_EDIT_TAGS_TO_REMOVE_MAX                  (2)
#define ARAD_PP_DEI_CFI_MAX                                      (1)
#define ARAD_PP_EG_VLAN_EDIT_NEW_LINK_PCP_PROFILE_NDXS           (4) 
#define ARAD_PP_EG_VLAN_EDIT_NEW_LINK_PCP_PROFILE_NDX_MAX        (ARAD_PP_EG_VLAN_EDIT_NEW_LINK_PCP_PROFILE_NDXS-1) 






#define ARAD_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_NONE 0
#define ARAD_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_SRC_OUTER_TAG 1
#define ARAD_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_SRC_INNER_TAG 2
#define ARAD_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_SRC_MAP 3

#define ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, src_suffix) \
  case SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_##src_suffix: \
    fld_val = ARAD_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_##src_suffix; \
  break;

#define ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD(src, fld_val) \
  switch (src) \
  { \
  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, NONE) \
  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, SRC_OUTER_TAG) \
  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, SRC_INNER_TAG) \
  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, SRC_MAP) \
    default: \
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_OUT_OF_RANGE_ERR, 555, exit); \
  }

#define ARAD_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, src_suffix) \
  case ARAD_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_##src_suffix: \
  src = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_##src_suffix; \
  break;

#define ARAD_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC(fld_val, src) \
  switch (fld_val) \
  { \
  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, NONE) \
  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, SRC_OUTER_TAG) \
  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, SRC_INNER_TAG) \
  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, SRC_MAP) \
  default: \
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_OUT_OF_RANGE_ERR, 556, exit); \
  }



#define SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_EMPTY 0
#define SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_OUTER_TAG 1
#define SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_INNER_TAG 2
#define SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_EDIT_INFO_0 3
#define SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_EDIT_INFO_1 4
#define SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_VSI 5

#define ARAD_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, src_suffix) \
  case SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_##src_suffix: \
  fld_val = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_##src_suffix; \
  break;

#define ARAD_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD(src, fld_val) \
  switch (src) \
  { \
  ARAD_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, EMPTY) \
  ARAD_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, OUTER_TAG) \
  ARAD_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, INNER_TAG) \
  ARAD_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, EDIT_INFO_0) \
  ARAD_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, EDIT_INFO_1) \
  ARAD_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, VSI) \
  default: \
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_VLAN_EDIT_VID_SRC_OUT_OF_RANGE_ERR, 557, exit); \
  }

#define ARAD_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, src_suffix) \
  case SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_##src_suffix: \
  src = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_##src_suffix; \
  break;

#define ARAD_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC(fld_val, src) \
  switch (fld_val) \
  { \
  ARAD_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, EMPTY) \
  ARAD_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, OUTER_TAG) \
  ARAD_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, INNER_TAG) \
  ARAD_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, EDIT_INFO_0) \
  ARAD_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, EDIT_INFO_1) \
  ARAD_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, VSI) \
  default: \
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_VLAN_EDIT_VID_SRC_OUT_OF_RANGE_ERR, 558, exit); \
  }

#define ARAD_PP_EG_VLAN_EDIT_PCP_DEI_MAP_TABLE_OFFSET(epni_entry_offset) \
            (epni_entry_offset >> 3)
#define ARAD_PP_EG_VLAN_EDIT_PCP_DEI_MAP_ENTRY_OFFSET(epni_entry_offset) \
            (epni_entry_offset & 0x7)


#define ARAD_PP_EG_VLAN_EDIT_DSCP_EXP_TO_PCP_DEI_MAP_TABLE_OFFSET(header_code ,dscp_exp) \
           ((header_code << 5) + (dscp_exp >> 3))

#define ARAD_PP_EG_VLAN_EDIT_DSCP_EXP_TO_PCP_DEI_MAP_ENTRY_OFFSET(dscp_exp) \
            (dscp_exp & 0x7)
#define ARAD_PP_EG_VLAN_EDIT_DSCP_EXP_TO_PCP_DEI_MAP_SIZE_OF_ENTRY 4


#define ARAD_PP_EG_VLAN_EDIT_PACKET_IS_TAGGED_TO_REG_ID(evec_id) \
            (evec_id >> 8)
#define ARAD_PP_EG_VLAN_EDIT_PACKET_IS_TAGGED_TO_FLD(evec_id) \
            (evec_id & 0xFF)









CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_eg_vlan_edit[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_GET_ERRS_PTR),
  

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_DEI_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_VLAN_EDIT_PCP_DEI_TBL_GET),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_eg_vlan_edit[] =
{
  
  {
    ARAD_PP_EG_VLAN_EDIT_TRANSMIT_OUTER_TAG_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VLAN_EDIT_TRANSMIT_OUTER_TAG_OUT_OF_RANGE_ERR",
    "The parameter 'transmit_outer_tag' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'pcp_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_VLAN_EDIT_KEY_MAPPING_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VLAN_EDIT_KEY_MAPPING_OUT_OF_RANGE_ERR",
    "The parameter 'key_mapping' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEYS_ARAD-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_VLAN_EDIT_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VLAN_EDIT_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'pep_edit_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR",
    "The parameter 'tag_format' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'edit_profile' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_VLAN_EDIT_TPID_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VLAN_EDIT_TPID_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_index' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR",
    "The parameter 'vid_source' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_EG_VLAN_EDIT_TAG_VID_SRCS_ARAD-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR",
    "The parameter 'pcp_dei_source' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_SRCS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR",
    "The parameter 'tags_to_remove' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  {
    ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_OUT_OF_RANGE_ERR",
    "pcp-dei source is out of range. \n\r "
    "Allowed values are inner-tag, outer-tag or src-map (see \n\r"
    "SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_VLAN_EDIT_VID_SRC_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VLAN_EDIT_VID_SRC_OUT_OF_RANGE_ERR",
    "Vid source is out of range. \n\r "
    "Allowed values are  (see \n\r"
    "SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_HDR_TYPE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_HDR_TYPE_NDX_OUT_OF_RANGE_ERR",
    "Packet header type ndx is out of range. \n\r "
    "Allowed values are  (see \n\r"
    "SOC_PPC_PKT_HDR_TYPE).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PKT_FRWRD_TYPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PPC_PKT_FRWRD_TYPE_NDX_OUT_OF_RANGE_ERR,",
    "Packet forward type ndx is out of range. \n\r "
    "Allowed values are  (see \n\r"
    "SOC_PPC_PKT_FRWRD_TYPE).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EPNI_PCP_DEI_MAP_KEY_OUT_OF_RANGE_ERR,
    "ARAD_PP_EPNI_PCP_DEI_MAP_KEY_OUT_OF_RANGE_ERR",
    "PCP_DEI_MAP_KEY value is out of range. \n\r "
    "Allowed values are  (see \n\r"
    "0-2).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





uint32
  arad_pp_eg_vlan_edit_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      low_reg,
      tmp,
      data[2] = {0},
      inner_pcp_dei, inner_tpid, inner_vid, outer_pcp_dei, outer_tpid, outer_vid, tags_to_remove,
      pcp_profile_ndx, up_ndx, tc_ndx, dp_ndx, packet_is_tagged_after_eve;
  uint64 reg_val;
  soc_reg_above_64_val_t reg_above_64_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  COMPILER_64_ZERO(reg_val);
  SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

  
  inner_pcp_dei = ARAD_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_SRC_MAP;
  inner_tpid = 0;
  inner_vid = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_EMPTY;
  outer_pcp_dei = ARAD_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_SRC_MAP;
  outer_tpid = 0;
  outer_vid = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_EMPTY;
  tags_to_remove = 0;
  packet_is_tagged_after_eve = 1;

  
  data[0] = ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_INFO(unit, outer_tpid, inner_tpid, outer_vid, inner_vid, outer_pcp_dei, inner_pcp_dei, tags_to_remove, packet_is_tagged_after_eve);

  arad_fill_table_with_entry(unit, EPNI_EVEC_TABLEm, MEM_BLOCK_ANY, data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  if (SOC_IS_JERICHO_PLUS(unit)) {
      ARAD_CLEAR(data, uint32, 2);
      
      data[0] = ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_INFO(unit, outer_tpid, inner_tpid, outer_vid, inner_vid, outer_pcp_dei, inner_pcp_dei, tags_to_remove, 0);

      arad_fill_table_with_entry(unit, EPNI_NATIVE_EVEC_TABLEm, MEM_BLOCK_ANY, data); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_EPNI_DEFAULT_AC_ENTRYr(unit, REG_PORT_ANY, &reg_val));
  low_reg = COMPILER_64_LO(reg_val);
  tmp = 0;
  soc_sand_bitstream_set_any_field(&tmp, 28, 3, &low_reg);
  COMPILER_64_SET(reg_val, 0, low_reg);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit,WRITE_EPNI_DEFAULT_AC_ENTRYr(unit, REG_PORT_ANY, reg_val));

  
  if (!SOC_IS_JERICHO_B0_AND_ABOVE(unit)) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit,WRITE_EPNI_PACKET_HAS_OUTER_VIDr(unit, REG_PORT_ANY, 0xfffe));
  } else {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit,WRITE_EPNI_PACKET_HAS_OUTER_VIDr(unit, REG_PORT_ANY, 0x0));
  }

  
  if (SOC_IS_JERICHO_B0(unit) || SOC_IS_QMX_B0(unit)) {
    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, soc_reg_above_64_set(unit, EPNI_PACKET_IS_TAGGED_AFTER_EVE_1r, SOC_CORE_ALL, 0, reg_above_64_val));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_reg_above_64_set(unit, EPNI_PACKET_IS_TAGGED_AFTER_EVE_2r, SOC_CORE_ALL, 0, reg_above_64_val));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, soc_reg_above_64_set(unit, EPNI_PACKET_IS_TAGGED_AFTER_EVE_3r, SOC_CORE_ALL, 0, reg_above_64_val));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, soc_reg_above_64_set(unit, EPNI_PACKET_IS_TAGGED_AFTER_EVE_4r, SOC_CORE_ALL, 0, reg_above_64_val));
  }

  
  pcp_profile_ndx = 0; 
  for (tc_ndx = 0; tc_ndx <= SOC_SAND_PP_TC_MAX; ++tc_ndx) 
  {
    up_ndx = tc_ndx;
    for (dp_ndx = 0; dp_ndx <= SOC_SAND_PP_DP_MAX; ++dp_ndx) 
    {
      res = arad_pp_eg_vlan_edit_pcp_map_untagged_set_unsafe(unit, pcp_profile_ndx, tc_ndx, dp_ndx, up_ndx, 0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    }
  }

  
    if (SOC_IS_JERICHO_PLUS(unit)) {
        uint32 mask, mask_sht, nof_bits, outlif_profile;
        uint32 start_bit = 0;
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, soc_reg_above_64_set(unit,
            EPNI_CFG_NATIVE_ESEM_KEY_SELECTr, SOC_CORE_ALL, 0, reg_above_64_val));

        
        
        nof_bits = (soc_property_get(unit, spn_NOF_L2CP_EGRESS_PROFILES_MAX, 1) / 2) & 0x3;
        nof_bits = (nof_bits > 2) ? 2 : nof_bits;
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        if (nof_bits != 0) {
            res = arad_pp_occ_mgmt_get_app_mask(unit, SOC_OCC_MGMT_TYPE_OUTLIF,
                            SOC_OCC_MGMT_OUTLIF_APP_L2CP_EGRESS_PROFILE, &mask);
            SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

            mask_sht = mask;
            for (outlif_profile = 0; outlif_profile < SOC_DPP_IMP_DEFS_GET(unit, outlif_profile_nof_bits); outlif_profile++) {
                if ((mask_sht & 1) == 0) {
                    start_bit++;
                    mask_sht = (mask_sht >> 1);
                } else {
                    break;
                }
            }

         
            for (outlif_profile = 0; outlif_profile < (1 << SOC_DPP_IMP_DEFS_GET(unit, outlif_profile_nof_bits)); outlif_profile++) {
                if (outlif_profile & mask) {
                    mask_sht = (outlif_profile & mask) >> start_bit;
                    SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, outlif_profile * 2, (uint32*)&mask_sht, 0, 2);
                }
            }
        }
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 108, exit, soc_reg_above_64_set(unit,
            EPNI_CFG_MAP_OUTLIF_PROFILE_TO_L_2_CP_PROFILEr, SOC_CORE_ALL, 0, reg_above_64_val));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 109, exit, soc_reg_above_64_set(unit,
            EPNI_CFG_MAP_OUTLIF_PROFILE_TO_NATIVE_L_2_CP_PROFILEr, SOC_CORE_ALL, 0, reg_above_64_val));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_init_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_vlan_edit_pep_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                                 *pep_key,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_PEP_INFO                   *pep_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pep_key);
  SOC_SAND_CHECK_NULL_INPUT(pep_info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pep_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_vlan_edit_pep_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                                 *pep_key,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_PEP_INFO                   *pep_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_PEP_KEY, pep_key, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_VLAN_EDIT_PEP_INFO, pep_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pep_info_set_verify()", 0, 0);
}

uint32
  arad_pp_eg_vlan_edit_pep_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                                 *pep_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_PEP_KEY, pep_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pep_info_get_verify()", 0, 0);
}


uint32
  arad_pp_eg_vlan_edit_pep_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                                 *pep_key,
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_PEP_INFO                   *pep_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pep_key);
  SOC_SAND_CHECK_NULL_INPUT(pep_info);

  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_clear(pep_info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pep_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_vlan_edit_command_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY                *command_key,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO               *command_info
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(command_key);
  SOC_SAND_CHECK_NULL_INPUT(command_info);

  entry_offset = ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_INDX(unit, command_key->tag_format, command_key->edit_profile);

  tbl_data.tags_to_remove = command_info->tags_to_remove;
  tbl_data.inner_tpid_index = command_info->inner_tag.tpid_index;
  tbl_data.outer_tpid_index = command_info->outer_tag.tpid_index;
  tbl_data.packet_is_tagged_after_eve = command_info->packet_is_tagged_after_eve;

  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD(
    command_info->inner_tag.pcp_dei_source,
    tbl_data.inner_pcp_dei_source
    );

  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD(
    command_info->outer_tag.pcp_dei_source,
    tbl_data.outer_pcp_dei_source
    );
  
  ARAD_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD(
    command_info->inner_tag.vid_source,
    tbl_data.inner_vid_source
    );

  ARAD_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD(
    command_info->outer_tag.vid_source,
    tbl_data.outer_vid_source
    );

  res = arad_pp_epni_egress_edit_cmd_tbl_set_unsafe(
    unit,
    entry_offset,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_command_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_vlan_edit_command_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY                *command_key,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO               *command_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_SET_VERIFY);

  res = SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_verify(unit, command_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO, command_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_command_info_set_verify()", 0, 0);
}

uint32
  arad_pp_eg_vlan_edit_command_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY                *command_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_GET_VERIFY);

  res = SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_verify(unit, command_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_command_info_get_verify()", 0, 0);
}


uint32
  arad_pp_eg_vlan_edit_command_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY                *command_key,
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO               *command_info
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(command_key);
  SOC_SAND_CHECK_NULL_INPUT(command_info);

  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_clear(command_info);

  entry_offset = ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_INDX(unit, command_key->tag_format, command_key->edit_profile);

  res = arad_pp_epni_egress_edit_cmd_tbl_get_unsafe(
    unit,
    entry_offset,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  command_info->tags_to_remove = (uint8)tbl_data.tags_to_remove;
  command_info->inner_tag.tpid_index = tbl_data.inner_tpid_index;
  command_info->outer_tag.tpid_index = tbl_data.outer_tpid_index;
  command_info->packet_is_tagged_after_eve = tbl_data.packet_is_tagged_after_eve;

  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC(
    tbl_data.inner_pcp_dei_source,
    command_info->inner_tag.pcp_dei_source
    );

  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC(
    tbl_data.outer_pcp_dei_source,
    command_info->outer_tag.pcp_dei_source
    );

  ARAD_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC(
    tbl_data.inner_vid_source,
    command_info->inner_tag.vid_source
    );

  ARAD_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC(
    tbl_data.outer_vid_source,
    command_info->outer_tag.vid_source
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_command_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint8                                 transmit_outer_tag
  )
{
  uint32
    res = SOC_SAND_OK,
    bit_val;
  ARAD_PP_EPNI_TX_TAG_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_UNSAFE);

  res = arad_pp_epni_tx_tag_table_tbl_get_unsafe(
          unit,
          core_id,
          vid_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  bit_val = SOC_SAND_BOOL2NUM(transmit_outer_tag);
  res = soc_sand_bitstream_set_any_field(&bit_val, local_port_ndx, 1, tbl_data.entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_epni_tx_tag_table_tbl_set_unsafe(
          unit,
          core_id,
          vid_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set_unsafe()", local_port_ndx, vid_ndx);
}

uint32
  arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint8                                 transmit_outer_tag
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_VERIFY);

  res = arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_verify(
    unit,
    local_port_ndx,
    vid_ndx
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set_verify()", local_port_ndx, vid_ndx);
}

uint32
  arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_verify()", local_port_ndx, vid_ndx);
}


uint32
  arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint8                                 *transmit_outer_tag
  )
{
  uint32
    res = SOC_SAND_OK,
    bit_val = 0;
  ARAD_PP_EPNI_TX_TAG_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(transmit_outer_tag);

  res = arad_pp_epni_tx_tag_table_tbl_get_unsafe(
          unit,
          core_id,
          vid_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field((uint32*)&tbl_data.entry, local_port_ndx, 1, &bit_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *transmit_outer_tag = SOC_SAND_NUM2BOOL(bit_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_unsafe()", local_port_ndx, vid_ndx);
}


uint32
  arad_pp_eg_vlan_edit_pcp_profile_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY        key_mapping
  )
{
  uint32
    res = SOC_SAND_OK,
    tc_dp_fld_val,
    dscp_exp_fld_val,
    bit_val;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_UNSAFE);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, PCP_DEI_PROFILE_USE_TC_DPf, &tc_dp_fld_val));
  bit_val = SOC_SAND_BOOL2NUM(key_mapping == SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_TC_DP);
  res = soc_sand_bitstream_set(&tc_dp_fld_val, pcp_profile_ndx, bit_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, PCP_DEI_PROFILE_USE_TC_DPf,  tc_dp_fld_val));
  
  if (SOC_IS_JERICHO_PLUS(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_NATIVE_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, NATIVE_PCP_DEI_PROFILE_USE_TC_DPf,  tc_dp_fld_val));
  }
  
  if (SOC_IS_JERICHO(unit) && SOC_IS_JERICHO_AND_BELOW(unit)) {
      if (pcp_profile_ndx <= ARAD_PP_EG_VLAN_EDIT_NEW_LINK_PCP_PROFILE_NDX_MAX) {
          tc_dp_fld_val = 0;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_NEW_LLNK_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, NEW_LINK_OUTER_PCP_DEI_PROFILE_USE_TC_DPf, &tc_dp_fld_val));
          res = soc_sand_bitstream_set(&tc_dp_fld_val, pcp_profile_ndx, bit_val);
          SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
          
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_NEW_LLNK_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, NEW_LINK_OUTER_PCP_DEI_PROFILE_USE_TC_DPf,  tc_dp_fld_val));
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_NEW_LLNK_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, NEW_LINK_INNER_PCP_DEI_PROFILE_USE_TC_DPf,  tc_dp_fld_val));
      }
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, PCP_DEI_PROFILE_MAP_DSCP_EXPf, &dscp_exp_fld_val));
  bit_val = SOC_SAND_BOOL2NUM(key_mapping == SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_DSCP_EXP);
  res = soc_sand_bitstream_set(&dscp_exp_fld_val, pcp_profile_ndx, bit_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, PCP_DEI_PROFILE_MAP_DSCP_EXPf,  dscp_exp_fld_val));
  
  if (SOC_IS_JERICHO_PLUS(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_NATIVE_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, NATIVE_PCP_DEI_PROFILE_MAP_DSCP_EXPf,  dscp_exp_fld_val));
  }
  
  if (SOC_IS_JERICHO(unit) && SOC_IS_JERICHO_AND_BELOW(unit)) {
      if (pcp_profile_ndx <= ARAD_PP_EG_VLAN_EDIT_NEW_LINK_PCP_PROFILE_NDX_MAX) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_NEW_LLNK_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, NEW_LINK_OUTER_PCP_DEI_PROFILE_MAP_DSCP_EXPf, &dscp_exp_fld_val));
          res = soc_sand_bitstream_set(&dscp_exp_fld_val, pcp_profile_ndx, bit_val);
          SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
          
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_NEW_LLNK_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, NEW_LINK_OUTER_PCP_DEI_PROFILE_MAP_DSCP_EXPf,  dscp_exp_fld_val));
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_NEW_LLNK_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, NEW_LINK_INNER_PCP_DEI_PROFILE_MAP_DSCP_EXPf,  dscp_exp_fld_val));
      }
  }


  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_profile_info_set_unsafe()", pcp_profile_ndx, 0);
}

uint32
  arad_pp_eg_vlan_edit_pcp_profile_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY        key_mapping
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(key_mapping, ARAD_PP_EG_VLAN_EDIT_KEY_MAPPING_MAX, ARAD_PP_EG_VLAN_EDIT_KEY_MAPPING_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_profile_info_set_verify()", pcp_profile_ndx, 0);
}

uint32
  arad_pp_eg_vlan_edit_pcp_profile_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_profile_info_get_verify()", pcp_profile_ndx, 0);
}


uint32
  arad_pp_eg_vlan_edit_pcp_profile_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY        *key_mapping
  )
{
  uint32
    res,
    tc_dp_fld_val,
    dscp_exp_fld_val,
    tc_dp_bit_val,
    dscp_exp_bit_val;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(key_mapping);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, PCP_DEI_PROFILE_USE_TC_DPf, &tc_dp_fld_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_PCP_DEI_PROFILEr, SOC_CORE_ALL, 0, PCP_DEI_PROFILE_MAP_DSCP_EXPf, &dscp_exp_fld_val));

  tc_dp_bit_val = SOC_SAND_GET_BIT(tc_dp_fld_val, pcp_profile_ndx);
  dscp_exp_bit_val = SOC_SAND_GET_BIT(dscp_exp_fld_val, pcp_profile_ndx);

  if (dscp_exp_bit_val) {
       *key_mapping = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_DSCP_EXP;
  } 
  else if (tc_dp_bit_val) {
      *key_mapping = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_TC_DP;
  } else {
      *key_mapping = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_PCP;
  }
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_profile_info_get_unsafe()", pcp_profile_ndx, 0);
}

STATIC uint32
  arad_pp_eg_vlan_edit_pcp_dei_tbl_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      epni_entry_offset,
    SOC_SAND_IN  uint8                                       out_pcp,
    SOC_SAND_IN  uint8                                     out_dei
  )
{
  uint32
    res = SOC_SAND_OK,
    epni_tbl_ndx,
    epni_entry_ndx;
  ARAD_PP_EPNI_PCP_DEI_MAP_TBL_DATA
    epni_tbl_data;
  uint32
    tmp;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_DEI_TBL_SET);

  tmp = ARAD_PP_PCP_DEI_TO_FLD_VAL(out_pcp, out_dei);

  epni_tbl_ndx = ARAD_PP_EG_VLAN_EDIT_PCP_DEI_MAP_TABLE_OFFSET(epni_entry_offset);            
  epni_entry_ndx =  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_MAP_ENTRY_OFFSET(epni_entry_offset);

  res = arad_pp_epni_pcp_dei_map_tbl_get_unsafe(
    unit,
    epni_tbl_ndx,
    &epni_tbl_data
    );

  res = soc_sand_bitstream_set_any_field(&tmp, epni_entry_ndx*4, 4, &epni_tbl_data.pcp_dei_map); 

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_epni_pcp_dei_map_tbl_set_unsafe(
    unit,
    epni_tbl_ndx,
    &epni_tbl_data
    );

  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_dei_tbl_set_unsafe()", epni_entry_offset, 0);
}

STATIC uint32
  arad_pp_eg_vlan_edit_pcp_dei_tbl_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      epni_entry_offset,
    SOC_SAND_OUT uint8                                       *out_pcp,
    SOC_SAND_OUT uint8                                     *out_dei
  )
{
 uint32
   res = SOC_SAND_OK, 
   epni_tbl_ndx,
   epni_entry_ndx;
 ARAD_PP_EPNI_PCP_DEI_MAP_TBL_DATA
    epni_tbl_data;
 uint32
   tmp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_DEI_TBL_GET);

  epni_tbl_ndx = ARAD_PP_EG_VLAN_EDIT_PCP_DEI_MAP_TABLE_OFFSET(epni_entry_offset);            
  epni_entry_ndx =  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_MAP_ENTRY_OFFSET(epni_entry_offset);

  res = arad_pp_epni_pcp_dei_map_tbl_get_unsafe(
    unit,
    epni_tbl_ndx,
    &epni_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  res = soc_sand_bitstream_get_any_field(&epni_tbl_data.pcp_dei_map, epni_entry_ndx*4, 4, &tmp); 
  ARAD_PP_PCP_DEI_FROM_FLD_VAL(tmp, *out_pcp, *out_dei); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_dei_tbl_get_unsafe()", epni_entry_offset, 0);
}




uint32
  arad_pp_eg_vlan_edit_pcp_map_stag_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_UNSAFE);

  res = arad_pp_eg_vlan_edit_pcp_dei_tbl_set_unsafe(
    unit,
    ARAD_PP_EPNI_PCP_DEI_TBL_INDX_STAG(pcp_profile_ndx, tag_pcp_ndx, tag_dei_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_stag_set_unsafe()", pcp_profile_ndx, tag_pcp_ndx);
}
 
uint32
  arad_pp_eg_vlan_edit_pcp_map_stag_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_pcp_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_dei_ndx, ARAD_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, ARAD_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_stag_set_verify()", pcp_profile_ndx, tag_pcp_ndx);
}

uint32
  arad_pp_eg_vlan_edit_pcp_map_stag_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_pcp_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_dei_ndx, ARAD_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_stag_get_verify()", pcp_profile_ndx, tag_pcp_ndx);
}


uint32
  arad_pp_eg_vlan_edit_pcp_map_stag_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = arad_pp_eg_vlan_edit_pcp_dei_tbl_get_unsafe(
    unit,
    ARAD_PP_EPNI_PCP_DEI_TBL_INDX_STAG(pcp_profile_ndx, tag_pcp_ndx, tag_dei_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_stag_get_unsafe()", pcp_profile_ndx, tag_pcp_ndx);
}


uint32
  arad_pp_eg_vlan_edit_pcp_map_ctag_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_UNSAFE);

  res = arad_pp_eg_vlan_edit_pcp_dei_tbl_set_unsafe(
    unit,
    ARAD_PP_EPNI_PCP_DEI_TBL_INDX_CTAG(pcp_profile_ndx, tag_up_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_ctag_set_unsafe()", pcp_profile_ndx, tag_up_ndx);
}

uint32
  arad_pp_eg_vlan_edit_pcp_map_ctag_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  uint8                                       tag_up_ndx,
    SOC_SAND_IN  uint8                                       out_pcp,
    SOC_SAND_IN  uint8                                     out_dei
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_up_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, ARAD_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_ctag_set_verify()", pcp_profile_ndx, tag_up_ndx);
}

uint32
  arad_pp_eg_vlan_edit_pcp_map_ctag_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_up_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_ctag_get_verify()", pcp_profile_ndx, tag_up_ndx);
}


uint32
  arad_pp_eg_vlan_edit_pcp_map_ctag_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = arad_pp_eg_vlan_edit_pcp_dei_tbl_get_unsafe(
    unit,
    ARAD_PP_EPNI_PCP_DEI_TBL_INDX_CTAG(pcp_profile_ndx, tag_up_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_ctag_get_unsafe()", pcp_profile_ndx, tag_up_ndx);
}


uint32
  arad_pp_eg_vlan_edit_pcp_map_untagged_set_unsafe(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_UNSAFE);

  res = arad_pp_eg_vlan_edit_pcp_dei_tbl_set_unsafe(
    unit,
    ARAD_PP_EPNI_PCP_DEI_TBL_INDX_UNTAGGED(pcp_profile_ndx, tc_ndx, dp_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_untagged_set_unsafe()", pcp_profile_ndx, tc_ndx);
}

uint32
  arad_pp_eg_vlan_edit_pcp_map_untagged_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, ARAD_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_untagged_set_verify()", pcp_profile_ndx, tc_ndx);
}

uint32
  arad_pp_eg_vlan_edit_pcp_map_untagged_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_untagged_get_verify()", pcp_profile_ndx, tc_ndx);
}


uint32
  arad_pp_eg_vlan_edit_pcp_map_untagged_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = arad_pp_eg_vlan_edit_pcp_dei_tbl_get_unsafe(
    unit,
    ARAD_PP_EPNI_PCP_DEI_TBL_INDX_UNTAGGED(pcp_profile_ndx, tc_ndx, dp_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_untagged_get_unsafe()", pcp_profile_ndx, tc_ndx);
}



uint32 
  arad_pp_eg_vlan_edit_pcp_map_dscp_exp_get_unsafe( 
    SOC_SAND_IN int                               unit, 
    SOC_SAND_IN SOC_PPC_PKT_FRWRD_TYPE                  pkt_type_ndx, 
    SOC_SAND_IN uint8                                 dscp_exp_ndx, 
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                         *out_pcp, 
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                        *out_dei 
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    dscp_exp_tbl_ndx,
    dscp_exp_entry_ndx;
  uint32
    tbl_data[2],
    tmp;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_UNSAFE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  dscp_exp_tbl_ndx = ARAD_PP_EG_VLAN_EDIT_DSCP_EXP_TO_PCP_DEI_MAP_TABLE_OFFSET(pkt_type_ndx,dscp_exp_ndx);            
  dscp_exp_entry_ndx =  ARAD_PP_EG_VLAN_EDIT_DSCP_EXP_TO_PCP_DEI_MAP_ENTRY_OFFSET(dscp_exp_ndx);

  res = READ_EPNI_DSCP_EXP_TO_PCP_DEIm(unit, MEM_BLOCK_ANY, dscp_exp_tbl_ndx, tbl_data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field( 
            &tbl_data[0], 
            dscp_exp_entry_ndx * ARAD_PP_EG_VLAN_EDIT_DSCP_EXP_TO_PCP_DEI_MAP_SIZE_OF_ENTRY, 
            ARAD_PP_EG_VLAN_EDIT_DSCP_EXP_TO_PCP_DEI_MAP_SIZE_OF_ENTRY, 
            &tmp);  
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 
  ARAD_PP_PCP_DEI_FROM_FLD_VAL(tmp, *out_pcp, *out_dei); 
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_dscp_exp_get_unsafe()", pkt_type_ndx, dscp_exp_ndx); 
}


uint32 
  arad_pp_eg_vlan_edit_pcp_map_dscp_exp_get_verify( 
    SOC_SAND_IN int                               unit, 
    SOC_SAND_IN SOC_PPC_PKT_FRWRD_TYPE                  pkt_type_ndx, 
    SOC_SAND_IN uint8                                 dscp_exp_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_GET_VERIFY);

  switch(pkt_type_ndx) {
    case SOC_PPC_PKT_FRWRD_TYPE_BRIDGE:
    case SOC_PPC_PKT_FRWRD_TYPE_IPV4_UC:
    case SOC_PPC_PKT_FRWRD_TYPE_IPV4_MC:
    case SOC_PPC_PKT_FRWRD_TYPE_IPV6_UC:
    case SOC_PPC_PKT_FRWRD_TYPE_IPV6_MC:
    case SOC_PPC_PKT_FRWRD_TYPE_MPLS:
    case SOC_PPC_PKT_FRWRD_TYPE_TRILL:
    case SOC_PPC_PKT_FRWRD_TYPE_CPU_TRAP:
    case SOC_PPC_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM:
    case SOC_PPC_PKT_FRWRD_TYPE_TM:
    break;
    default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_HDR_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  }

  if (pkt_type_ndx == SOC_PPC_PKT_FRWRD_TYPE_MPLS ) {
      SOC_SAND_ERR_IF_ABOVE_MAX(dscp_exp_ndx, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 30, exit);
  } 
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_dscp_exp_get_verify()", pkt_type_ndx, dscp_exp_ndx);
}


uint32 
  arad_pp_eg_vlan_edit_pcp_map_dscp_exp_set_unsafe( 
    SOC_SAND_IN int                           unit, 
    SOC_SAND_IN SOC_PPC_PKT_FRWRD_TYPE              pkt_type_ndx, 
    SOC_SAND_IN uint8                             dscp_exp_ndx, 
    SOC_SAND_IN SOC_SAND_PP_PCP_UP                      out_pcp, 
    SOC_SAND_IN SOC_SAND_PP_DEI_CFI                     out_dei 
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    dscp_exp_tbl_ndx,
    dscp_exp_entry_ndx;
  uint32
      tbl_data[2],
      tmp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_UNSAFE);

  dscp_exp_tbl_ndx = ARAD_PP_EG_VLAN_EDIT_DSCP_EXP_TO_PCP_DEI_MAP_TABLE_OFFSET(pkt_type_ndx,dscp_exp_ndx);            
  dscp_exp_entry_ndx =  ARAD_PP_EG_VLAN_EDIT_DSCP_EXP_TO_PCP_DEI_MAP_ENTRY_OFFSET(dscp_exp_ndx);

  res = READ_EPNI_DSCP_EXP_TO_PCP_DEIm(unit, MEM_BLOCK_ANY, dscp_exp_tbl_ndx, tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  tmp = ARAD_PP_PCP_DEI_TO_FLD_VAL(out_pcp, out_dei);

  res = soc_sand_bitstream_set_any_field(
      &tmp, 
      dscp_exp_entry_ndx * ARAD_PP_EG_VLAN_EDIT_DSCP_EXP_TO_PCP_DEI_MAP_SIZE_OF_ENTRY, 
      ARAD_PP_EG_VLAN_EDIT_DSCP_EXP_TO_PCP_DEI_MAP_SIZE_OF_ENTRY, 
      &tbl_data[0]);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = WRITE_EPNI_DSCP_EXP_TO_PCP_DEIm(unit, MEM_BLOCK_ANY, dscp_exp_tbl_ndx, tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  if (SOC_IS_JERICHO_PLUS(unit)) {
      res = WRITE_EPNI_NATIVE_DSCP_EXP_TO_PCP_DEIm(unit, MEM_BLOCK_ANY, dscp_exp_tbl_ndx, tbl_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_dscp_exp_set_unsafe()", 0, 0);
}

uint32 
  arad_pp_eg_vlan_edit_pcp_map_dscp_exp_set_verify( 
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_IN SOC_PPC_PKT_FRWRD_TYPE                     pkt_type_ndx, 
    SOC_SAND_IN uint8                                    dscp_exp_ndx, 
    SOC_SAND_IN SOC_SAND_PP_PCP_UP                             out_pcp, 
    SOC_SAND_IN SOC_SAND_PP_DEI_CFI                            out_dei 
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_VERIFY);

  switch(pkt_type_ndx) {
    case SOC_PPC_PKT_FRWRD_TYPE_BRIDGE:
    case SOC_PPC_PKT_FRWRD_TYPE_IPV4_UC:
    case SOC_PPC_PKT_FRWRD_TYPE_IPV4_MC:
    case SOC_PPC_PKT_FRWRD_TYPE_IPV6_UC:
    case SOC_PPC_PKT_FRWRD_TYPE_IPV6_MC:
    case SOC_PPC_PKT_FRWRD_TYPE_MPLS:
    case SOC_PPC_PKT_FRWRD_TYPE_TRILL:
    case SOC_PPC_PKT_FRWRD_TYPE_CPU_TRAP:
    case SOC_PPC_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM:
    case SOC_PPC_PKT_FRWRD_TYPE_TM:
    break;
    default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_HDR_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  }
  
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, ARAD_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 40, exit);
  if (pkt_type_ndx == SOC_PPC_PKT_FRWRD_TYPE_MPLS ) {
      SOC_SAND_ERR_IF_ABOVE_MAX(dscp_exp_ndx, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 50, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_dscp_exp_set_verify()", pkt_type_ndx, dscp_exp_ndx);
}


soc_error_t arad_pp_eg_vlan_edit_packet_is_tagged_set(
    SOC_SAND_IN int                                 unit,
    SOC_SAND_IN SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY    *eg_command_key,
    SOC_SAND_IN uint32                              is_tagged
    )
{
  uint32
    res = SOC_SAND_OK,
    reg,
    offset,
    evec_index = 0;
  soc_reg_above_64_val_t reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(eg_command_key);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  evec_index = ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_INDX(unit, eg_command_key->tag_format, eg_command_key->edit_profile);

  switch (ARAD_PP_EG_VLAN_EDIT_PACKET_IS_TAGGED_TO_REG_ID(evec_index))
  {
    case 0:
      reg = EPNI_PACKET_IS_TAGGED_AFTER_EVE_1r;
      break;
    case 1:
      reg = EPNI_PACKET_IS_TAGGED_AFTER_EVE_2r;
      break;
    case 2:
      reg = EPNI_PACKET_IS_TAGGED_AFTER_EVE_3r;
      break;
    case 3:
      reg = EPNI_PACKET_IS_TAGGED_AFTER_EVE_4r;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_VLAN_EDIT_KEY_MAPPING_OUT_OF_RANGE_ERR, 5, exit);
      break;
  }

  offset = ARAD_PP_EG_VLAN_EDIT_PACKET_IS_TAGGED_TO_FLD(evec_index);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg_above_64_get(unit, reg, SOC_CORE_ALL, 0, reg_val));

  res = soc_sand_bitstream_set_any_field(&is_tagged, offset, 1, reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_reg_above_64_set(unit, reg, SOC_CORE_ALL, 0, reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_packet_is_tagged_set()", evec_index, is_tagged);
}


soc_error_t arad_pp_eg_vlan_edit_packet_is_tagged_get(
    SOC_SAND_IN int                                 unit,
    SOC_SAND_IN SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY    *eg_command_key,
    SOC_SAND_OUT uint32                             *is_tagged
    )
{
  uint32
    res = SOC_SAND_OK,
    reg = EPNI_PACKET_IS_TAGGED_AFTER_EVE_1r,
    offset,
    evec_index = 0;
  soc_reg_above_64_val_t reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(eg_command_key);
  SOC_SAND_CHECK_NULL_INPUT(is_tagged);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  evec_index = ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_INDX(unit, eg_command_key->tag_format, eg_command_key->edit_profile);

  switch (ARAD_PP_EG_VLAN_EDIT_PACKET_IS_TAGGED_TO_REG_ID(evec_index))
  {
    case 0:
      reg = EPNI_PACKET_IS_TAGGED_AFTER_EVE_1r;
      break;
    case 1:
      reg = EPNI_PACKET_IS_TAGGED_AFTER_EVE_2r;
      break;
    case 2:
      reg = EPNI_PACKET_IS_TAGGED_AFTER_EVE_3r;
      break;
    case 3:
      reg = EPNI_PACKET_IS_TAGGED_AFTER_EVE_4r;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_VLAN_EDIT_KEY_MAPPING_OUT_OF_RANGE_ERR, 5, exit);
      break;
  }
  offset = ARAD_PP_EG_VLAN_EDIT_PACKET_IS_TAGGED_TO_FLD(evec_index);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg_above_64_get(unit, reg, SOC_CORE_ALL, 0, reg_val));

  res = soc_sand_bitstream_get_any_field(reg_val, offset, 1, is_tagged);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_packet_is_tagged_get()", evec_index, *is_tagged);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_vlan_edit_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_eg_vlan_edit;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_vlan_edit_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_eg_vlan_edit;
}

uint32
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_PEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->pvid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->up, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pep_edit_profile, ARAD_PP_EG_VLAN_EDIT_PEP_EDIT_PROFILE_MAX, ARAD_PP_EG_VLAN_EDIT_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_VLAN_EDIT_PEP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tag_format, ARAD_PP_EG_VLAN_EDIT_TAG_FORMAT_MAX, ARAD_PP_EG_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->edit_profile, ARAD_PP_EG_VLAN_EDIT_EDIT_PROFILE_MAX(unit), ARAD_PP_EG_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_verify()",0,0);
}

uint32
  SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_index, ARAD_PP_EG_VLAN_EDIT_TPID_INDEX_MAX, ARAD_PP_EG_VLAN_EDIT_TPID_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid_source, ARAD_PP_EG_VLAN_EDIT_VID_SOURCE_MAX, ARAD_PP_EG_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pcp_dei_source, ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SOURCE_MAX, ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tags_to_remove, ARAD_PP_EG_VLAN_EDIT_TAGS_TO_REMOVE_MAX, ARAD_PP_EG_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO, &(info->inner_tag), 11, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO, &(info->outer_tag), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


