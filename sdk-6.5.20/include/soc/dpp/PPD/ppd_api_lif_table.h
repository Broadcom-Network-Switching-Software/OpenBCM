/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_LIF_TABLE_INCLUDED__

#define __SOC_PPD_API_LIF_TABLE_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_lif_table.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_rif.h>













typedef enum
{
  
  SOC_PPD_LIF_TABLE_GET_BLOCK = SOC_PPD_PROC_DESC_BASE_LIF_TABLE_FIRST,
  SOC_PPD_LIF_TABLE_GET_BLOCK_PRINT,
  SOC_PPD_LIF_TABLE_ENTRY_GET,
  SOC_PPD_LIF_TABLE_ENTRY_GET_PRINT,
  SOC_PPD_LIF_TABLE_ENTRY_UPDATE,
  SOC_PPD_LIF_TABLE_ENTRY_ACCESSED_INFO_GET,
  SOC_PPD_LIF_TABLE_PRINT_BLOCK,
  SOC_PPD_LIF_TABLE_ENTRY_UPDATE_PRINT,
  SOC_PPD_LIF_TABLE_GET_PROCS_PTR,
  
  
  SOC_PPD_LIF_TABLE_PROCEDURE_DESC_LAST
} SOC_PPD_LIF_TABLE_PROCEDURE_DESC;











uint32
  soc_ppd_lif_table_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE         *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PPC_LIF_ENTRY_INFO                      *entries_array,
    SOC_SAND_OUT uint32                                *nof_entries
  );


uint32
  soc_ppd_lif_table_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_ENTRY_INFO                      *lif_entry_info
  );


uint32
  soc_ppd_lif_table_entry_update(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_INFO                      *lif_entry_info
  );


uint32
  soc_ppd_lif_table_entry_accessed_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_ndx,
    SOC_SAND_IN  uint8                               clear_access_stat,
    SOC_SAND_OUT  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO       *accessed_info
  );


uint32
  soc_ppd_lif_table_print_block(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE            *rule,
    SOC_SAND_IN SOC_SAND_TABLE_BLOCK_RANGE                        *block_range,
    SOC_SAND_IN SOC_PPC_LIF_ENTRY_INFO                          *entries_array,
    SOC_SAND_IN uint32                                      nof_entries
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

