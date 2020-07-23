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

#include <soc/mem.h>


#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_cos.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>





#define ARAD_PP_LIF_COS_COS_PROFILE_NDX_MIN                      (1)
#define ARAD_PP_LIF_COS_COS_PROFILE_NDX_MAX                      (63)
#define ARAD_PP_LIF_COS_MAP_TBL_NDX_MIN                          (ARAD_PP_LIF_COS_COS_PROFILE_NDX_MIN)
#define ARAD_PP_LIF_COS_MAP_TBL_NDX_MAX                          (ARAD_PP_LIF_COS_COS_PROFILE_NDX_MAX)
#define ARAD_PP_LIF_COS_MAP_TBL_NDX_4_LSB_MAX                    (15)
#define ARAD_PP_LIF_COS_TYPE_MAX                                 (SOC_PPC_NOF_LIF_COS_AC_PROFILE_TYPES-1)
#define ARAD_PP_LIF_COS_MAP_TABLE_MIN                            (1)
#define ARAD_PP_LIF_COS_MAP_TABLE_MAX                            (15)
#define ARAD_PP_LIF_COS_OUTER_TPID_MAX                           (1)
#define ARAD_PP_LIF_COS_AC_OFFSET_MAX                            (SOC_SAND_U8_MAX)

#ifdef BCM_88660_A0
# define ARAD_PP_LIF_COS_IN_DSCP_MAPPING_MAX_PROFILE             (15)
#endif 













CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_lif_cos[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_AC_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_AC_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_AC_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_AC_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_AC_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_AC_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_AC_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_AC_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_OPCODE_TYPES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_OPCODE_TYPES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_OPCODE_TYPES_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_OPCODE_TYPES_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_OPCODE_TYPES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_OPCODE_TYPES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_OPCODE_TYPES_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_OPCODE_TYPES_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_COS_GET_ERRS_PTR),
  

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_GET_UNSAFE),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_lif_cos[] =
{
  
  {
    ARAD_PP_LIF_COS_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_COS_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'profile_ndx' is out of range. \n\r "
    "The range is: 1 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_COS_COS_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_COS_COS_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'cos_profile_ndx' is out of range. \n\r "
    "The range is: 1 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'map_tbl_ndx' is out of range. \n\r "
    "The range is: 1 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'opcode_ndx' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_LIF_COS_OPCODE_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PPC_LIF_COS_OPCODE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'opcode_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_LIF_COS_OPCODE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_COS_TAG_TYPE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_COS_TAG_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'tag_type_ndx' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_COS_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_COS_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_LIF_COS_AC_PROFILE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_COS_MAP_TABLE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_COS_MAP_TABLE_OUT_OF_RANGE_ERR",
    "The parameter 'map_table' is out of range. \n\r "
    "The range is: 1 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_COS_IP_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_COS_IP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'ip_type' is out of range. \n\r "
    "Must be equal to SOC_SAND_PP_IP_TYPE_IPV4/6.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_COS_OUTER_TPID_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_COS_OUTER_TPID_OUT_OF_RANGE_ERR",
    "The parameter 'outer_tpid' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_COS_AC_OFFSET_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_COS_AC_OFFSET_OUT_OF_RANGE_ERR",
    "The parameter 'ac_offset' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  {
    ARAD_PP_LIF_MAP_WHEN_IP_MPLS_MISMATCH_ERR,
    "ARAD_PP_LIF_MAP_WHEN_IP_MPLS_MISMATCH_ERR",
    "'map_when_ip' and 'map_when_mpls' should have the same value for Arad-B.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_COS_MAP_FROM_TC_DP_DISABLED_ERR,
    "ARAD_PP_LIF_COS_MAP_FROM_TC_DP_DISABLED_ERR",
    "Mapping from tc & dp is disabled. To enable, use arad_pp_lif_cos_profile_info_set.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





STATIC int _arad_pp_lif_cos_init_unsafe_dscp_exp_map_callbaack(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int copyno, 
    SOC_SAND_IN int array_index, 
    SOC_SAND_IN int index, 
    SOC_SAND_OUT uint32 *value, 
    SOC_SAND_IN int entry_sz, 
    SOC_SAND_IN void *opaque)
{
  uint32 routing_protocol = 0;
  uint32 term_protocol = 0;
  uint32 dscp = 0;
  uint32 uindex = (uint32)index;

  *value = 0;
  SHR_BITCOPY_RANGE(&term_protocol, 0, &uindex, 8, 2);
  SHR_BITCOPY_RANGE(&routing_protocol, 0, &uindex, 10, 2);
  
  if (routing_protocol < 2 && term_protocol == 2 ) {
	SHR_BITCOPY_RANGE(&dscp, 5, &uindex, 0, 3);
	soc_mem_field_set(unit, IHP_DSCP_EXP_MAPm, value, DSCP_EXPf, &dscp);
  }
  else {
	 SHR_BITCOPY_RANGE(&dscp, 0, &uindex, 0, 8);
	 if (term_protocol <= 2 && routing_protocol <= 2 && dscp <= SOC_SAND_PP_L3_DSCP_MAX) {
        soc_mem_field_set(unit, IHP_DSCP_EXP_MAPm, value, DSCP_EXPf, &dscp);
     }
  }
  return 0;
}

STATIC int _arad_pp_lif_cos_init_unsafe_dscp_exp_remark_callbaack(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int copyno, 
    SOC_SAND_IN int array_index, 
    SOC_SAND_IN int index, 
    SOC_SAND_OUT uint32 *value, 
    SOC_SAND_IN int entry_sz, 
    SOC_SAND_IN void *opaque)
{
  uint32 routing_protocol = 0;
  uint32 cos_profile = 0;
  uint32 dscp = 0;
  uint32 uindex = (uint32)index;
  uint32 qos_max[] = {SOC_SAND_PP_L3_DSCP_MAX, SOC_SAND_PP_L3_DSCP_MAX, SOC_SAND_PP_MPLS_DSCP_MAX};

  *value = 0;
  SHR_BITCOPY_RANGE(&cos_profile, 0, &uindex, 0, 4);
  SHR_BITCOPY_RANGE(&dscp, 0, &uindex, 4, 8);
  SHR_BITCOPY_RANGE(&routing_protocol, 0, &uindex, 12, 2);

  if (routing_protocol <= 2 && cos_profile <= ARAD_PP_LIF_COS_MAP_TBL_NDX_4_LSB_MAX && dscp <= qos_max[routing_protocol]) {
    soc_mem_field_set(unit, IHP_DSCP_EXP_REMARKm, value, IN_DSCP_EXPf, &dscp);
  }

  return 0;
}

soc_error_t
  arad_pp_lif_cos_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  
  
  
  soc_error_t res;
  uint32 value;
  soc_reg_above_64_val_t reg_above64_val; 

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_PCID_LITE_SKIP(unit);



    
   
  
  
  
  
  
   
  if (SOC_IS_JERICHO(unit)) {
    
  
    
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_VTT_MAP_1_DSCP_EXP_INDEXr(unit, SOC_CORE_ALL, 0x1a1a1919));
  
    
    value = 0x19d919d9;
    SHR_BITCOPY_RANGE(reg_above64_val, 0, &value, 0, 32); 
    value = 0x1ada1ada;
    SHR_BITCOPY_RANGE(reg_above64_val, 32, &value, 0, 32); 
    value = 0x19ff19ff;
    SHR_BITCOPY_RANGE(reg_above64_val, 64, &value, 0, 32); 
    value = 0x1aff1aff;
    SHR_BITCOPY_RANGE(reg_above64_val, 96, &value, 0, 32); 
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_VTT_MAP_0_DSCP_EXP_INDEXr(unit, SOC_CORE_ALL, reg_above64_val));
  }

  
  
  res = arad_fill_table_with_variable_values_by_caching(unit, IHP_DSCP_EXP_MAPm, 0, MEM_BLOCK_ANY, -1, -1, 
                                                        _arad_pp_lif_cos_init_unsafe_dscp_exp_map_callbaack, NULL);
  SOCDNX_IF_ERR_EXIT(res);

  res = arad_fill_table_with_variable_values_by_caching(unit, IHP_DSCP_EXP_REMARKm, 0, MEM_BLOCK_ANY, -1, -1, 
                                                        _arad_pp_lif_cos_init_unsafe_dscp_exp_remark_callbaack, NULL);
  SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_lif_cos_ac_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_AC_PROFILE_INFO             *profile_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_AC_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_ac_profile_info_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_lif_cos_ac_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_AC_PROFILE_INFO             *profile_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_AC_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_ac_profile_info_set_verify()", profile_ndx, 0);
}

uint32
  arad_pp_lif_cos_ac_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_AC_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_ac_profile_info_get_verify()", profile_ndx, 0);
}


uint32
  arad_pp_lif_cos_ac_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_AC_PROFILE_INFO             *profile_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_AC_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_ac_profile_info_get_unsafe()", profile_ndx, 0);
}


uint32
  arad_pp_lif_cos_pwe_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PWE_PROFILE_INFO            *profile_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_pwe_profile_info_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_lif_cos_pwe_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PWE_PROFILE_INFO            *profile_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_pwe_profile_info_set_verify()", profile_ndx, 0);
}

uint32
  arad_pp_lif_cos_pwe_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_pwe_profile_info_get_verify()", profile_ndx, 0);
}


uint32
  arad_pp_lif_cos_pwe_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PWE_PROFILE_INFO            *profile_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_pwe_profile_info_get_unsafe()", profile_ndx, 0);
}


uint32
  arad_pp_lif_cos_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  cos_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_INFO                *profile_info
  )
{
  uint32
    res = SOC_SAND_OK,
    tmp_lo,
    tmp_hi;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY
    map_key;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY
    map_value;
  uint64
    reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  if (!profile_info->map_from_tc_dp)
  {
    

    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(&map_key);
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&map_value);

    map_key.tc = 0;
    map_key.dp = 0;

    map_value.tc = profile_info->forced_tc;
    map_value.dp = profile_info->forced_dp;

    res = arad_pp_lif_cos_profile_map_tc_dp_info_set_unsafe(
            unit,
            cos_profile_ndx,
            &map_key,
            &map_value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  res = sw_state_access[unit].dpp.soc.arad.pp.lif_cos.map_from_tc_dp.set(unit, profile_info->map_from_tc_dp);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  
  res = READ_IHP_COS_PROFILE_USE_L_2r(unit, REG_PORT_ANY, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  tmp_hi = COMPILER_64_HI(reg_val);
  tmp_lo = COMPILER_64_LO(reg_val);
  if (cos_profile_ndx < 32) {
    SOC_SAND_SET_BIT(tmp_lo, SOC_SAND_BOOL2NUM(profile_info->map_when_l2), cos_profile_ndx);
  } else {
    SOC_SAND_SET_BIT(tmp_hi, SOC_SAND_BOOL2NUM(profile_info->map_when_l2), cos_profile_ndx - 32);   
  }
  COMPILER_64_SET(reg_val, tmp_hi, tmp_lo);
  res = WRITE_IHP_COS_PROFILE_USE_L_2r(unit, REG_PORT_ANY, reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  
  res = READ_IHP_COS_PROFILE_USE_L_3r(unit, REG_PORT_ANY, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  tmp_hi = COMPILER_64_HI(reg_val);
  tmp_lo = COMPILER_64_LO(reg_val);
  if (cos_profile_ndx < 32) {
    SOC_SAND_SET_BIT(tmp_lo, SOC_SAND_BOOL2NUM(profile_info->map_when_ip), cos_profile_ndx);
  } else {
    SOC_SAND_SET_BIT(tmp_hi, SOC_SAND_BOOL2NUM(profile_info->map_when_ip), cos_profile_ndx - 32);    
  }  
  COMPILER_64_SET(reg_val, tmp_hi, tmp_lo);
  res = WRITE_IHP_COS_PROFILE_USE_L_3r(unit, REG_PORT_ANY, reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  
#ifdef BCM_88660_A0
  if ((SOC_IS_ARADPLUS_A0(unit) || SOC_IS_JERICHO(unit)) && (cos_profile_ndx < ARAD_PP_LIF_COS_IN_DSCP_MAPPING_MAX_PROFILE)) {
      uint32 mask = (1 << cos_profile_ndx);
      uint32 reg32_val;
      
      if (SOC_IS_ARADPLUS_A0(unit)) {
          res = READ_IHB_COS_PROFILE_TO_USE_LAYER_2_PCP_MAPr(unit, &reg32_val);
      } else {
          res = READ_IHP_COS_PROFILE_TO_USE_LAYER_2_PCP_MAPr(unit, REG_PORT_ANY, &reg32_val);
      }     
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      if (profile_info->use_layer2_pcp) {
          reg32_val |= mask;
      } else {
          reg32_val &= ~mask;
      }
      if (SOC_IS_ARADPLUS_A0(unit)) {
          res = WRITE_IHB_COS_PROFILE_TO_USE_LAYER_2_PCP_MAPr(unit, reg32_val);
      } else {
          res = WRITE_IHP_COS_PROFILE_TO_USE_LAYER_2_PCP_MAPr(unit, REG_PORT_ANY, reg32_val);
      }
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }
#endif 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_info_set_unsafe()", cos_profile_ndx, 0);
}

uint32
  arad_pp_lif_cos_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  cos_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_INFO                *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(cos_profile_ndx, ARAD_PP_LIF_COS_COS_PROFILE_NDX_MIN, ARAD_PP_LIF_COS_COS_PROFILE_NDX_MAX, ARAD_PP_LIF_COS_COS_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_INFO, profile_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_info_set_verify()", cos_profile_ndx, 0);
}

uint32
  arad_pp_lif_cos_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  cos_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(cos_profile_ndx, ARAD_PP_LIF_COS_COS_PROFILE_NDX_MIN, ARAD_PP_LIF_COS_COS_PROFILE_NDX_MAX, ARAD_PP_LIF_COS_COS_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_info_get_verify()", cos_profile_ndx, 0);
}


uint32
  arad_pp_lif_cos_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  cos_profile_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_INFO              *profile_info
  )
{
  uint32
    res = SOC_SAND_OK,
    bit_val;
  uint64 
    reg_val;
   
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY
    map_key;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY
    map_value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPC_LIF_COS_PROFILE_INFO_clear(profile_info);

  res = sw_state_access[unit].dpp.soc.arad.pp.lif_cos.map_from_tc_dp.get(unit, &profile_info->map_from_tc_dp);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  if (!profile_info->map_from_tc_dp)
  {
    
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(&map_key);
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&map_value);

    map_key.tc = 0;
    map_key.dp = 0;

    res = arad_pp_lif_cos_profile_map_tc_dp_info_get_unsafe(
            unit,
            cos_profile_ndx,
            &map_key,
            &map_value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    profile_info->forced_tc = map_value.tc;
    profile_info->forced_dp = map_value.dp;
  }

  
  res = READ_IHP_COS_PROFILE_USE_L_2r(unit, REG_PORT_ANY, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (cos_profile_ndx < 32) {
    bit_val = SOC_SAND_GET_BIT(COMPILER_64_LO(reg_val), cos_profile_ndx);
  } else {
    bit_val = SOC_SAND_GET_BIT(COMPILER_64_HI(reg_val), cos_profile_ndx - 32);
  }
  profile_info->map_when_l2 = SOC_SAND_BOOL2NUM(bit_val);

  
  res = READ_IHP_COS_PROFILE_USE_L_3r(unit, REG_PORT_ANY, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  if (cos_profile_ndx < 32) {
    bit_val = SOC_SAND_GET_BIT(COMPILER_64_LO(reg_val), cos_profile_ndx);
  } else {
    bit_val = SOC_SAND_GET_BIT(COMPILER_64_HI(reg_val), cos_profile_ndx - 32); 
  }
  profile_info->map_when_mpls = profile_info->map_when_ip = SOC_SAND_BOOL2NUM(bit_val);

    
#ifdef BCM_88660_A0
  if ((SOC_IS_ARADPLUS_A0(unit) || SOC_IS_JERICHO(unit)) && (cos_profile_ndx < ARAD_PP_LIF_COS_IN_DSCP_MAPPING_MAX_PROFILE)) {
      uint32 mask = (1 << cos_profile_ndx);
      uint32 reg32_val;

      if (SOC_IS_ARADPLUS_A0(unit)) {
          res = READ_IHB_COS_PROFILE_TO_USE_LAYER_2_PCP_MAPr(unit, &reg32_val);
      } else {
          res = READ_IHP_COS_PROFILE_TO_USE_LAYER_2_PCP_MAPr(unit, REG_PORT_ANY, &reg32_val);
      }
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      
      profile_info->use_layer2_pcp = ((reg32_val & mask) != 0) ? TRUE : FALSE;
  }
#endif 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_info_get_unsafe()", cos_profile_ndx, 0);
}


STATIC uint32
  arad_pp_lif_cos_profile_map_tbl_entry_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  uint32                                  key_lsb,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY     *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    access_key,
    entry_offset, 
    reg_offset;
  ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_value);

  access_key = ((map_tbl_ndx - 1) << 10) | key_lsb; 
  entry_offset = (access_key >> 2) & 0x3fff; 
  reg_offset = access_key & 0x3; 

  res = arad_pp_ihp_tc_dp_map_table_tbl_get_unsafe(
      unit,
      entry_offset,
      &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (reg_offset == 0) {
      tbl_data.traffic_class_0 = (uint32)map_value->tc;
      tbl_data.drop_precedence_0 = (uint32)map_value->dp;
  }
  else if (reg_offset == 1) {
      tbl_data.traffic_class_1 = (uint32)map_value->tc;
      tbl_data.drop_precedence_1 = (uint32)map_value->dp;
  }
  else if (reg_offset == 2) {
      tbl_data.traffic_class_2 = (uint32)map_value->tc;
      tbl_data.drop_precedence_2 = (uint32)map_value->dp;
  }
  else {
      tbl_data.traffic_class_3 = (uint32)map_value->tc;
      tbl_data.drop_precedence_3 = (uint32)map_value->dp;
  }

  res = arad_pp_ihp_tc_dp_map_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_tbl_entry_set_unsafe()", map_tbl_ndx, key_lsb);
}


STATIC uint32
  arad_pp_lif_cos_profile_map_tbl_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  uint32                                  key_lsb,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY     *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    access_key,
    entry_offset,
    reg_offset;
  ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_value);

  access_key = ((map_tbl_ndx - 1) << 10) | key_lsb; 
  entry_offset = (access_key >> 2) & 0x3fff; 
  reg_offset = access_key & 0x3; 

  res = arad_pp_ihp_tc_dp_map_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (reg_offset == 0) {
      map_value->tc = (SOC_SAND_PP_TC)tbl_data.traffic_class_0;
      map_value->dp = (SOC_SAND_PP_DP)tbl_data.drop_precedence_0;
  }
  else if (reg_offset == 1) {
      map_value->tc = (SOC_SAND_PP_TC)tbl_data.traffic_class_1;
      map_value->dp = (SOC_SAND_PP_DP)tbl_data.drop_precedence_1; 
  }
  else if (reg_offset == 2) {
      map_value->tc = (SOC_SAND_PP_TC)tbl_data.traffic_class_2;
      map_value->dp = (SOC_SAND_PP_DP)tbl_data.drop_precedence_2; 
  }
  else {
      map_value->tc = (SOC_SAND_PP_TC)tbl_data.traffic_class_3;
      map_value->dp = (SOC_SAND_PP_DP)tbl_data.drop_precedence_3;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_tbl_entry_get_unsafe()", map_tbl_ndx, key_lsb);
}


uint32
  arad_pp_lif_cos_profile_map_l2_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY    *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY     *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  key_lsb = ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2(
              map_key->outer_tpid,
              map_key->incoming_up,
              map_key->incoming_dei
            );

  res = arad_pp_lif_cos_profile_map_tbl_entry_set_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_l2_info_set_unsafe()", map_tbl_ndx, 0);
}

uint32
  arad_pp_lif_cos_profile_map_l2_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY    *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY     *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, ARAD_PP_LIF_COS_MAP_TBL_NDX_MIN, ARAD_PP_LIF_COS_MAP_TBL_NDX_MAX, ARAD_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY, map_key, 20, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY, map_value, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_l2_info_set_verify()", map_tbl_ndx, 0);
}

uint32
  arad_pp_lif_cos_profile_map_l2_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY    *map_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, ARAD_PP_LIF_COS_MAP_TBL_NDX_MIN, ARAD_PP_LIF_COS_MAP_TBL_NDX_MAX, ARAD_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY, map_key, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_l2_info_get_verify()", map_tbl_ndx, 0);
}


uint32
  arad_pp_lif_cos_profile_map_l2_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY      *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(map_value);

  key_lsb = ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2(
              map_key->outer_tpid,
              map_key->incoming_up,
              map_key->incoming_dei
            );

  res = arad_pp_lif_cos_profile_map_tbl_entry_get_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_l2_info_get_unsafe()", map_tbl_ndx, 0);
}


uint32
  arad_pp_lif_cos_profile_map_ip_info_set_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                    map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY      *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb,
    dscp_key,
    mem_val,
    dscp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  
  key_lsb = ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP(
              (map_key->ip_type == SOC_SAND_PP_IP_TYPE_IPV4 ?
               ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IPV4_PREFIX :
               ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IPV6_PREFIX),
              map_key->tos
            );

  res = arad_pp_lif_cos_profile_map_tbl_entry_set_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (map_tbl_ndx <= ARAD_PP_LIF_COS_MAP_TBL_NDX_4_LSB_MAX) {

      
      dscp_key = ((map_key->ip_type == SOC_SAND_PP_IP_TYPE_IPV4 ? 0 : 1) << 12) | (map_key->dscp_exp << 4) | map_tbl_ndx;

      res = READ_IHP_DSCP_EXP_REMARKm(unit, MEM_BLOCK_ANY, dscp_key, &mem_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      dscp = map_value->dscp;
      soc_mem_field_set(unit, IHP_DSCP_EXP_REMARKm, &mem_val, IN_DSCP_EXPf, &dscp);
      res = WRITE_IHP_DSCP_EXP_REMARKm(unit, MEM_BLOCK_ANY, dscp_key, &mem_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_ip_info_set_unsafe()", map_tbl_ndx, 0);
}


uint32
  arad_pp_lif_cos_profile_map_ip_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY    *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY     *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, ARAD_PP_LIF_COS_MAP_TBL_NDX_MIN, ARAD_PP_LIF_COS_MAP_TBL_NDX_MAX, ARAD_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY, map_key, 20, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY, map_value, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_ip_info_set_verify()", map_tbl_ndx, 0);
}

uint32
  arad_pp_lif_cos_profile_map_ip_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY    *map_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, ARAD_PP_LIF_COS_MAP_TBL_NDX_MIN, ARAD_PP_LIF_COS_MAP_TBL_NDX_MAX, ARAD_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY, map_key, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_ip_info_get_verify()", map_tbl_ndx, 0);
}


uint32
  arad_pp_lif_cos_profile_map_ip_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY    *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY     *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb,
    dscp_key,
    mem_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(map_value);

  
  key_lsb = ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP(
              (map_key->ip_type == SOC_SAND_PP_IP_TYPE_IPV4 ?
               ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IPV4_PREFIX :
               ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IPV6_PREFIX),
              map_key->tos
            );

  res = arad_pp_lif_cos_profile_map_tbl_entry_get_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (map_tbl_ndx <= ARAD_PP_LIF_COS_MAP_TBL_NDX_4_LSB_MAX) {

      
      dscp_key = ((map_key->ip_type == SOC_SAND_PP_IP_TYPE_IPV4 ? 0 : 1) << 12) | (map_key->tos << 4) | map_tbl_ndx;

      res = READ_IHP_DSCP_EXP_REMARKm(unit, MEM_BLOCK_ANY, dscp_key, &mem_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      soc_mem_field_get(unit, IHP_DSCP_EXP_REMARKm, &mem_val, IN_DSCP_EXPf, &(map_value->dscp));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_ip_info_get_unsafe()", map_tbl_ndx, 0);
}


uint32
  arad_pp_lif_cos_profile_map_mpls_label_info_set_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                    map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb,
    dscp_key,
    mem_val,
    dscp;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(map_key);
    SOC_SAND_CHECK_NULL_INPUT(map_value);

    key_lsb = ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS(
              map_key->in_exp
            );

    res = arad_pp_lif_cos_profile_map_tbl_entry_set_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   if (map_tbl_ndx <= ARAD_PP_LIF_COS_MAP_TBL_NDX_4_LSB_MAX) {

      
       dscp_key = (2 << 12) | (map_key->in_exp << 4) | map_tbl_ndx;

       res = READ_IHP_DSCP_EXP_REMARKm(unit, MEM_BLOCK_ANY, dscp_key, &mem_val);
       SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
       dscp = map_value->dscp;
       soc_mem_field_set(unit, IHP_DSCP_EXP_REMARKm, &mem_val, IN_DSCP_EXPf, &dscp);
       res = WRITE_IHP_DSCP_EXP_REMARKm(unit, MEM_BLOCK_ANY, dscp_key, &mem_val);
       SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
   }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_mpls_label_info_set_unsafe()", map_tbl_ndx, 0);
}

uint32
  arad_pp_lif_cos_profile_map_mpls_label_info_set_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                    map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, ARAD_PP_LIF_COS_MAP_TBL_NDX_MIN, ARAD_PP_LIF_COS_MAP_TBL_NDX_MAX, ARAD_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY, map_key, 20, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY, map_value, 30, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(map_value->dscp, SOC_SAND_PP_MPLS_DSCP_MAX, SOC_SAND_PP_DSCP_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_mpls_label_info_set_verify()", map_tbl_ndx, 0);
}

uint32
  arad_pp_lif_cos_profile_map_mpls_label_info_get_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                    map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, ARAD_PP_LIF_COS_MAP_TBL_NDX_MIN, ARAD_PP_LIF_COS_MAP_TBL_NDX_MAX, ARAD_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY, map_key, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_mpls_label_info_get_verify()", map_tbl_ndx, 0);
}


uint32
  arad_pp_lif_cos_profile_map_mpls_label_info_get_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                    map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb,
    dscp_key,
    mem_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(map_value);

  key_lsb = ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS(
              map_key->in_exp
            );

  res = arad_pp_lif_cos_profile_map_tbl_entry_get_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   if (map_tbl_ndx <= ARAD_PP_LIF_COS_MAP_TBL_NDX_4_LSB_MAX) {

      
       dscp_key = (2 << 12) | (map_key->in_exp << 4) | map_tbl_ndx;

       res = READ_IHP_DSCP_EXP_REMARKm(unit, MEM_BLOCK_ANY, dscp_key, &mem_val);
       SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
       soc_mem_field_get(unit, IHP_DSCP_EXP_REMARKm, &mem_val, IN_DSCP_EXPf, &(map_value->dscp));
   }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_mpls_label_info_get_unsafe()", map_tbl_ndx, 0);
}


uint32
  arad_pp_lif_cos_profile_map_tc_dp_info_set_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                    map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  key_lsb = ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP(
              map_key->tc,
              map_key->dp
            );

  res = arad_pp_lif_cos_profile_map_tbl_entry_set_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_tc_dp_info_set_unsafe()", map_tbl_ndx, 0);
}

uint32
  arad_pp_lif_cos_profile_map_tc_dp_info_set_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                    map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, ARAD_PP_LIF_COS_MAP_TBL_NDX_MIN, ARAD_PP_LIF_COS_MAP_TBL_NDX_MAX, ARAD_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY, map_key, 20, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY, map_value, 30, exit); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_tc_dp_info_set_verify()", map_tbl_ndx, 0);
}

uint32
  arad_pp_lif_cos_profile_map_tc_dp_info_get_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                    map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, ARAD_PP_LIF_COS_MAP_TBL_NDX_MIN, ARAD_PP_LIF_COS_MAP_TBL_NDX_MAX, ARAD_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY, map_key, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_tc_dp_info_get_verify()", map_tbl_ndx, 0);
}


uint32
  arad_pp_lif_cos_profile_map_tc_dp_info_get_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                    map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(map_value);

  key_lsb = ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP(
              map_key->tc,
              map_key->dp
            );

  res = arad_pp_lif_cos_profile_map_tbl_entry_get_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_profile_map_tc_dp_info_get_unsafe()", map_tbl_ndx, 0);
}


uint32
  arad_pp_lif_cos_opcode_types_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_TYPE               opcode_type
  )
{
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_OPCODE_TYPES_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_types_set_unsafe()", opcode_ndx, 0);
}

uint32
  arad_pp_lif_cos_opcode_types_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_TYPE                 opcode_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_OPCODE_TYPES_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_types_set_verify()", opcode_ndx, 0);
}

uint32
  arad_pp_lif_cos_opcode_types_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_OPCODE_TYPES_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_types_get_verify()", opcode_ndx, 0);
}


uint32
  arad_pp_lif_cos_opcode_types_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_TYPE               *opcode_type
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_OPCODE_TYPES_GET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_types_get_unsafe()", opcode_ndx, 0);
}


uint32
  arad_pp_lif_cos_opcode_ipv6_tos_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO        *action_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_ipv6_tos_map_set_unsafe()", opcode_ndx, ipv6_tos_ndx);
}

uint32
  arad_pp_lif_cos_opcode_ipv6_tos_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO        *action_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_ipv6_tos_map_set_verify()", opcode_ndx, ipv6_tos_ndx);
}

uint32
  arad_pp_lif_cos_opcode_ipv6_tos_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_ipv6_tos_map_get_verify()", opcode_ndx, ipv6_tos_ndx);
}


uint32
  arad_pp_lif_cos_opcode_ipv6_tos_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_ipv6_tos_map_get_unsafe()", opcode_ndx, ipv6_tos_ndx);
}


uint32
  arad_pp_lif_cos_opcode_ipv4_tos_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO        *action_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_ipv4_tos_map_set_unsafe()", opcode_ndx, ipv4_tos_ndx);
}

uint32
  arad_pp_lif_cos_opcode_ipv4_tos_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO        *action_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_ipv4_tos_map_set_verify()", opcode_ndx, ipv4_tos_ndx);
}

uint32
  arad_pp_lif_cos_opcode_ipv4_tos_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_ipv4_tos_map_get_verify()", opcode_ndx, ipv4_tos_ndx);
}


uint32
  arad_pp_lif_cos_opcode_ipv4_tos_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO        *action_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_ipv4_tos_map_get_unsafe()", opcode_ndx, ipv4_tos_ndx);
}


uint32
  arad_pp_lif_cos_opcode_tc_dp_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO        *action_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_tc_dp_map_set_unsafe()", opcode_ndx, tc_ndx);
}

uint32
  arad_pp_lif_cos_opcode_tc_dp_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO        *action_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_tc_dp_map_set_verify()", opcode_ndx, tc_ndx);
}

uint32
  arad_pp_lif_cos_opcode_tc_dp_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_tc_dp_map_get_verify()", opcode_ndx, tc_ndx);
}


uint32
  arad_pp_lif_cos_opcode_tc_dp_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_tc_dp_map_get_unsafe()", opcode_ndx, tc_ndx);
}


uint32
  arad_pp_lif_cos_opcode_vlan_tag_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_vlan_tag_map_set_unsafe()", opcode_ndx, 0);
}

uint32
  arad_pp_lif_cos_opcode_vlan_tag_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_vlan_tag_map_set_verify()", opcode_ndx, 0);
}

uint32
  arad_pp_lif_cos_opcode_vlan_tag_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_vlan_tag_map_get_verify()", opcode_ndx, 0);
}


uint32
  arad_pp_lif_cos_opcode_vlan_tag_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_cos_opcode_vlan_tag_map_get_unsafe()", opcode_ndx, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_lif_cos_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_lif_cos;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_lif_cos_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_lif_cos;
}

uint32
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_AC_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, ARAD_PP_LIF_COS_TYPE_MAX, ARAD_PP_LIF_COS_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->map_table, ARAD_PP_LIF_COS_MAP_TABLE_MIN, ARAD_PP_LIF_COS_MAP_TABLE_MAX, ARAD_PP_LIF_COS_MAP_TABLE_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_COS_AC_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PWE_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, ARAD_PP_LIF_COS_TYPE_MAX, ARAD_PP_LIF_COS_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->map_table, ARAD_PP_LIF_COS_MAP_TABLE_MAX, ARAD_PP_LIF_COS_MAP_TABLE_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_COS_PWE_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LIF_COS_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->forced_tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->forced_dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 15, exit);

  if (info->map_when_ip != info->map_when_mpls)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_MAP_WHEN_IP_MPLS_MISMATCH_ERR, 20, exit);
  }

#ifdef BCM_88660_A0
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->use_layer2_pcp, TRUE, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 24, exit);
#endif 

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_COS_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dscp, SOC_SAND_PP_L3_DSCP_MAX, SOC_SAND_PP_DSCP_OUT_OF_RANGE_ERR, 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_verify()",0,0);
}

uint32
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if ((info->ip_type != SOC_SAND_PP_IP_TYPE_IPV4) &&
      (info->ip_type != SOC_SAND_PP_IP_TYPE_IPV6))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_COS_IP_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_verify()",0,0);
}

uint32
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_exp, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_verify()",0,0);
}

uint32
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->outer_tpid, ARAD_PP_LIF_COS_OUTER_TPID_MAX, ARAD_PP_LIF_COS_COS_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->incoming_up, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_verify()",0,0);
}

uint32
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_verify()",0,0);
}

uint32
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


