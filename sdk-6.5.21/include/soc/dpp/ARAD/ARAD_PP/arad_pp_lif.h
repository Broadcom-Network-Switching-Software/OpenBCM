/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_LIF_INCLUDED__

#define __ARAD_PP_LIF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>





#define ARAD_PP_LIF_PORT_PROFILE_NDX_MAX                          (7)
#define ARAD_PP_SW_DB_MULTI_SET_VLAN_COMPRESSION_RANGE_NOF_MEMBER (32)
#define ARAD_PP_SW_DB_MULTI_SET_LIF_AC_PROFILE_NOF_MEMBERS(unit)  (SOC_DPP_DEFS_GET(unit, nof_vtt_program_selection_lines) + 1)
#define ARAD_PP_LIF_LIF_PROFILE_MAX                              (15)




#define ARAD_PP_LIF_LOCAL_INLIF_HANDLE_TO_BASE_LIF_ID(lif_handle)             ( (lif_handle%2==0) ? (lif_handle/2) :  (lif_handle/2 + 1) )
#define ARAD_PP_LIF_LOCAL_INLIF_HANDLE_TO_ADDITIONAL_LIF_ID(lif_handle)       ( (lif_handle%2==0) ? (lif_handle/2  + 1) :  (lif_handle/2) )
#define ARAD_PP_LIF_LOCAL_INLIF_HANDLE_TO_ADDITIONAL_OFFSET(base_lif, ext_lif) ( (ext_lif>base_lif) ? TRUE : FALSE ) 
#define ARAD_PP_LIF_LOCAL_INLIF_HANDLE_TO_SEM_RESULT_ID(base_lif, ext_lif)    (  SOC_SAND_MIN(base_lif,ext_lif) )

#define ARAD_PP_LIF_SEM_RESULT_ID_TO_LOCAL_INLIF_HANDLE(sem_result_id)    (sem_result_id*2 )

#define ARAD_PP_LIF_PROTECTION_POINTER_NOF_BITS_ARAD             (14)
#define ARAD_PP_LIF_PROTECTION_POINTER_MASK_ARAD                 (SOC_SAND_BITS_MASK((ARAD_PP_LIF_PROTECTION_POINTER_NOF_BITS_ARAD - 1), 0))
#define ARAD_PP_LIF_PROTECTION_POINTER_DEFAULT_VALUE             ((1 << ARAD_PP_LIF_PROTECTION_POINTER_NOF_BITS_ARAD) - (1))





typedef enum
{
  
  ARAD_PP_L2_LIF_PWE_MAP_RANGE_SET = ARAD_PP_PROC_DESC_BASE_LIF_FIRST,
  ARAD_PP_L2_LIF_PWE_MAP_RANGE_SET_UNSAFE,
  ARAD_PP_L2_LIF_PWE_MAP_RANGE_SET_VERIFY,
  ARAD_PP_L2_LIF_PWE_MAP_RANGE_GET,
  ARAD_PP_L2_LIF_PWE_MAP_RANGE_GET_VERIFY,
  ARAD_PP_L2_LIF_PWE_MAP_RANGE_GET_UNSAFE,
  ARAD_PP_L2_LIF_PWE_ADD,
  ARAD_PP_L2_LIF_PWE_ADD_UNSAFE,
  ARAD_PP_L2_LIF_PWE_ADD_INTERNAL_UNSAFE,
  ARAD_PP_L2_LIF_PWE_ADD_VERIFY,
  ARAD_PP_L2_LIF_PWE_GET,
  ARAD_PP_L2_LIF_PWE_GET_UNSAFE,
  ARAD_PP_L2_LIF_PWE_GET_INTERNAL_UNSAFE,
  ARAD_PP_L2_LIF_PWE_GET_VERIFY,
  ARAD_PP_L2_LIF_PWE_REMOVE,
  ARAD_PP_L2_LIF_PWE_REMOVE_UNSAFE,
  ARAD_PP_L2_LIF_PWE_REMOVE_VERIFY,
  ARAD_PP_L2_LIF_VLAN_COMPRESSION_ADD,
  ARAD_PP_L2_LIF_VLAN_COMPRESSION_ADD_UNSAFE,
  ARAD_PP_L2_LIF_VLAN_COMPRESSION_ADD_VERIFY,
  ARAD_PP_L2_LIF_VLAN_COMPRESSION_RANGE_VALID_SET_UNSAFE,
  ARAD_PP_L2_LIF_VLAN_COMPRESSION_RANGE_VALID_GET_UNSAFE,
  ARAD_PP_L2_LIF_VLAN_COMPRESSION_IS_VALID_RANGE,
  ARAD_PP_L2_LIF_VLAN_COMPRESSION_REMOVE,
  ARAD_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_UNSAFE,
  ARAD_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_VERIFY,
  ARAD_PP_L2_LIF_VLAN_COMPRESSION_GET,
  ARAD_PP_L2_LIF_VLAN_COMPRESSION_GET_UNSAFE,
  ARAD_PP_L2_LIF_VLAN_COMPRESSION_GET_VERIFY,
  ARAD_PP_L2_LIF_AC_MAP_KEY_SET,
  ARAD_PP_L2_LIF_AC_MAP_KEY_SET_UNSAFE,
  ARAD_PP_L2_LIF_AC_MAP_KEY_SET_VERIFY,
  ARAD_PP_L2_LIF_AC_MAP_KEY_GET,
  ARAD_PP_L2_LIF_AC_MAP_KEY_GET_VERIFY,
  ARAD_PP_L2_LIF_AC_MAP_KEY_GET_UNSAFE,
  SOC_PPC_L2_LIF_AC_MP_INFO_SET,
  SOC_PPC_L2_LIF_AC_MP_INFO_SET_UNSAFE,
  SOC_PPC_L2_LIF_AC_MP_INFO_SET_VERIFY,
  SOC_PPC_L2_LIF_AC_MP_INFO_GET,
  SOC_PPC_L2_LIF_AC_MP_INFO_GET_VERIFY,
  SOC_PPC_L2_LIF_AC_MP_INFO_GET_UNSAFE,
  ARAD_PP_L2_LIF_AC_ADD,
  ARAD_PP_L2_LIF_AC_ADD_UNSAFE,
  ARAD_PP_L2_LIF_AC_ADD_INTERNAL_UNSAFE,
  ARAD_PP_L2_LIF_AC_ADD_VERIFY,
  ARAD_PP_L2_LIF_AC_GET,
  ARAD_PP_L2_LIF_AC_GET_UNSAFE,
  ARAD_PP_L2_LIF_AC_GET_INTERNAL_UNSAFE,
  ARAD_PP_L2_LIF_AC_GET_VERIFY,
  ARAD_PP_L2_LIF_AC_WITH_COS_ADD,
  ARAD_PP_L2_LIF_AC_WITH_COS_ADD_UNSAFE,
  ARAD_PP_L2_LIF_AC_WITH_COS_ADD_VERIFY,
  ARAD_PP_L2_LIF_AC_WITH_COS_GET,
  ARAD_PP_L2_LIF_AC_WITH_COS_GET_UNSAFE,
  ARAD_PP_L2_LIF_AC_WITH_COS_GET_VERIFY,
  ARAD_PP_L2_LIF_AC_REMOVE,
  ARAD_PP_L2_LIF_AC_REMOVE_UNSAFE,
  ARAD_PP_L2_LIF_AC_REMOVE_VERIFY,
  ARAD_PP_L2_LIF_L2CP_TRAP_SET,
  ARAD_PP_L2_LIF_L2CP_TRAP_SET_UNSAFE,
  ARAD_PP_L2_LIF_L2CP_TRAP_SET_VERIFY,
  ARAD_PP_L2_LIF_L2CP_TRAP_GET,
  ARAD_PP_L2_LIF_L2CP_TRAP_GET_VERIFY,
  ARAD_PP_L2_LIF_L2CP_TRAP_GET_UNSAFE,
  ARAD_PP_L2_LIF_ISID_ADD,
  ARAD_PP_L2_LIF_ISID_ADD_UNSAFE,
  ARAD_PP_L2_LIF_ISID_ADD_INTERNAL_UNSAFE,
  ARAD_PP_L2_LIF_ISID_ADD_VERIFY,
  ARAD_PP_L2_LIF_ISID_GET,
  ARAD_PP_L2_LIF_ISID_GET_UNSAFE,
  ARAD_PP_L2_LIF_ISID_GET_INTERNAL_UNSAFE,
  ARAD_PP_L2_LIF_ISID_GET_VERIFY,
  ARAD_PP_L2_LIF_VSI_TO_ISID,
  ARAD_PP_L2_LIF_VSI_TO_ISID_UNSAFE,
  ARAD_PP_L2_LIF_VSI_TO_ISID_VERIFY,
  ARAD_PP_L2_LIF_ISID_REMOVE,
  ARAD_PP_L2_LIF_ISID_REMOVE_UNSAFE,
  ARAD_PP_L2_LIF_ISID_REMOVE_VERIFY,
  ARAD_PP_L2_LIF_GRE_ADD,
  ARAD_PP_L2_LIF_GRE_ADD_UNSAFE,
  ARAD_PP_L2_LIF_GRE_ADD_INTERNAL_UNSAFE,
  ARAD_PP_L2_LIF_GRE_ADD_VERIFY,
  ARAD_PP_L2_LIF_GRE_GET,
  ARAD_PP_L2_LIF_GRE_GET_UNSAFE,
  ARAD_PP_L2_LIF_GRE_GET_INTERNAL_UNSAFE,
  ARAD_PP_L2_LIF_GRE_GET_VERIFY,
  ARAD_PP_L2_LIF_GRE_REMOVE,
  ARAD_PP_L2_LIF_GRE_REMOVE_UNSAFE,
  ARAD_PP_L2_LIF_GRE_REMOVE_VERIFY,
  ARAD_PP_L2_LIF_VXLAN_ADD,
  ARAD_PP_L2_LIF_VXLAN_ADD_UNSAFE,
  ARAD_PP_L2_LIF_VXLAN_ADD_INTERNAL_UNSAFE,
  ARAD_PP_L2_LIF_VXLAN_ADD_VERIFY,
  ARAD_PP_L2_LIF_VXLAN_GET,
  ARAD_PP_L2_LIF_VXLAN_GET_UNSAFE,
  ARAD_PP_L2_LIF_VXLAN_GET_INTERNAL_UNSAFE,
  ARAD_PP_L2_LIF_VXLAN_GET_VERIFY,
  ARAD_PP_L2_LIF_VXLAN_REMOVE,
  ARAD_PP_L2_LIF_VXLAN_REMOVE_UNSAFE,
  ARAD_PP_L2_LIF_VXLAN_REMOVE_VERIFY,
  ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_SET,
  ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_SET_UNSAFE,
  ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_SET_VERIFY,
  ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_GET,
  ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_GET_UNSAFE,
  ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_GET_VERIFY,
  ARAD_PP_LIF_GET_PROCS_PTR,
  ARAD_PP_LIF_GET_ERRS_PTR,
  

  
  ARAD_PP_LIF_PROCEDURE_DESC_LAST
} ARAD_PP_LIF_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_LIF_SUCCESS_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LIF_FIRST,
  ARAD_PP_LIF_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_KEY_MAPPING_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_KEY_MAPPING_ENTRY_EXISTS_ERR,
  ARAD_PP_LIF_KEY_MAPPING_NOT_SIMPLE_VLAN_TRANSLATION_ERR,
  ARAD_PP_LIF_HANDLE_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_DEFAULT_FRWD_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ORIENTATION_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_TPID_PROFILE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_SERVICE_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_VLAN_DOMAIN_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_VID_TAG_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_VLAN_RANGE_FIRST_OVER_LAST_ERR,
  ARAD_PP_LIF_VLAN_RANGE_VLAN_RANGE_ALREADY_DEFINED_ERR,
  ARAD_PP_LIF_KEY_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_RAW_KEY_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_LEARN_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_L2CP_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_OPCODE_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_NOF_LIF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ISID_DOMAIN_OUT_OF_RANGE_ERR,
  

  ARAD_PP_LIF_VSI_IS_NOT_P2P_ERR,
  ARAD_PP_LIF_ILLEGAL_KEY_MAPPING_ERR,
  ARAD_PP_LIF_INNER_EXISTS_OUTER_NOT_ERR,
  ARAD_PP_LIF_VSI_ASSIGNMENT_MODE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_LIF_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_PWE_TERM_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_PWE_ACTION_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_DEFAULT_FRWRD_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_PORT_DA_NOT_FOUND_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_PROTECTION_POINTER_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_PROTECTION_PASS_VAL_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_PWE_MPLS_TUNNEL_MODEL_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_AC_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_AC_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_PWE_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_PWE_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_AC_P2P_ORIENTATION_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_PWE_P2P_ORIENTATION_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_DA_TYPE_OUT_OF_RANGE_ERR,  
  ARAD_PP_LIF_VC_LABEL_KEY_ILLEGAL_ERR,
  ARAD_PP_LIF_ADD_VSI_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_OFFSET_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_GRE_KEY_VPN_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_GRE_KEY_VSI_NOT_EQ_LIF_ERR,
  ARAD_PP_LIF_VXLAN_KEY_VPN_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_VXLAN_KEY_VSI_NOT_EQ_LIF_ERR,
  ARAD_PP_LIF_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_NETWORK_GROUP_OUT_OF_RANGE_ERR,

  ARAD_PP_LIF_PPPOE_SESSION_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_VRF_FOR_L2TP_OUT_OF_RANGE_ERROR,

  
  ARAD_PP_LIF_ERR_LAST
} ARAD_PP_LIF_ERR;









uint32
  arad_pp_lif_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_l2_lif_pwe_map_range_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_IN_VC_RANGE                      *in_vc_range
  );

uint32
  arad_pp_l2_lif_pwe_map_range_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_IN_VC_RANGE                      *in_vc_range
  );

uint32
  arad_pp_l2_lif_pwe_map_range_get_verify(
    SOC_SAND_IN  int                                     unit
  );


uint32
  arad_pp_l2_lif_pwe_map_range_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PPC_L2_LIF_IN_VC_RANGE                      *in_vc_range
  );


uint32
  arad_pp_l2_lif_pwe_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO        *pwe_additional_info,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_INFO                     *pwe_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_l2_lif_pwe_add_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO        *pwe_additional_info,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_INFO                     *pwe_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_l2_lif_pwe_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_INFO                     *pwe_info
  );


uint32
  arad_pp_l2_lif_pwe_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO            *pwe_additional_info,
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_INFO                     *pwe_info,
    SOC_SAND_OUT uint8                                 *found
  );



uint32
  arad_pp_l2_lif_pwe_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                            in_vc_label,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                                  *lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO              *pwe_additional_info,
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_INFO                         *pwe_info,
    SOC_SAND_OUT uint8                                     *found
  );


uint32
  arad_pp_l2_lif_pwe_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label
  );


uint32
  arad_pp_l2_lif_pwe_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  uint8                                remove_from_sem,
    SOC_SAND_IN  uint8                                is_gal,
    SOC_SAND_OUT uint32                                  *lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO        *pwe_additional_info
  );

uint32
  arad_pp_l2_lif_pwe_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  uint8                                remove_from_sem
  );


uint32
  arad_pp_l2_lif_vlan_compression_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                   *vlan_range_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_l2_lif_vlan_compression_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                   *vlan_range_key
  );


uint32
  arad_pp_l2_lif_vlan_compression_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                   *vlan_range_key
  );

uint32
  arad_pp_l2_lif_vlan_compression_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                   *vlan_range_key
  );


uint32
  arad_pp_l2_lif_vlan_compression_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                        core_id,
    SOC_SAND_IN  uint32                                     local_port_ndx,
    SOC_SAND_OUT SOC_PPC_L2_PORT_VLAN_RANGE_INFO            *vlan_range_info
  );

uint32
  arad_pp_l2_lif_vlan_compression_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                     local_port_ndx,
    SOC_SAND_OUT SOC_PPC_L2_PORT_VLAN_RANGE_INFO            *vlan_range_info
  );


uint32
  arad_pp_l2_lif_ac_map_key_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER             *qual_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE              key_mapping
  );

uint32
  arad_pp_l2_lif_ac_map_key_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER                 *qual_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE                  key_mapping
  );

uint32
  arad_pp_l2_lif_ac_map_key_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER                 *qual_key
  );


uint32
  arad_pp_l2_lif_ac_map_key_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER             *qual_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE              *key_mapping
  );


uint32
  arad_pp_l2_lif_ac_mp_info_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                       lif_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MP_INFO            *info
  );

uint32
  arad_pp_l2_lif_ac_mp_info_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                       lif_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MP_INFO            *info
  );

uint32
  arad_pp_l2_lif_ac_mp_info_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                       lif_ndx
  );


uint32
  arad_pp_l2_lif_ac_mp_info_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                       lif_ndx,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_MP_INFO            *info
  );


uint32
  arad_pp_l2_lif_ac_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_INFO                      *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );


uint32
  arad_pp_l2_lif_ac_add_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                                  lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  );


uint32
  arad_pp_l2_lif_ac_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_INFO                      *ac_info
  );


uint32
  arad_pp_l2_lif_ac_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_INFO                      *ac_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_l2_lif_ac_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                                  *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                     *found
  );

uint32
  arad_pp_l2_lif_ac_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key
  );


uint32
  arad_pp_l2_lif_ac_with_cos_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              base_lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_GROUP_INFO                *acs_group_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_l2_lif_ac_with_cos_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              base_lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_GROUP_INFO                *acs_group_info
  );


uint32
  arad_pp_l2_lif_ac_with_cos_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *base_lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_GROUP_INFO                *acs_group_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_l2_lif_ac_with_cos_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key
  );


uint32
  arad_pp_l2_lif_ac_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  );

uint32
  arad_pp_l2_lif_ac_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key
  );


uint32
  arad_pp_l2_lif_l2cp_trap_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY                     *l2cp_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE             handle_type
  );

uint32
  arad_pp_l2_lif_l2cp_trap_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY                     *l2cp_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE             handle_type
  );

uint32
  arad_pp_l2_lif_l2cp_trap_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY                     *l2cp_key
  );


uint32
  arad_pp_l2_lif_l2cp_trap_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY                     *l2cp_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE             *handle_type
  );


uint32
  arad_pp_l2_lif_isid_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_INFO                    *isid_info,
    SOC_SAND_IN  uint8                                       is_ingress,
    SOC_SAND_IN  uint8                                       is_egress,
    SOC_SAND_IN  uint8                                       isem_only,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_l2_lif_isid_add_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                         vsi_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                    *isid_key,
    SOC_SAND_IN  uint8                                      ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                             lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_INFO                   *isid_info,
    SOC_SAND_IN  uint8                                       is_ingress,
    SOC_SAND_IN  uint8                                     is_egress,
    SOC_SAND_IN  uint8                                      isem_only,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                   *success
  );

uint32
  arad_pp_l2_lif_isid_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_INFO                    *isid_info
  );


uint32
  arad_pp_l2_lif_isid_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_SYS_VSI_ID                          *vsi_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_ISID_INFO                    *isid_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_l2_lif_isid_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                    *isid_key,
    SOC_SAND_IN  uint8                                      ignore_key, 
    SOC_SAND_OUT SOC_PPC_LIF_ID                             *lif_index,
    SOC_SAND_OUT SOC_PPC_SYS_VSI_ID                         *vsi_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_ISID_INFO                   *isid_info,
    SOC_SAND_OUT uint8                                      *found
  );

uint32
  arad_pp_l2_lif_isid_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key
  );


uint32
  arad_pp_l2_lif_vsi_to_isid_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_ISID                                  *isid_id
  );

uint32
  arad_pp_l2_lif_vsi_to_isid_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                              vsi_ndx
  );


uint32
  arad_pp_l2_lif_isid_remove_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  uint8                                       is_ingress,
    SOC_SAND_IN  uint8                                       is_egress,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                          vsi,
    SOC_SAND_IN  uint8                                       isem_only,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  );

uint32
  arad_pp_l2_lif_isid_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                   *isid_key
  );



uint32
  arad_pp_l2_lif_gre_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_INFO                    *gre_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_l2_lif_gre_add_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                    *gre_key,
    SOC_SAND_IN  uint8                                      ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                             lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_INFO                   *gre_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                   *success
  );

uint32
  arad_pp_l2_lif_gre_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_INFO                    *gre_info
  );


uint32
  arad_pp_l2_lif_gre_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_GRE_INFO                    *gre_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_l2_lif_gre_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                    *gre_key,
    SOC_SAND_IN  uint8                                      ignore_key, 
    SOC_SAND_OUT SOC_PPC_LIF_ID                             *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_GRE_INFO                   *gre_info,
    SOC_SAND_OUT uint8                                      *found
  );

uint32
  arad_pp_l2_lif_gre_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key
  );


uint32
  arad_pp_l2_lif_gre_remove_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  );

uint32
  arad_pp_l2_lif_gre_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                   *gre_key
  );


uint32
  arad_pp_l2_lif_vxlan_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_INFO                    *vxlan_info,
    SOC_SAND_IN  int                                         ignore_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_l2_lif_vxlan_add_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                    *vxlan_key,
    SOC_SAND_IN  uint8                                      ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                             lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_INFO                   *vxlan_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                   *success
  );

uint32
  arad_pp_l2_lif_vxlan_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_INFO                    *vxlan_info,
    SOC_SAND_IN  int                                         igrore_key
  );


uint32
  arad_pp_l2_lif_vxlan_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_VXLAN_INFO                    *vxlan_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_l2_lif_vxlan_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                    *vxlan_key,
    SOC_SAND_IN  uint8                                      ignore_key, 
    SOC_SAND_OUT SOC_PPC_LIF_ID                             *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_VXLAN_INFO                   *vxlan_info,
    SOC_SAND_OUT uint8                                      *found
  );

uint32
  arad_pp_l2_lif_vxlan_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key
  );


uint32
  arad_pp_l2_lif_vxlan_remove_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  );

uint32
  arad_pp_l2_lif_vxlan_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                   *vxlan_key
  );


uint32
  arad_pp_l2_lif_default_frwrd_info_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY        *dflt_frwrd_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION     *action_info
  );

uint32
  arad_pp_l2_lif_default_frwrd_info_set_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY        *dflt_frwrd_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION     *action_info
  );

uint32
  arad_pp_l2_lif_default_frwrd_info_get_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY        *dflt_frwrd_key    
  );


uint32
  arad_pp_l2_lif_default_frwrd_info_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY        *dflt_frwrd_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION     *action_info
  );



uint32 arad_pp_l2_lif_extender_add_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY        *extender_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                     lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO       *extender_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE           *success);


uint32 arad_pp_l2_lif_extender_add_internal_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY        *extender_key,
    SOC_SAND_IN  uint8                              ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                     lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO       *extender_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE           *success);


uint32 arad_pp_l2_lif_extender_add_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY        *extender_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                     lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO       *extender_info);



uint32 arad_pp_l2_lif_extender_get(
    int                                unit,
    SOC_PPC_L2_LIF_EXTENDER_KEY        *extender_key,
    SOC_PPC_LIF_ID                     *lif_index,
    SOC_PPC_L2_LIF_EXTENDER_INFO       *extender_info,
    uint8                              *found);

soc_error_t arad_pp_l2_lif_extender_remove(
    int                                     unit,
    SOC_PPC_L2_LIF_EXTENDER_KEY             *extender_key,
    int                                     *lif_index);




uint32
  arad_pp_l2_lif_ac_key_parse_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY             *isem_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_KEY               *ac_key
  );

uint32
  arad_pp_l2_lif_pwe_key_parse_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY              *isem_key,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_LABEL               *pwe
  );

uint32
  arad_pp_l2_lif_isid_key_parse_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY              *isem_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_ISID_KEY              *isid_key
  );

soc_error_t
arad_pp_lif_additional_data_set(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  int lif_id, 
        SOC_SAND_IN  uint8 is_ingress,
        SOC_SAND_IN  uint64 data,
        SOC_SAND_IN  uint8 is_clear
   );


soc_error_t
arad_pp_lif_additional_data_get(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  int lif_id, 
        SOC_SAND_IN  uint8 is_ingress,
        SOC_SAND_OUT uint64 *data
   );

 
int 
arad_pp_lif_index_to_sem_result( 
    int                    unit,
    SOC_PPC_LIF_ID         base_lif_id,
    SOC_PPC_LIF_ID         ext_lif_id,
    uint32                 *sem_result
   ); 

uint32 
arad_pp_sem_result_to_lif_index( 
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 sem_result
   );

soc_error_t
arad_pp_lif_is_wide_entry(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  int lif_id, 
        SOC_SAND_IN  uint8 is_ingress,
        SOC_SAND_OUT uint8 *is_wide_entry,
        SOC_SAND_OUT uint8 *ext_type);

void
arad_pp_l2_lif_convert_protection_fields_to_hw(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  uint32 protection_pointer,
        SOC_SAND_IN  uint8  protection_pass_value,
        SOC_SAND_OUT uint32 *hw_protection_pointer,
        SOC_SAND_OUT uint32 *hw_protection_path);


void
arad_pp_l2_lif_convert_protection_fields_from_hw(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  uint32 hw_protection_pointer,
        SOC_SAND_IN  uint32 hw_protection_path,
        SOC_SAND_OUT uint32 *protection_pointer,
        SOC_SAND_OUT uint8  *protection_pass_value);


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_lif_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_lif_get_errs_ptr(void);

uint32
  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER *info
  );

uint32
  SOC_PPC_L2_LIF_IN_VC_RANGE_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_IN_VC_RANGE *info
  );

uint32
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO *info
  );

uint32
  SOC_PPC_L2_LIF_PWE_LEARN_RECORD_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_LEARN_RECORD *info
  );

uint32
  SOC_PPC_L2_LIF_PWE_INFO_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_INFO *info
  );

uint32
  SOC_PPC_L2_VLAN_RANGE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY *info
  );

uint32
  SOC_PPC_L2_LIF_AC_KEY_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY *info
  );

uint32
  SOC_PPC_L2_LIF_AC_LEARN_RECORD_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_LEARN_RECORD *info
  );

uint32
  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO *info
  );

uint32
  SOC_PPC_L2_LIF_AC_INFO_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_INFO *info
  );

uint32
  SOC_PPC_L2_LIF_AC_MP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MP_INFO *info
  );

uint32
  SOC_PPC_L2_LIF_L2CP_KEY_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY *info
  );

uint32
  SOC_PPC_L2_LIF_AC_GROUP_INFO_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_GROUP_INFO *info
  );

uint32
  SOC_PPC_L2_LIF_ISID_KEY_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY *info
  );

uint32
  SOC_PPC_L2_LIF_ISID_INFO_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_INFO *info
  );

uint32
  SOC_PPC_L2_LIF_TRILL_INFO_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_TRILL_INFO *info
  );

uint32
  SOC_PPC_L2_LIF_GRE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY *info
  );

uint32
  SOC_PPC_L2_LIF_GRE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_INFO *info
  );

uint32
  SOC_PPC_L2_LIF_VXLAN_KEY_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY *info
  );

uint32
  SOC_PPC_L2_LIF_VXLAN_INFO_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_INFO *info
  );

uint32
  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY *info
  );

uint32
  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION *info
  );

uint32 SOC_PPC_L2_LIF_EXTENDER_KEY_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY *info);

uint32 SOC_PPC_L2_LIF_EXTENDER_INFO_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO *info);



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

