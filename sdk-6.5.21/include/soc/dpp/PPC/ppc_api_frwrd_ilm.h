/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_FRWRD_ILM_INCLUDED__

#define __SOC_PPC_API_FRWRD_ILM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>














typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 mask_port;
  
  uint8 mask_inrif;

} SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_U32_RANGE labels_range;
  
  SOC_SAND_PP_MPLS_EXP exp_map_tbl[SOC_SAND_PP_NOF_BITS_IN_EXP];

} SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MPLS_LABEL in_label;
  
  SOC_SAND_PP_MPLS_LABEL in_label_second;
  
  SOC_SAND_PP_MPLS_EXP mapped_exp;
  
  SOC_PPC_PORT in_local_port;
  
  SOC_PPC_RIF_ID inrif;
  
  uint32 flags;
  
  uint32 in_core;
  
  uint16 vrf;
} SOC_PPC_FRWRD_ILM_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO key_info;
  
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO elsp_info;

} SOC_PPC_FRWRD_ILM_GLBL_INFO;










void
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO *info
  );

void
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO *info
  );

void
  SOC_PPC_FRWRD_ILM_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_KEY *info
  );

void
  SOC_PPC_FRWRD_ILM_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO *info
  );

void
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO *info
  );

void
  SOC_PPC_FRWRD_ILM_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY *info
  );

void
  SOC_PPC_FRWRD_ILM_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

