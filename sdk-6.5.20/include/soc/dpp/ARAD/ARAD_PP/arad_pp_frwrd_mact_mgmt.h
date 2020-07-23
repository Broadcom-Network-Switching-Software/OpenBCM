/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifndef __ARAD_PP_FRWRD_MACT_MGMT_INCLUDED__

#define __ARAD_PP_FRWRD_MACT_MGMT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/PPC/ppc_api_frwrd_mact_mgmt.h>






#define ARAD_LEARNING_DMA_CHANNEL_USED SOC_MEM_FIFO_DMA_CHANNEL_0


#define ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES                                  (8)
#define ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED                              (2)


#define ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SIZE             (0x4000)
#define ARAD_PP_FRWRD_MACT_LIMIT_MAPPED_RANGE_SIZE          (0x4000)









typedef enum
{
  
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_SET = ARAD_PP_PROC_DESC_BASE_FRWRD_MACT_MGMT_FIRST,
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_SET_PRINT,
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_GET,
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_GET_PRINT,
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_GET_UNSAFE,
  SOC_PPC_FRWRD_MACT_AGING_INFO_SET,
  ARAD_PP_FRWRD_MACT_ONE_PASS_SET,
  SOC_PPC_FRWRD_MACT_AGING_INFO_SET_PRINT,
  SOC_PPC_FRWRD_MACT_AGING_INFO_SET_UNSAFE,
  ARAD_PP_FRWRD_MACT_AGING_ONE_PASS_SET_UNSAFE,
  SOC_PPC_FRWRD_MACT_AGING_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_MACT_AGING_INFO_GET,
  SOC_PPC_FRWRD_MACT_AGING_INFO_GET_PRINT,
  SOC_PPC_FRWRD_MACT_AGING_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_MACT_AGING_INFO_GET_UNSAFE,
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET,
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_PRINT,
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET,
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_PRINT,
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_UNSAFE,
  ARAD_PP_AGING_NUM_OF_CYCLES_GET,
  ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET,
  ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_PRINT,
  ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_UNSAFE,
  ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_VERIFY,
  ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET,
  ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_PRINT,
  ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_VERIFY,
  ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_UNSAFE,
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET,
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_PRINT,
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET,
  ARAD_PP_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_SET,
  ARAD_PP_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_SET_PRINT,
  ARAD_PP_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_SET_UNSAFE,
  ARAD_PP_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_GET,
  ARAD_PP_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_GET_UNSAFE,
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_PRINT,
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET,
  ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_PRINT,
  ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_UNSAFE,
  ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_VERIFY,
  ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET,
  ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_PRINT,
  ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_VERIFY,
  ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_UNSAFE,
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET,
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_PRINT,
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_UNSAFE,
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_VERIFY,
  ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET,
  ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_PRINT,
  ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_UNSAFE,
  ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_VERIFY,
  ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET,
  ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_PRINT,
  ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_VERIFY,
  ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_UNSAFE,
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_SET,
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_SET_PRINT,
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_GET,
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_GET_PRINT,
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_GET_UNSAFE,
  SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET,
  SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_PRINT,
  SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET,
  SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_PRINT,
  SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET,
  ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_PRINT,
  ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_UNSAFE,
  ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_VERIFY,
  ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET,
  ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_PRINT,
  ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_VERIFY,
  ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_UNSAFE,
  SOC_PPC_FRWRD_MACT_PORT_INFO_SET,
  SOC_PPC_FRWRD_MACT_PORT_INFO_SET_PRINT,
  SOC_PPC_FRWRD_MACT_PORT_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_MACT_PORT_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_MACT_PORT_INFO_GET,
  SOC_PPC_FRWRD_MACT_PORT_INFO_GET_PRINT,
  SOC_PPC_FRWRD_MACT_PORT_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_MACT_PORT_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_MACT_TRAP_INFO_SET,
  ARAD_PP_FRWRD_MACT_TRAP_INFO_SET_PRINT,
  ARAD_PP_FRWRD_MACT_TRAP_INFO_SET_UNSAFE,
  ARAD_PP_FRWRD_MACT_TRAP_INFO_SET_VERIFY,
  ARAD_PP_FRWRD_MACT_TRAP_INFO_GET,
  ARAD_PP_FRWRD_MACT_TRAP_INFO_GET_PRINT,
  ARAD_PP_FRWRD_MACT_TRAP_INFO_GET_VERIFY,
  ARAD_PP_FRWRD_MACT_TRAP_INFO_GET_UNSAFE,
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET,
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_PRINT,
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET,
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_PRINT,
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_MACT_EVENT_GET,
  ARAD_PP_FRWRD_MACT_EVENT_GET_PRINT,
  ARAD_PP_FRWRD_MACT_EVENT_GET_UNSAFE,
  ARAD_PP_FRWRD_MACT_EVENT_GET_VERIFY,
  ARAD_PP_FRWRD_MACT_EVENT_PARSE,
  ARAD_PP_FRWRD_MACT_EVENT_PARSE_PRINT,
  ARAD_PP_FRWRD_MACT_EVENT_PARSE_UNSAFE,
  ARAD_PP_FRWRD_MACT_EVENT_PARSE_VERIFY,
  ARAD_PP_FRWRD_MACT_MGMT_GET_PROCS_PTR,
  ARAD_PP_FRWRD_MACT_MGMT_GET_ERRS_PTR,
  

   ARAD_PP_FRWRD_MACT_MAX_AGE_GET,
   ARAD_PP_FRWRD_MACT_AGE_CONF_DEFAULT_GET,
   ARAD_PP_FRWRD_MACT_AGE_CONF_WRITE,
   ARAD_PP_FRWRD_MACT_AGE_MODIFY_GET,
   ARAD_PP_FRWRD_MACT_AGE_CONF_READ,
   ARAD_PP_FRWRD_MACT_EVENT_KEY_INDEX_GET,
   SOC_PPC_FRWRD_MACT_LEARN_MSGS_OLP_MSG_SET,
   SOC_PPC_FRWRD_MACT_LEARN_MSGS_OLP_MSG_GET,
   ARAD_PP_FRWRD_MACT_EVENT_WRITE,
   SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_VERIFY,
   ARAD_PP_FRWRD_MACT_INIT,
   ARAD_PP_FRWRD_MACT_MGMT_INIT,
   ARAD_PP_FRWRD_MACT_REGS_INIT,
   ARAD_PP_FRWRD_MACT_IS_INGRESS_LEARNING_GET,
   SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF_GET,
   SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE,
   SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_UNSAFE,
   SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_SET,
   SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_SET_UNSAFE, 
   ARAD_PP_FRWRD_MACT_MIM_INIT_SET,
   ARAD_PP_FRWRD_MACT_MIM_INIT_GET,
   ARAD_PP_FRWRD_MACT_ROUTED_LEARNING_SET,
   ARAD_PP_FRWRD_MACT_ROUTED_LEARNING_GET,
  
  ARAD_PP_FRWRD_MACT_MGMT_PROCEDURE_DESC_LAST
} ARAD_PP_FRWRD_MACT_MGMT_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_FRWRD_MACT_FID_PROFILE_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FRWRD_MACT_MGMT_FIRST,
  ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PPC_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
  SOC_PPC_FRWRD_MACT_EVENT_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_VSI_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_HEADER_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR,
  SOC_PPC_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR,
  SOC_PPC_FRWRD_MACT_SHADOW_MODE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_MGMT_BUFF_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR,
  

  ARAD_PP_FRWRD_MACT_MGMT_SEC_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_MAC_LIMIT_FID_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_MAC_LIMIT_GENERATE_EVENT_OUT_OF_RANGE_ERR,
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_IS_LAG_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_ACTION_TRAP_CODE_LSB_INVALID_ERR,
  ARAD_PP_FRWRD_MACT_FID_FAIL_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_FIFO_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_GLBL_LIMIT_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_FID_LIMIT_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_FID_LIMIT_DIS_ERR,
  ARAD_PP_FRWRD_MACT_MSG_LEN_ERR,
  ARAD_PP_FRWRD_MACT_MSG_IS_NOT_LEARN_MSG_ERR,
  ARAD_PP_FRARD_MACT_FRWRD_MACT_LOOKUP_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_SA_LOOKUP_MIM_AND_SA_AUTH_CANNOT_COEXIST_ERR,
  ARAD_PP_FRWRD_MACT_MIM_IS_NOT_ENABLED_ERR,
  ARAD_PP_FRWRD_MACT_FID_AGING_PROFILE_OUT_OF_RANGE_ERR,

  
  ARAD_PP_FRWRD_MACT_MGMT_ERR_LAST
} ARAD_PP_FRWRD_MACT_MGMT_ERR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 deleted;
  
  uint8 aged_out;
  
  uint8 refreshed;

} ARAD_PP_FRWRD_MACT_AGING_EVENT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
   ARAD_PP_FRWRD_MACT_AGING_EVENT age_action[ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES][ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

} ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 age_delete[ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

  
  uint32 age_aged_out[ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

  
  uint32 age_refresh[ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

} ARAD_PP_FRWRD_MACT_AGING_MODIFICATION;










soc_error_t arad_pp_frwrd_mact_is_dma_supported(SOC_SAND_IN int unit, SOC_SAND_OUT uint32 *dma_supported);



uint32
  arad_pp_frwrd_mact_is_age_resolution_low(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint8 *is_age_resolution_low
  );

uint32
  arad_pp_frwrd_mact_mgmt_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint8
  arad_pp_frwrd_mact_mgmt_is_b0_high_resolution(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_frwrd_mact_lookup_type_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_LOOKUP_TYPE         lookup_type,
    SOC_SAND_IN  uint8                                  learn_enable 
  );


soc_error_t
  arad_pp_frwrd_mact_cpu_counter_learn_limit_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  uint8         disable
  );


soc_error_t
  arad_pp_frwrd_mact_cpu_counter_learn_limit_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT  uint8        *is_enabled
  );


soc_error_t
  arad_pp_frwrd_mact_transplant_static_set (
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint8   enable
  );


soc_error_t
  arad_pp_frwrd_mact_transplant_static_get (
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint8*  is_enabled
  );

uint32
  arad_pp_frwrd_mact_oper_mode_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );

uint32
  arad_pp_frwrd_mact_oper_mode_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );


uint32
  arad_pp_frwrd_mact_oper_mode_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );


uint32
  arad_pp_frwrd_mact_aging_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_INFO               *aging_info
  );

uint32
  arad_pp_frwrd_mact_aging_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_INFO               *aging_info
  );


uint32
  arad_pp_frwrd_mact_age_conf_write(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  uint8                       aging_cfg_ptr,
      SOC_SAND_IN  ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE  *conf
    );


uint32
  arad_pp_frwrd_mact_aging_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_AGING_INFO               *aging_info
  );



uint32
  arad_pp_frwrd_mact_aging_one_pass_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO   *pass_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
  );




uint32
  arad_pp_frwrd_mact_aging_events_handle_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );

uint32
  arad_pp_frwrd_mact_aging_events_handle_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );


uint32
  arad_pp_frwrd_mact_aging_events_handle_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );

soc_error_t arad_pp_aging_num_of_cycles_get (
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  aging_profile,
    SOC_SAND_OUT int     *fid_aging_cycles
);

uint32
  arad_pp_frwrd_mact_fid_profile_to_fid_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_IN  SOC_PPC_FID                                 fid
  );

uint32
  arad_pp_frwrd_mact_fid_profile_to_fid_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_IN  SOC_PPC_FID                                 fid
  );

uint32
  arad_pp_frwrd_mact_fid_profile_to_fid_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx
  );


uint32
  arad_pp_frwrd_mact_fid_profile_to_fid_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_OUT SOC_PPC_FID                                 *fid
  );


uint32
  arad_pp_frwrd_mact_mac_limit_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );

uint32
  arad_pp_frwrd_mact_mac_limit_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );


uint32
  arad_pp_frwrd_mact_mac_limit_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );


uint32
  arad_pp_frwrd_mact_learn_profile_limit_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  );

uint32
  arad_pp_frwrd_mact_learn_profile_limit_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  );

uint32
  arad_pp_frwrd_mact_learn_profile_limit_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx
  );


uint32
  arad_pp_frwrd_mact_learn_profile_limit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  );


uint32
  arad_pp_frwrd_mact_event_handle_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  );

uint32
  arad_pp_frwrd_mact_event_handle_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  );

uint32
  arad_pp_frwrd_mact_event_handle_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx
  );


uint32
  arad_pp_frwrd_mact_event_handle_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT uint32                                  *event_handle_profile
  );


uint32
  arad_pp_frwrd_mact_fid_aging_profile_set_unsafe(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32         mac_learn_profile_ndx,
    SOC_SAND_IN  uint32         fid_aging_profile
  );

uint32
  arad_pp_frwrd_mact_fid_aging_profile_set_verify(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32         mac_learn_profile_ndx,
    SOC_SAND_IN  uint32         fid_aging_profile
  );

uint32
  arad_pp_frwrd_mact_fid_aging_profile_get_verify(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32         mac_learn_profile_ndx
  );


uint32
  arad_pp_frwrd_mact_fid_aging_profile_get_unsafe(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32         mac_learn_profile_ndx,
    SOC_SAND_OUT uint32        *fid_aging_profile
  );


uint32
  arad_pp_frwrd_mact_event_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_BUFFER            *event_buf
  );



uint32
  arad_pp_frwrd_mact_event_handle_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  );

uint32
  arad_pp_frwrd_mact_event_handle_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  );

uint32
  arad_pp_frwrd_mact_event_handle_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key
  );


uint32
  arad_pp_frwrd_mact_event_handle_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  );


uint32
  arad_pp_frwrd_mact_learn_msgs_distribution_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

uint32
  arad_pp_frwrd_mact_learn_msgs_distribution_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );


uint32
  arad_pp_frwrd_mact_learn_msgs_distribution_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );


uint32
  arad_pp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

uint32
  arad_pp_frwrd_mact_shadow_msgs_distribution_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );


uint32
  arad_pp_frwrd_mact_shadow_msgs_distribution_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );


uint32
  arad_pp_frwrd_mact_mac_limit_exceeded_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO  *exceed_info
  );




uint32
  arad_pp_frwrd_mact_mac_limit_range_map_info_get_unsafe(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  int8                                           range_num,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO    *map_info
  );

uint32
  arad_pp_frwrd_mact_mac_limit_range_map_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int8                                   range_num
  );


uint32
  arad_pp_frwrd_mact_mac_limit_mapping_info_get_unsafe(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO      *map_info
  );


uint32
  arad_pp_frwrd_mact_limit_mapped_val_to_table_index_get_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  uint32                                 mapped_val,
      SOC_SAND_OUT uint32                                 *limit_tbl_idx,
      SOC_SAND_OUT uint32                                 *is_reserved
    );



uint32
  arad_pp_frwrd_mact_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_PORT_INFO                *port_info
  );

uint32
  arad_pp_frwrd_mact_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_PORT_INFO                *port_info
  );

uint32
  arad_pp_frwrd_mact_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  );


uint32
  arad_pp_frwrd_mact_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_PORT_INFO                *port_info
  );


uint32
  arad_pp_frwrd_mact_trap_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                      *action_profile
  );

uint32
  arad_pp_frwrd_mact_trap_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                      *action_profile
  );

uint32
  arad_pp_frwrd_mact_trap_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx
  );


uint32
  arad_pp_frwrd_mact_trap_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_OUT SOC_PPC_ACTION_PROFILE                      *action_profile
  );


uint32
  arad_pp_frwrd_mact_ip_compatible_mc_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  );

uint32
  arad_pp_frwrd_mact_ip_compatible_mc_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  );


uint32
  arad_pp_frwrd_mact_ip_compatible_mc_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  );



uint32
  arad_pp_frwrd_mact_event_parse_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_BUFFER            *event_buf,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_INFO              *mact_event
  );

uint32
  arad_pp_frwrd_mact_learn_msg_parse_unsafe(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN   SOC_PPC_FRWRD_MACT_LEARN_MSG                  *learn_msg,
    SOC_SAND_IN   SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF             *learn_msg_conf,
    SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO       *learn_events
  );

uint32
arad_pp_frwrd_mact_learn_msg_conf_get_unsafe(
  SOC_SAND_IN   int                             unit,
  SOC_SAND_IN   SOC_PPC_FRWRD_MACT_LEARN_MSG            *learn_msg,
  SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF       *learn_msg_conf
  );

uint32
  arad_pp_frwrd_mact_mim_init_set_unsafe(
      SOC_SAND_IN   int                                   unit,
      SOC_SAND_IN   uint8                                    mim_initialized
  );

uint32
  arad_pp_frwrd_mact_mim_init_get_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_OUT  uint8                            *mim_initialized
  );


uint32
  arad_pp_frwrd_mact_routed_learning_set_unsafe(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  uint32                appFlags
  );


uint32
  arad_pp_frwrd_mact_routed_learning_get_unsafe(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_OUT uint32                *appFlags
  );

uint32
  arad_pp_frwrd_mact_event_parse_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_BUFFER            *event_buf
  );
void
  ARAD_PP_FRWRD_MACT_AGING_MODIFICATION_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_AGING_MODIFICATION  *info
  );

void
  ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE  *info
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_mact_mgmt_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_mact_mgmt_get_errs_ptr(void);

uint32
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_AGING_INFO_verify(
    SOC_SAND_IN   int                                   unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY *info
  );

uint32
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  );

uint32
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_verify(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_verify(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_PORT_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_EVENT_INFO_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_EVENT_BUFFER_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_BUFFER *info
  );

soc_error_t arad_pp_frwrd_mact_event_handle_info_set_dma(SOC_SAND_IN int unit, SOC_SAND_IN int enable_dma);

soc_error_t arad_pp_frwrd_mact_learning_dma_set( SOC_SAND_IN  int unit );

soc_error_t arad_pp_frwrd_mact_learning_dma_unset(SOC_SAND_IN int unit);

void arad_pp_frwrd_mact_learning_dma_event_handler(
      SOC_SAND_INOUT  void  *unit_ptr,
      SOC_SAND_INOUT  void  *event_type_ptr,
      SOC_SAND_INOUT  void  *cmc_ptr,
      SOC_SAND_INOUT  void  *ch_ptr,
      SOC_SAND_INOUT  void  *unused4);

soc_error_t arad_pp_frwrd_mact_clear_access_bit(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 fid,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS             *mac
  );

soc_error_t arad_pp_frwrd_mact_opport_mode_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT  uint32                *opport
  );

extern volatile sal_sem_t     jer_mact_fifo_sem[SOC_SAND_MAX_DEVICE];




#include <soc/dpp/SAND/Utils/sand_footer.h>




#endif
