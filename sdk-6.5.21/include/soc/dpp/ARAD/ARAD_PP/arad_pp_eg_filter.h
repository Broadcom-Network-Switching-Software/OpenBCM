
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_EG_FILTER_INCLUDED__

#define __ARAD_PP_EG_FILTER_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_port.h>
#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_eg_filter.h>
#include <soc/dpp/PPC/ppc_api_llp_parse.h>
#include <soc/dpp/PPC/ppc_api_llp_filter.h>
















typedef enum
{
  
  SOC_PPC_EG_FILTER_PORT_INFO_SET = ARAD_PP_PROC_DESC_BASE_EG_FILTER_FIRST,
  SOC_PPC_EG_FILTER_PORT_INFO_SET_PRINT,
  SOC_PPC_EG_FILTER_PORT_INFO_SET_UNSAFE,
  SOC_PPC_EG_FILTER_PORT_INFO_SET_VERIFY,
  SOC_PPC_EG_FILTER_PORT_INFO_GET,
  SOC_PPC_EG_FILTER_PORT_INFO_GET_PRINT,
  SOC_PPC_EG_FILTER_PORT_INFO_GET_VERIFY,
  SOC_PPC_EG_FILTER_PORT_INFO_GET_UNSAFE,
  ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET,
  ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_PRINT,
  ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_UNSAFE,
  ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_VERIFY,
  ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET,
  ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_PRINT,
  ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_VERIFY,
  ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_UNSAFE,
  ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET,
  ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_PRINT,
  ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_UNSAFE,
  ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_VERIFY,
  ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET,
  ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_PRINT,
  ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_VERIFY,
  ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_UNSAFE,
  ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET,
  ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PRINT,
  ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_UNSAFE,
  ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_VERIFY,
  ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET,
  ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PRINT,
  ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_VERIFY,
  ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_UNSAFE,
  ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET,
  ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PRINT,
  ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_UNSAFE,
  ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_VERIFY,
  ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET,
  ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_PRINT,
  ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_VERIFY,
  ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_UNSAFE,
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_SET,
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_SET_PRINT,
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_SET_UNSAFE,
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_SET_VERIFY,
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_GET,
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_GET_PRINT,
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_GET_VERIFY,
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_GET_UNSAFE,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_PRINT,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_UNSAFE,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_VERIFY,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_PRINT,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_VERIFY,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_UNSAFE,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_PRINT,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_UNSAFE,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_VERIFY,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET_PRINT,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET_VERIFY,
  ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET_UNSAFE,
  ARAD_PP_EG_FILTER_VSI_MEMBERSHIP_GET,
  ARAD_PP_EG_FILTER_VSI_MEMBERSHIP_GET_PRINT,
  ARAD_PP_EG_FILTER_VSI_MEMBERSHIP_GET_VERIFY,
  ARAD_PP_EG_FILTER_VSI_MEMBERSHIP_GET_UNSAFE,
  ARAD_PP_EG_FILTER_GET_PROCS_PTR,
  ARAD_PP_EG_FILTER_GET_ERRS_PTR,
  



  
  ARAD_PP_EG_FILTER_PROCEDURE_DESC_LAST
} ARAD_PP_EG_FILTER_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_EG_FILTER_IS_MEMBER_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_EG_FILTER_FIRST,
  ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_FILTER_ACCEPT_OUT_OF_RANGE_ERR,
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_FILTER_ORIENTATION_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_FILTER_FILTER_MASK_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_FILTER_ACCEPTABLE_FRAMES_PROFILE_OUT_OF_RANGE_ERR,
  
  ARAD_PP_EG_PVLAN_FEATURE_DISABLED_ERR,
  ARAD_PP_EG_SPLIT_HORIZON_FEATURE_DISABLED_ERR,
  ARAD_PP_EG_FLTER_PORT_SPLIT_DISABLE_ERR,
  ARAD_PP_EG_FLTER_PORT_MTU_DISABLE_ERR,
  

  
  ARAD_PP_EG_FILTER_ERR_LAST
} ARAD_PP_EG_FILTER_ERR;









uint32
  arad_pp_eg_filter_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_eg_filter_port_info_set_unsafe(
    SOC_SAND_IN  int                          unit,
	SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PORT_INFO          *port_info
  );

uint32
  arad_pp_eg_filter_port_info_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PORT_INFO          *port_info
  );

uint32
  arad_pp_eg_filter_port_info_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx
  );


uint32
  arad_pp_eg_filter_port_info_get_unsafe(
    SOC_SAND_IN  int                          unit,
	SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_PORT_INFO          *port_info
  );

#ifdef BCM_88660_A0
uint32
  arad_pp_eg_filter_global_info_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_GLOBAL_INFO   *global_info
  );

uint32
  arad_pp_eg_filter_global_info_set_verify(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN SOC_PPC_EG_FILTER_GLOBAL_INFO   *global_info
  );

uint32
  arad_pp_eg_filter_global_info_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_GLOBAL_INFO   *global_info
  );

uint32
  arad_pp_eg_filter_global_info_get_unsafe(
    SOC_SAND_OUT int                          unit,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_GLOBAL_INFO   *global_info
  );
#endif 


uint32
  arad_pp_eg_filter_vsi_port_membership_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  );

uint32
  arad_pp_eg_filter_vsi_port_membership_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  );

uint32
  arad_pp_eg_filter_vsi_port_membership_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx
  );


uint32
  arad_pp_eg_filter_vsi_port_membership_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_OUT uint8                          *is_member
  );

uint32
  arad_pp_eg_filter_vsi_membership_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx
  );


uint32
  arad_pp_eg_filter_vsi_membership_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,    
    SOC_SAND_OUT uint32                               ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE]
  );



soc_error_t
  arad_pp_eg_filter_default_port_membership_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PORT                 out_port_ndx,
    SOC_SAND_IN  uint8                        is_member
  );


soc_error_t
  arad_pp_eg_filter_default_port_membership_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PORT                 out_port_ndx,
    SOC_SAND_OUT uint8                        *is_member
  );


uint32
  arad_pp_eg_filter_cvid_port_membership_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  );

uint32
  arad_pp_eg_filter_cvid_port_membership_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  );

uint32
  arad_pp_eg_filter_cvid_port_membership_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx
  );


uint32
  arad_pp_eg_filter_cvid_port_membership_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_OUT uint8                          *is_member
  );


uint32
  arad_pp_eg_filter_port_acceptable_frames_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          eg_acceptable_frames_port_profile,
    SOC_SAND_IN  uint32                          llvp_port_profile,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO          *eg_prsr_out_key,
    SOC_SAND_IN  uint8                           accept
  );

uint32
  arad_pp_eg_filter_port_acceptable_frames_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          eg_acceptable_frames_port_profile,
    SOC_SAND_IN  uint32                          llvp_port_profile,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO          *eg_prsr_out_key,
    SOC_SAND_IN  uint8                           accept
  );

uint32
  arad_pp_eg_filter_port_acceptable_frames_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *eg_prsr_out_key
  );


uint32
  arad_pp_eg_filter_port_acceptable_frames_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           port_profile,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *eg_prsr_out_key,
    SOC_SAND_OUT uint8                                 *accept
  );


uint32
  arad_pp_eg_filter_pep_acceptable_frames_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx,
    SOC_SAND_IN  uint8                                 accept
  );

uint32
  arad_pp_eg_filter_pep_acceptable_frames_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx,
    SOC_SAND_IN  uint8                                 accept
  );

uint32
  arad_pp_eg_filter_pep_acceptable_frames_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx
  );


uint32
  arad_pp_eg_filter_pep_acceptable_frames_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx,
    SOC_SAND_OUT uint8                                 *accept
  );


uint32
  arad_pp_eg_filter_pvlan_port_type_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE           pvlan_port_type
  );

uint32
  arad_pp_eg_filter_pvlan_port_type_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE           pvlan_port_type
  );

uint32
  arad_pp_eg_filter_pvlan_port_type_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx
  );


uint32
  arad_pp_eg_filter_pvlan_port_type_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE    *pvlan_port_type
  );


uint32
  arad_pp_eg_filter_split_horizon_out_ac_orientation_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION  orientation
  );

uint32
  arad_pp_eg_filter_split_horizon_out_ac_orientation_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION  orientation
  );

uint32
  arad_pp_eg_filter_split_horizon_out_ac_orientation_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx
  );


uint32
  arad_pp_eg_filter_split_horizon_out_ac_orientation_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION  *orientation
  );



uint32
  arad_pp_eg_filter_split_horizon_out_lif_orientation_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        lif_eep_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  );

uint32
  arad_pp_eg_filter_split_horizon_out_lif_orientation_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        lif_eep_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  );


uint32
  arad_pp_eg_filter_split_horizon_out_lif_orientation_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           lif_eep_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION      *orientation
  );

uint32
  arad_pp_eg_filter_split_horizon_out_lif_orientation_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           eep_ndx
  );



CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_filter_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_filter_get_errs_ptr(void);

uint32
  SOC_PPC_EG_FILTER_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PORT_INFO *info
  );

#ifdef BCM_88660_A0
uint32
  ARAD_PP_EG_FILTER_GLOBAL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_FILTER_GLOBAL_INFO *info
  );
#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



