/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_LLP_PARSE_INCLUDED__

#define __ARAD_PP_LLP_PARSE_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_llp_parse.h>






#define ARAD_PP_LLP_PARSE_VLAN_COMPRESSION_OUTER_COMP_NDX        (0)
#define ARAD_PP_LLP_PARSE_VLAN_COMPRESSION_INNER_COMP_NDX        (1)











typedef enum
{
  
  SOC_PPC_LLP_PARSE_TPID_VALUES_SET = ARAD_PP_PROC_DESC_BASE_LLP_PARSE_FIRST,
  SOC_PPC_LLP_PARSE_TPID_VALUES_SET_PRINT,
  SOC_PPC_LLP_PARSE_TPID_VALUES_SET_UNSAFE,
  SOC_PPC_LLP_PARSE_TPID_VALUES_SET_VERIFY,
  SOC_PPC_LLP_PARSE_TPID_VALUES_GET,
  SOC_PPC_LLP_PARSE_TPID_VALUES_GET_PRINT,
  SOC_PPC_LLP_PARSE_TPID_VALUES_GET_VERIFY,
  SOC_PPC_LLP_PARSE_TPID_VALUES_GET_UNSAFE,
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_SET,
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_SET_PRINT,
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_SET_UNSAFE,
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_SET_VERIFY,
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_GET,
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_GET_PRINT,
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_GET_VERIFY,
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_GET_UNSAFE,
  ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET,
  ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PRINT,
  ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_UNSAFE,
  ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_VERIFY,
  ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET,
  ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_PRINT,
  ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_VERIFY,
  ARAD_PP_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_UNSAFE,
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_SET,
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_SET_PRINT,
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_SET_UNSAFE,
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_SET_VERIFY,
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_GET,
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_GET_PRINT,
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_GET_VERIFY,
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_GET_UNSAFE,
  ARAD_PP_LLP_PARSE_GET_PROCS_PTR,
  ARAD_PP_LLP_PARSE_GET_ERRS_PTR,
  



  
  ARAD_PP_LLP_PARSE_PROCEDURE_DESC_LAST
} ARAD_PP_LLP_PARSE_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_LLP_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LLP_PARSE_FIRST,
  ARAD_PP_LLP_PARSE_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_PARSE_TPID_PROFILE_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_PARSE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_PARSE_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_PARSE_OUTER_TPID_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_PARSE_INNER_TPID_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_PARSE_TAG_FORMAT_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_PARSE_PRIORITY_TAG_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_PARSE_DLFT_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_PARSE_DFLT_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR,
  
  ARAD_PP_LLP_PARSE_TYPE_ILLEGAL_ERR,
  ARAD_PP_LLP_PARSE_TRAP_CODE_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_PARSE_PRIO_TAG_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_PARSE_PRIORITY_TYPE_ILLEGAL_ERR,

  
  ARAD_PP_LLP_PARSE_ERR_LAST
} ARAD_PP_LLP_PARSE_ERR;

typedef enum
{
  
  ARAD_PP_LLP_PARSE_TAG_FORMAT_NONE = 0,
  
  ARAD_PP_LLP_PARSE_TAG_FORMAT_TPID1 = 1,
  
  ARAD_PP_LLP_PARSE_TAG_FORMAT_TPID2 = 2,
  
  ARAD_PP_LLP_PARSE_TAG_FORMAT_TPID3 = 3,
  
  ARAD_PP_NOF_LLP_PARSE_TAG_FORMATS = 4
}ARAD_PP_LLP_PARSE_TAG_FORMAT;








uint32
  arad_pp_llp_parse_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_llp_parse_tpid_values_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_VALUES                   *tpid_vals
  );

uint32
  arad_pp_llp_parse_tpid_values_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_VALUES                   *tpid_vals
  );

uint32
  arad_pp_llp_parse_tpid_values_get_verify(
    SOC_SAND_IN  int                                     unit
  );


uint32
  arad_pp_llp_parse_tpid_values_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_TPID_VALUES                   *tpid_vals
  );


uint32
  arad_pp_llp_parse_tpid_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  );

uint32
  arad_pp_llp_parse_tpid_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  );

uint32
  arad_pp_llp_parse_tpid_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx
  );


uint32
  arad_pp_llp_parse_tpid_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tpid_profile_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  );


uint32
  arad_pp_llp_parse_port_profile_to_tpid_profile_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint32                                  tpid_profile_id
  );

uint32
  arad_pp_llp_parse_port_profile_to_tpid_profile_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint32                                  tpid_profile_id
  );

uint32
  arad_pp_llp_parse_port_profile_to_tpid_profile_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx
  );


uint32
  arad_pp_llp_parse_port_profile_to_tpid_profile_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_OUT uint32                                  *tpid_profile_id
  );


uint32
  arad_pp_llp_parse_packet_format_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );
soc_error_t
  arad_pp_llp_parse_packet_format_eg_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );
soc_error_t
  arad_pp_llp_parse_packet_format_eg_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );

uint32
  arad_pp_llp_parse_packet_format_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );

uint32
  arad_pp_llp_parse_packet_format_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key
  );


uint32
  arad_pp_llp_parse_packet_format_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_parse_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_parse_get_errs_ptr(void);

uint32
  SOC_PPC_LLP_PARSE_TPID_VALUES_verify(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_VALUES *info
  );

uint32
  SOC_PPC_LLP_PARSE_TPID_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_INFO *info
  );

uint32
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO *info
  );

uint32
  SOC_PPC_LLP_PARSE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO *info
  );

uint32
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

