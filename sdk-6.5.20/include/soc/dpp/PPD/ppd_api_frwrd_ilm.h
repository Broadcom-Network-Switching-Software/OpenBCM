/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_FRWRD_ILM_INCLUDED__

#define __SOC_PPD_API_FRWRD_ILM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ilm.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_FRWRD_ILM_GLBL_INFO_SET = SOC_PPD_PROC_DESC_BASE_FRWRD_ILM_FIRST,
  SOC_PPD_FRWRD_ILM_GLBL_INFO_SET_PRINT,
  SOC_PPD_FRWRD_ILM_GLBL_INFO_GET,
  SOC_PPD_FRWRD_ILM_GLBL_INFO_GET_PRINT,
  SOC_PPD_FRWRD_ILM_ADD,
  SOC_PPD_FRWRD_ILM_ADD_PRINT,
  SOC_PPD_FRWRD_ILM_GET,
  SOC_PPD_FRWRD_ILM_GET_PRINT,
  SOC_PPD_FRWRD_ILM_GET_BLOCK,
  SOC_PPD_FRWRD_ILM_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_ILM_REMOVE,
  SOC_PPD_FRWRD_ILM_REMOVE_PRINT,
  SOC_PPD_FRWRD_ILM_TABLE_CLEAR,
  SOC_PPD_FRWRD_ILM_TABLE_CLEAR_PRINT,
  SOC_PPD_FRWRD_ILM_GET_PROCS_PTR,
  
  
  SOC_PPD_FRWRD_ILM_PROCEDURE_DESC_LAST
} SOC_PPD_FRWRD_ILM_PROCEDURE_DESC;










uint32
  soc_ppd_frwrd_ilm_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_INFO                 *glbl_info
  );


uint32
  soc_ppd_frwrd_ilm_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_INFO                 *glbl_info
  );


uint32
  soc_ppd_frwrd_ilm_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO                 *ilm_val,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_frwrd_ilm_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO                 *ilm_val,
    SOC_SAND_OUT uint8                               *found
  );


uint32
  soc_ppd_frwrd_ilm_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_KEY                       *ilm_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO                 *ilm_vals,
    SOC_SAND_OUT uint32                                *nof_entries
  );


uint32
  soc_ppd_frwrd_ilm_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY                       *ilm_key
  );


uint32
  soc_ppd_frwrd_ilm_table_clear(
    SOC_SAND_IN  int                               unit
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

