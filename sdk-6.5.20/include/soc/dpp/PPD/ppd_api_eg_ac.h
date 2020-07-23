/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_EG_AC_INCLUDED__

#define __SOC_PPD_API_EG_AC_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_eg_ac.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_EG_AC_INFO_SET = SOC_PPD_PROC_DESC_BASE_EG_AC_FIRST,
  SOC_PPD_EG_AC_INFO_SET_PRINT,
  SOC_PPD_EG_AC_INFO_GET,
  SOC_PPD_EG_AC_INFO_GET_PRINT,
  SOC_PPD_EG_AC_MP_INFO_SET,
  SOC_PPD_EG_AC_MP_INFO_SET_PRINT,
  SOC_PPD_EG_AC_MP_INFO_GET,
  SOC_PPD_EG_AC_MP_INFO_GET_PRINT,
  SOC_PPD_EG_AC_PORT_VSI_INFO_ADD,
  SOC_PPD_EG_AC_PORT_VSI_INFO_ADD_PRINT,
  SOC_PPD_EG_AC_PORT_VSI_INFO_REMOVE,
  SOC_PPD_EG_AC_PORT_VSI_INFO_REMOVE_PRINT,
  SOC_PPD_EG_AC_PORT_VSI_INFO_GET,
  SOC_PPD_EG_AC_PORT_VSI_INFO_GET_PRINT,
  SOC_PPD_EG_AC_PORT_CVID_INFO_ADD,
  SOC_PPD_EG_AC_PORT_CVID_INFO_ADD_PRINT,
  SOC_PPD_EG_AC_PORT_CVID_INFO_REMOVE,
  SOC_PPD_EG_AC_PORT_CVID_INFO_REMOVE_PRINT,
  SOC_PPD_EG_AC_PORT_CVID_INFO_GET,
  SOC_PPD_EG_AC_PORT_CVID_INFO_GET_PRINT,
  SOC_PPD_EG_AC_PORT_CVID_MAP,
  SOC_PPD_EG_AC_PORT_CVID_MAP_PRINT,
  SOC_PPD_EG_AC_PORT_VSI_MAP,
  SOC_PPD_EG_AC_PORT_VSI_MAP_PRINT,
  SOC_PPD_EG_AC_GET_PROCS_PTR,
  



  
  SOC_PPD_EG_AC_PROCEDURE_DESC_LAST
} SOC_PPD_EG_AC_PROCEDURE_DESC;










uint32
  soc_ppd_eg_ac_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info
  );


uint32
  soc_ppd_eg_ac_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO                          *ac_info
  );


uint32
  soc_ppd_eg_ac_mp_info_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                          out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_MP_INFO                  *info
  );


uint32
  soc_ppd_eg_ac_mp_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                          out_ac_ndx,
    SOC_SAND_OUT SOC_PPC_EG_AC_MP_INFO                  *info
  );


uint32
  soc_ppd_eg_ac_port_vsi_info_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_eg_ac_port_vsi_info_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac
  );


uint32
  soc_ppd_eg_ac_port_vsi_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY                       *vbp_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac,
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                               *found
  );


uint32
  soc_ppd_eg_ac_port_cvid_info_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_eg_ac_port_cvid_info_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac
  );


uint32
  soc_ppd_eg_ac_port_cvid_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT SOC_PPC_AC_ID                               *out_ac,
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                               *found
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

