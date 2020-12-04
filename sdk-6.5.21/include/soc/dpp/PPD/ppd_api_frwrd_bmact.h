/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_FRWRD_BMACT_INCLUDED__

#define __SOC_PPD_API_FRWRD_BMACT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_frwrd_bmact.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_FRWRD_BMACT_BVID_INFO_SET = SOC_PPD_PROC_DESC_BASE_FRWRD_BMACT_FIRST,
  SOC_PPD_FRWRD_BMACT_BVID_INFO_SET_PRINT,
  SOC_PPD_FRWRD_BMACT_BVID_INFO_GET,
  SOC_PPD_FRWRD_BMACT_BVID_INFO_GET_PRINT,
  SOC_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET,
  SOC_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_PRINT,
  SOC_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET,
  SOC_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_PRINT,
  SOC_PPD_FRWRD_BMACT_ENTRY_ADD,
  SOC_PPD_FRWRD_BMACT_ENTRY_ADD_PRINT,
  SOC_PPD_FRWRD_BMACT_ENTRY_REMOVE,
  SOC_PPD_FRWRD_BMACT_ENTRY_REMOVE_PRINT,
  SOC_PPD_FRWRD_BMACT_ENTRY_GET,
  SOC_PPD_FRWRD_BMACT_ENTRY_GET_PRINT,
  SOC_PPD_FRWRD_BMACT_GET_PROCS_PTR,
  SOC_PPD_FRWRD_BMACT_INIT,
  SOC_PPD_FRWRD_BMACT_INIT_PRINT,
  SOC_PPD_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET,
  SOC_PPD_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET_PRINT,
  SOC_PPD_FRWRD_BMACT_EG_VLAN_PCP_MAP_GET,
  SOC_PPD_FRWRD_BMACT_EG_VLAN_PCP_MAP_GET_PRINT,
  
  
  SOC_PPD_FRWRD_BMACT_PROCEDURE_DESC_LAST
} SOC_PPD_FRWRD_BMACT_PROCEDURE_DESC;










uint32
  soc_ppd_frwrd_bmact_init(
    SOC_SAND_IN  int                               unit
  );


uint32
  soc_ppd_frwrd_bmact_bvid_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         bvid_ndx,
    SOC_SAND_IN  SOC_PPC_BMACT_BVID_INFO                     *bvid_info
  );


uint32
  soc_ppd_frwrd_bmact_bvid_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         bvid_ndx,
    SOC_SAND_OUT SOC_PPC_BMACT_BVID_INFO                     *bvid_info
  );


uint32
  soc_ppd_frwrd_bmact_pbb_te_bvid_range_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_BMACT_PBB_TE_VID_RANGE              *pbb_te_bvids
  );


uint32
  soc_ppd_frwrd_bmact_pbb_te_bvid_range_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_BMACT_PBB_TE_VID_RANGE              *pbb_te_bvids
  );


uint32
  soc_ppd_frwrd_bmact_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_KEY                     *bmac_key,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_INFO                    *bmact_entry_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_frwrd_bmact_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_KEY                     *bmac_key
  );


uint32
  soc_ppd_frwrd_bmact_entry_get(
    SOC_SAND_IN     int                               unit,
    SOC_SAND_INOUT  SOC_PPC_BMACT_ENTRY_KEY                     *bmac_key,
	SOC_SAND_OUT    SOC_PPC_BMACT_ENTRY_INFO                    *bmact_entry_info,
	SOC_SAND_OUT    uint8                               *found
  );
  

uint32
  soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  );


uint32
  soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

