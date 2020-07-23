/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/


#ifndef __SOC_PPD_API_FRWRD_MACT_MGMT_INCLUDED__

#define __SOC_PPD_API_FRWRD_MACT_MGMT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_frwrd_mact_mgmt.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>












typedef enum
{
  
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_SET = SOC_PPD_PROC_DESC_BASE_FRWRD_MACT_MGMT_FIRST,
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_GET,
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_AGING_INFO_SET,
  SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_SET,
  SOC_PPD_FRWRD_MACT_AGING_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_AGING_INFO_GET,
  SOC_PPD_FRWRD_MACT_AGING_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET,
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET,
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET,
  SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_PRINT,
  SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET,
  SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_PRINT,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_SET,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_GET,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET,
  SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET,
  SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_PRINT,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_PRINT,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET,
  SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET,
  SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET,
  SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET,
  SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_PORT_INFO_SET,
  SOC_PPD_FRWRD_MACT_PORT_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_PORT_INFO_GET,
  SOC_PPD_FRWRD_MACT_PORT_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_TRAP_INFO_SET,
  SOC_PPD_FRWRD_MACT_TRAP_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_TRAP_INFO_GET,
  SOC_PPD_FRWRD_MACT_TRAP_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET,
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET,
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_EVENT_GET,
  SOC_PPD_FRWRD_MACT_EVENT_GET_PRINT,
  SOC_PPD_FRWRD_MACT_EVENT_PARSE,
  SOC_PPD_FRWRD_MACT_EVENT_PARSE_PRINT,
  SOC_PPD_FRWRD_MACT_MGMT_GET_PROCS_PTR,
  SOC_PPD_FRWRD_MACT_MIM_INIT_SET,
  SOC_PPD_FRWRD_MACT_MIM_INIT_GET,
  SOC_PPD_FRWRD_MACT_ROUTED_LEARNING_SET,
  SOC_PPD_FRWRD_MACT_ROUTED_LEARNING_GET,
  

  SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE,
  SOC_PPD_FRWRD_MACT_LOOKUP_TYPE_SET,

  
  SOC_PPD_FRWRD_MACT_MGMT_PROCEDURE_DESC_LAST
} SOC_PPD_FRWRD_MACT_MGMT_PROCEDURE_DESC;











uint32
  soc_ppd_frwrd_mact_lookup_type_set(      
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_LOOKUP_TYPE         lookup_type
  );


uint32
  soc_ppd_frwrd_mact_oper_mode_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );


uint32
  soc_ppd_frwrd_mact_oper_mode_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );



uint32
  soc_ppd_frwrd_mact_fid_aging_profile_set(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       mac_learn_profile_ndx,
    SOC_SAND_IN  uint32       fid_aging_profile
  );


uint32
  soc_ppd_frwrd_mact_fid_aging_profile_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       mac_learn_profile_ndx,
    SOC_SAND_OUT uint32      *fid_aging_profile
  );



uint32
  soc_ppd_frwrd_mact_aging_profile_config(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       fid_aging_profile,
    SOC_SAND_IN uint32        fid_aging_cycles
  );


uint32
  soc_ppd_frwrd_mact_aging_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_INFO               *aging_info
  );


uint32
  soc_ppd_frwrd_mact_aging_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_AGING_INFO               *aging_info
  );



uint32
  soc_ppd_frwrd_mact_aging_one_pass_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO   *pass_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
  );



uint32
  soc_ppd_frwrd_mact_aging_events_handle_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );


uint32
  soc_ppd_frwrd_mact_aging_events_handle_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );


uint32
  soc_ppd_frwrd_mact_fid_profile_to_fid_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                fid_profile_ndx,
    SOC_SAND_IN  SOC_PPC_FID                                 fid
  );


uint32
  soc_ppd_frwrd_mact_fid_profile_to_fid_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                fid_profile_ndx,
    SOC_SAND_OUT SOC_PPC_FID                                 *fid
  );


uint32
  soc_ppd_frwrd_mact_mac_limit_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );


uint32
  soc_ppd_frwrd_mact_mac_limit_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );


uint32
  soc_ppd_frwrd_mact_learn_profile_limit_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  );


uint32
  soc_ppd_frwrd_mact_learn_profile_limit_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                mac_learn_profile_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  );


uint32
  soc_ppd_frwrd_mact_mac_limit_exceeded_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO  *exceed_info
  );


uint32
  soc_ppd_frwrd_mact_event_handle_profile_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                               mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                               event_handle_profile
  );


uint32
  soc_ppd_frwrd_mact_event_handle_profile_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                               mac_learn_profile_ndx,
    SOC_SAND_OUT uint32                               *event_handle_profile
  );


uint32
  soc_ppd_frwrd_mact_event_handle_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY        *event_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO       *handle_info
  );


uint32
  soc_ppd_frwrd_mact_event_handle_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY        *event_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO       *handle_info
  );


uint32
  soc_ppd_frwrd_mact_learn_msgs_distribution_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO          *distribution_info
  );


uint32
  soc_ppd_frwrd_mact_learn_msgs_distribution_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO          *distribution_info
  );


uint32
  soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO          *distribution_info
  );


uint32
  soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO          *distribution_info
  );


uint32
  soc_ppd_frwrd_mact_port_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  SOC_PPC_PORT                               local_port_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_PORT_INFO               *port_info
  );


uint32
  soc_ppd_frwrd_mact_port_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  SOC_PPC_PORT                               local_port_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_PORT_INFO               *port_info
  );


uint32
  soc_ppd_frwrd_mact_trap_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE               trap_type_ndx,
    SOC_SAND_IN  uint32                               port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                     *action_profile
  );


uint32
  soc_ppd_frwrd_mact_trap_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE               trap_type_ndx,
    SOC_SAND_IN  uint32                               port_profile_ndx,
    SOC_SAND_OUT SOC_PPC_ACTION_PROFILE                     *action_profile
  );


uint32
  soc_ppd_frwrd_mact_ip_compatible_mc_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO   *info
  );


uint32
  soc_ppd_frwrd_mact_ip_compatible_mc_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO   *info
  );


uint32
  soc_ppd_frwrd_mact_event_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_BUFFER            *event_buf
  );


uint32
  soc_ppd_frwrd_mact_event_parse(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_BUFFER            *event_buf,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_INFO              *mact_event
  );



uint32
  soc_ppd_frwrd_mact_learn_msg_parse(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_LEARN_MSG                 *learn_msg,
    SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO     *learn_events
  );



uint32
  soc_ppd_frwrd_mact_mim_init_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN   uint8                              mim_initialized
                                  );

uint32
  soc_ppd_frwrd_mact_mim_init_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT uint8                               *mim_initialized
                                  );


uint32
  soc_ppd_frwrd_mact_routed_learning_set(
    SOC_SAND_IN  int                             unit, 
    SOC_SAND_IN  uint32                             appFlags
                                        );


uint32
  soc_ppd_frwrd_mact_routed_learning_get(
    SOC_SAND_IN  int                             unit, 
    SOC_SAND_OUT uint32                             *appFlags
                                        );



uint32
  soc_ppd_frwrd_mact_mac_limit_range_map_info_get(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  int8                                           range_num,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO    *map_info
  );


uint32
  soc_ppd_frwrd_mact_mac_limit_mapping_info_get(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO      *map_info
  );


uint32
  soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mapped_val,
    SOC_SAND_OUT uint32                                 *limit_tbl_idx,
    SOC_SAND_OUT uint32                                 *is_reserved
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

