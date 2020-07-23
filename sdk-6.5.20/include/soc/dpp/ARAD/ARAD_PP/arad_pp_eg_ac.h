
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_EG_AC_INCLUDED__

#define __ARAD_PP_EG_AC_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_eg_ac.h>
#include <soc/dpp/PPC/ppc_api_general.h>














typedef enum
{
  
  SOC_PPC_EG_AC_INFO_SET = ARAD_PP_PROC_DESC_BASE_EG_AC_FIRST,
  SOC_PPC_EG_AC_INFO_SET_PRINT,
  SOC_PPC_EG_AC_INFO_SET_UNSAFE,
  SOC_PPC_EG_AC_INFO_SET_VERIFY,
  SOC_PPC_EG_AC_INFO_GET,
  SOC_PPC_EG_AC_INFO_GET_PRINT,
  SOC_PPC_EG_AC_INFO_GET_VERIFY,
  SOC_PPC_EG_AC_INFO_GET_UNSAFE,
  SOC_PPC_EG_AC_MP_INFO_SET,
  SOC_PPC_EG_AC_MP_INFO_SET_PRINT,
  SOC_PPC_EG_AC_MP_INFO_SET_UNSAFE,
  SOC_PPC_EG_AC_MP_INFO_SET_VERIFY,
  SOC_PPC_EG_AC_MP_INFO_GET,
  SOC_PPC_EG_AC_MP_INFO_GET_PRINT,
  SOC_PPC_EG_AC_MP_INFO_GET_VERIFY,
  SOC_PPC_EG_AC_MP_INFO_GET_UNSAFE,
  ARAD_PP_EG_AC_PORT_VSI_INFO_ADD,
  ARAD_PP_EG_AC_PORT_VSI_INFO_ADD_PRINT,
  ARAD_PP_EG_AC_PORT_VSI_INFO_ADD_UNSAFE,
  ARAD_PP_EG_AC_PORT_VSI_INFO_ADD_VERIFY,
  ARAD_PP_EG_AC_PORT_VSI_INFO_REMOVE,
  ARAD_PP_EG_AC_PORT_VSI_INFO_REMOVE_PRINT,
  ARAD_PP_EG_AC_PORT_VSI_INFO_REMOVE_UNSAFE,
  ARAD_PP_EG_AC_PORT_VSI_INFO_REMOVE_VERIFY,
  ARAD_PP_EG_AC_PORT_VSI_INFO_GET,
  ARAD_PP_EG_AC_PORT_VSI_INFO_GET_PRINT,
  ARAD_PP_EG_AC_PORT_VSI_INFO_GET_UNSAFE,
  ARAD_PP_EG_AC_PORT_VSI_INFO_GET_VERIFY,
  ARAD_PP_EG_AC_PORT_CVID_INFO_ADD,
  ARAD_PP_EG_AC_PORT_CVID_INFO_ADD_PRINT,
  ARAD_PP_EG_AC_PORT_CVID_INFO_ADD_UNSAFE,
  ARAD_PP_EG_AC_PORT_CVID_INFO_ADD_VERIFY,
  ARAD_PP_EG_AC_PORT_CVID_INFO_REMOVE,
  ARAD_PP_EG_AC_PORT_CVID_INFO_REMOVE_PRINT,
  ARAD_PP_EG_AC_PORT_CVID_INFO_REMOVE_UNSAFE,
  ARAD_PP_EG_AC_PORT_CVID_INFO_REMOVE_VERIFY,
  ARAD_PP_EG_AC_PORT_CVID_INFO_GET,
  ARAD_PP_EG_AC_PORT_CVID_INFO_GET_PRINT,
  ARAD_PP_EG_AC_PORT_CVID_INFO_GET_UNSAFE,
  ARAD_PP_EG_AC_PORT_CVID_INFO_GET_VERIFY,
  ARAD_PP_EG_AC_PORT_CVID_MAP,
  ARAD_PP_EG_AC_PORT_CVID_MAP_PRINT,
  ARAD_PP_EG_AC_PORT_CVID_MAP_UNSAFE,
  ARAD_PP_EG_AC_PORT_CVID_MAP_VERIFY,
  ARAD_PP_EG_AC_PORT_VSI_MAP,
  ARAD_PP_EG_AC_PORT_VSI_MAP_PRINT,
  ARAD_PP_EG_AC_PORT_VSI_MAP_UNSAFE,
  ARAD_PP_EG_AC_PORT_VSI_MAP_VERIFY,
  ARAD_PP_EG_AC_GET_PROCS_PTR,
  ARAD_PP_EG_AC_GET_ERRS_PTR,
  

  
  ARAD_PP_EG_AC_PROCEDURE_DESC_LAST
} ARAD_PP_EG_AC_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_EG_AC_SUCCESS_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_EG_AC_FIRST,
  ARAD_PP_EG_AC_VLAN_DOMAIN_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_AC_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_AC_PCP_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_AC_EDIT_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_AC_NOF_TAGS_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_AC_MAX_LEVEL_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_AC_TYPE_INVALID_ERR,
  
  ARAD_PP_ESEM_REMOVE_FAILED,

  
  ARAD_PP_EG_AC_ERR_LAST
} ARAD_PP_EG_AC_ERR;









uint32
  arad_pp_eg_ac_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_eg_ac_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info
  );

uint32
  arad_pp_eg_ac_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info
  );

uint32
  arad_pp_eg_ac_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx
  );


uint32
  arad_pp_eg_ac_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO                          *ac_info
  );


uint32
  arad_pp_eg_ac_mp_info_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_MP_INFO                *info
  );

uint32
  arad_pp_eg_ac_mp_info_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_MP_INFO                *info
  );

uint32
  arad_pp_eg_ac_mp_info_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx
  );


uint32
  arad_pp_eg_ac_mp_info_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_OUT SOC_PPC_EG_AC_MP_INFO                *info
  );


uint32
  arad_pp_eg_ac_port_vsi_info_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_eg_ac_port_vsi_info_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info
  );


uint32
  arad_pp_eg_ac_port_vsi_info_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac
  );

uint32
  arad_pp_eg_ac_port_vsi_info_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key
  );


uint32
  arad_pp_eg_ac_port_vsi_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac,
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_eg_ac_port_vsi_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key
  );


uint32
  arad_pp_eg_ac_port_cvid_info_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_eg_ac_port_cvid_info_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info
  );


uint32
  arad_pp_eg_ac_port_cvid_info_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac
  );

uint32
  arad_pp_eg_ac_port_cvid_info_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key
  );


uint32
  arad_pp_eg_ac_port_cvid_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac,
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_eg_ac_port_cvid_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_ac_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_ac_get_errs_ptr(void);

uint32
  SOC_PPC_EG_AC_VBP_KEY_verify(
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY *info
  );

uint32
  SOC_PPC_EG_AC_CEP_PORT_KEY_verify(
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY *info
  );

uint32
  SOC_PPC_EG_VLAN_EDIT_CEP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_CEP_INFO *info
  );

uint32
  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO *info
  );

uint32
  SOC_PPC_EG_AC_VLAN_EDIT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_AC_VLAN_EDIT_INFO *info
  );

uint32
  SOC_PPC_EG_AC_INFO_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO *info
  );

uint32
  SOC_PPC_EG_AC_MP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_AC_MP_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



