/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_LLP_VID_ASSIGN_INCLUDED__

#define __ARAD_PP_LLP_VID_ASSIGN_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/PPC/ppc_api_llp_vid_assign.h>






#define ARAD_PP_SW_DB_PP_PORTS_NOF_U32  (ARAD_PP_NOF_PORTS/32)









typedef struct ARAD_LLP_VID_ASSIGN_s
{
  SHR_BITDCL vid_sa_based_enable[ARAD_PP_SW_DB_PP_PORTS_NOF_U32];
} ARAD_LLP_VID_ASSIGN_t;

typedef enum
{
  
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_SET = ARAD_PP_PROC_DESC_BASE_LLP_VID_ASSIGN_FIRST,
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_SET_PRINT,
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_SET_UNSAFE,
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_SET_VERIFY,
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_GET,
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_GET_PRINT,
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_GET_VERIFY,
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_GET_UNSAFE,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_ADD,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_ADD_PRINT,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_ADD_UNSAFE,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_ADD_VERIFY,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_REMOVE,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_REMOVE_PRINT,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_REMOVE_UNSAFE,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_REMOVE_VERIFY,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_GET,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_GET_PRINT,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_GET_UNSAFE,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_GET_VERIFY,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_PRINT,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_UNSAFE,
  ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_VERIFY,
  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET,
  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_PRINT,
  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_UNSAFE,
  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_VERIFY,
  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET,
  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET_PRINT,
  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET_VERIFY,
  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET_UNSAFE,
  ARAD_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_SET,
  ARAD_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_PRINT,
  ARAD_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_UNSAFE,
  ARAD_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_VERIFY,
  ARAD_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_GET,
  ARAD_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_PRINT,
  ARAD_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_VERIFY,
  ARAD_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_UNSAFE,
  ARAD_PP_LLP_VID_ASSIGN_GET_PROCS_PTR,
  ARAD_PP_LLP_VID_ASSIGN_GET_ERRS_PTR,
  



  
  ARAD_PP_LLP_VID_ASSIGN_PROCEDURE_DESC_LAST
} ARAD_PP_LLP_VID_ASSIGN_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_LLP_VID_ASSIGN_SUCCESS_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LLP_VID_ASSIGN_FIRST,
  ARAD_PP_LLP_VID_ASSIGN_ENTRY_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_VID_ASSIGN_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_VID_ASSIGN_ETHER_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_VID_ASSIGN_RULE_TYPE_OUT_OF_RANGE_ERR,
  
   ARAD_PP_LLP_VID_ASSIGN_SUBNET_PREF_IS_ZERO_ERR,
   ARAD_PP_LLP_VID_ASSIGN_DIS_AUTH_EN_ERR,



  
  ARAD_PP_LLP_VID_ASSIGN_ERR_LAST
} ARAD_PP_LLP_VID_ASSIGN_ERR;









uint32
  arad_pp_llp_vid_assign_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_llp_vid_assign_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                         core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info
  );

uint32
  arad_pp_llp_vid_assign_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info
  );

uint32
  arad_pp_llp_vid_assign_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  );


uint32
  arad_pp_llp_vid_assign_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                         core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info
  );


uint32
  arad_pp_llp_vid_assign_mac_based_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_llp_vid_assign_mac_based_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info
  );


uint32
  arad_pp_llp_vid_assign_mac_based_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key
  );

uint32
  arad_pp_llp_vid_assign_mac_based_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key
  );


uint32
  arad_pp_llp_vid_assign_mac_based_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_llp_vid_assign_mac_based_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key
  );


uint32
  arad_pp_llp_vid_assign_mac_based_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE           *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key_arr,
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_MAC_INFO             *vid_assign_info_arr,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_llp_vid_assign_mac_based_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE           *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );


uint32
  arad_pp_llp_vid_assign_ipv4_subnet_based_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info
  );

uint32
  arad_pp_llp_vid_assign_ipv4_subnet_based_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info
  );

uint32
  arad_pp_llp_vid_assign_ipv4_subnet_based_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  );


uint32
  arad_pp_llp_vid_assign_ipv4_subnet_based_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info
  );


uint32
  arad_pp_llp_vid_assign_protocol_based_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_llp_vid_assign_protocol_based_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info
  );

uint32
  arad_pp_llp_vid_assign_protocol_based_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx
  );


uint32
  arad_pp_llp_vid_assign_protocol_based_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_vid_assign_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_vid_assign_get_errs_ptr(void);

uint32
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO *info
  );

uint32
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO *info
  );

uint32
  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO *info
  );

uint32
  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO *info
  );

uint32
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_verify(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

