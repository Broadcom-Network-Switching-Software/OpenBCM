/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_LLP_MIRROR_INCLUDED__

#define __ARAD_PP_LLP_MIRROR_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_llp_mirror.h>






#define ARAD_PP_LLP_MIRROR_NOF_VID_MIRROR_INDICES (6)









typedef enum
{
  
  ARAD_PP_LLP_MIRROR_PORT_VLAN_ADD = ARAD_PP_PROC_DESC_BASE_LLP_MIRROR_FIRST,
  ARAD_PP_LLP_MIRROR_PORT_VLAN_ADD_PRINT,
  ARAD_PP_LLP_MIRROR_PORT_VLAN_ADD_UNSAFE,
  ARAD_PP_LLP_MIRROR_PORT_VLAN_ADD_VERIFY,
  ARAD_PP_LLP_MIRROR_PORT_VLAN_REMOVE,
  ARAD_PP_LLP_MIRROR_PORT_VLAN_REMOVE_PRINT,
  ARAD_PP_LLP_MIRROR_PORT_VLAN_REMOVE_UNSAFE,
  ARAD_PP_LLP_MIRROR_PORT_VLAN_REMOVE_VERIFY,
  ARAD_PP_LLP_MIRROR_PORT_VLAN_GET,
  ARAD_PP_LLP_MIRROR_PORT_VLAN_GET_PRINT,
  ARAD_PP_LLP_MIRROR_PORT_VLAN_GET_UNSAFE,
  ARAD_PP_LLP_MIRROR_PORT_VLAN_GET_VERIFY,
  ARAD_PP_LLP_MIRROR_PORT_DFLT_SET,
  ARAD_PP_LLP_MIRROR_PORT_DFLT_SET_PRINT,
  ARAD_PP_LLP_MIRROR_PORT_DFLT_SET_UNSAFE,
  ARAD_PP_LLP_MIRROR_PORT_DFLT_SET_VERIFY,
  ARAD_PP_LLP_MIRROR_PORT_DFLT_GET,
  ARAD_PP_LLP_MIRROR_PORT_DFLT_GET_PRINT,
  ARAD_PP_LLP_MIRROR_PORT_DFLT_GET_VERIFY,
  ARAD_PP_LLP_MIRROR_PORT_DFLT_GET_UNSAFE,
  ARAD_PP_LLP_MIRROR_GET_PROCS_PTR,
  ARAD_PP_LLP_MIRROR_GET_ERRS_PTR,
  

  
  ARAD_PP_LLP_MIRROR_PROCEDURE_DESC_LAST
} ARAD_PP_LLP_MIRROR_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_LLP_MIRROR_MIRROR_PROFILE_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LLP_MIRROR_FIRST,
  ARAD_PP_LLP_MIRROR_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_MIRROR_TAGGED_DFLT_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_MIRROR_UNTAGGED_DFLT_OUT_OF_RANGE_ERR,
  

  ARAD_PP_VID_NDX_OUT_OF_RANGE_NO_ZERO_ERR,

  
  ARAD_PP_LLP_MIRROR_ERR_LAST
} ARAD_PP_LLP_MIRROR_ERR;









uint32
  arad_pp_llp_mirror_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_llp_mirror_port_vlan_add_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                  mirror_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_llp_mirror_port_vlan_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                  mirror_profile
  );


uint32
  arad_pp_llp_mirror_port_vlan_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );

uint32
  arad_pp_llp_mirror_port_vlan_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );


uint32
  arad_pp_llp_mirror_port_vlan_get_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint32                                  *mirror_profile
  );

uint32
  arad_pp_llp_mirror_port_vlan_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );


uint32
  arad_pp_llp_mirror_port_dflt_set_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO           *dflt_mirroring_info
  );

uint32
  arad_pp_llp_mirror_port_dflt_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO           *dflt_mirroring_info
  );

uint32
  arad_pp_llp_mirror_port_dflt_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  );


uint32
  arad_pp_llp_mirror_port_dflt_get_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO           *dflt_mirroring_info
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_mirror_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_mirror_get_errs_ptr(void);

uint32
  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

