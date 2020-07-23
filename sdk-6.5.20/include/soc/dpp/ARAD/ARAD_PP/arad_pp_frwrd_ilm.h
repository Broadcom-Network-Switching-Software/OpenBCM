/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_PP_FRWRD_ILM_INCLUDED__

#define __ARAD_PP_FRWRD_ILM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>













typedef enum
{
  
  SOC_PPC_FRWRD_ILM_GLBL_INFO_SET = ARAD_PP_PROC_DESC_BASE_FRWRD_ILM_FIRST,
  SOC_PPC_FRWRD_ILM_GLBL_INFO_SET_PRINT,
  SOC_PPC_FRWRD_ILM_GLBL_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_ILM_GLBL_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_ILM_GLBL_INFO_GET,
  SOC_PPC_FRWRD_ILM_GLBL_INFO_GET_PRINT,
  SOC_PPC_FRWRD_ILM_GLBL_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_ILM_GLBL_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_ILM_ADD,
  ARAD_PP_FRWRD_ILM_ADD_PRINT,
  ARAD_PP_FRWRD_ILM_ADD_UNSAFE,
  ARAD_PP_FRWRD_ILM_ADD_VERIFY,
  ARAD_PP_FRWRD_ILM_GET,
  ARAD_PP_FRWRD_ILM_GET_PRINT,
  ARAD_PP_FRWRD_ILM_GET_UNSAFE,
  ARAD_PP_FRWRD_ILM_GET_VERIFY,
  ARAD_PP_FRWRD_ILM_GET_BLOCK,
  ARAD_PP_FRWRD_ILM_GET_BLOCK_PRINT,
  ARAD_PP_FRWRD_ILM_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_ILM_GET_BLOCK_VERIFY,
  ARAD_PP_FRWRD_ILM_REMOVE,
  ARAD_PP_FRWRD_ILM_REMOVE_PRINT,
  ARAD_PP_FRWRD_ILM_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_ILM_REMOVE_VERIFY,
  ARAD_PP_FRWRD_ILM_TABLE_CLEAR,
  ARAD_PP_FRWRD_ILM_TABLE_CLEAR_PRINT,
  ARAD_PP_FRWRD_ILM_TABLE_CLEAR_UNSAFE,
  ARAD_PP_FRWRD_ILM_TABLE_CLEAR_VERIFY,
  ARAD_PP_FRWRD_ILM_GET_PROCS_PTR,
  ARAD_PP_FRWRD_ILM_GET_ERRS_PTR,
  

  
  ARAD_PP_FRWRD_ILM_PROCEDURE_DESC_LAST
} ARAD_PP_FRWRD_ILM_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_FRWRD_ILM_SUCCESS_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FRWRD_ILM_FIRST,
  

   SOC_PPC_FRWRD_ILM_KEY_INPORT_NOT_MASKED_ERR,
   SOC_PPC_FRWRD_ILM_KEY_INRIF_NOT_MASKED_ERR,
   ARAD_PP_FRWRD_ILM_EEI_NOT_MPLS_ERR,
   SOC_PPC_FRWRD_ILM_KEY_MAPPED_EXP_NOT_ZERO_ERR,
   SOC_PPC_FRWRD_ILM_KEY_MASK_NOT_SUPPORTED_ERR,

  
  ARAD_PP_FRWRD_ILM_ERR_LAST
} ARAD_PP_FRWRD_ILM_ERR;

typedef struct
{
  
  uint8 mask_inrif;

  
  uint8 mask_port;

}  ARAD_PP_ILM_INFO;









uint32
  arad_pp_frwrd_ilm_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_ilm_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_INFO                 *glbl_info
  );

uint32
  arad_pp_frwrd_ilm_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_INFO                 *glbl_info
  );

uint32
  arad_pp_frwrd_ilm_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_ilm_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_INFO                 *glbl_info
  );


uint32
  arad_pp_frwrd_ilm_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO                 *ilm_val,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_ilm_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO                 *ilm_val
  );


uint32
  arad_pp_frwrd_ilm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO                 *ilm_val,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_frwrd_ilm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY                       *ilm_key
  );


uint32
  arad_pp_frwrd_ilm_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_KEY                       *ilm_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO                 *ilm_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_ilm_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );


uint32
  arad_pp_frwrd_ilm_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY                       *ilm_key
  );

uint32
  arad_pp_frwrd_ilm_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY                       *ilm_key
  );


uint32
  arad_pp_frwrd_ilm_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_frwrd_ilm_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  );


  void
    arad_pp_frwrd_ilm_lem_key_parse(
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_KEY *key,
      SOC_SAND_OUT SOC_PPC_FRWRD_ILM_KEY                       *ilm_key
    );

  void
    arad_pp_frwrd_ilm_lem_payload_parse(
      SOC_SAND_IN int                                      unit,
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_PAYLOAD *payload,
      SOC_SAND_OUT  SOC_PPC_FRWRD_DECISION_INFO                 *ilm_val
    );

void
    arad_pp_frwrd_ilm_lem_key_build(
       SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY  *ilm_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY *key
    );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_ilm_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_ilm_get_errs_ptr(void);

uint32
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO *info
  );

uint32
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO *info
  );

uint32
  SOC_PPC_FRWRD_ILM_KEY_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY *info
  );

uint32
  SOC_PPC_FRWRD_ILM_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


