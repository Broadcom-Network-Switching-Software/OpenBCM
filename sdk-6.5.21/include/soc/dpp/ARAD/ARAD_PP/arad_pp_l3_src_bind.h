/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __ARAD_PP_L3_SRC_BIND_INCLUDED__

#define __ARAD_PP_L3_SRC_BIND_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_l3_src_bind.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>














typedef enum
{
  
  ARAD_PP_SRC_BIND_IPV4_ADD = ARAD_PP_PROC_DESC_BASE_SRC_BIND_FIRST,
  ARAD_PP_SRC_BIND_IPV4_ADD_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_ADD_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_ADD_VERIFY,
  ARAD_PP_SRC_BIND_IPV4_GET,
  ARAD_PP_SRC_BIND_IPV4_GET_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_GET_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_GET_VERIFY,
  ARAD_PP_SRC_BIND_IPV4_REMOVE,
  ARAD_PP_SRC_BIND_IPV4_REMOVE_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_REMOVE_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_REMOVE_VERIFY,
  ARAD_PP_SRC_BIND_IPV4_TABLE_CLEAR,
  ARAD_PP_SRC_BIND_IPV4_TABLE_CLEAR_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_TABLE_CLEAR_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV4_TABLE_CLEAR_VERIFY,
  ARAD_PP_SRC_BIND_IPV6_ADD,
  ARAD_PP_SRC_BIND_IPV6_ADD_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_ADD_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_ADD_VERIFY,
  ARAD_PP_SRC_BIND_IPV6_GET,
  ARAD_PP_SRC_BIND_IPV6_GET_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_GET_INTERNAL_UNSAFE,  
  ARAD_PP_SRC_BIND_IPV6_GET_VERIFY,
  ARAD_PP_SRC_BIND_IPV6_REMOVE,
  ARAD_PP_SRC_BIND_IPV6_REMOVE_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_REMOVE_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_REMOVE_VERIFY,
  ARAD_PP_SRC_BIND_IPV6_TABLE_CLEAR,
  ARAD_PP_SRC_BIND_IPV6_TABLE_CLEAR_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_TABLE_CLEAR_INTERNAL_UNSAFE,
  ARAD_PP_SRC_BIND_IPV6_TABLE_CLEAR_VERIFY, 
  ARAD_PP_SRC_BIND_IPV6_STATIC_TCAM_INIT_UNSAFE,
  ARAD_PP_SRC_BIND_GET_PROCS_PTR,
  ARAD_PP_SRC_BIND_GET_ERRS_PTR,
  

  
  ARAD_PP_SRC_BIND_PROCEDURE_DESC_LAST
} ARAD_PP_SRC_BIND_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_SRC_BIND_INVALID_SMAC_ERR = ARAD_PP_ERR_DESC_BASE_SRC_BIND_FIRST,
  ARAD_PP_SRC_BIND_IPV4_INVALID_SIP_ERR,
  ARAD_PP_SRC_BIND_IPV6_INVALID_SIP_ERR,
  ARAD_PP_SRC_BIND_INFO_IS_NULL_ERR,
  ARAD_PP_SRC_BIND_SW_DB_NO_RES_ERR,
  


  
  ARAD_PP_SRC_BIND_ERR_LAST
} ARAD_PP_SRC_BIND_ERR;

#define ARAD_PP_SRC_BIND_IPV4_MAX_SPOOF_ID (1024*32)

typedef struct 
{
  
  SOC_SAND_OCC_BM_PTR
    srcbind_arb_occ;

  uint16 spoof_id_ref_count[ARAD_PP_SRC_BIND_IPV4_MAX_SPOOF_ID];

} ARAD_SRC_BINDS;








uint32
  arad_pp_src_bind_ipv6_static_tcam_init_unsafe(
    SOC_SAND_IN int unit
  );

uint32
  arad_pp_src_bind_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32 
  arad_pp_src_bind_ipv4_add_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY           *src_bind_info
    );


uint32 
  arad_pp_src_bind_ipv4_add_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY           *src_bind_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32 
  arad_pp_src_bind_ipv4_add_internal_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY           *src_bind_info,
      SOC_SAND_IN  uint8                                  ignore_key,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32 
  arad_pp_src_bind_ipv4_get_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY           *src_bind_info
    );


uint32 
  arad_pp_src_bind_ipv4_get_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY           *src_bind_info,
      SOC_SAND_OUT uint8                                 *found
    );

uint32 
  arad_pp_src_bind_ipv4_get_internal_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY           *src_bind_info,
      SOC_SAND_IN  uint8                                  ignore_key,
      SOC_SAND_OUT uint8                                 *found,
      SOC_SAND_OUT uint8                                 *found_isem,
      SOC_SAND_OUT uint8                                 *found_lem,
      SOC_SAND_OUT uint16                                *spoof_id
  );

uint32
  arad_pp_src_bind_ipv4_remove_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY           *src_bind_info
    );


uint32
  arad_pp_src_bind_ipv4_remove_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY           *src_bind_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32
  arad_pp_src_bind_ipv4_remove_internal_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY           *src_bind_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32
  arad_pp_src_bind_ipv4_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_src_bind_ipv4_table_clear_unsafe(
      SOC_SAND_IN  int                                 unit
    );

uint32
  arad_pp_src_bind_ipv4_table_clear_internal_unsafe(
      SOC_SAND_IN  int                                 unit
    );


uint32
    arad_pp_src_bind_ipv4_host_get_block_unsafe(
      SOC_SAND_IN    int                                        unit,
      SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key,
      SOC_SAND_OUT   SOC_PPC_SRC_BIND_IPV4_ENTRY               *ipv4_info,
      SOC_SAND_OUT   SOC_PPC_FRWRD_IP_ROUTE_STATUS             *ipv4_status,
      SOC_SAND_OUT   uint32                                    *nof_entries
  );

uint32 
  arad_pp_src_bind_ipv6_add_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY           *src_bind_info
    );


uint32 
  arad_pp_src_bind_ipv6_add_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY           *src_bind_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32 
  arad_pp_src_bind_ipv6_add_internal_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY           *src_bind_info,
      SOC_SAND_IN  uint8                                  ignore_key,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32 
  arad_pp_src_bind_ipv6_get_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY           *src_bind_info
    );


uint32 
  arad_pp_src_bind_ipv6_get_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY           *src_bind_info,
      SOC_SAND_OUT uint8                                 *found
    );

uint32 
  arad_pp_src_bind_ipv6_get_internal_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY           *src_bind_info,
      SOC_SAND_IN  uint8                                  ignore_key,
      SOC_SAND_OUT uint8                                 *found,
      SOC_SAND_OUT uint8                                 *found_isem,
      SOC_SAND_OUT uint8                                 *found_lem,
      SOC_SAND_OUT uint16                                *spoof_id
    );

uint32
  arad_pp_src_bind_ipv6_remove_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY           *src_bind_info
    );


uint32
  arad_pp_src_bind_ipv6_remove_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY           *src_bind_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32
  arad_pp_src_bind_ipv6_remove_internal_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY           *src_bind_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
    );

uint32
  arad_pp_src_bind_ipv6_table_clear_verify(
      SOC_SAND_IN  int                                 unit
    );


uint32
  arad_pp_src_bind_ipv6_table_clear_unsafe(
      SOC_SAND_IN  int                                 unit
    );

uint32
  arad_pp_src_bind_ipv6_table_clear_internal_unsafe(
      SOC_SAND_IN  int                                 unit
    );


uint32
  arad_pp_src_bind_ipv6_get_block_unsafe(
    SOC_SAND_IN    int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE     *block_range_key,
    SOC_SAND_OUT   SOC_PPC_SRC_BIND_IPV6_ENTRY        *src_bind_ipv6,
    SOC_SAND_OUT   SOC_PPC_FRWRD_IP_ROUTE_STATUS      *ipv6_status,
    SOC_SAND_OUT   uint32                             *nof_entries
  );

uint32
  arad_pp_ip6_compression_tcam_init_unsafe(
    SOC_SAND_IN int unit
  );


soc_error_t 
  arad_pp_ip6_compression_add(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_INOUT SOC_PPC_IPV6_COMPRESSED_ENTRY    *ip6_compression,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE           *success
    );



soc_error_t
  arad_pp_ip6_compression_delete(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_INOUT  SOC_PPC_IPV6_COMPRESSED_ENTRY   *ip6_compression,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE           *success
    );


soc_error_t 
  arad_pp_ip6_compression_get(
      SOC_SAND_IN     int                                 unit,
      SOC_SAND_INOUT  SOC_PPC_IPV6_COMPRESSED_ENTRY      *ip6_compression,
      SOC_SAND_OUT    uint8                              *found
    );


soc_error_t 
  arad_pp_pppoe_anti_spoofing_add(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_INOUT SOC_PPC_SRC_BIND_PPPOE_ENTRY    *pppoe_entry,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE           *success
    );



soc_error_t
  arad_pp_pppoe_anti_spoofing_delete(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_INOUT  SOC_PPC_SRC_BIND_PPPOE_ENTRY   *pppoe_entry,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE           *success
    );


soc_error_t 
  arad_pp_pppoe_anti_spoofing_get(
      SOC_SAND_IN     int                                 unit,
      SOC_SAND_INOUT  SOC_PPC_SRC_BIND_PPPOE_ENTRY      *pppoe_entry,
      SOC_SAND_OUT    uint8                              *found
    );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_src_bind_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_src_bind_get_errs_ptr(void);

uint32
  SOC_PPC_SRC_BIND_IPV4_ENTRY_verify(
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY *info
  );

uint32
  SOC_PPC_SRC_BIND_IPV6_ENTRY_verify(
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



