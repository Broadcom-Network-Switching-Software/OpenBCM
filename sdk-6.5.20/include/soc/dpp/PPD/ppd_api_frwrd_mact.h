/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_FRWRD_MACT_INCLUDED__

#define __SOC_PPD_API_FRWRD_MACT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_frwrd_mact.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_FRWRD_MACT_ENTRY_ADD = SOC_PPD_PROC_DESC_BASE_FRWRD_MACT_FIRST,
  SOC_PPD_FRWRD_MACT_ENTRY_ADD_PRINT,
  SOC_PPD_FRWRD_MACT_ENTRY_REMOVE,
  SOC_PPD_FRWRD_MACT_ENTRY_REMOVE_PRINT,
  SOC_PPD_FRWRD_MACT_ENTRY_GET,
  SOC_PPD_FRWRD_MACT_ENTRY_GET_PRINT,
  SOC_PPD_FRWRD_MACT_ENTRY_PACK,
  SOC_PPD_FRWRD_MACT_ENTRY_PACK_PRINT,
  SOC_PPD_FRWRD_MACT_TRAVERSE,
  SOC_PPD_FRWRD_MACT_TRAVERSE_PRINT,
  SOC_PPD_FRWRD_MACT_TRAVERSE_BY_MAC_LIMIT_PER_TUNNEL,
  SOC_PPD_FRWRD_MACT_TRAVERSE_BY_MAC_LIMIT_PER_TUNNEL_PRINT,
  SOC_PPD_FRWRD_MACT_TRAVERSE_STATUS_GET,
  SOC_PPD_FRWRD_MACT_TRAVERSE_STATUS_GET_PRINT,
  SOC_PPD_FRWRD_MACT_GET_BLOCK,
  SOC_PPD_FRWRD_MACT_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_MACT_PRINT_BLOCK,
  SOC_PPD_FRWRD_MACT_PRINT_BLOCK_PRINT,
  SOC_PPD_FRWRD_MACT_GET_PROCS_PTR,
  
  
  SOC_PPD_FRWRD_MACT_PROCEDURE_DESC_LAST
} SOC_PPD_FRWRD_MACT_PROCEDURE_DESC;










uint32
  soc_ppd_frwrd_mact_app_to_prefix_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        app_id,
    SOC_SAND_OUT uint32                        *prefix);


uint32
  soc_ppd_frwrd_mact_entry_pack(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint8                                       insert,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint32                                      *data,
    SOC_SAND_OUT uint8                                       *data_len
  );


uint32
  soc_ppd_frwrd_mact_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_frwrd_mact_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  int                                sw_only
  );


uint32
  soc_ppd_frwrd_mact_ipmc_table_clear(
    SOC_SAND_IN  int                                 unit
  );


uint32
  soc_ppd_frwrd_mact_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint8                               *found
  );


uint32
  soc_ppd_frwrd_mact_traverse(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                               wait_till_finish,
    SOC_SAND_OUT uint32                                *nof_matched_entries
  );


uint32
  soc_ppd_frwrd_mact_traverse_by_mac_limit_per_tunnel(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_OUT uint32                                *nof_matched_entries
  );



uint32
  soc_ppd_frwrd_mact_traverse_status_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO     *status
  );


uint32
  soc_ppd_frwrd_mact_traverse_mode_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  );

uint32
  soc_ppd_frwrd_mact_traverse_mode_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  );



uint32
  soc_ppd_frwrd_mact_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TABLE_TYPE               mact_type,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_OUT uint32                                *nof_entries
  );

#if SOC_PPC_DEBUG_IS_LVL1

uint32
  soc_ppd_frwrd_mact_print_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_IN  uint32                                nof_entries,
    SOC_SAND_IN  uint32                                flavors
  );
#endif


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

