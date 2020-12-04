/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_VSI_INCLUDED__

#define __ARAD_PP_VSI_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_vsi.h>






#define ARAD_PP_STP_TOPOLOGY_MIN                    (0)









typedef enum
{
  
  ARAD_PP_VSI_MAP_ADD = ARAD_PP_PROC_DESC_BASE_VSI_FIRST,
  ARAD_PP_VSI_MAP_ADD_PRINT,
  ARAD_PP_VSI_MAP_ADD_UNSAFE,
  ARAD_PP_VSI_SYS_TO_LOCAL_MAP_GET_UNSAFE,
  ARAD_PP_VSI_MAP_ADD_VERIFY,
  ARAD_PP_VSI_MAP_REMOVE,
  ARAD_PP_VSI_MAP_REMOVE_PRINT,
  ARAD_PP_VSI_MAP_REMOVE_UNSAFE,
  ARAD_PP_VSI_MAP_REMOVE_VERIFY,
  SOC_PPC_VSI_INFO_SET,
  SOC_PPC_VSI_INFO_SET_PRINT,
  SOC_PPC_VSI_INFO_SET_UNSAFE,
  SOC_PPC_VSI_INFO_SET_VERIFY,
  SOC_PPC_VSI_INFO_GET,
  SOC_PPC_VSI_INFO_GET_PRINT,
  SOC_PPC_VSI_INFO_GET_VERIFY,
  SOC_PPC_VSI_INFO_GET_UNSAFE,
  ARAD_PP_VSI_GET_PROCS_PTR,
  ARAD_PP_VSI_GET_ERRS_PTR,
  ARAD_PP_VSI_EGRESS_PROFILE_SET,
  ARAD_PP_VSI_EGRESS_PROFILE_SET_UNSAFE,
  ARAD_PP_VSI_EGRESS_PROFILE_SET_VERIFY,
  ARAD_PP_VSI_EGRESS_PROFILE_GET,
  ARAD_PP_VSI_EGRESS_PROFILE_GET_UNSAFE,
  ARAD_PP_VSI_EGRESS_PROFILE_GET_VERIFY,
  

  
  ARAD_PP_VSI_PROCEDURE_DESC_LAST
} ARAD_PP_VSI_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_VSI_SUCCESS_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_VSI_FIRST,
  ARAD_PP_VSI_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_MAC_LEARN_PROFILE_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_ORIENTATION_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_DA_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_EGRESS_PROFILE_INDEX_OUT_OF_RANGE_ERR,
  

  ARAD_PP_VSI_PROFILE_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_TRAP_CODE_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_MAP_NOT_SUPPORTED_ERR,

  
  ARAD_PP_VSI_ERR_LAST
} ARAD_PP_VSI_ERR;

typedef struct
{
  uint32 vdc_vsi_mapping_info; 
}ARAD_PP_VDC_VSI_MAP; 








  
uint32
  arad_pp_vsi_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_vsi_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PPC_VSI_INFO                            *vsi_info
  );

uint32
  arad_pp_vsi_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PPC_VSI_INFO                            *vsi_info
  );

uint32
  arad_pp_vsi_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx
  );


uint32
  arad_pp_vsi_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_PPC_VSI_INFO                            *vsi_info
  );


uint32
  arad_pp_vsi_egress_mtu_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                is_forwarding_mtu_filter,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_IN  uint32                               mtu_val
  );

uint32
  arad_pp_vsi_egress_mtu_set_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_IN  uint32                               mtu_val
  );

uint32
  arad_pp_vsi_egress_mtu_get_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx
  );


uint32
  arad_pp_vsi_egress_mtu_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                is_forwarding_mtu_filter,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_OUT  uint32                              *mtu_val
  );


uint32
  arad_pp_vsi_egress_mtu_profile_get(
     SOC_SAND_IN  int       unit,
     SOC_SAND_IN  uint8     is_forwarding_mtu_filter,
     SOC_SAND_IN  uint32    mtu_value,
     SOC_SAND_OUT uint32    *mtu_profile
  );


uint32
  arad_pp_vsi_egress_mtu_check_enable_set(
     SOC_SAND_IN int        unit,
     SOC_SAND_IN uint8      is_forwarding_mtu_filter,
     SOC_SAND_IN uint32     header_code,
     SOC_SAND_IN uint8      enable
  );


uint32
  arad_pp_vsi_egress_mtu_check_enable_get(
     SOC_SAND_IN  int        unit,
     SOC_SAND_IN  uint8      is_forwarding_mtu_filter,
     SOC_SAND_IN  uint32     header_code,
     SOC_SAND_OUT uint8      *enable
  );


uint32
  arad_pp_vsi_l2cp_trap_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_HANDLE_TYPE               handle_type
  );


uint32
  arad_pp_vsi_l2cp_trap_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_OUT SOC_PPC_VSI_L2CP_HANDLE_TYPE               *handle_type
  );

uint32
  arad_pp_vsi_l2cp_trap_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_HANDLE_TYPE               handle_type
  );

uint32
  arad_pp_vsi_l2cp_trap_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_KEY                       *l2cp_key
  );

uint32
  SOC_PPC_VSI_INFO_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PPC_VSI_INFO *info,
    SOC_SAND_IN  SOC_PPC_VSI_ID    vsi_ndx
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
