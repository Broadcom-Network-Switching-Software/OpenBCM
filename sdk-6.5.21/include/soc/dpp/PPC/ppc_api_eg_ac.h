/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_EG_AC_INCLUDED__

#define __SOC_PPC_API_EG_AC_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_eg_encap.h>













typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 vlan_domain;
  
  SOC_PPC_VSI_ID vsi;

} SOC_PPC_EG_AC_VBP_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 vlan_domain;
  
  SOC_SAND_PP_VLAN_ID cvid;
  
  uint32 pep_edit_profile;
   
  SOC_PPC_VSI_ID vsi; 

} SOC_PPC_EG_AC_CEP_PORT_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_VLAN_ID cvid;
  
  SOC_SAND_PP_PCP_UP up;
  
  uint8 pcp_profile;
  
  uint32 edit_profile;

} SOC_PPC_EG_VLAN_EDIT_CEP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_MP_LEVEL mp_level;
  
  uint8 is_valid;

} SOC_PPC_EG_AC_MP_INFO;










void
  SOC_PPC_EG_AC_VBP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_VBP_KEY *info
  );

void
  SOC_PPC_EG_AC_CEP_PORT_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_CEP_PORT_KEY *info
  );

void
  SOC_PPC_EG_VLAN_EDIT_CEP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_CEP_INFO *info
  );

void
  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_VLAN_INFO *info
  );

void
  SOC_PPC_EG_AC_VLAN_EDIT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_VLAN_EDIT_INFO *info
  );

void
  SOC_PPC_EG_AC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO *info
  );

void
  SOC_PPC_EG_AC_MP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_MP_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_EG_AC_VBP_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY *info
  );

void
  SOC_PPC_EG_AC_CEP_PORT_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY *info
  );

void
  SOC_PPC_EG_VLAN_EDIT_CEP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_CEP_INFO *info
  );

void
  SOC_PPC_EG_AC_MP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_MP_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

