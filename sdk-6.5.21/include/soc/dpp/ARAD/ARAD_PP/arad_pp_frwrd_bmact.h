/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_FRWRD_BMACT_INCLUDED__

#define __ARAD_PP_FRWRD_BMACT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>

#include <soc/dpp/PPC/ppc_api_frwrd_bmact.h>





#define ARAD_PP_LEM_ACCESS_ASD_LEARN_TYPE                     (16)
#define ARAD_PP_LEM_ACCESS_ASD_LEARN_TYPE_LEN                 (1)
#define ARAD_PP_LEM_ACCESS_ASD_LEARN_FEC_PTR                  (0)
#define ARAD_PP_LEM_ACCESS_ASD_DOMAIN                         (17)
#define ARAD_PP_LEM_ACCESS_ASD_DOMAIN_LEN                     (5)

#define ARAD_PP_MIM_VPN_INVALID(_unit_) ((SOC_IS_JERICHO_A0(_unit_) | SOC_IS_JERICHO_B0(_unit_) | SOC_IS_QMX(_unit_)) ? 0x7fff: 0xffff)









typedef enum
{
  
  ARAD_PP_FRWRD_BMACT_BVID_INFO_SET = ARAD_PP_PROC_DESC_BASE_FRWRD_BMACT_FIRST,
  ARAD_PP_FRWRD_BMACT_BVID_INFO_SET_PRINT,
  ARAD_PP_FRWRD_BMACT_BVID_INFO_SET_UNSAFE,
  ARAD_PP_FRWRD_BMACT_BVID_INFO_SET_VERIFY,
  ARAD_PP_FRWRD_BMACT_BVID_INFO_GET,
  ARAD_PP_FRWRD_BMACT_BVID_INFO_GET_PRINT,
  ARAD_PP_FRWRD_BMACT_BVID_INFO_GET_VERIFY,
  ARAD_PP_FRWRD_BMACT_BVID_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET,
  ARAD_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_PRINT,
  ARAD_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_UNSAFE,
  ARAD_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_VERIFY,
  ARAD_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET,
  ARAD_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_PRINT,
  ARAD_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_VERIFY,
  ARAD_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_UNSAFE,
  ARAD_PP_FRWRD_BMACT_ENTRY_ADD,
  ARAD_PP_FRWRD_BMACT_ENTRY_ADD_PRINT,
  ARAD_PP_FRWRD_BMACT_ENTRY_ADD_UNSAFE,
  ARAD_PP_FRWRD_BMACT_ENTRY_ADD_VERIFY,
  ARAD_PP_FRWRD_BMACT_ENTRY_REMOVE,
  ARAD_PP_FRWRD_BMACT_ENTRY_REMOVE_PRINT,
  ARAD_PP_FRWRD_BMACT_ENTRY_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_BMACT_ENTRY_REMOVE_VERIFY,
  ARAD_PP_FRWRD_BMACT_ENTRY_GET,
  ARAD_PP_FRWRD_BMACT_ENTRY_GET_PRINT,
  ARAD_PP_FRWRD_BMACT_ENTRY_GET_UNSAFE,
  ARAD_PP_FRWRD_BMACT_ENTRY_GET_VERIFY,
  ARAD_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET_UNSAFE,
  ARAD_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET_VERIFY,
  ARAD_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_GET_UNSAFE,
  ARAD_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_GET_VERIFY,
  ARAD_PP_FRWRD_BMACT_GET_PROCS_PTR,
  ARAD_PP_FRWRD_BMACT_GET_ERRS_PTR,
  ARAD_PP_FRWRD_BMACT_INIT,
  ARAD_PP_FRWRD_BMACT_INIT_UNSAFE,
  ARAD_PP_FRWRD_BMACT_INIT_PRINT,
  ARAD_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET,
  ARAD_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_GET,
  

  
  ARAD_PP_FRWRD_BMACT_PROCEDURE_DESC_LAST
} ARAD_PP_FRWRD_BMACT_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_FRWRD_BMACT_SUCCESS_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FRWRD_BMACT_FIRST,
  ARAD_PP_FRWRD_BMACT_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_BMACT_B_FID_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_BMACT_SYS_PORT_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_BMACT_I_SID_DOMAIN_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_BMACT_KEY_FLAGS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_BMACT_DA_NOT_FOUND_ACTION_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_BMACT_MAC_IN_MAC_CHECK_IF_ENABLED_ERR,
  ARAD_PP_FRWRD_BMACT_SA_AUTH_ENABLED_ERR,
  ARAD_PP_FRWRD_BMACT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR,
  

  
  ARAD_PP_FRWRD_BMACT_ERR_LAST
} ARAD_PP_FRWRD_BMACT_ERR;










uint32
  arad_pp_frwrd_bmact_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_is_mac_in_mac_enabled(
  SOC_SAND_IN  int     unit,
  SOC_SAND_OUT  uint8     *enabled
  );


uint32
  arad_pp_frwrd_bmact_mac_in_mac_enable(
    SOC_SAND_IN  int           unit
  );


uint32
  arad_pp_frwrd_bmact_bvid_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    bvid_ndx,
    SOC_SAND_IN  SOC_PPC_BMACT_BVID_INFO                *bvid_info
  );

uint32
  arad_pp_frwrd_bmact_bvid_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    bvid_ndx,
    SOC_SAND_IN  SOC_PPC_BMACT_BVID_INFO                *bvid_info
  );

uint32
  arad_pp_frwrd_bmact_bvid_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    bvid_ndx
  );


uint32
  arad_pp_frwrd_bmact_bvid_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    bvid_ndx,
    SOC_SAND_OUT SOC_PPC_BMACT_BVID_INFO                *bvid_info
  );


uint32
  arad_pp_frwrd_bmact_pbb_te_bvid_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_BMACT_PBB_TE_VID_RANGE         *pbb_te_bvids
  );

uint32
  arad_pp_frwrd_bmact_pbb_te_bvid_range_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_BMACT_PBB_TE_VID_RANGE         *pbb_te_bvids
  );

uint32
  arad_pp_frwrd_bmact_pbb_te_bvid_range_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_bmact_pbb_te_bvid_range_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_BMACT_PBB_TE_VID_RANGE         *pbb_te_bvids
  );


uint32
  arad_pp_frwrd_bmact_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_KEY                *bmac_key,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_INFO               *bmact_entry_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  );

uint32
  arad_pp_frwrd_bmact_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_KEY                *bmac_key,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_INFO               *bmact_entry_info
  );

uint32
  arad_pp_frwrd_bmact_key_parse(
    SOC_SAND_IN   int                                unit,
    SOC_SAND_IN   ARAD_PP_LEM_ACCESS_KEY                *key,
    SOC_SAND_OUT  SOC_PPC_BMACT_ENTRY_KEY               *bmac_key
  );

uint32
  arad_pp_frwrd_bmact_payload_convert(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             flags,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_PAYLOAD         *payload,
    SOC_SAND_OUT  SOC_PPC_BMACT_ENTRY_KEY           *bmac_key,
    SOC_SAND_OUT SOC_PPC_BMACT_ENTRY_INFO           *bmact_entry_info
  );


uint32
  arad_pp_frwrd_bmact_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_KEY                     *bmac_key
  );

uint32
  arad_pp_frwrd_bmact_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_KEY                     *bmac_key
  );


uint32
  arad_pp_frwrd_bmact_entry_get_unsafe(
    SOC_SAND_IN     int                                 unit,
    SOC_SAND_INOUT  SOC_PPC_BMACT_ENTRY_KEY                *bmac_key,
    SOC_SAND_OUT    SOC_PPC_BMACT_ENTRY_INFO               *bmact_entry_info,
    SOC_SAND_OUT    uint8                                  *found
  );

uint32
  arad_pp_frwrd_bmact_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_KEY                *bmac_key
  );

uint32
  arad_pp_frwrd_bmact_eg_vlan_pcp_map_set_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  );

uint32
  arad_pp_frwrd_bmact_eg_vlan_pcp_map_set_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
    );

uint32
  arad_pp_frwrd_bmact_eg_vlan_pcp_map_get_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  );

uint32
  arad_pp_frwrd_bmact_eg_vlan_pcp_map_get_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
    );

CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_bmact_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_bmact_get_errs_ptr(void);

uint32
  SOC_PPC_BMACT_BVID_INFO_verify(
    SOC_SAND_IN  SOC_PPC_BMACT_BVID_INFO *info
  );

uint32
  SOC_PPC_BMACT_PBB_TE_VID_RANGE_verify(
    SOC_SAND_IN  SOC_PPC_BMACT_PBB_TE_VID_RANGE *info
  );

uint32
  SOC_PPC_BMACT_ENTRY_KEY_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_KEY *info
  );

uint32
  SOC_PPC_BMACT_ENTRY_INFO_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
