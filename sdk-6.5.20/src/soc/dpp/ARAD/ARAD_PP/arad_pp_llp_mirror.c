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

#define _ERR_MSG_MODULE_NAME BSL_SOC_LLP



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>





#define ARAD_PP_LLP_MIRROR_MIRROR_PROFILE_MAX                    DPP_MIRROR_ACTION_NDX_MAX
#define ARAD_PP_LLP_MIRROR_TAGGED_DFLT_MAX                       ARAD_PP_LLP_MIRROR_MIRROR_PROFILE_MAX
#define ARAD_PP_LLP_MIRROR_UNTAGGED_DFLT_MAX                     ARAD_PP_LLP_MIRROR_MIRROR_PROFILE_MAX

#define ARAD_PP_LLP_MIRROR_TAGGED_NDX   (6)
#define ARAD_PP_LLP_MIRROR_UNTAGGED_NDX (7)

#define ARAD_PP_LLP_MIRROR_PROFILE_INVALID (0)













CONST SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_llp_mirror[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_VLAN_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_VLAN_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_VLAN_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_VLAN_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_VLAN_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_VLAN_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_VLAN_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_VLAN_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_VLAN_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_VLAN_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_VLAN_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_VLAN_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_DFLT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_DFLT_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_DFLT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_DFLT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_DFLT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_DFLT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_DFLT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_PORT_DFLT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_MIRROR_GET_ERRS_PTR),
  

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_llp_mirror[] =
{
  
  {
    ARAD_PP_LLP_MIRROR_MIRROR_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_MIRROR_MIRROR_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'mirror_profile' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_MIRROR_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_MIRROR_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_MIRROR_TAGGED_DFLT_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_MIRROR_TAGGED_DFLT_OUT_OF_RANGE_ERR",
    "The parameter 'tagged_dflt' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_MIRROR_UNTAGGED_DFLT_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_MIRROR_UNTAGGED_DFLT_OUT_OF_RANGE_ERR",
    "The parameter 'untagged_dflt' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

    {
      ARAD_PP_VID_NDX_OUT_OF_RANGE_NO_ZERO_ERR,
      "ARAD_PP_VID_NDX_OUT_OF_RANGE_NO_ZERO_ERR",
      "The parameter 'vid_ndx' is out of range. \n\r "
      "The range is: 1 - 4095.\n\r ",
      SOC_SAND_SVR_ERR,
      FALSE
    },

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





uint32
  arad_pp_llp_mirror_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_init_unsafe()", 0, 0);
}

STATIC void
  arad_pp_llp_mirror_port_vlan_reg_fld_desc_get(
	  SOC_SAND_IN  uint32		vid_ndx,
	  SOC_SAND_OUT soc_reg_t	*reg_desc,
	  SOC_SAND_OUT soc_field_t	*fld_desc
  )
{
   switch (vid_ndx) {
   case 0:
	   *reg_desc = IHP_LL_MIRROR_VID_01r;
	   *fld_desc = LL_MIRROR_VID_0f;
	   break;
   case 1:
	    *reg_desc = IHP_LL_MIRROR_VID_01r;
		*fld_desc = LL_MIRROR_VID_1f;
		break;
	case 2:
	    *reg_desc = IHP_LL_MIRROR_VID_23r;
		*fld_desc = LL_MIRROR_VID_2f;
		break;
	case 3:
	    *reg_desc = IHP_LL_MIRROR_VID_23r;
		*fld_desc = LL_MIRROR_VID_3f;
		break;
	case 4:
	    *reg_desc = IHP_LL_MIRROR_VID_45r;
		*fld_desc = LL_MIRROR_VID_4f;
		break;
	case 5:
	    *reg_desc = IHP_LL_MIRROR_VID_45r;
		*fld_desc = LL_MIRROR_VID_5f;
		break;
   default:
	    *reg_desc = -1;
		*fld_desc = -1;
		break;
   }
}


uint32
  arad_pp_llp_mirror_port_vlan_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                  local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                               vid_ndx,
    SOC_SAND_IN  uint32                                      mirror_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK,
    ll_mirror_profile_tbl_offset,
    internal_vid_ndx;
  ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ll_mirror_profile_tbl_data;
  uint8
    first_appear;
  SOC_SAND_SUCCESS_FAILURE
    multi_set_success;
  soc_reg_t
	  reg_desc;
  soc_field_t
	  fld_desc;
  



  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_VLAN_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);

  *success = SOC_SAND_SUCCESS;




  
  res = arad_sw_db_multiset_add(
    unit,
    ARAD_SW_DB_CORE_ANY,
	ARAD_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE,
    &vid_ndx,
    &internal_vid_ndx,
    &first_appear,
    &multi_set_success
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (multi_set_success != SOC_SAND_SUCCESS)
  {
    *success = multi_set_success;
    goto exit;
  }

  if (first_appear)
  {
	  
	  arad_pp_llp_mirror_port_vlan_reg_fld_desc_get(internal_vid_ndx, &reg_desc, &fld_desc);

	  
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, reg_desc, REG_PORT_ANY, 0, fld_desc,  vid_ndx));
  }

  ll_mirror_profile_tbl_offset =
    ARAD_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(internal_vid_ndx, local_port_ndx);

  ll_mirror_profile_tbl_data.ll_mirror_profile = mirror_profile;

  res = arad_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
    unit,
    core_id,
    ll_mirror_profile_tbl_offset,
    &ll_mirror_profile_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  res = arad_pp_sw_db_llp_mirror_port_vlan_is_exist_set(
          unit,
          core_id,
          local_port_ndx,
          internal_vid_ndx,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_vlan_add_unsafe()", local_port_ndx, vid_ndx);
}

uint32
  arad_pp_llp_mirror_port_vlan_add_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                      mirror_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_VLAN_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(mirror_profile, ARAD_PP_LLP_MIRROR_MIRROR_PROFILE_MAX, ARAD_PP_LLP_MIRROR_MIRROR_PROFILE_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    vid_ndx,
    1,
    SOC_SAND_PP_VLAN_ID_MAX,
    ARAD_PP_VID_NDX_OUT_OF_RANGE_NO_ZERO_ERR,
    20,
    exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(mirror_profile, ARAD_PP_LLP_MIRROR_MIRROR_PROFILE_MAX, ARAD_PP_LLP_MIRROR_MIRROR_PROFILE_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_vlan_add_verify()", local_port_ndx, vid_ndx);
}


uint32
  arad_pp_llp_mirror_port_vlan_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  uint32
    res = SOC_SAND_OK,
    ll_mirror_profile_tbl_offset,
    internal_vid_ndx,
    ref_count;
  ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ll_mirror_profile_tbl_data;
  uint8
    last_appear;
  soc_reg_t
	  reg_desc;
  soc_field_t
	  fld_desc;
 


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_VLAN_REMOVE_UNSAFE);



  
  res = arad_sw_db_multiset_lookup(
    unit,
	ARAD_SW_DB_CORE_ANY,
    ARAD_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE,
    &vid_ndx,
    &internal_vid_ndx,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ref_count > 0)
  {
    

    ll_mirror_profile_tbl_offset =
      ARAD_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(internal_vid_ndx, local_port_ndx);

    res = arad_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
      unit,
      core_id,
      ll_mirror_profile_tbl_offset,
      &ll_mirror_profile_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (ll_mirror_profile_tbl_data.ll_mirror_profile != ARAD_PP_LLP_MIRROR_PROFILE_INVALID)
    {
      
      ll_mirror_profile_tbl_data.ll_mirror_profile =
        ARAD_PP_LLP_MIRROR_PROFILE_INVALID;

      res = arad_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
        unit,
        core_id,
        ll_mirror_profile_tbl_offset,
        &ll_mirror_profile_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      
      res = arad_sw_db_multiset_remove_by_index(
        unit,
		ARAD_SW_DB_CORE_ANY,
        ARAD_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE,
        internal_vid_ndx,
        &last_appear
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      
      res = arad_pp_sw_db_llp_mirror_port_vlan_is_exist_set(
              unit,
              core_id,
              local_port_ndx,
              internal_vid_ndx,
              FALSE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      
      ll_mirror_profile_tbl_offset =
        ARAD_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(ARAD_PP_LLP_MIRROR_TAGGED_NDX, local_port_ndx);

      res = arad_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
        unit,
        core_id,
        ll_mirror_profile_tbl_offset,
        &ll_mirror_profile_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      ll_mirror_profile_tbl_offset =
        ARAD_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(internal_vid_ndx, local_port_ndx);

      res = arad_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
        unit,
        core_id,
        ll_mirror_profile_tbl_offset,
        &ll_mirror_profile_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (last_appear)
      {
		  
		  arad_pp_llp_mirror_port_vlan_reg_fld_desc_get(internal_vid_ndx, &reg_desc, &fld_desc);

		  
		  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, reg_desc, REG_PORT_ANY, 0, fld_desc,  0));
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_vlan_remove_unsafe()", local_port_ndx, vid_ndx);
}

uint32
  arad_pp_llp_mirror_port_vlan_remove_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_VLAN_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    vid_ndx,
    1,
    SOC_SAND_PP_VLAN_ID_MAX,
    ARAD_PP_VID_NDX_OUT_OF_RANGE_NO_ZERO_ERR,
    20,
    exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_vlan_remove_verify()", local_port_ndx, vid_ndx);
}


uint32
  arad_pp_llp_mirror_port_vlan_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint32                                      *mirror_profile
  )
{
  uint32
    res = SOC_SAND_OK,
    ll_mirror_profile_tbl_offset,
    internal_vid_ndx,
    ref_count;
  ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ll_mirror_profile_tbl_data;
  uint8
    is_port_vlan_exists;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_VLAN_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mirror_profile);

 

  *mirror_profile = 0;

  
  res = arad_sw_db_multiset_lookup(
    unit,
	ARAD_SW_DB_CORE_ANY,
    ARAD_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE,
    &vid_ndx,
    &internal_vid_ndx,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (ref_count != 0)
  {
    res = arad_pp_sw_db_llp_mirror_port_vlan_is_exist_get(
            unit,
            core_id,
            local_port_ndx,
            internal_vid_ndx,
            &is_port_vlan_exists
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (is_port_vlan_exists == TRUE)
    {
      
      ll_mirror_profile_tbl_offset =
        ARAD_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(internal_vid_ndx, local_port_ndx);

      res = arad_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
              unit,
              core_id,
              ll_mirror_profile_tbl_offset,
              &ll_mirror_profile_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      *mirror_profile = ll_mirror_profile_tbl_data.ll_mirror_profile;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_vlan_get_unsafe()", local_port_ndx, vid_ndx);
}

uint32
  arad_pp_llp_mirror_port_vlan_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_VLAN_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    vid_ndx,
    1,
    SOC_SAND_PP_VLAN_ID_MAX,
    ARAD_PP_VID_NDX_OUT_OF_RANGE_NO_ZERO_ERR,
    20,
    exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_vlan_get_verify()", local_port_ndx, vid_ndx);
}


uint32
  arad_pp_llp_mirror_port_dflt_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                    local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO               *dflt_mirroring_info
  )
{
  uint32
    res = SOC_SAND_OK,
    ll_mirror_profile_tbl_offset,
    internal_vid_ndx;
  ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ll_mirror_profile_tbl_data;
  uint8
    is_port_vlan_exist;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_DFLT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dflt_mirroring_info);
 
  if (dflt_mirroring_info->is_tagged_dflt) {
      
      ll_mirror_profile_tbl_offset =
        ARAD_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(ARAD_PP_LLP_MIRROR_TAGGED_NDX, local_port_ndx);

      ll_mirror_profile_tbl_data.ll_mirror_profile = dflt_mirroring_info->tagged_dflt;

      res = arad_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
              unit,
              core_id,
              ll_mirror_profile_tbl_offset,
              &ll_mirror_profile_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  
  if (dflt_mirroring_info->is_untagged_only) {
      
      ll_mirror_profile_tbl_offset =
        ARAD_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(ARAD_PP_LLP_MIRROR_UNTAGGED_NDX, local_port_ndx);

      ll_mirror_profile_tbl_data.ll_mirror_profile = dflt_mirroring_info->untagged_dflt;

      res = arad_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
        unit,
        core_id,
        ll_mirror_profile_tbl_offset,
        &ll_mirror_profile_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  if (dflt_mirroring_info->is_untagged_only && dflt_mirroring_info->is_tagged_dflt) {
      
      
      for (internal_vid_ndx = 0; internal_vid_ndx < ARAD_PP_LLP_MIRROR_NOF_VID_MIRROR_INDICES; internal_vid_ndx++)
      {
        if (!SOC_DPP_PP_ENABLE(unit)) 
        {
            is_port_vlan_exist = FALSE;
        } 
        else 
        {
            res = arad_pp_sw_db_llp_mirror_port_vlan_is_exist_get(
                    unit,
                    core_id,
                    local_port_ndx,
                    internal_vid_ndx,
                    &is_port_vlan_exist
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
        }
        if (is_port_vlan_exist == FALSE)
        {
          ll_mirror_profile_tbl_offset =
            ARAD_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(internal_vid_ndx, local_port_ndx);
          
          res = arad_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
            unit,
            core_id,
            ll_mirror_profile_tbl_offset,
            &ll_mirror_profile_tbl_data
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
      }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_dflt_set_unsafe()", local_port_ndx, 0);
}

uint32
  arad_pp_llp_mirror_port_dflt_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO               *dflt_mirroring_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_DFLT_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO, dflt_mirroring_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_dflt_set_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_llp_mirror_port_dflt_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_DFLT_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_dflt_get_verify()", local_port_ndx, 0);
}


uint32
  arad_pp_llp_mirror_port_dflt_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO               *dflt_mirroring_info
  )
{
  uint32
    res = SOC_SAND_OK,
    ll_mirror_profile_tbl_offset;
  ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ll_mirror_profile_tbl_data;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_MIRROR_PORT_DFLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dflt_mirroring_info);



  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_clear(dflt_mirroring_info);

  
  ll_mirror_profile_tbl_offset =
    ARAD_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(ARAD_PP_LLP_MIRROR_TAGGED_NDX, local_port_ndx);

  res = arad_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
    unit,
    core_id,
    ll_mirror_profile_tbl_offset,
    &ll_mirror_profile_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  dflt_mirroring_info->tagged_dflt = ll_mirror_profile_tbl_data.ll_mirror_profile;
  dflt_mirroring_info->is_tagged_dflt = ll_mirror_profile_tbl_data.ll_mirror_profile ? 1 : 0;

  
  ll_mirror_profile_tbl_offset =
    ARAD_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(ARAD_PP_LLP_MIRROR_UNTAGGED_NDX, local_port_ndx);

  res = arad_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
    unit,
    core_id,
    ll_mirror_profile_tbl_offset,
    &ll_mirror_profile_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  dflt_mirroring_info->untagged_dflt = ll_mirror_profile_tbl_data.ll_mirror_profile;
  dflt_mirroring_info->is_untagged_only = ll_mirror_profile_tbl_data.ll_mirror_profile ? 1 : 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_mirror_port_dflt_get_unsafe()", local_port_ndx, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_mirror_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_llp_mirror;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_mirror_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_llp_mirror;
}

uint32
  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tagged_dflt, ARAD_PP_LLP_MIRROR_TAGGED_DFLT_MAX, ARAD_PP_LLP_MIRROR_TAGGED_DFLT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->untagged_dflt, ARAD_PP_LLP_MIRROR_UNTAGGED_DFLT_MAX, ARAD_PP_LLP_MIRROR_UNTAGGED_DFLT_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


