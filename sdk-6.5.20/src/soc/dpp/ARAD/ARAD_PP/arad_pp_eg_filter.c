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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_filter.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_parse.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/PPD/ppd_api_eg_filter.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/mcm/memregs.h> 
#include <soc/cmic.h>        
#include <soc/drv.h> 

#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/cosq.h>
        

#ifdef BCM_88660_A0
#include <soc/dpp/PPD/ppd_api_lif.h>
#endif 







#define ARAD_PP_EG_PARSE_TPID_PROFILE_NDX_MAX                    (7)
#define ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_NDX_MAX        (3)
#define ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_MAX            ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_NDX_MAX + 1
#define ARAD_PP_EG_FILTER_VLAN_FORMAT_NDX_MAX                    (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)
#define ARAD_PP_EG_FILTER_ACCEPT_MAX                             (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)
#define SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_MAX                    (SOC_PPC_NOF_EG_FILTER_PVLAN_PORT_TYPES-1)
#define ARAD_PP_EG_FILTER_ORIENTATION_MAX                        (SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1)
#define ARAD_PP_EG_FILTER_FILTER_MASK_MAX                        (SOC_PPC_EG_FILTER_PORT_ENABLE_ALL)
#define ARAD_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_MAX          (3)
#ifdef BCM_88660_A0

#define ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_SINGLE_INLIF_MASK      0xFFFF

#define ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_NOF_SINGLE_INLIF_BITS  (16)
#endif 

#define ARAD_PP_TRAP_MGMT_EPNI_PMF_MIRROR_PROFILE_TABLE_INDEX_MAX  (16)





#define ARAD_PP_LLP_PVLAN_FEATURE_CHECK_IF_ENABLED(unit,fld_val)  \
  if(fld_val != 0)      \
  {                                                              \
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_PVLAN_FEATURE_DISABLED_ERR, 150, exit);   \
  }

#define ARAD_PP_LLP_SPLIT_HORIZON_FEATURE_CHECK_IF_ENABLED(unit,fld_val)  \
  if(fld_val != 1)      \
  {                                                              \
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_SPLIT_HORIZON_FEATURE_DISABLED_ERR, 150, exit);   \
  }











static const uint32 arad_pp_eg_filter_same_if_disable_reg_per_lif_profile_defaults[] = { 0xC200, 0xC200, 0xC200, 0xC200 };


#define ARAD_PP_INLIF_PROFILE_LOGICAL_INTF_SAME_FILTER  0xC202


#define ARAD_PP_INLIF_PROFILE_LOGICAL_INTF_SAME_FILTER_DEVICE_SCOPE  0xC200
#define ARAD_PP_INLIF_PROFILE_LOGICAL_INTF_SAME_FILTER_SYSTEM_SCOPE  0x0002


CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Arad_pp_procedure_desc_element_eg_filter[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_VSI_MEMBERSHIP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_VSI_MEMBERSHIP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_VSI_MEMBERSHIP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_VSI_MEMBERSHIP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_FILTER_GET_ERRS_PTR),
  



  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_eg_filter[] =
{
  
  {
    ARAD_PP_EG_FILTER_IS_MEMBER_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_FILTER_IS_MEMBER_OUT_OF_RANGE_ERR",
    "The parameter 'is_member' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port_acc_frames_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_format_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_FILTER_ACCEPT_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_FILTER_ACCEPT_OUT_OF_RANGE_ERR",
    "The parameter 'accept' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'pvlan_port_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_EG_FILTER_PVLAN_PORT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_FILTER_ORIENTATION_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_FILTER_ORIENTATION_OUT_OF_RANGE_ERR",
    "The parameter 'orientation' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_SPLIT_HORIZON_ORIENTATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_FILTER_FILTER_MASK_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_FILTER_FILTER_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'filter_mask' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'acceptable_frames_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  {
    ARAD_PP_EG_PVLAN_FEATURE_DISABLED_ERR,
    "ARAD_PP_EG_PVLAN_FEATURE_DISABLED_ERR",
    "The private VLAN feature is disabled by operation mode. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_SPLIT_HORIZON_FEATURE_DISABLED_ERR,
    "ARAD_PP_EG_SPLIT_HORIZON_FEATURE_DISABLED_ERR",
    "The split-horizon filter feature is disabled by operation mode.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_FLTER_PORT_SPLIT_DISABLE_ERR,
    "ARAD_PP_EG_FLTER_PORT_SPLIT_DISABLE_ERR",
    "The split-horizon filter cannot be disable per port.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_FLTER_PORT_MTU_DISABLE_ERR,
    "ARAD_PP_EG_FLTER_PORT_MTU_DISABLE_ERR",
    "The MTU filter cannot be disable per port.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};






STATIC uint32
  arad_pp_eg_filter_pvlan_type_to_hw_val(
    SOC_SAND_IN SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE pvlan_type
  )
{
  switch(pvlan_type)
  {
  case SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_ISOLATED:
    return 1;
  case SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_COMMUNITY:
    return 3;
  default:
  case SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS:
    return 0;
  }
}

uint32
  arad_pp_eg_filter_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    reg_val=0;
  uint32
    acceptable_frames_reg_val[4],
    res = SOC_SAND_OK;
  
  uint32 data32 = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_PCID_LITE_SKIP(unit);
  
  reg_val = 0x20A0; 
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, WRITE_EGQ_PRIVATE_VLAN_FILTERr(unit, REG_PORT_ANY, reg_val));
  
  
  if (SOC_IS_JERICHO(unit)) {
      res = arad_pp_init_orientation_maps(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      reg_val = 0x20; 
      res = WRITE_EPNI_CFG_SPLIT_HORIZON_FILTERr(unit, SOC_CORE_ALL, reg_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

  	  
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR, READ_EPNI_CFG_EES_ACTION_TRAPr(unit, SOC_CORE_ALL, &data32));
	  soc_reg_field_set(unit, EPNI_CFG_EES_ACTION_TRAPr, &data32, CFG_EES_ACTION_TRAP_MIRROR_CMDf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_EES_ACTION_TRAPr, &data32, CFG_EES_ACTION_TRAP_MIRROR_ENf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_EES_ACTION_TRAPr, &data32, CFG_EES_ACTION_TRAP_MIRROR_STRENGTHf, 1);
	  soc_reg_field_set(unit, EPNI_CFG_EES_ACTION_TRAPr, &data32, CFG_EES_ACTION_TRAP_FWD_ENf,  0);
	  soc_reg_field_set(unit, EPNI_CFG_EES_ACTION_TRAPr, &data32, CFG_EES_ACTION_TRAP_FWD_STRENGTHf,  1);
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_CFG_EES_ACTION_TRAPr(unit, SOC_CORE_ALL, data32));

      if (SOC_IS_JERICHO_PLUS(unit))
      {
          
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR, READ_EPNI_CFG_EGRESS_MEMBERSHIP_FILTERr(unit, &data32));
          soc_reg_field_set(unit, EPNI_CFG_EGRESS_MEMBERSHIP_FILTERr, &data32, CFG_EGRESS_MEMBERSHIP_TRAP_MIRROR_CMDf, 0);
          soc_reg_field_set(unit, EPNI_CFG_EGRESS_MEMBERSHIP_FILTERr, &data32, CFG_EGRESS_MEMBERSHIP_TRAP_MIRROR_ENf, 0);
          soc_reg_field_set(unit, EPNI_CFG_EGRESS_MEMBERSHIP_FILTERr, &data32, CFG_EGRESS_MEMBERSHIP_TRAP_MIRROR_STRENGTHf, 1);
          soc_reg_field_set(unit, EPNI_CFG_EGRESS_MEMBERSHIP_FILTERr, &data32, CFG_EGRESS_MEMBERSHIP_TRAP_FWD_ENf,  0);
          soc_reg_field_set(unit, EPNI_CFG_EGRESS_MEMBERSHIP_FILTERr, &data32, CFG_EGRESS_MEMBERSHIP_TRAP_FWD_STRENGTHf,  1);
          
          soc_reg_field_set(unit, EPNI_CFG_EGRESS_MEMBERSHIP_FILTERr, &data32, CFG_DEVICE_EGRESS_MEMBERSHIP_TYPEf, !(SOC_DPP_CONFIG(unit)->pp.egress_membership_mode));
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  13,  exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_CFG_EGRESS_MEMBERSHIP_FILTERr(unit, data32));
      }
      else
      {
          
          
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR, READ_EPNI_CFG_VSI_MEMBERSHIP_FILTERr(unit, SOC_CORE_ALL, &data32));
          soc_reg_field_set(unit, EPNI_CFG_VSI_MEMBERSHIP_FILTERr, &data32, CFG_VSI_MEMBERSHIP_TRAP_MIRROR_CMDf, 0);
          soc_reg_field_set(unit, EPNI_CFG_VSI_MEMBERSHIP_FILTERr, &data32, CFG_VSI_MEMBERSHIP_TRAP_MIRROR_ENf, 0);
          soc_reg_field_set(unit, EPNI_CFG_VSI_MEMBERSHIP_FILTERr, &data32, CFG_VSI_MEMBERSHIP_TRAP_MIRROR_STRENGTHf, 1);
          soc_reg_field_set(unit, EPNI_CFG_VSI_MEMBERSHIP_FILTERr, &data32, CFG_VSI_MEMBERSHIP_TRAP_FWD_ENf,  0);
          soc_reg_field_set(unit, EPNI_CFG_VSI_MEMBERSHIP_FILTERr, &data32, CFG_VSI_MEMBERSHIP_TRAP_FWD_STRENGTHf,  1);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  13,  exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_CFG_VSI_MEMBERSHIP_FILTERr(unit, SOC_CORE_ALL, data32));

          
          
          if (SOC_IS_JERICHO_B0_AND_ABOVE(unit)) {
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 13, exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_EGRESS_MEMBERSHIP_MODEr(unit, SOC_CORE_ALL, SOC_DPP_CONFIG(unit)->pp.egress_membership_mode));
          }
      }

	  
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR, READ_EPNI_CFG_STP_STATE_TRAPr(unit, SOC_CORE_ALL, &data32));
	  soc_reg_field_set(unit, EPNI_CFG_STP_STATE_TRAPr, &data32, CFG_STP_STATE_TRAP_MIRROR_CMDf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_STP_STATE_TRAPr, &data32, CFG_STP_STATE_TRAP_MIRROR_ENf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_STP_STATE_TRAPr, &data32, CFG_STP_STATE_TRAP_MIRROR_STRENGTHf,  1);
	  soc_reg_field_set(unit, EPNI_CFG_STP_STATE_TRAPr, &data32, CFG_STP_STATE_TRAP_FWD_ENf,  0);
	  soc_reg_field_set(unit, EPNI_CFG_STP_STATE_TRAPr, &data32, CFG_STP_STATE_TRAP_FWD_STRENGTHf,  1);
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_CFG_STP_STATE_TRAPr(unit, SOC_CORE_ALL, data32));

	  
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR, READ_EPNI_CFG_ACC_FRAME_TYPE_TRAPr(unit, SOC_CORE_ALL, &data32));
	  soc_reg_field_set(unit, EPNI_CFG_ACC_FRAME_TYPE_TRAPr, &data32, CFG_ACC_FRAME_TYPE_TRAP_MIRROR_CMDf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_ACC_FRAME_TYPE_TRAPr, &data32, CFG_ACC_FRAME_TYPE_TRAP_MIRROR_ENf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_ACC_FRAME_TYPE_TRAPr, &data32, CFG_ACC_FRAME_TYPE_TRAP_MIRROR_STRENGTHf,  1);
	  soc_reg_field_set(unit, EPNI_CFG_ACC_FRAME_TYPE_TRAPr, &data32, CFG_ACC_FRAME_TYPE_TRAP_FWD_ENf,  0);
	  soc_reg_field_set(unit, EPNI_CFG_ACC_FRAME_TYPE_TRAPr, &data32, CFG_ACC_FRAME_TYPE_TRAP_FWD_STRENGTHf,  1);
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  17,  exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_CFG_ACC_FRAME_TYPE_TRAPr(unit, SOC_CORE_ALL, data32));


	  
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  18,  exit, ARAD_REG_ACCESS_ERR, READ_EPNI_CFG_MTU_TRAPr(unit, SOC_CORE_ALL, &data32));
	  soc_reg_field_set(unit, EPNI_CFG_MTU_TRAPr, &data32, CFG_MTU_TRAP_MIRROR_CMDf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_MTU_TRAPr, &data32, CFG_MTU_TRAP_MIRROR_ENf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_MTU_TRAPr, &data32, CFG_MTU_TRAP_MIRROR_STRENGTHf,  1);
	  soc_reg_field_set(unit, EPNI_CFG_MTU_TRAPr, &data32, CFG_MTU_TRAP_FWD_ENf,  0);
	  soc_reg_field_set(unit, EPNI_CFG_MTU_TRAPr, &data32, CFG_MTU_TRAP_FWD_STRENGTHf,  1);
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  19,  exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_CFG_MTU_TRAPr(unit, SOC_CORE_ALL, data32));

	  
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, READ_EPNI_CFG_PROTECTION_PATH_TRAPr(unit, SOC_CORE_ALL, &data32));
	  soc_reg_field_set(unit, EPNI_CFG_PROTECTION_PATH_TRAPr, &data32, CFG_PROTECTION_PATH_TRAP_MIRROR_CMDf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_PROTECTION_PATH_TRAPr, &data32, CFG_PROTECTION_PATH_TRAP_MIRROR_ENf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_PROTECTION_PATH_TRAPr, &data32, CFG_PROTECTION_PATH_TRAP_MIRROR_STRENGTHf,  1);
	  soc_reg_field_set(unit, EPNI_CFG_PROTECTION_PATH_TRAPr, &data32, CFG_PROTECTION_PATH_TRAP_FWD_ENf,  0);
	  soc_reg_field_set(unit, EPNI_CFG_PROTECTION_PATH_TRAPr, &data32, CFG_PROTECTION_PATH_TRAP_FWD_STRENGTHf,  1);
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_CFG_PROTECTION_PATH_TRAPr(unit, SOC_CORE_ALL, data32));

	  
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR, READ_EPNI_CFG_SPLIT_HORIZON_TRAPr(unit, SOC_CORE_ALL, &data32));
	  soc_reg_field_set(unit, EPNI_CFG_SPLIT_HORIZON_TRAPr, &data32, CFG_SPLIT_HORIZON_TRAP_MIRROR_CMDf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_SPLIT_HORIZON_TRAPr, &data32, CFG_SPLIT_HORIZON_TRAP_MIRROR_ENf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_SPLIT_HORIZON_TRAPr, &data32, CFG_SPLIT_HORIZON_TRAP_MIRROR_STRENGTHf, 1);
	  soc_reg_field_set(unit, EPNI_CFG_SPLIT_HORIZON_TRAPr, &data32, CFG_SPLIT_HORIZON_TRAP_FWD_ENf,  0);
	  soc_reg_field_set(unit, EPNI_CFG_SPLIT_HORIZON_TRAPr, &data32, CFG_SPLIT_HORIZON_TRAP_FWD_STRENGTHf,  1);
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  23,  exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_CFG_SPLIT_HORIZON_TRAPr(unit, SOC_CORE_ALL, data32));

	  
          
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  24,  exit, ARAD_REG_ACCESS_ERR, READ_EPNI_CFG_GLEM_TRAPr(unit, SOC_CORE_ALL, &data32));
	  soc_reg_field_set(unit, EPNI_CFG_GLEM_TRAPr, &data32, CFG_GLEM_TRAP_MIRROR_CMDf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_GLEM_TRAPr, &data32, CFG_GLEM_TRAP_MIRROR_ENf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_GLEM_TRAPr, &data32, CFG_GLEM_TRAP_MIRROR_STRENGTHf, 0);
	  soc_reg_field_set(unit, EPNI_CFG_GLEM_TRAPr, &data32, CFG_GLEM_TRAP_FWD_ENf,  0);
	  soc_reg_field_set(unit, EPNI_CFG_GLEM_TRAPr, &data32, CFG_GLEM_TRAP_FWD_STRENGTHf,  0);
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_CFG_GLEM_TRAPr(unit, SOC_CORE_ALL, data32));

  } else {
      reg_val = 8; 
      res = WRITE_EGQ_SPLIT_HORIZON_FILTERr(unit, REG_PORT_ANY, reg_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  
  soc_sand_os_memset(acceptable_frames_reg_val, 0xff, sizeof(acceptable_frames_reg_val));
  res = WRITE_EGQ_ACCEPTABLE_FRAME_TYPE_TABLEr(unit, REG_PORT_ANY, acceptable_frames_reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if (SOC_IS_ARAD_A0(unit)) 
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_PP_CONFIGr, SOC_CORE_ALL, 0, ENABLE_UC_SAME_INTERFACE_FILTERf,  0x1));
  }

  
  if (SOC_IS_JERICHO_PLUS(unit)) {
      soc_reg_above_64_val_t data;

      
      SOC_REG_ABOVE_64_ALLONES(data);

      arad_fill_table_with_entry(unit, EPNI_EGRESS_MEMBERSHIPm, MEM_BLOCK_ANY, data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_CFG_OUT_PP_PORT_DEFAULT_MEMBERSHIPr(unit, data));
  }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit))
    {
        uint32 inlif_profile_same_if_mask = 0;
        uint32 inlif_profile_index;
        SOC_PPC_EG_FILTER_GLOBAL_INFO info; 

        
        SOC_PPC_EG_FILTER_GLOBAL_INFO_clear(&info);

        if (soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_BRIDGE_FILTER_ENABLE, 0))
        {
            
            res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,(unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_SIMPLE_SAME_INTERFACE, 1, &inlif_profile_same_if_mask));
            SOC_SAND_CHECK_FUNC_RESULT(res, 99, exit);

            
            for (inlif_profile_index = 0; inlif_profile_index < SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_profile_index++)
            {
                if ((inlif_profile_index & inlif_profile_same_if_mask) == inlif_profile_same_if_mask)
                {
                    info.in_lif_profile_disable_same_interface_filter_bitmap |= (1 << inlif_profile_index);
                }
            }
        }

        if (soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_SAME_FILTER_ENABLE, 0))
        {
            inlif_profile_same_if_mask = 0;
            res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,(unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_LOGICAL_INTF_SAME_FILTER, 1, &inlif_profile_same_if_mask));
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

            for (inlif_profile_index = 0; inlif_profile_index < SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_profile_index++)
            {
                if ((inlif_profile_index & inlif_profile_same_if_mask) == inlif_profile_same_if_mask)
                {
                    info.in_lif_profile_enable_logical_intf_same_filter_bitmap |= (1 << inlif_profile_index);
                }
            }
        }

        res = arad_pp_eg_filter_global_info_set_unsafe(unit, &info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 99, exit);
    }
#endif 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_init_unsafe()", 0, 0);
}



STATIC uint32
  arad_pp_eg_filter_pvlan_type_from_hw_val(
    SOC_SAND_IN uint32 pvlan_val
  )
{
  switch(pvlan_val)
  {
  case 1:
    return SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_ISOLATED;
  case 3:
    return SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_COMMUNITY;
  default:
  case 0:
    return SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS;
  }
}


uint32
  arad_pp_eg_filter_port_info_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PORT_INFO          *port_info
  )
{
  uint32
    filters;
  ARAD_PP_EGQ_PP_PPCT_TBL_DATA
    egq_pp_ppct_tbl;
  ARAD_PP_EPNI_PP_PCT_TBL_DATA
    epni_pp_pct_tbl;
  uint32
    res = SOC_SAND_OK;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PORT_INFO_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(port_info);

  filters = port_info->filter_mask;

  res = arad_pp_egq_pp_ppct_tbl_get_unsafe(unit, core_id, out_port_ndx, &egq_pp_ppct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core_id, out_port_ndx, &epni_pp_pct_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  
  egq_pp_ppct_tbl.acceptable_frame_type_profile = port_info->acceptable_frames_profile;
  epni_pp_pct_tbl.acceptable_frame_type_profile = egq_pp_ppct_tbl.acceptable_frame_type_profile;
  
  if (filters == SOC_PPC_EG_FILTER_PORT_ENABLE_NONE || (filters & SOC_PPC_EG_EGQ_FILTER_PORT_ENABLE_NONE)) 
  {
    egq_pp_ppct_tbl.disable_filtering = TRUE;
  }
  else
  {
    egq_pp_ppct_tbl.disable_filtering = FALSE;
  }
  if (filters == SOC_PPC_EG_FILTER_PORT_ENABLE_NONE || (filters & SOC_PPC_EG_EPNI_FILTER_PORT_ENABLE_NONE)) 
  {
    epni_pp_pct_tbl.disable_filter = TRUE;
  }
  else
  {
    epni_pp_pct_tbl.disable_filter = FALSE;
  }

  



  epni_pp_pct_tbl.egress_vsi_filter_enable = 
    ARAD_PP_MASK_IS_ON(filters, SOC_PPC_EG_FILTER_PORT_ENABLE_VSI_MEMBERSHIP);

  egq_pp_ppct_tbl.enable_same_interface_filters_ports = 
    ARAD_PP_MASK_IS_ON(filters, SOC_PPC_EG_FILTER_PORT_ENABLE_SAME_INTERFACE);

  egq_pp_ppct_tbl.enable_unicast_same_interface_filter = 
    ARAD_PP_MASK_IS_ON(filters, SOC_PPC_EG_FILTER_PORT_ENABLE_SAME_INTERFACE);

  egq_pp_ppct_tbl.unknown_uc_da_action_filter =
    ARAD_PP_MASK_IS_ON(filters, SOC_PPC_EG_FILTER_PORT_ENABLE_UC_UNKNOW_DA);

  egq_pp_ppct_tbl.unknown_mc_da_action_filter =
    ARAD_PP_MASK_IS_ON(filters, SOC_PPC_EG_FILTER_PORT_ENABLE_MC_UNKNOW_DA);

  egq_pp_ppct_tbl.unknown_bc_da_action_filter =
    ARAD_PP_MASK_IS_ON(filters, SOC_PPC_EG_FILTER_PORT_ENABLE_BC_UNKNOW_DA);

  epni_pp_pct_tbl.egress_stp_filter_enable =
    ARAD_PP_MASK_IS_ON(filters, SOC_PPC_EG_FILTER_PORT_ENABLE_STP);

  

  res = arad_pp_egq_pp_ppct_tbl_set_unsafe(unit, core_id, out_port_ndx, &egq_pp_ppct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_epni_pp_pct_tbl_set_unsafe(unit, core_id, out_port_ndx, &epni_pp_pct_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_port_info_set_unsafe()", out_port_ndx, 0);
}

uint32
  arad_pp_eg_filter_port_info_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PORT_INFO          *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_FILTER_PORT_INFO, port_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_port_info_set_verify()", out_port_ndx, 0);
}

uint32
  arad_pp_eg_filter_port_info_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_port_info_get_verify()", out_port_ndx, 0);
}


uint32
  arad_pp_eg_filter_port_info_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_PORT_INFO          *port_info
  )
{
  uint32
    filters;
  ARAD_PP_EGQ_PP_PPCT_TBL_DATA
    egq_pp_ppct_tbl;
  ARAD_PP_EPNI_PP_PCT_TBL_DATA
    epni_pp_pct_tbl;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PPC_EG_FILTER_PORT_INFO_clear(port_info);

  filters = 0;

  res = arad_pp_egq_pp_ppct_tbl_get_unsafe(unit, core_id, out_port_ndx, &egq_pp_ppct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core_id, out_port_ndx, &epni_pp_pct_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  
  port_info->acceptable_frames_profile = egq_pp_ppct_tbl.acceptable_frame_type_profile;
  
  if (egq_pp_ppct_tbl.disable_filtering && epni_pp_pct_tbl.disable_filter)
  {
    port_info->filter_mask = SOC_PPC_EG_FILTER_PORT_ENABLE_NONE;
    goto exit;
  }

  if (egq_pp_ppct_tbl.disable_filtering)
  {
    filters |= SOC_PPC_EG_EGQ_FILTER_PORT_ENABLE_NONE;
  }
  else
  {
    if (egq_pp_ppct_tbl.enable_same_interface_filters_ports)
    {
      filters |= SOC_PPC_EG_FILTER_PORT_ENABLE_SAME_INTERFACE;
    }
    if (egq_pp_ppct_tbl.unknown_uc_da_action_filter)
    {
      filters |= SOC_PPC_EG_FILTER_PORT_ENABLE_UC_UNKNOW_DA;
    }
    if (egq_pp_ppct_tbl.unknown_mc_da_action_filter)
    {
      filters |= SOC_PPC_EG_FILTER_PORT_ENABLE_MC_UNKNOW_DA;
    }
    if (egq_pp_ppct_tbl.unknown_bc_da_action_filter)
    {
      filters |= SOC_PPC_EG_FILTER_PORT_ENABLE_BC_UNKNOW_DA;
    }
  }

  if (epni_pp_pct_tbl.disable_filter)
  {
    filters |= SOC_PPC_EG_EPNI_FILTER_PORT_ENABLE_NONE;
  }
  else
  {
    if (epni_pp_pct_tbl.egress_vsi_filter_enable)
    {
      filters |= SOC_PPC_EG_FILTER_PORT_ENABLE_VSI_MEMBERSHIP;
    }
    if (epni_pp_pct_tbl.egress_stp_filter_enable)
    {
      filters |= SOC_PPC_EG_FILTER_PORT_ENABLE_STP;
    }
    if (epni_pp_pct_tbl.acceptable_frame_type_profile)
    {
      filters |= SOC_PPC_EG_FILTER_PORT_ENABLE_PEP_ACCEPTABLE_FRM_TYPES;
    }
  }
  
  filters |= SOC_PPC_EG_FILTER_PORT_ENABLE_SPLIT_HORIZON;
  filters |= SOC_PPC_EG_FILTER_PORT_ENABLE_MTU;


  port_info->filter_mask = filters;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_port_info_get_unsafe()", out_port_ndx, 0);
}

#ifdef BCM_88660_A0
uint32
  arad_pp_eg_filter_global_info_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_GLOBAL_INFO   *global_info
  )
{
    uint32 inlif_profile_idx;
    uint32 reg_val_arr[] = {0, 0};
    uint64 reg_val64;
    uint32
        res = SOC_SAND_OK;
    uint32
        single_inlif_val = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PORT_INFO_GET_UNSAFE);
    SOC_SAND_CHECK_NULL_INPUT(global_info);

    
    if (!soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_SAME_FILTER_ENABLE, 0)) {
        for (inlif_profile_idx = 0; inlif_profile_idx < SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_profile_idx++)
        {
            
            single_inlif_val = arad_pp_eg_filter_same_if_disable_reg_per_lif_profile_defaults[inlif_profile_idx];
            if (global_info->in_lif_profile_disable_same_interface_filter_bitmap & (1 << inlif_profile_idx))
            {
                
                single_inlif_val = 0;
            }
        
            SHR_BITCOPY_RANGE(reg_val_arr, inlif_profile_idx * ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_NOF_SINGLE_INLIF_BITS,
                              &single_inlif_val, 0,
                              ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_NOF_SINGLE_INLIF_BITS);
        }
    }
    
    else if (!soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_BRIDGE_FILTER_ENABLE, 0))
    {
        for (inlif_profile_idx = 0; inlif_profile_idx < SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_profile_idx++)
        {
            
            single_inlif_val = arad_pp_eg_filter_same_if_disable_reg_per_lif_profile_defaults[inlif_profile_idx];
            if (global_info->in_lif_profile_enable_logical_intf_same_filter_bitmap & (1 << inlif_profile_idx))
            {
                
                single_inlif_val = ARAD_PP_INLIF_PROFILE_LOGICAL_INTF_SAME_FILTER;
            }

            SHR_BITCOPY_RANGE(reg_val_arr, inlif_profile_idx * ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_NOF_SINGLE_INLIF_BITS,
                              &single_inlif_val, 0,
                              ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_NOF_SINGLE_INLIF_BITS);
        }
    }
    
    else 
    {
        for (inlif_profile_idx = 0; inlif_profile_idx < SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_profile_idx++)
        {
            
            single_inlif_val = ARAD_PP_INLIF_PROFILE_LOGICAL_INTF_SAME_FILTER_DEVICE_SCOPE;
            if (global_info->in_lif_profile_disable_same_interface_filter_bitmap & (1 << inlif_profile_idx))
            {
                single_inlif_val = 0;
            }
            if (global_info->in_lif_profile_enable_logical_intf_same_filter_bitmap & (1 << inlif_profile_idx))
            {
                single_inlif_val |= ARAD_PP_INLIF_PROFILE_LOGICAL_INTF_SAME_FILTER_SYSTEM_SCOPE;
            }

            SHR_BITCOPY_RANGE(reg_val_arr, inlif_profile_idx * ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_NOF_SINGLE_INLIF_BITS,
                              &single_inlif_val, 0,
                             ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_NOF_SINGLE_INLIF_BITS);
        }

    }

    COMPILER_64_SET(reg_val64, reg_val_arr[1], reg_val_arr[0]);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_EGQ_SAME_INTERFACE_FILTERr(unit, REG_PORT_ANY, reg_val64));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_global_info_set_unsafe()", reg_val_arr[0], reg_val_arr[1]);
}

uint32
  arad_pp_eg_filter_global_info_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_GLOBAL_INFO   *global_info
  )
{

  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PORT_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_EG_FILTER_GLOBAL_INFO, global_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_global_info_set_verify()", unit, 0);
}

uint32
  arad_pp_eg_filter_global_info_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_GLOBAL_INFO   *global_info
  )
{
uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PORT_INFO_SET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_global_info_get_verify()", unit, 0);
}

uint32
  arad_pp_eg_filter_global_info_get_unsafe(
    SOC_SAND_OUT int                          unit,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_GLOBAL_INFO   *global_info
  )
{
    uint32 inlif_profile_idx;
    uint64 reg_val;
    uint32 reg_val_arr[2];
    uint32
        res = SOC_SAND_OK;
    uint32 
        single_inlif_val = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PORT_INFO_GET_UNSAFE);
    SOC_SAND_CHECK_NULL_INPUT(global_info);
    SOC_PPC_EG_FILTER_GLOBAL_INFO_clear(global_info);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EGQ_SAME_INTERFACE_FILTERr(unit, REG_PORT_ANY, &reg_val));
    reg_val_arr[0] = COMPILER_64_LO(reg_val);
    reg_val_arr[1] = COMPILER_64_HI(reg_val);

    for (inlif_profile_idx = 0; inlif_profile_idx < SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_profile_idx++)
    {
        single_inlif_val = 0;
        SHR_BITCOPY_RANGE(&single_inlif_val, 0,
                          reg_val_arr, inlif_profile_idx * ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_NOF_SINGLE_INLIF_BITS,
                          ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_NOF_SINGLE_INLIF_BITS);

        if ((single_inlif_val & ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_SINGLE_INLIF_MASK) == 0)
        {
            global_info->in_lif_profile_disable_same_interface_filter_bitmap |= (1 << inlif_profile_idx);
        }
    }

    if (soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_SAME_FILTER_ENABLE, 0))
    {
        for (inlif_profile_idx = 0; inlif_profile_idx < SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_profile_idx++)
        {
            single_inlif_val = 0;
            SHR_BITCOPY_RANGE(&single_inlif_val, 0,
                              reg_val_arr, inlif_profile_idx * ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_NOF_SINGLE_INLIF_BITS,
                              ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_NOF_SINGLE_INLIF_BITS);

            if ((single_inlif_val & ARAD_PP_EG_FILTER_SAME_IF_DISABLE_REG_SINGLE_INLIF_MASK) == ARAD_PP_INLIF_PROFILE_LOGICAL_INTF_SAME_FILTER)
            {
                global_info->in_lif_profile_enable_logical_intf_same_filter_bitmap |= (1 << inlif_profile_idx);
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_global_info_get_unsafe()", unit, 0);
}
#endif 


uint32
  arad_pp_eg_filter_vsi_port_membership_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  )
{
  ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA
    egq_vsi_membership_tbl;
  uint32
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_UNSAFE);

  if (!SOC_IS_JERICHO_PLUS(unit)) {
      res = arad_pp_egq_vsi_membership_tbl_get_unsafe(unit, core_id, vsid_ndx, &egq_vsi_membership_tbl);
  } else {
      res = arad_pp_epni_egress_membership_tbl_get_unsafe(unit, core_id, vsid_ndx, &egq_vsi_membership_tbl);
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  tmp = SOC_SAND_BOOL2NUM(is_member);
  if (SOC_IS_JERICHO(unit)) {
      tmp = !tmp;
  }

  res = soc_sand_bitstream_set_any_field(&tmp,out_port_ndx,1,egq_vsi_membership_tbl.vsi_membership);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!SOC_IS_JERICHO_PLUS(unit)) {
      res = arad_pp_egq_vsi_membership_tbl_set_unsafe(unit, core_id, vsid_ndx, &egq_vsi_membership_tbl);
  } else {
      res = arad_pp_epni_egress_membership_tbl_set_unsafe(unit, core_id, vsid_ndx, &egq_vsi_membership_tbl);
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_vsi_port_membership_set_unsafe()", vsid_ndx, out_port_ndx);
}

uint32
  arad_pp_eg_filter_vsi_port_membership_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_NOF(vsid_ndx, SOC_DPP_DEFS_GET(unit, nof_vsi_lowers), ARAD_PP_RIF_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_vsi_port_membership_set_verify()", vsid_ndx, out_port_ndx);
}

uint32
  arad_pp_eg_filter_vsi_port_membership_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_NOF(vsid_ndx, SOC_DPP_DEFS_GET(unit, nof_vsi_lowers), ARAD_PP_RIF_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_vsi_port_membership_get_verify()", vsid_ndx, out_port_ndx);
}


uint32
  arad_pp_eg_filter_vsi_port_membership_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_OUT uint8                          *is_member
  )
{
  ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA
    egq_vsi_membership_tbl;
  uint32
    tmp=0;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_member);

  if (!SOC_IS_JERICHO_PLUS(unit)) {
      res = arad_pp_egq_vsi_membership_tbl_get_unsafe(unit, core_id, vsid_ndx, &egq_vsi_membership_tbl);
  } else {
      res = arad_pp_epni_egress_membership_tbl_get_unsafe(unit, core_id, vsid_ndx, &egq_vsi_membership_tbl);
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field(egq_vsi_membership_tbl.vsi_membership,out_port_ndx,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if (SOC_IS_JERICHO(unit)) {
      tmp = !tmp;
  }
  *is_member = SOC_SAND_NUM2BOOL(tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_vsi_port_membership_get_unsafe()", vsid_ndx, out_port_ndx);
}

uint32
  arad_pp_eg_filter_vsi_membership_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx    
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_VSI_MEMBERSHIP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsid_ndx, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_vsi_membership_get_verify()", vsid_ndx, 0);
}


soc_error_t
  arad_pp_eg_filter_default_port_membership_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PORT                 out_port_ndx,
    SOC_SAND_IN  uint8                        is_member
  )
{

  uint32
    tmp = !is_member;
  soc_reg_above_64_val_t
    reg_val;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(READ_EPNI_CFG_OUT_PP_PORT_DEFAULT_MEMBERSHIPr(unit, reg_val));

  soc_sand_bitstream_set_any_field(&tmp, out_port_ndx, 1, reg_val);

  SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_OUT_PP_PORT_DEFAULT_MEMBERSHIPr(unit, reg_val));

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_pp_eg_filter_default_port_membership_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PORT                 out_port_ndx,
    SOC_SAND_OUT uint8                        *is_member
  )
{

  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(READ_EPNI_CFG_OUT_PP_PORT_DEFAULT_MEMBERSHIPr(unit, reg_val));

  soc_sand_bitstream_get_any_field(reg_val, out_port_ndx, 1, &tmp);

  (*is_member) = !tmp;

exit:
  SOCDNX_FUNC_RETURN;
}



uint32
  arad_pp_eg_filter_vsi_membership_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_OUT uint32                               ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA
    egq_vsi_membership_tbl;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_VSI_MEMBERSHIP_GET_UNSAFE);

  if (!SOC_IS_JERICHO_PLUS(unit)) {
      res = arad_pp_egq_vsi_membership_tbl_get_unsafe(unit, core_id, vsid_ndx, &egq_vsi_membership_tbl);
  } else {
      res = arad_pp_epni_egress_membership_tbl_get_unsafe(unit, core_id, vsid_ndx, &egq_vsi_membership_tbl);
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  sal_memset(ports, 0, SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE * sizeof(uint32));
  sal_memcpy(ports, &egq_vsi_membership_tbl, sizeof(egq_vsi_membership_tbl));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_vsi_membership_get_unsafe()", vsid_ndx, 0);
}


uint32
  arad_pp_eg_filter_cvid_port_membership_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_cvid_port_membership_set_unsafe()", cvid_ndx, out_port_ndx);
}

uint32
  arad_pp_eg_filter_cvid_port_membership_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cvid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_cvid_port_membership_set_verify()", cvid_ndx, out_port_ndx);
}

uint32
  arad_pp_eg_filter_cvid_port_membership_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cvid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_cvid_port_membership_get_verify()", cvid_ndx, out_port_ndx);
}


uint32
  arad_pp_eg_filter_cvid_port_membership_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_OUT uint8                          *is_member
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_member);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_cvid_port_membership_get_unsafe()", cvid_ndx, out_port_ndx);
}


uint32
  arad_pp_eg_filter_port_acceptable_frames_set_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         eg_acceptable_frames_port_profile,
    SOC_SAND_IN  uint32                         llvp_port_profile,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO         *eg_prsr_out_key,
    SOC_SAND_IN  uint8                          accept
  )
{
  uint32
    tmp,
    entry_offset;
  soc_reg_above_64_val_t
    egq_fld_val,
    egq_reg_val;
  uint64
    epni_fld_val,
    epni_reg_val;
  uint32
      data32[2];
  ARAD_PP_EPNI_LLVP_TABLE_TBL_DATA
    epni_llvp_tbl;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(eg_prsr_out_key);


  SOC_REG_ABOVE_64_CLEAR(egq_fld_val);
  SOC_REG_ABOVE_64_CLEAR(egq_reg_val);
  COMPILER_64_SET(epni_reg_val, 0, 0);
  COMPILER_64_SET(epni_fld_val, 0, 0);

  
  entry_offset = ARAD_PP_TBL_EGQ_ACC_FRM_ENTRY_OFFSET( 
                   eg_acceptable_frames_port_profile,
                   eg_prsr_out_key->outer_tpid,
                   eg_prsr_out_key->is_outer_prio,
                   eg_prsr_out_key->inner_tpid
                 );

  tmp = SOC_SAND_BOOL2NUM(accept);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_EGQ_ACCEPTABLE_FRAME_TYPE_TABLEr(unit, REG_PORT_ANY, egq_reg_val));
  ARAD_FLD_FROM_REG_ABOVE_64(EGQ_ACCEPTABLE_FRAME_TYPE_TABLEr, ACCEPTABLE_FRAME_TYPE_TABLEf, egq_fld_val, egq_reg_val, 5, exit);
  soc_sand_bitstream_set_any_field(&tmp, entry_offset, 1, egq_fld_val);
  ARAD_FLD_TO_REG_ABOVE_64(EGQ_ACCEPTABLE_FRAME_TYPE_TABLEr, ACCEPTABLE_FRAME_TYPE_TABLEf, egq_fld_val, egq_reg_val, 10, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_EGQ_ACCEPTABLE_FRAME_TYPE_TABLEr(unit, REG_PORT_ANY, egq_reg_val));
  
  entry_offset = ARAD_PP_TBL_IHP_LLVP_KEY_ENTRY_OFFSET( 
                   llvp_port_profile,
                   eg_prsr_out_key->outer_tpid,
                   eg_prsr_out_key->is_outer_prio,
                   eg_prsr_out_key->inner_tpid
                 );

  res = arad_pp_epni_llvp_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &epni_llvp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  entry_offset = (epni_llvp_tbl.incoming_tag_format << 2) + eg_acceptable_frames_port_profile;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_EPNI_ACCEPTABLE_FRAME_TYPEr(unit, REG_PORT_ANY, &epni_reg_val));
  epni_fld_val = soc_reg64_field_get(unit, EPNI_ACCEPTABLE_FRAME_TYPEr, epni_reg_val, ACCEPTABLE_FRAME_TYPEf);
  
  data32[0] = COMPILER_64_LO(epni_fld_val);
  data32[1] = COMPILER_64_HI(epni_fld_val);
  soc_sand_bitstream_set_any_field(&tmp, entry_offset, 1, data32);
  COMPILER_64_SET(epni_fld_val, data32[1], data32[0]);
  soc_reg64_field_set(unit, EPNI_ACCEPTABLE_FRAME_TYPEr, &epni_reg_val, ACCEPTABLE_FRAME_TYPEf, epni_fld_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_EPNI_ACCEPTABLE_FRAME_TYPEr(unit, REG_PORT_ANY, epni_reg_val));
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_port_acceptable_frames_set_unsafe()", eg_acceptable_frames_port_profile, 0);
}

uint32
  arad_pp_eg_filter_port_acceptable_frames_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          eg_acceptable_frames_port_profile,
    SOC_SAND_IN  uint32                          llvp_port_profile,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO          *eg_prsr_out_key,
    SOC_SAND_IN  uint8                           accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(eg_acceptable_frames_port_profile, ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_MAX, ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(llvp_port_profile, ARAD_PP_EG_PARSE_TPID_PROFILE_NDX_MAX, ARAD_PP_EG_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR, 15, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_PARSE_INFO, eg_prsr_out_key, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_port_acceptable_frames_set_verify()", eg_acceptable_frames_port_profile, 0);
}

uint32
  arad_pp_eg_filter_port_acceptable_frames_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           port_profile,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO             *eg_prsr_out_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile, ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_MAX, ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_PARSE_INFO, eg_prsr_out_key, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_port_acceptable_frames_get_verify()", port_profile, 0);
}


uint32
  arad_pp_eg_filter_port_acceptable_frames_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           port_profile,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO             *eg_prsr_out_key,
    SOC_SAND_OUT uint8                          *accept
  )
{
  uint32
    entry_offset,
    tmp = 0;
  soc_reg_above_64_val_t
    egq_fld_val,
    egq_reg_val;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(eg_prsr_out_key);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  SOC_REG_ABOVE_64_CLEAR(egq_fld_val);
	SOC_REG_ABOVE_64_CLEAR(egq_reg_val);

  
  entry_offset = ARAD_PP_TBL_EGQ_ACC_FRM_ENTRY_OFFSET(
                   port_profile,
                   eg_prsr_out_key->outer_tpid,
                   eg_prsr_out_key->is_outer_prio,
                   eg_prsr_out_key->inner_tpid
                 );

  

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_EGQ_ACCEPTABLE_FRAME_TYPE_TABLEr(unit, REG_PORT_ANY, egq_reg_val));
  ARAD_FLD_FROM_REG_ABOVE_64(EGQ_ACCEPTABLE_FRAME_TYPE_TABLEr, ACCEPTABLE_FRAME_TYPE_TABLEf, egq_fld_val, egq_reg_val, 5, exit);
  res = soc_sand_bitstream_get_any_field(egq_fld_val, entry_offset, 1, &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *accept = SOC_SAND_NUM2BOOL(tmp);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_port_acceptable_frames_get_unsafe()", port_profile, 0);
}


uint32
  arad_pp_eg_filter_pep_acceptable_frames_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx,
    SOC_SAND_IN  uint8                          accept
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pep_key);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_pep_acceptable_frames_set_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_filter_pep_acceptable_frames_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx,
    SOC_SAND_IN  uint8                          accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_VERIFY);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_PEP_KEY, pep_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vlan_format_ndx, ARAD_PP_EG_FILTER_VLAN_FORMAT_NDX_MAX, ARAD_PP_EG_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(accept, ARAD_PP_EG_FILTER_ACCEPT_MAX, ARAD_PP_EG_FILTER_ACCEPT_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_pep_acceptable_frames_set_verify()", 0, 0);
}

uint32
  arad_pp_eg_filter_pep_acceptable_frames_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_PEP_KEY, pep_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vlan_format_ndx, ARAD_PP_EG_FILTER_VLAN_FORMAT_NDX_MAX, ARAD_PP_EG_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_pep_acceptable_frames_get_verify()", 0, 0);
}


uint32
  arad_pp_eg_filter_pep_acceptable_frames_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx,
    SOC_SAND_OUT uint8                          *accept
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pep_key);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_pep_acceptable_frames_get_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_filter_pvlan_port_type_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                *src_sys_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE    pvlan_port_type
  )
{
  ARAD_PP_EGQ_PP_PPCT_TBL_DATA
    egq_pp_ppct_tbl;
  uint32
    fld_val,
    entry_offset,
    phy_sys_port_ndx,
    tmp = 0,
    entry_index;
  uint32
    fap_id,
    local_port,
    lag_info_idx;
  ARAD_REG_FIELD
    is_lag_fld;
  SOC_PPC_LAG_INFO
    *in_info = NULL,
    *out_info = NULL;
  ARAD_PP_EGQ_AUX_TABLE_TBL_DATA
    aux_table_tbl_data;
  uint32
    res = SOC_SAND_OK;
  int   core = SOC_CORE_INVALID;
  uint8 is_fap_id_local;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_SET_UNSAFE);
  
  SOC_SAND_CHECK_NULL_INPUT(src_sys_port_ndx);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_AUXILIARY_DATA_TABLEr, SOC_CORE_ALL, 0, AUXILIARY_DATA_TABLE_MODEf, &fld_val));

  ARAD_PP_LLP_PVLAN_FEATURE_CHECK_IF_ENABLED(unit,fld_val);

  tmp = arad_pp_eg_filter_pvlan_type_to_hw_val(pvlan_port_type);

  
  if (src_sys_port_ndx->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT)
  {      
      phy_sys_port_ndx = src_sys_port_ndx->sys_id;
      
      
      entry_index = phy_sys_port_ndx >> 4;
      entry_offset = SOC_SAND_GET_BITS_RANGE(phy_sys_port_ndx,3,0);
      
      res = arad_pp_egq_aux_table_tbl_get_unsafe(
              unit,
              entry_index,
              &aux_table_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      
      res = soc_sand_set_field(
              &aux_table_tbl_data.aux_table_data,
              2*entry_offset+1,
              2*entry_offset,
              tmp
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      
      res = arad_pp_egq_aux_table_tbl_set_unsafe(
              unit,
              entry_index,
              &aux_table_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
    
    res = arad_sys_phys_to_local_port_map_get_unsafe(
            unit,
            src_sys_port_ndx->sys_id,
            &fap_id,
            &local_port
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_dpp_is_fap_id_local_and_get_core_id(unit, fap_id, &is_fap_id_local, &core));
  
    if (!is_fap_id_local)
    {   
      ARAD_DO_NOTHING_AND_EXIT;
    }
  
    res = arad_pp_egq_pp_ppct_tbl_get_unsafe(unit, core, local_port, &egq_pp_ppct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  
    egq_pp_ppct_tbl.pvlan_port_type = tmp;
  
    res = arad_pp_egq_pp_ppct_tbl_set_unsafe(unit, core, local_port, &egq_pp_ppct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

 
  if (src_sys_port_ndx->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_LAG)
  {
    ARAD_ALLOC(in_info, SOC_PPC_LAG_INFO, 1, "arad_pp_eg_filter_pvlan_port_type_set_unsafe.in_info");
    ARAD_ALLOC(out_info, SOC_PPC_LAG_INFO, 1, "arad_pp_eg_filter_pvlan_port_type_set_unsafe.out_info");
    res = arad_ports_lag_get_unsafe(
        unit,
        src_sys_port_ndx->sys_id,
        in_info, 
        out_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    for (lag_info_idx = 0; lag_info_idx < in_info->nof_entries; lag_info_idx++)
    {
      res = arad_ports_logical_sys_id_build(
        unit,
        TRUE,
        src_sys_port_ndx->sys_id,
        in_info->members[lag_info_idx].member_id,
        0,
        &phy_sys_port_ndx);
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      
      is_lag_fld.lsb = 15;
      is_lag_fld.msb = 15;
      phy_sys_port_ndx &= (uint32)(~ARAD_FLD_MASK_OLD(is_lag_fld));

      
      entry_index = phy_sys_port_ndx >> 4;
      entry_offset = SOC_SAND_GET_BITS_RANGE(phy_sys_port_ndx,3,0);

      res = arad_pp_egq_aux_table_tbl_get_unsafe(
              unit,
              entry_index,
              &aux_table_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      
      res = soc_sand_set_field(
              &aux_table_tbl_data.aux_table_data,
              2*entry_offset+1,
              2*entry_offset,
              tmp
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
      
      res = arad_pp_egq_aux_table_tbl_set_unsafe(
              unit,
              entry_index,
              &aux_table_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  
      res = arad_sys_phys_to_local_port_map_get_unsafe(
              unit,
              in_info->members[lag_info_idx].sys_port,
              &fap_id,
              &local_port
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 140, exit, soc_dpp_is_fap_id_local_and_get_core_id(unit, fap_id, &is_fap_id_local, &core));

      if (!is_fap_id_local)
      {
        ARAD_DO_NOTHING_AND_EXIT;
      }
  
      res = arad_pp_egq_pp_ppct_tbl_get_unsafe(unit, core, local_port, &egq_pp_ppct_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
  
      egq_pp_ppct_tbl.pvlan_port_type = tmp;
  
      res = arad_pp_egq_pp_ppct_tbl_set_unsafe(unit, core, local_port, &egq_pp_ppct_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
    }
  }

exit:
  ARAD_FREE(in_info);
  ARAD_FREE(out_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_pvlan_port_type_set_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_filter_pvlan_port_type_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                *src_sys_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE    pvlan_port_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_SET_VERIFY);

  res = arad_pp_SAND_PP_SYS_PORT_ID_verify(src_sys_port_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(pvlan_port_type, SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_MAX, SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_pvlan_port_type_set_verify()", 0, 0);
}

uint32
  arad_pp_eg_filter_pvlan_port_type_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                *src_sys_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_GET_VERIFY);

  res = arad_pp_SAND_PP_SYS_PORT_ID_verify(src_sys_port_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_pvlan_port_type_get_verify()", 0, 0);
}


uint32
  arad_pp_eg_filter_pvlan_port_type_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                *src_sys_port_ndx,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE    *pvlan_port_type
  )
{
  uint32
    fld_val,
    entry_offset,
    tmp,
    phy_sys_port_ndx,
    entry_index;
  ARAD_PP_EGQ_AUX_TABLE_TBL_DATA
    aux_table_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pvlan_port_type);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_AUXILIARY_DATA_TABLEr, SOC_CORE_ALL, 0, AUXILIARY_DATA_TABLE_MODEf, &fld_val));

  ARAD_PP_LLP_PVLAN_FEATURE_CHECK_IF_ENABLED(unit,fld_val);

  phy_sys_port_ndx = ARAD_PP_SAND_SYS_PORT_ENCODE(src_sys_port_ndx);

  entry_index = phy_sys_port_ndx >> 4;
  entry_offset = SOC_SAND_GET_BITS_RANGE(phy_sys_port_ndx,3,0);

  res = arad_pp_egq_aux_table_tbl_get_unsafe(
          unit,
          entry_index,
          &aux_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = SOC_SAND_GET_BITS_RANGE(aux_table_tbl_data.aux_table_data,2*entry_offset+1,2*entry_offset);

  *pvlan_port_type = arad_pp_eg_filter_pvlan_type_from_hw_val(tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_pvlan_port_type_get_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_filter_split_horizon_out_ac_orientation_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_split_horizon_out_ac_orientation_set_unsafe()", out_ac_ndx, 0);
}

uint32
  arad_pp_eg_filter_split_horizon_out_ac_orientation_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_split_horizon_out_ac_orientation_set_verify()", out_ac_ndx, 0);
}


uint32
  arad_pp_eg_filter_split_horizon_out_ac_orientation_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION      *orientation
  )
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_UNSAFE);

    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_split_horizon_out_ac_orientation_get_unsafe()", out_ac_ndx, 0);
}

uint32
  arad_pp_eg_filter_split_horizon_out_ac_orientation_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx
  )
{     
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_VERIFY);

    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_split_horizon_out_ac_orientation_get_verify()", out_ac_ndx, 0);
}


uint32
  arad_pp_eg_filter_split_horizon_out_lif_orientation_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        lif_eep_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  )
{
 uint32
    fld_val,
    entry_offset,
    tmp,
    entry_index;
  ARAD_PP_EGQ_AUX_TABLE_TBL_DATA
    aux_table_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_AUXILIARY_DATA_TABLEr, SOC_CORE_ALL, 0, AUXILIARY_DATA_TABLE_MODEf, &fld_val));

  ARAD_PP_LLP_SPLIT_HORIZON_FEATURE_CHECK_IF_ENABLED(unit,fld_val);

  entry_index = ARAD_PP_EGQ_AUX_TABLE_TBL_ORIENTATION_MODE_OFFSET(lif_eep_ndx);
  entry_offset = ARAD_PP_EGQ_AUX_TABLE_TBL_ORIENTATION_MODE_ENTRY_OFFSET(lif_eep_ndx);

  res = arad_pp_egq_aux_table_tbl_get_unsafe(
          unit,
          entry_index,
          &aux_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = (orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB)?1:0;

  res = soc_sand_set_field(
          &aux_table_tbl_data.aux_table_data,
          entry_offset,
          entry_offset,
          tmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_egq_aux_table_tbl_set_unsafe(
          unit,
          entry_index,
          &aux_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_split_horizon_out_lif_orientation_set_unsafe()", lif_eep_ndx, 0);
}

uint32
  arad_pp_eg_filter_split_horizon_out_lif_orientation_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        lif_eep_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_VERIFY);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, lif_eep_ndx, SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(orientation, ARAD_PP_EG_FILTER_ORIENTATION_MAX, ARAD_PP_EG_FILTER_ORIENTATION_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;   

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_split_horizon_out_lif_orientation_set_verify()", lif_eep_ndx, 0);
}


uint32
  arad_pp_eg_filter_split_horizon_out_lif_orientation_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           lif_eep_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION      *orientation
  )
{
  uint32
    fld_val,
    entry_offset,
    tmp,
    entry_index;
  ARAD_PP_EGQ_AUX_TABLE_TBL_DATA
    aux_table_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(orientation);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_AUXILIARY_DATA_TABLEr, SOC_CORE_ALL, 0, AUXILIARY_DATA_TABLE_MODEf, &fld_val));

  ARAD_PP_LLP_SPLIT_HORIZON_FEATURE_CHECK_IF_ENABLED(unit,fld_val);

  entry_index = ARAD_PP_EGQ_AUX_TABLE_TBL_ORIENTATION_MODE_OFFSET(lif_eep_ndx);
  entry_offset = ARAD_PP_EGQ_AUX_TABLE_TBL_ORIENTATION_MODE_ENTRY_OFFSET(lif_eep_ndx);

  res = arad_pp_egq_aux_table_tbl_get_unsafe(
          unit,
          entry_index,
          &aux_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = SOC_SAND_GET_BITS_RANGE(aux_table_tbl_data.aux_table_data,entry_offset,entry_offset);

  *orientation = (tmp == 1)?SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB:SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_split_horizon_out_lif_orientation_get_unsafe()", lif_eep_ndx, 0);
}

uint32
  arad_pp_eg_filter_split_horizon_out_lif_orientation_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           lif_eep_ndx
  )
{     
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_VERIFY);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, lif_eep_ndx, SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  
  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;   

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_filter_split_horizon_out_lif_orientation_get_verify()", lif_eep_ndx, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_filter_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_eg_filter;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_filter_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_eg_filter;
}
uint32
  SOC_PPC_EG_FILTER_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if ((info->filter_mask != SOC_PPC_EG_FILTER_PORT_ENABLE_ALL) && 
      (info->filter_mask != SOC_PPC_EG_FILTER_PORT_ENABLE_NONE))
  {
    if ((info->filter_mask & SOC_PPC_EG_FILTER_PORT_ENABLE_SPLIT_HORIZON) == 0)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_FLTER_PORT_SPLIT_DISABLE_ERR, 20, exit);
    }
    if ((info->filter_mask & SOC_PPC_EG_FILTER_PORT_ENABLE_MTU) == 0)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_FLTER_PORT_MTU_DISABLE_ERR, 30, exit);
    }
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->acceptable_frames_profile, ARAD_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_MAX, ARAD_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_OUT_OF_RANGE_ERR, 11, exit);
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_FILTER_PORT_INFO_verify()",0,0);
}

#ifdef BCM_88660_A0
uint32
  ARAD_PP_EG_FILTER_GLOBAL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_FILTER_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_lif_profile_disable_same_interface_filter_bitmap, SOC_OCC_MGMT_EG_FILTER_INLIF_PROFILE_BITMAP_MAX, ARAD_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_lif_profile_enable_logical_intf_same_filter_bitmap, SOC_OCC_MGMT_EG_FILTER_INLIF_PROFILE_BITMAP_MAX, ARAD_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_FILTER_PORT_INFO_verify()",0,0);
}
#endif 


#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
