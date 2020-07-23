/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_VSI_INCLUDED__

#define __SOC_PPD_API_VSI_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_vsi.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_VSI_MAP_ADD = SOC_PPD_PROC_DESC_BASE_VSI_FIRST,
  SOC_PPD_VSI_MAP_ADD_PRINT,
  SOC_PPD_VSI_MAP_REMOVE,
  SOC_PPD_VSI_MAP_REMOVE_PRINT,
  SOC_PPD_VSI_INFO_SET,
  SOC_PPD_VSI_INFO_SET_PRINT,
  SOC_PPD_VSI_INFO_GET,
  SOC_PPD_VSI_INFO_GET_PRINT,
  SOC_PPD_VSI_GET_PROCS_PTR,
  SOC_PPD_VSI_EGRESS_PROFILE_SET,
  SOC_PPD_VSI_EGRESS_PROFILE_SET_PRINT,
  SOC_PPD_VSI_EGRESS_PROFILE_GET,
  SOC_PPD_VSI_EGRESS_PROFILE_GET_PRINT,
  
  
  SOC_PPD_VSI_PROCEDURE_DESC_LAST
} SOC_PPD_VSI_PROCEDURE_DESC;










uint32
  soc_ppd_vsi_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PPC_VSI_INFO                            *vsi_info
  );


uint32
  soc_ppd_vsi_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_PPC_VSI_INFO                            *vsi_info
  );


uint32 soc_ppd_vsi_egress_mtu_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                is_forwarding_mtu_filter,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_IN  uint32                               mtu_val
  );


uint32
  soc_ppd_vsi_egress_mtu_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint8              is_forwarding_mtu_filter,
    SOC_SAND_IN  uint32             vsi_profile_ndx,
    SOC_SAND_OUT uint32             *mtu_val
  );


uint32
  soc_ppd_vsi_l2cp_trap_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_HANDLE_TYPE               handle_type
  );


uint32
  soc_ppd_vsi_l2cp_trap_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_OUT SOC_PPC_VSI_L2CP_HANDLE_TYPE               *handle_type
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

