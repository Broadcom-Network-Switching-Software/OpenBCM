/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_LAG_INCLUDED__

#define __SOC_PPD_API_LAG_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_lag.h>

#include <soc/dpp/PPD/ppd_api_general.h>














typedef enum
{
  
  SOC_PPD_LAG_SET = SOC_PPD_PROC_DESC_BASE_LAG_FIRST,
  SOC_PPD_LAG_SET_PRINT,
  SOC_PPD_LAG_GET,
  SOC_PPD_LAG_GET_PRINT,
  SOC_PPD_LAG_MEMBER_ADD,
  SOC_PPD_LAG_MEMBER_ADD_PRINT,
  SOC_PPD_LAG_MEMBER_REMOVE,
  SOC_PPD_LAG_MEMBER_REMOVE_PRINT,
  SOC_PPD_LAG_HASHING_GLOBAL_INFO_SET,
  SOC_PPD_LAG_HASHING_GLOBAL_INFO_SET_PRINT,
  SOC_PPD_LAG_HASHING_GLOBAL_INFO_GET,
  SOC_PPD_LAG_HASHING_GLOBAL_INFO_GET_PRINT,
  SOC_PPD_LAG_HASHING_PORT_INFO_SET,
  SOC_PPD_LAG_HASHING_PORT_INFO_SET_PRINT,
  SOC_PPD_LAG_HASHING_PORT_INFO_GET,
  SOC_PPD_LAG_HASHING_PORT_INFO_GET_PRINT,
  SOC_PPD_LAG_HASHING_MASK_SET,
  SOC_PPD_LAG_HASHING_MASK_SET_PRINT,
  SOC_PPD_LAG_HASHING_MASK_GET,
  SOC_PPD_LAG_HASHING_MASK_GET_PRINT,
  SOC_PPD_LAG_GET_PROCS_PTR,
  
  
  SOC_PPD_LAG_PROCEDURE_DESC_LAST
} SOC_PPD_LAG_PROCEDURE_DESC;










uint32
  soc_ppd_lag_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_INFO                            *lag_info
  );


uint32
  soc_ppd_lag_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lag_ndx,
    SOC_SAND_OUT SOC_PPC_LAG_INFO                            *lag_info
  );


uint32
  soc_ppd_lag_member_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER                          *member,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_lag_member_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER                   *member
  );


uint32
  soc_ppd_lag_hashing_global_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  );


uint32
  soc_ppd_lag_hashing_global_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  );


uint32
  soc_ppd_lag_hashing_port_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_HASH_PORT_INFO                  *lag_hash_info
  );


uint32
  soc_ppd_lag_hashing_port_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_OUT SOC_PPC_LAG_HASH_PORT_INFO                  *lag_hash_info
  );


uint32
  soc_ppd_lag_hashing_mask_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PPC_HASH_MASK_INFO       *mask_info
  );


uint32
  soc_ppd_lag_hashing_mask_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PPC_HASH_MASK_INFO       *mask_info
  );


uint32
  soc_ppd_lag_lb_key_range_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LAG_INFO                            *lag_info);



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

