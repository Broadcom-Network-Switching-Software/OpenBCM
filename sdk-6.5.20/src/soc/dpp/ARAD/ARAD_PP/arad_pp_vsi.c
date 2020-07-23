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

#define _ERR_MSG_MODULE_NAME BSL_SOC_VSI
#include <soc/mem.h>




#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vsi.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#define ARAD_PP_VSI_FID_PROFILE_ID_MAX                           6
#define ARAD_PP_VSI_PROFILE_ID_MAX                               15
#define ARAD_PP_HIGH_VSI_PROFILE_ID_MAX                          3
#define ARAD_PP_VSI_MAC_LEARN_PROFILE_ID_MAX                     7
#define ARAD_PP_VSI_EGRESS_PROFILE_INDEX_MAX                     3
#define ARAD_PP_VSI_EGRESS_PROFILE_INDEX_MIN                     1
#define ARAD_PP_VSI_PROFILE_PMF_LSB                              2
#define ARAD_PP_VSI_PROFILE_PMF_MASK                             0xc




          









CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_vsi[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_SYS_TO_LOCAL_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_VSI_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_VSI_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_VSI_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_VSI_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_VSI_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_VSI_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_VSI_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_VSI_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_GET_ERRS_PTR),
  

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_vsi[] =
{
  
  {
    ARAD_PP_VSI_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'default_forward_profile' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR",
    "The parameter 'stp_topology_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'fid_profile_id' is out of range. \n\r "
    "The range is: 0 - 6/ or SOC_PPC_VSI_FID_IS_VSID.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_MAC_LEARN_PROFILE_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_MAC_LEARN_PROFILE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'mac_learn_profile_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_ORIENTATION_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_ORIENTATION_OUT_OF_RANGE_ERR",
    "The parameter 'orientation' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_DA_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_DA_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'da_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_DA_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  {
    ARAD_PP_VSI_TRAP_CODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'action_profile.trap_code' is out of range. \n\r "
    "The range is: SOC_PPC_TRAP_CODE_UNKNOWN_DA_0 - SOC_PPC_TRAP_CODE_UNKNOWN_DA_7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_OUT_OF_RANGE_ERR",
    "'vsi' is out of range. \n\r "
    "The range is: 1 to 16K-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





uint32
  arad_pp_vsi_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32 value = 0;
  uint32 res;
  uint32 shift = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {

      

      if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "high_vsi_fp", 0))
      {
          shift = ARAD_PP_VSI_PROFILE_PMF_LSB;
      }
      soc_reg_field_set(unit, IHP_HIGH_VSI_PROFILEr, &value, HIGH_VSI_PROFILE_0f, 0 << shift);
      soc_reg_field_set(unit, IHP_HIGH_VSI_PROFILEr, &value, HIGH_VSI_PROFILE_1f, 1 << shift);
      soc_reg_field_set(unit, IHP_HIGH_VSI_PROFILEr, &value, HIGH_VSI_PROFILE_2f, 2 << shift);
      soc_reg_field_set(unit, IHP_HIGH_VSI_PROFILEr, &value, HIGH_VSI_PROFILE_3f, 3 << shift);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, WRITE_IHP_HIGH_VSI_PROFILEr(unit, value));
  }

  
  if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mtu_advanced_mode", 0)) 
  {
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1050, exit, READ_EPNI_LINK_FILTER_ENABLEr(unit, REG_PORT_ANY, &value));
      soc_reg_field_set(unit, EPNI_LINK_FILTER_ENABLEr, &value, MTU_CHECK_ENABLEf, 0x3E);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1060, exit, WRITE_EPNI_LINK_FILTER_ENABLEr(unit, REG_PORT_ANY, value));
  }
  else if (SOC_IS_ARADPLUS_AND_BELOW(unit))
  {
      uint32 entry_data[2];
      uint32 entry_index, vsi_profile_data_entry, vsi_profile, vsi_ndx;
      
      entry_index = 0;
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 27, exit, READ_EGQ_VSI_PROFILEm(unit, MEM_BLOCK_ANY, entry_index, entry_data));
      soc_EGQ_VSI_PROFILEm_field_get(unit, entry_data, VSI_PROFILE_DATAf, &vsi_profile_data_entry);

      
      for (vsi_ndx = 1; vsi_ndx <=3; vsi_ndx++)
      {
          vsi_profile = vsi_ndx;
          SHR_BITCOPY_RANGE(&vsi_profile_data_entry, ((vsi_ndx%16)*2), &vsi_profile, 0, 2);
      }
      soc_EGQ_VSI_PROFILEm_field_set(unit, entry_data, VSI_PROFILE_DATAf, &vsi_profile_data_entry);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 28, exit, WRITE_EGQ_VSI_PROFILEm(unit, MEM_BLOCK_ANY, entry_index, entry_data));      
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_init_unsafe()", 0, 0);
}



uint32
  arad_pp_vsi_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PPC_VSI_INFO                            *vsi_info
  )
{
  uint32  fid_val;
  uint32  res = SOC_SAND_OK;
  uint32  frwrd_dest;
  uint32  temp;
  uint32  entry_data[2];
  uint32 entry_index, vsi_profile_data_entry, vsi_profile;
  uint32 pmf_shift;
  ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_DATA
    mact_fid_profile_db_tbl_data;
  ARAD_PP_IHP_FID_CLASS_2_FID_TBL_DATA
    fid_class_2_fid_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_VSI_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  

  res = arad_pp_fwd_decision_in_buffer_build(
    unit,
    ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
    &vsi_info->default_forwarding,
    &frwrd_dest,
    &temp 
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  if (SOC_IS_QAX(unit) && !SOC_IS_QUX(unit)) {
      res = arad_pp_ihp_vsi_my_mac_tbl_set_unsafe(unit, vsi_ndx, vsi_info->enable_my_mac);
      SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);
  }

  if (vsi_ndx < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers)) {

    
    ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_DATA
      vsi_low_cfg_1_data;
    ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_DATA
      vsi_low_cfg_2_data;

    ARAD_CLEAR(&vsi_low_cfg_1_data, ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_DATA, 1);
    ARAD_CLEAR(&vsi_low_cfg_2_data, ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_DATA, 1);    

    res = arad_pp_ihp_vsi_low_cfg_1_tbl_get_unsafe(unit, vsi_ndx, &vsi_low_cfg_1_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

    res = arad_pp_ihp_vsi_low_cfg_2_tbl_get_unsafe(unit, vsi_ndx, &vsi_low_cfg_2_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    
    if (!SOC_IS_QAX(unit) || SOC_IS_QUX(unit)) {
        vsi_low_cfg_1_data.my_mac_valid = SOC_SAND_NUM2BOOL(vsi_info->enable_my_mac);
    }
    vsi_low_cfg_2_data.da_not_found_destination = (vsi_info->clear_on_destroy) ? 0 : frwrd_dest;
    vsi_low_cfg_2_data.fid_class = (vsi_info->fid_profile_id != SOC_PPC_VSI_FID_IS_VSID) ?
                                    vsi_info->fid_profile_id : 7; 

    vsi_low_cfg_1_data.topology_id = vsi_info->stp_topology_id;
    vsi_low_cfg_2_data.profile = vsi_info->profile_ingress; 

    res = arad_pp_ihp_vsi_low_cfg_1_tbl_set_unsafe(unit, vsi_ndx, &vsi_low_cfg_1_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = arad_pp_ihp_vsi_low_cfg_2_tbl_set_unsafe(unit, vsi_ndx, &vsi_low_cfg_2_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    
    if (vsi_info->fid_profile_id == SOC_PPC_VSI_FID_IS_VSID)
    {
      fid_val = vsi_ndx;
    }
    else
    {
      res = arad_pp_ihp_fid_class_2_fid_tbl_get_unsafe(
              unit,
              vsi_info->fid_profile_id,
              &fid_class_2_fid_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);
      fid_val = fid_class_2_fid_tbl_data.fid;
    }
    
    
    
    if (SOC_IS_JERICHO(unit) ||
        !soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mtu_advanced_mode", 0)
        || (vsi_ndx < 1) || (vsi_ndx > 3)) 
    {
        entry_index = vsi_ndx >> 4; 
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 27, exit, READ_EGQ_VSI_PROFILEm(unit, MEM_BLOCK_ANY, entry_index, entry_data));
        soc_EGQ_VSI_PROFILEm_field_get(unit, entry_data, VSI_PROFILE_DATAf, &vsi_profile_data_entry);

        
        vsi_profile = vsi_info->profile_egress & 0x3;
        SHR_BITCOPY_RANGE(&vsi_profile_data_entry, ((vsi_ndx%16)*2), &vsi_profile, 0, 2);
        soc_EGQ_VSI_PROFILEm_field_set(unit, entry_data, VSI_PROFILE_DATAf, &vsi_profile_data_entry);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 28, exit, WRITE_EGQ_VSI_PROFILEm(unit, MEM_BLOCK_ANY, entry_index, entry_data));
    }

  } else {

    
    ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_DATA
      vsi_high_da_not_found_dest_data;
    ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_DATA
      vsi_high_profile_data;
    ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_DATA
      vsi_high_my_mac_data;
    const SOC_PPC_VSI_ID rel_index = vsi_ndx - SOC_DPP_DEFS_GET(unit, nof_vsi_lowers);

    if (soc_mem_is_valid(unit, IHP_VSI_HIGH_CFGm)) {
        
        res = arad_pp_ihp_vsi_high_info_tbl_set_unsafe(unit, rel_index, frwrd_dest, vsi_info->profile_ingress);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }else{
        if (!SOC_IS_QUX(unit)) {
            pmf_shift = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "high_vsi_fp", 0)) ? ARAD_PP_VSI_PROFILE_PMF_LSB : 0;

            vsi_high_da_not_found_dest_data.da_not_found_destination = frwrd_dest;
            res = arad_pp_ihp_vsi_high_da_not_found_destination_tbl_set_unsafe(unit, rel_index, &vsi_high_da_not_found_dest_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            res = arad_pp_ihp_vsi_high_profile_tbl_get_unsafe(unit, rel_index / 8, &vsi_high_profile_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
            vsi_high_profile_data.index[rel_index % 8] = (vsi_info->profile_ingress >> pmf_shift);
            res = arad_pp_ihp_vsi_high_profile_tbl_set_unsafe(unit, rel_index / 8, &vsi_high_profile_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

            res = arad_pp_ihp_vsi_high_my_mac_tbl_get_unsafe(unit, rel_index / 8, &vsi_high_my_mac_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);
            vsi_high_my_mac_data.valid[rel_index % 8] = vsi_info->enable_my_mac;
            res = arad_pp_ihp_vsi_high_my_mac_tbl_set_unsafe(unit, rel_index / 8, &vsi_high_my_mac_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);
        }
    }        

    fid_val = vsi_ndx;

  }

  res = arad_pp_ihp_mact_fid_profile_db_tbl_get_unsafe(unit, fid_val / 8, &mact_fid_profile_db_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  mact_fid_profile_db_tbl_data.profile_pointer[fid_val % 8] = vsi_info->mac_learn_profile_id;
  res = arad_pp_ihp_mact_fid_profile_db_tbl_set_unsafe(unit, fid_val / 8, &mact_fid_profile_db_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_info_set_unsafe()", vsi_ndx, 0);
}

uint32
  arad_pp_vsi_info_set_verify(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID    vsi_ndx,
    SOC_SAND_IN  SOC_PPC_VSI_INFO  *vsi_info
  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_VSI_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_ndx, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_VSI_INFO_verify(unit, vsi_info, vsi_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_info_set_verify()", vsi_ndx, 0);
}

uint32
  arad_pp_vsi_info_get_verify(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID    vsi_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_VSI_INFO_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_ndx, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_info_get_verify()", vsi_ndx, 0);
}


uint32
  arad_pp_vsi_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_PPC_VSI_INFO                            *vsi_info
  )
{
  uint32  fid_val;
  uint32  res = SOC_SAND_OK;
  uint32  frwrd_dest = 0;
  uint32  temp = 0;
  uint32 whole_entry_data[2];
  uint32 pmf_shift;
  uint32 vsi_profile_data, vsi_profile;
  int entry_index;


  ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_DATA
    mact_fid_profile_db_tbl_data;
  ARAD_PP_IHP_FID_CLASS_2_FID_TBL_DATA
    fid_class_2_fid_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_VSI_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);
  SOC_PPC_VSI_INFO_clear(vsi_info);

  if (SOC_IS_QAX(unit) && !SOC_IS_QUX(unit)) {
      res = arad_pp_ihp_vsi_my_mac_tbl_get_unsafe(unit, vsi_ndx, &(vsi_info->enable_my_mac));
      SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);
  }

  if (vsi_ndx < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers)) {

    
    ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_DATA
      vsi_low_cfg_1_data;
    ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_DATA
      vsi_low_cfg_2_data;

    res = arad_pp_ihp_vsi_low_cfg_1_tbl_get_unsafe(unit, vsi_ndx, &vsi_low_cfg_1_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = arad_pp_ihp_vsi_low_cfg_2_tbl_get_unsafe(unit, vsi_ndx, &vsi_low_cfg_2_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
    if (!SOC_IS_QAX(unit) || SOC_IS_QUX(unit)) {
        vsi_info->enable_my_mac = SOC_SAND_NUM2BOOL(vsi_low_cfg_1_data.my_mac_valid);
    }
    vsi_info->stp_topology_id = vsi_low_cfg_1_data.topology_id;
    vsi_info->profile_ingress = vsi_low_cfg_2_data.profile;
    frwrd_dest = vsi_low_cfg_2_data.da_not_found_destination;

    
    if (vsi_low_cfg_2_data.fid_class == 7)
    {
      vsi_info->fid_profile_id = SOC_PPC_VSI_FID_IS_VSID;
      fid_val = vsi_ndx;
    }
    else
    {
      vsi_info->fid_profile_id = vsi_low_cfg_2_data.fid_class;
      res = arad_pp_ihp_fid_class_2_fid_tbl_get_unsafe( unit, vsi_info->fid_profile_id, &fid_class_2_fid_tbl_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
      fid_val = fid_class_2_fid_tbl_data.fid;
    }
    
    
    entry_index = vsi_ndx >> 4; 
    res = READ_EGQ_VSI_PROFILEm(unit, MEM_BLOCK_ANY, entry_index, whole_entry_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);
    soc_EGQ_VSI_PROFILEm_field_get(unit, whole_entry_data, VSI_PROFILE_DATAf, &vsi_profile_data);
    
    vsi_profile = 0;
    SHR_BITCOPY_RANGE(&vsi_profile, 0, &vsi_profile_data, (vsi_ndx%16)*2, 2);
    vsi_info->profile_egress = (vsi_info->profile_egress & (0xFFFFFFFC)) | vsi_profile; 

  } else {

    
    ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_DATA
      vsi_high_da_not_found_dest_data;
    ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_DATA
      vsi_high_profile_data;
    ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_DATA
      vsi_high_my_mac_data;
    const SOC_PPC_VSI_ID rel_index = vsi_ndx - SOC_DPP_DEFS_GET(unit, nof_vsi_lowers);

    if (soc_mem_is_valid(unit, IHP_VSI_HIGH_CFGm)) {
        
        res = arad_pp_ihp_vsi_high_info_tbl_get_unsafe(unit, rel_index, &frwrd_dest, &(vsi_info->profile_ingress));
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }else{
        if (!SOC_IS_QUX(unit)) {
            pmf_shift = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "high_vsi_fp", 0)) ? ARAD_PP_VSI_PROFILE_PMF_LSB : 0;

            res = arad_pp_ihp_vsi_high_da_not_found_destination_tbl_get_unsafe(unit, rel_index, &vsi_high_da_not_found_dest_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            frwrd_dest = vsi_high_da_not_found_dest_data.da_not_found_destination;

            res = arad_pp_ihp_vsi_high_profile_tbl_get_unsafe(unit, rel_index / 8, &vsi_high_profile_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
            vsi_info->profile_ingress = (vsi_high_profile_data.index[rel_index % 8] << pmf_shift);

            res = arad_pp_ihp_vsi_high_my_mac_tbl_get_unsafe(unit, rel_index / 8, &vsi_high_my_mac_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
            vsi_info->enable_my_mac = vsi_high_my_mac_data.valid[rel_index % 8];
        }
    }        

    vsi_info->stp_topology_id = 0;
    vsi_info->fid_profile_id = SOC_PPC_VSI_FID_IS_VSID;
    fid_val = vsi_ndx;

  }

  

  res = arad_pp_fwd_decision_in_buffer_parse(
    unit,    
    frwrd_dest,
    temp ,
    ARAD_PP_FWD_DECISION_PARSE_DEST,
    &vsi_info->default_forwarding
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihp_mact_fid_profile_db_tbl_get_unsafe(unit, fid_val / 8, &mact_fid_profile_db_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  vsi_info->mac_learn_profile_id = mact_fid_profile_db_tbl_data.profile_pointer[fid_val % 8];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_info_get_unsafe()", vsi_ndx, 0);
}


uint32
  arad_pp_vsi_egress_mtu_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                is_forwarding_mtu_filter,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_IN  uint32                               mtu_val
  )
{
  uint32  res;
  uint64 reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_EGRESS_PROFILE_SET_UNSAFE);

  if (is_forwarding_mtu_filter)
  {
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EGQ_MTUr(unit, REG_PORT_ANY, &reg_val));
      switch (vsi_profile_ndx) {
      case 1:
           soc_reg64_field32_set(unit, EGQ_MTUr, &reg_val, MTU_1f, mtu_val);
           break;
      case 2:
           soc_reg64_field32_set(unit, EGQ_MTUr, &reg_val, MTU_2f, mtu_val);
           break;
      case 3:
           soc_reg64_field32_set(unit, EGQ_MTUr, &reg_val, MTU_3f, mtu_val);
           break;
      default:
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 15, exit);
      }
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_EGQ_MTUr(unit, REG_PORT_ANY, reg_val));
  }
  else
  {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EPNI_MTUr(unit, REG_PORT_ANY, &reg_val));
      switch (vsi_profile_ndx) {
      case 1:
           soc_reg64_field32_set(unit, EPNI_MTUr, &reg_val, MTU_1f, mtu_val);
           break;
      case 2:
           soc_reg64_field32_set(unit, EPNI_MTUr, &reg_val, MTU_2f, mtu_val);
           break;
      case 3:
           soc_reg64_field32_set(unit, EPNI_MTUr, &reg_val, MTU_3f, mtu_val);
           break;
      
      
      default:
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 35, exit);
      }
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_EPNI_MTUr(unit, REG_PORT_ANY, reg_val));
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_egress_mtu_set_unsafe()", vsi_profile_ndx, 0);
}

uint32
  arad_pp_vsi_egress_mtu_set_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_IN  uint32                               mtu_val
  )
{
  uint32
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_EGRESS_PROFILE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_profile_ndx, ARAD_PP_VSI_EGRESS_PROFILE_INDEX_MAX ,ARAD_PP_VSI_EGRESS_PROFILE_INDEX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_ERR_IF_BELOW_MIN(vsi_profile_ndx, ARAD_PP_VSI_EGRESS_PROFILE_INDEX_MIN ,ARAD_PP_VSI_EGRESS_PROFILE_INDEX_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_egress_mtu_set_verify()", vsi_profile_ndx, 0);
}

uint32
  arad_pp_vsi_egress_mtu_get_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_EGRESS_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_profile_ndx, ARAD_PP_VSI_EGRESS_PROFILE_INDEX_MAX, ARAD_PP_VSI_EGRESS_PROFILE_INDEX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_ERR_IF_BELOW_MIN(vsi_profile_ndx, ARAD_PP_VSI_EGRESS_PROFILE_INDEX_MIN, ARAD_PP_VSI_EGRESS_PROFILE_INDEX_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_egress_mtu_get_verify()", vsi_profile_ndx, 0);
}


uint32
  arad_pp_vsi_egress_mtu_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                             is_forwarding_mtu_filter,
    SOC_SAND_IN  uint32                            vsi_profile_ndx,
    SOC_SAND_OUT uint32                            *mtu_val
  )
{
  uint32  res;
  uint64  reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_EGRESS_PROFILE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(mtu_val);

  if (is_forwarding_mtu_filter) 
  {
      res = READ_EGQ_MTUr(unit, REG_PORT_ANY, &reg_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

      switch (vsi_profile_ndx) {
      case 1:
           *mtu_val = soc_reg64_field32_get(unit, EGQ_MTUr, reg_val, MTU_1f);
           break;
      case 2:
           *mtu_val = soc_reg64_field32_get(unit, EGQ_MTUr, reg_val, MTU_2f);
           break;
      case 3:
           *mtu_val = soc_reg64_field32_get(unit, EGQ_MTUr, reg_val, MTU_3f);
           break;
      default:
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 15, exit);
      }
  }
  else
  {
      res = READ_EPNI_MTUr(unit, REG_PORT_ANY, &reg_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

      switch (vsi_profile_ndx) {
      case 1:
           *mtu_val = soc_reg64_field32_get(unit, EPNI_MTUr, reg_val, MTU_1f);
           break;
      case 2:
           *mtu_val = soc_reg64_field32_get(unit, EPNI_MTUr, reg_val, MTU_2f);
           break;
      case 3:
           *mtu_val = soc_reg64_field32_get(unit, EPNI_MTUr, reg_val, MTU_3f);
           break;
      default:
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 20, exit);
      }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_egress_mtu_get_unsafe()", vsi_profile_ndx, 0);
}


uint32
  arad_pp_vsi_egress_mtu_profile_get(
     SOC_SAND_IN int        unit,
     SOC_SAND_IN uint8      is_forwarding_mtu_filter,
     SOC_SAND_IN uint32     mtu_value,
     SOC_SAND_OUT uint32    *mtu_profile
  )
{
    uint32 res;
    uint32 local_mtu_profile, local_mtu_value;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(mtu_profile);

    *mtu_profile = 0;
    
    
    for (local_mtu_profile = 1; local_mtu_profile <= 3; local_mtu_profile++) 
    {
        res = arad_pp_vsi_egress_mtu_get_unsafe(unit, is_forwarding_mtu_filter, local_mtu_profile, &local_mtu_value);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        if (local_mtu_value == mtu_value) 
        {
            
            *mtu_profile = local_mtu_profile;
            break;
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_egress_mtu_profile_get_unsafe()", mtu_value, 0);
}


uint32
  arad_pp_vsi_egress_mtu_check_enable_set(
     SOC_SAND_IN int        unit,
     SOC_SAND_IN uint8      is_forwarding_mtu_filter,
     SOC_SAND_IN uint32     header_code,
     SOC_SAND_IN uint8      enable
  )
{
    uint32 res;
    uint32 reg_val, fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (header_code >= bcmForwardingTypeCount) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
    }

    
    if (is_forwarding_mtu_filter) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EGQ_MTU_CHECK_ENABLEr(unit, REG_PORT_ANY, &reg_val));
        fld_val = soc_reg_field_get(unit, EGQ_MTU_CHECK_ENABLEr, reg_val, MTU_CHECK_ENABLEf);
    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EPNI_LINK_FILTER_ENABLEr(unit, REG_PORT_ANY, &reg_val));
        fld_val = soc_reg_field_get(unit, EPNI_LINK_FILTER_ENABLEr, reg_val, MTU_CHECK_ENABLEf);
    }

    
    if (enable) {
        fld_val |= (1 << header_code);
    }
    else {
        fld_val &= ~(1 << header_code);
    }

    if (is_forwarding_mtu_filter) { 
        soc_reg_field_set(unit, EGQ_MTU_CHECK_ENABLEr, &reg_val, MTU_CHECK_ENABLEf, fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_EGQ_MTU_CHECK_ENABLEr(unit, REG_PORT_ANY, reg_val));
    } else {
        soc_reg_field_set(unit, EPNI_LINK_FILTER_ENABLEr, &reg_val, MTU_CHECK_ENABLEf, fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_EPNI_LINK_FILTER_ENABLEr(unit, REG_PORT_ANY, reg_val));
    }
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_egress_mtu_check_enable_set()", header_code, 0);
}


uint32
  arad_pp_vsi_egress_mtu_check_enable_get(
     SOC_SAND_IN  int        unit,
     SOC_SAND_IN  uint8      is_forwarding_mtu_filter,
     SOC_SAND_IN  uint32     header_code,
     SOC_SAND_OUT uint8      *enable
  )
{
    uint32 res;
    uint32 reg_val, fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(enable);

    
    if (is_forwarding_mtu_filter) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EGQ_MTU_CHECK_ENABLEr(unit, REG_PORT_ANY, &reg_val));
        fld_val = soc_reg_field_get(unit, EGQ_MTU_CHECK_ENABLEr, reg_val, MTU_CHECK_ENABLEf);
    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EPNI_LINK_FILTER_ENABLEr(unit, REG_PORT_ANY, &reg_val));
        fld_val = soc_reg_field_get(unit, EPNI_LINK_FILTER_ENABLEr, reg_val, MTU_CHECK_ENABLEf);
    }

    
    *enable = (fld_val & (1 << header_code));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_egress_mtu_check_enable_get()", header_code, 0);
}


uint32
  arad_pp_vsi_l2cp_trap_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_HANDLE_TYPE               handle_type
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    index_ingress, index_egress;
  soc_reg_above_64_val_t  reg_above_64;
  uint32 is_tunnel=0;
  uint32 is_peer=0;
  uint32 is_drop=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(l2cp_key);

  index_ingress = SOC_PPC_VSI_L2CP_KEY_ENTRY_OFFSET(l2cp_key->l2cp_profile_ingress, l2cp_key->da_mac_address_lsb);
  index_egress = SOC_PPC_VSI_L2CP_KEY_ENTRY_OFFSET(l2cp_key->l2cp_profile_egress, l2cp_key->da_mac_address_lsb);

  switch (handle_type) {
    case SOC_PPC_VSI_L2CP_HANDLE_TYPE_TUNNEL:
        is_tunnel = 1;
        is_peer = 0;
        is_drop = 0;
        break;
    case SOC_PPC_VSI_L2CP_HANDLE_TYPE_PEER:
        is_tunnel = 0;
        is_peer = 1;
        is_drop = 0;
        break;
    case SOC_PPC_VSI_L2CP_HANDLE_TYPE_DROP:
        is_tunnel = 0;
        is_peer = 0;
        is_drop = 1;
        break;
    case SOC_PPC_VSI_L2CP_HANDLE_TYPE_NORMAL:
        is_tunnel = 0;
        is_peer = 0;
        is_drop = 0;
        break;
    default:
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }

  
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MEF_L_2_CP_TRANSPARENT_BITMAPr(unit, REG_PORT_ANY, reg_above_64));
  SHR_BITCOPY_RANGE(reg_above_64, index_ingress, &is_tunnel, 0, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_MEF_L_2_CP_TRANSPARENT_BITMAPr(unit, REG_PORT_ANY, reg_above_64));
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MEF_L_2_CP_PEER_BITMAPr(unit, 0, reg_above_64));
  SHR_BITCOPY_RANGE(reg_above_64, index_ingress, &is_peer, 0, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IHP_MEF_L_2_CP_PEER_BITMAPr(unit, SOC_CORE_ALL, reg_above_64));
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IHP_MEF_L_2_CP_DROP_BITMAPr(unit, 0, reg_above_64));
  SHR_BITCOPY_RANGE(reg_above_64, index_ingress, &is_drop, 0, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IHP_MEF_L_2_CP_DROP_BITMAPr(unit, SOC_CORE_ALL, reg_above_64));

  
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_EPNI_MEF_L_2_CP_TRANSPARANT_BITMAPr(unit, REG_PORT_ANY, reg_above_64));
  SHR_BITCOPY_RANGE(reg_above_64, index_egress, &is_tunnel, 0, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_EPNI_MEF_L_2_CP_TRANSPARANT_BITMAPr(unit, REG_PORT_ANY, reg_above_64));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_l2cp_trap_set_unsafe()", 0, 0);
}


uint32
  arad_pp_vsi_l2cp_trap_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_OUT SOC_PPC_VSI_L2CP_HANDLE_TYPE               *handle_type
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    index_ingress;
  soc_reg_above_64_val_t  reg_above_64;
  uint32 is_tunnel=0;
  uint32 is_peer=0;
  uint32 is_drop=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(l2cp_key);

  index_ingress = SOC_PPC_VSI_L2CP_KEY_ENTRY_OFFSET(l2cp_key->l2cp_profile_ingress, l2cp_key->da_mac_address_lsb);

  
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MEF_L_2_CP_TRANSPARENT_BITMAPr(unit, REG_PORT_ANY, reg_above_64));
  SHR_BITCOPY_RANGE(&is_tunnel, 0, reg_above_64, index_ingress, 1);
  if (is_tunnel) {
	  *handle_type = SOC_PPC_VSI_L2CP_HANDLE_TYPE_TUNNEL;
  }
  else {
	  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
	  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MEF_L_2_CP_PEER_BITMAPr(unit, 0, reg_above_64));
	  SHR_BITCOPY_RANGE(&is_peer, 0, reg_above_64, index_ingress, 1);
	  if (is_peer) {
		  *handle_type = SOC_PPC_VSI_L2CP_HANDLE_TYPE_PEER;
	  }
	  else {
		  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
		  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MEF_L_2_CP_DROP_BITMAPr(unit, 0, reg_above_64));
		  SHR_BITCOPY_RANGE(&is_drop, 0, reg_above_64, index_ingress, 1);
		  if (is_drop) {
			  *handle_type = SOC_PPC_VSI_L2CP_HANDLE_TYPE_DROP;
		  }
		  else {
			  *handle_type = SOC_PPC_VSI_L2CP_HANDLE_TYPE_NORMAL;
		  }
	  }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_l2cp_trap_get_unsafe()", 0, 0);
}

uint32
  arad_pp_vsi_l2cp_trap_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_HANDLE_TYPE               handle_type
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(handle_type, SOC_PPC_VSI_L2CP_HANDLE_TYPES-1, 0, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_l2cp_trap_set_verify()", 0, 0);
}

uint32
  arad_pp_vsi_l2cp_trap_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_KEY                       *l2cp_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_l2cp_trap_get_verify()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_vsi_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_vsi;
}


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_vsi_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_vsi;
}

uint32
  SOC_PPC_VSI_INFO_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PPC_VSI_INFO *info,
    SOC_SAND_IN  SOC_PPC_VSI_ID    vsi_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mac_learn_profile_id, ARAD_PP_VSI_MAC_LEARN_PROFILE_ID_MAX, ARAD_PP_VSI_MAC_LEARN_PROFILE_ID_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  
  

  if (vsi_ndx < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers)) {
    
    if (info->fid_profile_id != SOC_PPC_VSI_FID_IS_VSID)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->fid_profile_id, ARAD_PP_VSI_FID_PROFILE_ID_MAX, ARAD_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR, 20, exit);
    }
    SOC_SAND_ERR_IF_ABOVE_NOF(info->stp_topology_id, SOC_DPP_DEFS_GET(unit, nof_topology_ids), ARAD_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR, 22, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(info->profile_ingress, ARAD_PP_VSI_PROFILE_ID_MAX, ARAD_PP_VSI_PROFILE_ID_OUT_OF_RANGE_ERR, 24, exit);
  } else {
    
    if (info->fid_profile_id != SOC_PPC_VSI_FID_IS_VSID) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR, 30, exit);
    }
    if (info->stp_topology_id) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR, 32, exit);
    }
    
    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "high_vsi_fp", 0))
    {
        SOC_SAND_ERR_IF_NOT_EQUALS_VALUE( (info->profile_ingress & (~ARAD_PP_VSI_PROFILE_PMF_MASK)), 0, ARAD_PP_VSI_PROFILE_ID_OUT_OF_RANGE_ERR, 44, exit);
    }
    else
    {
        SOC_SAND_ERR_IF_ABOVE_MAX(info->profile_ingress, ARAD_PP_HIGH_VSI_PROFILE_ID_MAX, ARAD_PP_VSI_PROFILE_ID_OUT_OF_RANGE_ERR, 34, exit);
    }


  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_VSI_INFO_verify()",0,0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
