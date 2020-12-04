/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_TRAP_MGMT_INCLUDED__

#define __SOC_PPD_API_TRAP_MGMT_INCLUDED__




#include <soc/dpp/pkt.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>


#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>












typedef enum
{
  
  SOC_PPD_TRAP_FRWRD_PROFILE_INFO_SET = SOC_PPD_PROC_DESC_BASE_TRAP_MGMT_FIRST,
  SOC_PPD_TRAP_FRWRD_PROFILE_INFO_SET_PRINT,
  SOC_PPD_TRAP_FRWRD_PROFILE_INFO_GET,
  SOC_PPD_TRAP_FRWRD_PROFILE_INFO_GET_PRINT,
  SOC_PPD_TRAP_SNOOP_PROFILE_INFO_SET,
  SOC_PPD_TRAP_SNOOP_PROFILE_INFO_SET_PRINT,
  SOC_PPD_TRAP_SNOOP_PROFILE_INFO_GET,
  SOC_PPD_TRAP_SNOOP_PROFILE_INFO_GET_PRINT,
  SOC_PPD_TRAP_TO_EG_ACTION_MAP_SET,
  SOC_PPD_TRAP_TO_EG_ACTION_MAP_SET_PRINT,
  SOC_PPD_TRAP_TO_EG_ACTION_MAP_GET,
  SOC_PPD_TRAP_TO_EG_ACTION_MAP_GET_PRINT,
  SOC_PPD_TRAP_EG_PROFILE_INFO_SET,
  SOC_PPD_TRAP_EG_PROFILE_INFO_SET_PRINT,
  SOC_PPD_TRAP_EG_PROFILE_INFO_GET,
  SOC_PPD_TRAP_EG_PROFILE_INFO_GET_PRINT,
  SOC_PPD_TRAP_MACT_EVENT_GET,
  SOC_PPD_TRAP_MACT_EVENT_GET_PRINT,
  SOC_PPD_TRAP_MACT_EVENT_PARSE,
  SOC_PPD_TRAP_MACT_EVENT_PARSE_PRINT,
  SOC_PPD_TRAP_PACKET_PARSE,
  SOC_PPD_TRAP_PACKET_PARSE_PRINT,
  SOC_PPD_TRAP_MGMT_GET_PROCS_PTR,
  
  
  SOC_PPD_TRAP_MGMT_PROCEDURE_DESC_LAST
} SOC_PPD_TRAP_MGMT_PROCEDURE_DESC;










uint32
  soc_ppd_trap_frwrd_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO      *profile_info,
    SOC_SAND_IN  int                                    core_id
  );


uint32
  soc_ppd_trap_frwrd_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO      *profile_info
  );


uint32
  soc_ppd_trap_snoop_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_IN  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO      *profile_info
  );


uint32
  soc_ppd_trap_snoop_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO      *profile_info
  );


uint32
  soc_ppd_trap_to_eg_action_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                trap_type_bitmap_ndx,
    SOC_SAND_IN  uint32                                eg_action_profile
  );


uint32
  soc_ppd_trap_to_eg_action_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                trap_type_bitmap_ndx,
    SOC_SAND_OUT uint32                                *eg_action_profile
  );


uint32
  soc_ppd_trap_eg_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO         *profile_info
  );


uint32
  soc_ppd_trap_eg_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_OUT SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO         *profile_info
  );


uint32
  soc_ppd_trap_mact_event_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT uint32                                buff[SOC_PPC_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_OUT uint32                                *buff_len
  );


uint32
  soc_ppd_trap_mact_event_parse(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                buff[SOC_PPC_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_IN  uint32                                buff_len,
    SOC_SAND_OUT SOC_PPC_TRAP_MACT_EVENT_INFO                *mact_event
  );


uint32
  soc_ppd_trap_packet_parse(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                 *buff,
    SOC_SAND_IN  uint32                                buff_len,
    SOC_SAND_OUT SOC_PPC_TRAP_PACKET_INFO              *packet_info,
    SOC_SAND_OUT soc_pkt_t                              *dnx_pkt
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

