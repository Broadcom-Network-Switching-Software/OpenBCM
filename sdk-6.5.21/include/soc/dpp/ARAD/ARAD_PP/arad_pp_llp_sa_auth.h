/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_LLP_SA_AUTH_INCLUDED__

#define __ARAD_PP_LLP_SA_AUTH_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>





#define ARAD_PP_SA_AUTH_ENTRY_IS_DYNAMIC (FALSE)
#define ARAD_PP_SA_AUTH_ENTRY_AGE (3)





#define ARAD_PP_LLP_SA_NO_AUTH(mac_based_info)    \
      (((mac_based_info)->expect_system_port.sys_id == SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_PORTS) &&    \
      ((mac_based_info)->expect_tag_vid == SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_VIDS) &&     \
      ((mac_based_info)->tagged_only == FALSE))

#define ARAD_PP_LLP_SA_SET_NO_AUTH(mac_based_info)    \
      (mac_based_info)->expect_system_port.sys_id = SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_PORTS;    \
      (mac_based_info)->expect_tag_vid = SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_VIDS;     \
      (mac_based_info)->tagged_only = FALSE;





typedef enum
{
  
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_SET = ARAD_PP_PROC_DESC_BASE_LLP_SA_AUTH_FIRST,
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_SET_PRINT,
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_SET_UNSAFE,
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_SET_VERIFY,
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_GET,
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_GET_PRINT,
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_GET_VERIFY,
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_GET_UNSAFE,
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_SET,
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_SET_PRINT,
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_SET_UNSAFE,
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_SET_VERIFY,
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_GET,
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_GET_PRINT,
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_GET_VERIFY,
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_GET_UNSAFE,
  ARAD_PP_LLP_SA_AUTH_GET_BLOCK,
  ARAD_PP_LLP_SA_AUTH_GET_BLOCK_PRINT,
  ARAD_PP_LLP_SA_AUTH_GET_BLOCK_UNSAFE,
  ARAD_PP_LLP_SA_AUTH_GET_BLOCK_VERIFY,
  ARAD_PP_LLP_SA_AUTH_GET_PROCS_PTR,
  ARAD_PP_LLP_SA_AUTH_GET_ERRS_PTR,
  
  ARAD_PP_SA_BASED_KEY_TO_LEM_KEY_MAP,
  ARAD_PP_SA_BASED_KEY_FROM_LEM_KEY_MAP,
  ARAD_PP_SA_BASED_PAYLOAD_FROM_LEM_PAYLOAD_MAP,

  
  ARAD_PP_LLP_SA_AUTH_PROCEDURE_DESC_LAST
} ARAD_PP_LLP_SA_AUTH_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_LLP_SA_AUTH_SUCCESS_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LLP_SA_AUTH_FIRST,
  ARAD_PP_LLP_SA_AUTH_EXPECT_SYSTEM_PORT_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_SA_AUTH_RULE_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_SA_AUTH_PORT_OUT_OF_RANGE_ERR,
  

  ARAD_PP_LLP_SA_AUTH_FEATURE_DISABLED_ERR,
  ARAD_PP_LLP_SA_AUTH_ACCEPT_ALL_ILLEGAL_ERR,
  ARAD_PP_LLP_SA_BASED_LEM_KEY_MISMATCH_ERR,
  ARAD_PP_LLP_AUTH_EN_VID_ASSIGN_DIS_ERR,
  
  ARAD_PP_LLP_SA_AUTH_ERR_LAST
} ARAD_PP_LLP_SA_AUTH_ERR;









uint32
  arad_pp_llp_sa_auth_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_sa_based_key_to_lem_key_map(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS        *mac_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *key
    );

uint32
  arad_pp_sa_based_key_from_lem_key_map(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *key,
      SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS        *mac_key
    );
uint32
  arad_pp_sa_based_trill_key_to_lem_key_map(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS          *mac_key,
      SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID          *expect_system_port,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY           *key
    );
uint32
  arad_pp_sa_based_trill_key_from_lem_key_map(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *key,
      SOC_SAND_OUT SOC_SAND_PP_SYS_PORT_ID        *expect_system_port,
      SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS        *mac_key
    );
uint32
  arad_pp_sa_based_payload_from_lem_payload_map(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_PAYLOAD        *lem_payload,
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_MAC_INFO      *auth_info,
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_MAC_INFO   *vid_assign_info
  );


uint32
  arad_pp_llp_sa_auth_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_PORT_INFO               *port_auth_info
  );

uint32
  arad_pp_llp_sa_auth_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_PORT_INFO               *port_auth_info
  );

uint32
  arad_pp_llp_sa_auth_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  );


uint32
  arad_pp_llp_sa_auth_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_PORT_INFO               *port_auth_info
  );


uint32
  arad_pp_llp_sa_auth_mac_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MAC_INFO                *mac_auth_info,
    SOC_SAND_IN  uint8                                 enable,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_llp_sa_auth_mac_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MAC_INFO                *mac_auth_info,
    SOC_SAND_IN  uint8                                 enable
  );

uint32
  arad_pp_llp_sa_auth_mac_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key
  );


uint32
  arad_pp_llp_sa_auth_mac_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_MAC_INFO                *mac_auth_info,
    SOC_SAND_OUT uint8                                 *enable
  );


uint32
  arad_pp_llp_sa_auth_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MATCH_RULE              *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key_arr,
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_MAC_INFO                *auth_info_arr,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_llp_sa_auth_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MATCH_RULE              *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_sa_auth_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_sa_auth_get_errs_ptr(void);

uint32
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_PORT_INFO *info
  );

uint32
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MAC_INFO *info
  );

uint32
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_verify(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MATCH_RULE *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

