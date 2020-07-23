/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_METERING_INCLUDED__

#define __SOC_PPD_API_METERING_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_metering.h>

#include <soc/dpp/PPD/ppd_api_general.h>















typedef enum
{
  
  SOC_PPD_MTR_METERS_GROUP_INFO_SET = SOC_PPD_PROC_DESC_BASE_METERING_FIRST,
  SOC_PPD_MTR_METERS_GROUP_INFO_SET_PRINT,
  SOC_PPD_MTR_METERS_GROUP_INFO_GET,
  SOC_PPD_MTR_METERS_GROUP_INFO_GET_PRINT,
  SOC_PPD_MTR_BW_PROFILE_ADD,
  SOC_PPD_MTR_BW_PROFILE_ADD_PRINT,
  SOC_PPD_MTR_BW_PROFILE_GET,
  SOC_PPD_MTR_BW_PROFILE_GET_PRINT,
  SOC_PPD_MTR_BW_PROFILE_REMOVE,
  SOC_PPD_MTR_BW_PROFILE_REMOVE_PRINT,
  SOC_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_SET,
  SOC_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_PRINT,
  SOC_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_GET,
  SOC_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_PRINT,
  SOC_PPD_MTR_ETH_POLICER_ENABLE_SET,
  SOC_PPD_MTR_ETH_POLICER_ENABLE_SET_PRINT,
  SOC_PPD_MTR_ETH_POLICER_ENABLE_GET,
  SOC_PPD_MTR_ETH_POLICER_ENABLE_GET_PRINT,
  SOC_PPD_MTR_ETH_POLICER_PARAMS_SET,
  SOC_PPD_MTR_ETH_POLICER_PARAMS_SET_PRINT,
  SOC_PPD_MTR_ETH_POLICER_PARAMS_GET,
  SOC_PPD_MTR_ETH_POLICER_PARAMS_GET_PRINT,
  SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET,
  SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_PRINT,
  SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_GET,
  SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_GET_PRINT,
  SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET,
  SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_PRINT,
  SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET,
  SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_PRINT,
  SOC_PPD_METERING_GET_PROCS_PTR,
  
  
  SOC_PPD_METERING_PROCEDURE_DESC_LAST
} SOC_PPD_METERING_PROCEDURE_DESC;










uint32
  soc_ppd_mtr_meters_group_info_set(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_GROUP_INFO                *mtr_group_info
  );


uint32
  soc_ppd_mtr_meters_group_info_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_GROUP_INFO                *mtr_group_info
  );


uint32
  soc_ppd_mtr_bw_profile_add(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_IN  uint32                                bw_profile_ndx, 
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO           *bw_profile_info,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO           *exact_bw_profile_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
  );


uint32
  soc_ppd_mtr_bw_profile_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_IN  uint32                                bw_profile_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO           *bw_profile_info
  );


uint32
  soc_ppd_mtr_bw_profile_remove(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_IN  uint32                                bw_profile_ndx
  );


uint32
  soc_ppd_mtr_meter_ins_to_bw_profile_map_set(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                  *meter_ins_ndx,
    SOC_SAND_IN  uint32                                bw_profile_id
  );


uint32
  soc_ppd_mtr_meter_ins_to_bw_profile_map_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                  *meter_ins_ndx,
    SOC_SAND_OUT uint32                                *bw_profile_id
  );


uint32
  soc_ppd_mtr_eth_policer_enable_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                               enable
  );


uint32
  soc_ppd_mtr_eth_policer_enable_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT uint8                               *enable
  );


uint32
  soc_ppd_mtr_eth_policer_params_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO                 *policer_info
  );


uint32
  soc_ppd_mtr_eth_policer_params_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO                 *policer_info
  );


uint32  
  soc_ppd_mtr_eth_policer_glbl_profile_set(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32                glbl_profile_idx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO *policer_info
  );


uint32  
  soc_ppd_mtr_eth_policer_glbl_profile_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_OUT uint32                glbl_profile_idx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO *policer_info
  );


uint32  
  soc_ppd_mtr_eth_policer_glbl_profile_map_set(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PPC_PORT                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_IN  uint32                glbl_profile_idx
  );


uint32  
  soc_ppd_mtr_eth_policer_glbl_profile_map_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PPC_PORT                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_OUT uint32                *glbl_profile_idx
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

