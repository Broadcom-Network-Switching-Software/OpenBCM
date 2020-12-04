/******************************************************************
*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* FILENAME:       DuneDriver/ppd/include/soc_ppd_api_l3_src_bind.h
*
* MODULE PREFIX:  soc_ppd_src_bind
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
 */

#ifndef __SOC_PPD_API_L3_SRC_BIND_INCLUDED__

#define __SOC_PPD_API_L3_SRC_BIND_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_l3_src_bind.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv4.h>

#include <soc/dpp/PPD/ppd_api_general.h>









typedef enum
{
  
  SOC_PPD_SRC_BIND_IPV4_ADD = SOC_PPD_PROC_DESC_BASE_SRC_BIND_FIRST,
  SOC_PPD_SRC_BIND_IPV4_ADD_PRINT,
  SOC_PPD_SRC_BIND_IPV4_GET,
  SOC_PPD_SRC_BIND_IPV4_GET_PRINT,
  SOC_PPD_SRC_BIND_IPV4_REMOVE,
  SOC_PPD_SRC_BIND_IPV4_REMOVE_PRINT,
  SOC_PPD_SRC_BIND_IPV4_TABLE_CLEAR,
  SOC_PPD_SRC_BIND_IPV4_TABLE_CLEAR_PRINT,
  SOC_PPD_SRC_BIND_IPV6_ADD,
  SOC_PPD_SRC_BIND_IPV6_ADD_PRINT,
  SOC_PPD_SRC_BIND_IPV6_GET,
  SOC_PPD_SRC_BIND_IPV6_GET_PRINT,
  SOC_PPD_SRC_BIND_IPV6_REMOVE,
  SOC_PPD_SRC_BIND_IPV6_REMOVE_PRINT,
  SOC_PPD_SRC_BIND_IPV6_TABLE_CLEAR,
  SOC_PPD_SRC_BIND_IPV6_TABLE_CLEAR_PRINT,
  SOC_PPD_SRC_BIND_GET_PROCS_PTR,
  
  
  SOC_PPD_SRC_BIND_PROCEDURE_DESC_LAST
} SOC_PPD_SRC_BIND_PROCEDURE_DESC;









uint32
  soc_ppd_src_bind_ipv4_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );


uint32
  soc_ppd_src_bind_ipv4_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT uint8                           *found
  );


uint32
  soc_ppd_src_bind_ipv4_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );


uint32
  soc_ppd_src_bind_ipv4_table_clear(
    SOC_SAND_IN  int                           unit
  );


uint32
  soc_ppd_src_bind_ipv4_host_get_block(
    SOC_SAND_IN    int                              unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE      *block_range,
    SOC_SAND_OUT   SOC_PPC_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT   SOC_PPC_FRWRD_IP_ROUTE_STATUS   *routes_status,
    SOC_SAND_OUT   uint32                          *nof_entries
  );


uint32
  soc_ppd_src_bind_ipv6_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );


uint32
  soc_ppd_src_bind_ipv6_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT uint8                           *found
  );


uint32
  soc_ppd_src_bind_ipv6_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );


uint32
  soc_ppd_src_bind_ipv6_table_clear(
    SOC_SAND_IN  int                           unit
  );


uint32
  soc_ppd_src_bind_ipv6_get_block(
    SOC_SAND_IN    int                              unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE  *block_range,
    SOC_SAND_OUT   SOC_PPC_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT   SOC_PPC_FRWRD_IP_ROUTE_STATUS   *routes_status,
    SOC_SAND_OUT   uint32                          *nof_entries
  );


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


