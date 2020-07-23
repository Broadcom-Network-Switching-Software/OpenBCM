
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_LIF_TABLE_INCLUDED__

#define __ARAD_PP_LIF_TABLE_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_lif_table.h>
#include <soc/dpp/PPC/ppc_api_general.h>













typedef enum
{
  
  ARAD_PP_LIF_TABLE_GET_BLOCK = ARAD_PP_PROC_DESC_BASE_LIF_TABLE_FIRST,
  ARAD_PP_LIF_TABLE_GET_BLOCK_PRINT,
  ARAD_PP_LIF_TABLE_GET_BLOCK_UNSAFE,
  ARAD_PP_LIF_TABLE_GET_BLOCK_VERIFY,
  ARAD_PP_LIF_TABLE_ENTRY_GET,
  ARAD_PP_LIF_TABLE_ENTRY_GET_PRINT,
  ARAD_PP_LIF_TABLE_ENTRY_GET_UNSAFE,
  ARAD_PP_LIF_TABLE_ENTRY_GET_VERIFY,
  ARAD_PP_LIF_TABLE_ENTRY_UPDATE,
  ARAD_PP_LIF_TABLE_ENTRY_UPDATE_PRINT,
  ARAD_PP_LIF_TABLE_ENTRY_UPDATE_UNSAFE,
  ARAD_PP_LIF_TABLE_ENTRY_UPDATE_VERIFY,
  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_GET,
  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_GET_UNSAFE,
  ARAD_PP_LIF_TABLE_GET_PROCS_PTR,
  ARAD_PP_LIF_TABLE_GET_ERRS_PTR,
  

  
  ARAD_PP_LIF_TABLE_PROCEDURE_DESC_LAST
} ARAD_PP_LIF_TABLE_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_LIF_TABLE_TYPE_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LIF_TABLE_FIRST,
  ARAD_PP_LIF_TABLE_ENTRIES_TYPE_BM_OUT_OF_RANGE_ERR,
  
  ARAD_PP_LIF_TABLE_ENTRY_UPDATE_TYPE_MISMATCH_ERR,

  
  ARAD_PP_LIF_TABLE_ERR_LAST
} ARAD_PP_LIF_TABLE_ERR;









uint32
  arad_pp_lif_table_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );
 

uint32
  arad_pp_lif_table_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE         *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PPC_LIF_ENTRY_INFO                      *entries_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_lif_table_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE         *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );


uint32
  arad_pp_lif_table_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_ENTRY_INFO                      *lif_entry_info
  );

uint32
  arad_pp_lif_table_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_ndx
  );


uint32
  arad_pp_lif_table_entry_update_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_INFO                      *lif_entry_info
  );

uint32
  arad_pp_lif_table_entry_update_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_INFO                      *lif_entry_info
  );


uint32
  arad_pp_lif_table_entry_accessed_info_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_ndx,
    SOC_SAND_IN  uint8                               clear_access_stat,
    SOC_SAND_OUT  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO       *accessed_info
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_lif_table_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_lif_table_get_errs_ptr(void);

uint32
  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO    *info,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_TYPE             type
  );

uint32
  SOC_PPC_LIF_ENTRY_INFO_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_INFO *info
  );

uint32
  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_verify(
    SOC_SAND_IN  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



