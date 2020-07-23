
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
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_ac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_esem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>





#define ARAD_PP_EG_AC_VLAN_DOMAIN_MAX                            (SOC_SAND_U32_MAX)
#define ARAD_PP_EG_AC_PEP_EDIT_PROFILE_MAX                       (7)
#define ARAD_PP_EG_AC_PCP_PROFILE_MAX                            (15)
#define ARAD_PP_EG_AC_EDIT_PROFILE_MAX                           (SOC_SAND_U32_MAX)
#define ARAD_PP_EG_AC_NOF_TAGS_MAX                               (SOC_PPC_VLAN_TAGS_MAX)













CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_eg_ac[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_MP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_MP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_MP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_MP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_MP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_MP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_MP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_AC_MP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_INFO_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_INFO_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_INFO_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_INFO_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_INFO_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_INFO_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_INFO_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_INFO_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_INFO_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_INFO_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_INFO_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_INFO_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_INFO_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_INFO_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_INFO_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_INFO_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_MAP_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_MAP_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_CVID_MAP_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_MAP_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_MAP_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_PORT_VSI_MAP_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_AC_GET_ERRS_PTR),
  

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_eg_ac[] =
{
  
  {
    ARAD_PP_EG_AC_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_AC_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_AC_VLAN_DOMAIN_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_AC_VLAN_DOMAIN_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_domain' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_AC_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_AC_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'pep_edit_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_AC_PCP_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_AC_PCP_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'pcp_profile' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_AC_EDIT_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_AC_EDIT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'edit_profile' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_AC_NOF_TAGS_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_AC_NOF_TAGS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_tags' is out of range. \n\r "
    "The range is: 2 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_AC_MAX_LEVEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_AC_MAX_LEVEL_OUT_OF_RANGE_ERR",
    "The parameter 'max_level' is out of range. \n\r "
    "The range is: Arad - B: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_AC_TYPE_INVALID_ERR,
    "ARAD_PP_EG_AC_TYPE_INVALID_ERR",
    "The parameter 'out_ac_ndx' is incorrect. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  {
    ARAD_PP_ESEM_REMOVE_FAILED,
    "ARAD_PP_ESEM_REMOVE_FAILED",
    "Failed removing an existing ESEM entry\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
   
  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





uint32
  arad_pp_eg_ac_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    fld_val;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  fld_val = 1;
  if (SOC_IS_JERICHO_PLUS_A0(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EDB_ESEM_0_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, ESEM_0_MNGMNT_UNIT_ENABLEf,  fld_val));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EDB_ESEM_1_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, ESEM_1_MNGMNT_UNIT_ENABLEf,  fld_val));
  }
  else {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EDB_ESEM_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, ESEM_MNGMNT_UNIT_ENABLEf,  fld_val));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_init_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_ac_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info
  )
{
  uint32
    res = SOC_SAND_OK,
    ind;
  ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT
    tbl_data;
  uint32
    vid_idx;
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
    cur_eep_type = 0;
  uint8 is_full_entry_extension = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_AC_INFO_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
 
  res = arad_pp_eg_encap_access_key_prefix_type_get_unsafe(
          unit, 
          out_ac_ndx, 
          &cur_eep_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (cur_eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC) 
  {
    res = arad_pp_eg_encap_access_out_ac_entry_format_tbl_get_unsafe(
            unit,
            out_ac_ndx,
            &tbl_data
          );  
    SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);
  } 
  else 
  {
    soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));    
  }
 
  
  tbl_data.outlif_profile = ac_info->edit_info.lif_profile;
  tbl_data.pcp_dei_profile = ac_info->edit_info.pcp_profile;
  tbl_data.vlan_edit_profile = ac_info->edit_info.edit_profile;
  tbl_data.oam_lif_set = ac_info->edit_info.oam_lif_set;

  tbl_data.vid[0] = 0;
  tbl_data.vid[1] = 0;
  
  for (vid_idx = 0; vid_idx < ac_info->edit_info.nof_tags; vid_idx++)
  {
    tbl_data.vid[vid_idx] = ac_info->edit_info.vlan_tags[vid_idx].vid;
  }

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    tbl_data.use_as_data_entry = ac_info->edit_info.use_as_data_entry;
    
    for (ind = 0; ind < SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE; ind++) {
      tbl_data.data[ind] = ac_info->edit_info.data[ind];
    }
  }
#endif

  res = arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_unsafe(
          unit,
          out_ac_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 
  
  if (SOC_IS_JERICHO(unit)) {
      res = arad_pp_eg_encap_protection_info_set_unsafe(
                unit, out_ac_ndx, is_full_entry_extension, &(ac_info->protection_info));
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_info_set_unsafe()", out_ac_ndx, 0);
}

uint32
  arad_pp_eg_ac_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_AC_INFO_SET_VERIFY);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, out_ac_ndx, SOC_PPC_AC_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_AC_INFO, unit, ac_info, 20, exit);

#ifdef BCM_88660_A0
  
  if (SOC_IS_ARADPLUS(unit) && (ac_info->edit_info.use_as_data_entry) && ((out_ac_ndx & 0x1) != 0x0))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_AC_TYPE_INVALID_ERR, 30, exit);
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_info_set_verify()", out_ac_ndx, 0);
}

uint32
  arad_pp_eg_ac_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_AC_INFO_GET_VERIFY);
  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, out_ac_ndx, SOC_PPC_AC_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_info_get_verify()", out_ac_ndx, 0);
}


uint32
  arad_pp_eg_ac_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO                              *ac_info
  )
{
  uint32
    res = SOC_SAND_OK,
    ind;
  ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT
    tbl_data;
  uint32
    vid_idx;
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
    cur_eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC;
  uint8 is_full_entry_extension = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_AC_INFO_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_PPC_EG_AC_INFO_clear(ac_info);

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

  SOC_PPC_EG_AC_INFO_clear(ac_info);

  arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, out_ac_ndx, &cur_eep_type);
  if (cur_eep_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE && cur_eep_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC) {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_AC_TYPE_INVALID_ERR, 10, exit); 
  } 

  if (cur_eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE) {
    ARAD_DO_NOTHING_AND_EXIT;
  }
    
  
  res = arad_pp_eg_encap_access_out_ac_entry_format_tbl_get_unsafe(
          unit,
          out_ac_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ac_info->edit_info.pcp_profile = tbl_data.pcp_dei_profile;
  ac_info->edit_info.edit_profile = tbl_data.vlan_edit_profile;
  ac_info->edit_info.oam_lif_set = tbl_data.oam_lif_set;
  ac_info->edit_info.lif_profile = tbl_data.outlif_profile;
  
  for (vid_idx = 0; vid_idx < SOC_PPC_VLAN_TAGS_MAX; vid_idx++)
  {
    ac_info->edit_info.vlan_tags[vid_idx].vid = tbl_data.vid[vid_idx];
  }
  
  ac_info->edit_info.nof_tags = 2;
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    ac_info->edit_info.use_as_data_entry = tbl_data.use_as_data_entry;
    
    for (ind = 0; ind < SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE; ind++) {
        ac_info->edit_info.data[ind] = tbl_data.data[ind];
    }
  }
#endif

  
  if (SOC_IS_JERICHO(unit)) {
      res = arad_pp_eg_encap_protection_info_get_unsafe(
                unit, out_ac_ndx, is_full_entry_extension, &(ac_info->protection_info));
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_info_get_unsafe()", out_ac_ndx, 0);
}


uint32
  arad_pp_eg_ac_mp_info_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_MP_INFO                *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_AC_MP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_mp_info_set_unsafe()", out_ac_ndx, 0);
}

uint32
  arad_pp_eg_ac_mp_info_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_MP_INFO                *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_AC_MP_INFO_SET_VERIFY);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 35, exit);
     
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_mp_info_set_verify()", out_ac_ndx, 0);
}

uint32
  arad_pp_eg_ac_mp_info_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_AC_MP_INFO_GET_VERIFY);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_mp_info_get_verify()", out_ac_ndx, 0);
}


uint32
  arad_pp_eg_ac_mp_info_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_OUT SOC_PPC_EG_AC_MP_INFO                *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_AC_MP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_mp_info_get_unsafe()", out_ac_ndx, 0);
}


uint32
  arad_pp_eg_ac_port_vsi_info_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                                   out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                           *vbp_key,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                              *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ESEM_KEY
    esem_key;
  ARAD_PP_ESEM_ENTRY
    esem_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_VSI_INFO_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vbp_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  
  esem_key.key_type = ARAD_PP_ESEM_KEY_TYPE_VD_VSI;
  esem_key.key_info.vd_vsi.vd = vbp_key->vlan_domain;
  esem_key.key_info.vd_vsi.vsi = vbp_key->vsi;

  
  esem_val.out_ac = out_ac_ndx;

  res = arad_pp_esem_entry_add_unsafe(
          unit,
          &esem_key,
          &esem_val,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if(*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES)
  {
    res = arad_pp_eg_ac_info_set_unsafe(
            unit,
            out_ac_ndx,
            ac_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_vsi_info_add_unsafe()", out_ac_ndx, 0);
}

uint32
  arad_pp_eg_ac_port_vsi_info_add_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                                   out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                           *vbp_key,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                              *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_VSI_INFO_ADD_VERIFY);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, out_ac_ndx, SOC_PPC_AC_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_AC_VBP_KEY, vbp_key, 20, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_AC_INFO, unit, ac_info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_vsi_info_add_verify()", out_ac_ndx, 0);
}


uint32
  arad_pp_eg_ac_port_vsi_info_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                           *vbp_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                                   *out_ac
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ESEM_KEY
    esem_key;
  ARAD_PP_ESEM_ENTRY
    esem_val;
  uint8
    is_found,
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_VSI_INFO_REMOVE_UNSAFE);

  
  esem_key.key_type = ARAD_PP_ESEM_KEY_TYPE_VD_VSI;
  esem_key.key_info.vd_vsi.vd = vbp_key->vlan_domain;
  esem_key.key_info.vd_vsi.vsi = vbp_key->vsi;

  res = arad_pp_esem_entry_get_unsafe(
          unit,
          &esem_key,
          &esem_val,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (is_found)
  {
    *out_ac = esem_val.out_ac;
  
    res = arad_pp_esem_entry_remove_unsafe(
            unit,
            &esem_key,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (!success)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_ESEM_REMOVE_FAILED, 30, exit);
    }
  }
  else
  {
    *out_ac = SOC_PPC_AC_ID_INVALID;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_vsi_info_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_ac_port_vsi_info_remove_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                           *vbp_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_VSI_INFO_REMOVE_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_AC_VBP_KEY, vbp_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_vsi_info_remove_verify()", 0, 0);
}


uint32
  arad_pp_eg_ac_port_vsi_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                           *vbp_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                                   *out_ac,
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO                              *ac_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ESEM_KEY
    esem_key;
  ARAD_PP_ESEM_ENTRY
    esem_val;
  uint8
    is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_VSI_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vbp_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPC_EG_AC_INFO_clear(ac_info);

  
  esem_key.key_type = ARAD_PP_ESEM_KEY_TYPE_VD_VSI;
  esem_key.key_info.vd_vsi.vd = vbp_key->vlan_domain;
  esem_key.key_info.vd_vsi.vsi = vbp_key->vsi;

  
  res = arad_pp_esem_entry_get_unsafe(
          unit,
          &esem_key,
          &esem_val,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  *out_ac = esem_val.out_ac;

  *found = is_found;

  if (is_found)
  {
    res = arad_pp_eg_ac_info_get_unsafe(
            unit,
            esem_val.out_ac,
            ac_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_vsi_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_ac_port_vsi_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                           *vbp_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_VSI_INFO_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_AC_VBP_KEY, vbp_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_vsi_info_get_verify()", 0, 0);
}



uint32
  arad_pp_eg_ac_port_cvid_info_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ESEM_KEY
    esem_key;
  ARAD_PP_ESEM_ENTRY
    esem_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_CVID_INFO_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  
  esem_key.key_type = ARAD_PP_ESEM_KEY_TYPE_VD_VSI_CVID;  
  esem_key.key_info.vd_cvid.vd = cep_key->vlan_domain;
  esem_key.key_info.vd_cvid.vsi = cep_key->vsi;
  esem_key.key_info.vd_cvid.cvid = cep_key->cvid;

  
  esem_val.out_ac = out_ac_ndx;

  res = arad_pp_esem_entry_add_unsafe(
          unit,
          &esem_key,
          &esem_val,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if(*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES)
  {
    res = arad_pp_eg_ac_info_set_unsafe(
            unit,
            out_ac_ndx,
            ac_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_cvid_info_add_unsafe()", out_ac_ndx, 0);
}

uint32
  arad_pp_eg_ac_port_cvid_info_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_CVID_INFO_ADD_VERIFY);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, out_ac_ndx, SOC_PPC_AC_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_AC_CEP_PORT_KEY, cep_key, 20, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_AC_INFO, unit, ac_info, 30, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_cvid_info_add_verify()", out_ac_ndx, 0);
}


uint32
  arad_pp_eg_ac_port_cvid_info_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ESEM_KEY
    esem_key;
  ARAD_PP_ESEM_ENTRY
    esem_val;
  uint8
    is_found,
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_CVID_INFO_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);

  
  esem_key.key_type = ARAD_PP_ESEM_KEY_TYPE_VD_VSI_CVID;
  esem_key.key_info.vd_cvid.vd = cep_key->vlan_domain;
  esem_key.key_info.vd_cvid.cvid = cep_key->cvid;
  esem_key.key_info.vd_cvid.vsi = cep_key->vsi;
  
  res = arad_pp_esem_entry_get_unsafe(
          unit,
          &esem_key,
          &esem_val,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (is_found)
  {
    *out_ac = esem_val.out_ac;
  
    res = arad_pp_esem_entry_remove_unsafe(
            unit,
            &esem_key,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (!success)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_ESEM_REMOVE_FAILED, 30, exit);
    }
  }
  else
  {
    *out_ac = SOC_PPC_AC_ID_INVALID;
  }

  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_cvid_info_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_ac_port_cvid_info_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_CVID_INFO_REMOVE_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_AC_CEP_PORT_KEY, cep_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_cvid_info_remove_verify()", 0, 0);
}


uint32
  arad_pp_eg_ac_port_cvid_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac,
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ESEM_KEY
    esem_key;
  ARAD_PP_ESEM_ENTRY
    esem_val;
  uint8
    is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_CVID_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPC_EG_AC_INFO_clear(ac_info);

  
  esem_key.key_type = ARAD_PP_ESEM_KEY_TYPE_VD_VSI_CVID;
  esem_key.key_info.vd_cvid.vd = cep_key->vlan_domain;
  esem_key.key_info.vd_cvid.cvid = cep_key->cvid;
  esem_key.key_info.vd_cvid.vsi = cep_key->vsi;

  
  res = arad_pp_esem_entry_get_unsafe(
          unit,
          &esem_key,
          &esem_val,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  *out_ac = esem_val.out_ac;
  *found = is_found;

  if (is_found)
  {
    res = arad_pp_eg_ac_info_get_unsafe(
      unit,
      esem_val.out_ac,
      ac_info
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_cvid_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_ac_port_cvid_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_AC_PORT_CVID_INFO_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_AC_CEP_PORT_KEY, cep_key, 10, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_ac_port_cvid_info_get_verify()", 0, 0);
}



CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_ac_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_eg_ac;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_ac_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_eg_ac;
}
uint32
  SOC_PPC_EG_AC_VBP_KEY_verify(
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vlan_domain, ARAD_PP_EG_AC_VLAN_DOMAIN_MAX, ARAD_PP_EG_AC_VLAN_DOMAIN_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_AC_VBP_KEY_verify()",0,0);
}

uint32
  SOC_PPC_EG_AC_CEP_PORT_KEY_verify(
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vlan_domain, ARAD_PP_EG_AC_VLAN_DOMAIN_MAX, ARAD_PP_EG_AC_VLAN_DOMAIN_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cvid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pep_edit_profile, ARAD_PP_EG_AC_PEP_EDIT_PROFILE_MAX, ARAD_PP_EG_AC_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_AC_CEP_PORT_KEY_verify()",0,0);
}

uint32
  SOC_PPC_EG_VLAN_EDIT_CEP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_CEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cvid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->up, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pcp_profile, ARAD_PP_EG_AC_PCP_PROFILE_MAX, ARAD_PP_EG_AC_PCP_PROFILE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->edit_profile, ARAD_PP_EG_AC_EDIT_PROFILE_MAX, ARAD_PP_EG_AC_EDIT_PROFILE_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_VLAN_EDIT_CEP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_AC_VLAN_EDIT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_AC_VLAN_EDIT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < 2; ++ind)
  {
    ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_VLAN_EDIT_VLAN_INFO, &(info->vlan_tags[ind]), 10, exit);
  }

  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_tags, ARAD_PP_EG_AC_NOF_TAGS_MAX, ARAD_PP_EG_AC_NOF_TAGS_MAX, ARAD_PP_EG_AC_NOF_TAGS_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->edit_profile, ARAD_PP_EG_AC_EDIT_PROFILE_MAX, ARAD_PP_EG_AC_EDIT_PROFILE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pcp_profile, ARAD_PP_EG_AC_PCP_PROFILE_MAX, ARAD_PP_EG_AC_PCP_PROFILE_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_AC_VLAN_EDIT_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_AC_INFO_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_AC_VLAN_EDIT_INFO, &(info->edit_info), 10, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_PROTECTION_INFO, unit, &(info->protection_info), 20, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_AC_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_AC_MP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mp_level, ARAD_PP_MP_LEVEL_MAX, SOC_PPC_MP_LEVEL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_AC_MP_INFO_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


