#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_VSI
#include <soc/mem.h>




#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_rif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mpls_term.h>
#include <soc/dpp/drv.h>
#include <soc/drv.h>
#include <soc/dpp/mbcm_pp.h>

#include <soc/dpp/PPD/ppd_api_lif_table.h>


#include <shared/bitop.h>
 
#define ARAD_PP_LIF_NULL 0xFFFFFFFF





#define ARAD_PP_RIF_TTL_SCOPE_INDEX_MAX          7 

#define ARAD_PP_RIF_ISEM_RES_MPLS_TYPE_PROTOCOL_OR_LSP 2
#define ARAD_PP_RIF_FIRST_LABEL_MAX              ((1<<20) - 1)
#define ARAD_PP_RIF_LAST_LABEL_MAX               ((1<<20) - 1)

#define ARAD_PP_RIF_MPLS_TERM_PROFILE_LSB_NOF_BITS    (2)
#define ARAD_PP_RIF_MPLS_TERM_PROFILE_LSB_MASK        ((1 << ARAD_PP_RIF_MPLS_TERM_PROFILE_LSB_NOF_BITS) - 1)
#define ARAD_PP_RIF_MPLS_TERM_PROFILE_MSB_MASK        (1 << ARAD_PP_RIF_MPLS_TERM_PROFILE_LSB_NOF_BITS)













CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_rif[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_MPLS_LABELS_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_MPLS_LABELS_RANGE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_MPLS_LABELS_RANGE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_MPLS_LABELS_RANGE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_MPLS_LABELS_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_MPLS_LABELS_RANGE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_MPLS_LABELS_RANGE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_MPLS_LABELS_RANGE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_ADD_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_GET_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_MPLS_LABEL_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_GET_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_IP_TUNNEL_TERM_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_VSID_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_VSID_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_VSID_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_VSID_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_VSID_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_VSID_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_VSID_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_VSID_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_RIF_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_TTL_SCOPE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_TTL_SCOPE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_TTL_SCOPE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_TTL_SCOPE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_TTL_SCOPE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_TTL_SCOPE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_TTL_SCOPE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_TTL_SCOPE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_RIF_GET_ERRS_PTR),
  



  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_rif[] =
{
  {
    ARAD_PP_RIF_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_RIF_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_DIP_KEY_OUT_OF_RANGE_ERR,
    "ARAD_PP_RIF_DIP_KEY_OUT_OF_RANGE_ERR",
    "The parameter 'dip_key' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_FIRST_LABEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_RIF_FIRST_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'first_label' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_LAST_LABEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_RIF_LAST_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'last_label' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_COS_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_RIF_COS_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'cos_profile' is out of range. \n\r "
    "The range is: 0 - 63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_TTL_SCOPE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_RIF_TTL_SCOPE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'ttl_scope_index' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_ROUTING_ENABLERS_BM_OUT_OF_RANGE_ERR,
    "ARAD_PP_RIF_ROUTING_ENABLERS_BM_OUT_OF_RANGE_ERR",
    "The parameter 'routing_enablers_bm' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_MPLS_LABEL_INVALID_RANGE_ERR,
    "ARAD_PP_RIF_MPLS_LABEL_INVALID_RANGE_ERR",
    "The MPLS label range is invalid (first > last). \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR,
    "ARAD_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR",
    "invalid key for MPLS termination, \n\r "
    "key type set to Label only, and key inserted \n\r "
    "includes VSI != 0\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_VRF_NOT_FOUND_ERR,
    "ARAD_PP_RIF_VRF_NOT_FOUND_ERR",
    "VRF not found in IN_RIF_CONFIG_TABLE\n\r "
    "key type set to Label only, and key inserted \n\r ",
    SOC_SAND_SVR_WRN,
    TRUE
  },
  {
    ARAD_PP_RIF_LIF_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_RIF_LIF_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'lif_profile' is out of range.\n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_TPID_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_RIF_TPID_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile' is out of range.\n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_RIF_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'default_forward_profile' is out of range.\n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_RIF_SERVICE_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_RIF_SERVICE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'service_type' is out of range.\n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_RIF_VSI_ASSIGNMENT_MODE_OUT_OF_RANGE_ERR,
    "ARAD_RIF_VSI_ASSIGNMENT_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'vsi_assignment_mode' is out of range.\n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_RIF_PROTECTION_POINTER_OUT_OF_RANGE_ERR,
    "ARAD_RIF_PROTECTION_POINTER_OUT_OF_RANGE_ERR",
    "The parameter 'protection_pointer' is out of range.\n\r "
    "The range is: 0 - 0x3fff.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_RIF_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_RIF_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'processing_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_MPLS_TERM_MODEL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_RIF_MPLS_TERM_NEXT_PRTCL_OUT_OF_RANGE_ERR,
    "ARAD_RIF_MPLS_TERM_NEXT_PRTCL_OUT_OF_RANGE_ERR",
    "The parameter 'next_prtcl' is out of range. \n\r "
    "The range is: SOC_PPC_NOF_L3_NEXT_PRTCL_TYPES- SOC_PPC_NOF_L3_NEXT_PRTCL_TYPES.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_RIF_TRAP_CODE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_RIF_TRAP_CODE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code_index' is out of range.\n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_RIF_LEARN_RECORD_OUT_OF_RANGE_ERR,
    "ARAD_RIF_LEARN_RECORD_OUT_OF_RANGE_ERR",
    "The parameter 'learn_record' is out of range.\n\r "
    "The content should be type FEC and dest id up to 0x7fff (FEC pointer) or type FLOW and dest_id up to 0x7ffc with the two least significant bits 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_RIF_FORWARADING_CODE_OUT_OF_RANGE_ERR,
    "ARAD_RIF_FORWARADING_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'forwarding_code' is out of range.\n\r "
    "The range is:0-SOC_TMC_NOF_PKT_FRWRD_TYPES.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_IP_TUNNEL_TERM_KEY_ILLEGAL_ERR,
    "ARAD_PP_RIF_IP_TUNNEL_TERM_KEY_ILLEGAL_ERR",
    "unexpected key for IP tunnel .\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },


  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};







uint32
  arad_pp_get_in_rif_profile_from_vrf_internal_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID  vrf_id,
    SOC_SAND_OUT uint8   *out_is_in_rif_profile
  )
{
  uint32  res = SOC_SAND_OK;
  uint32  data[ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE];
  int rif_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_GET_IN_RIF_PROFILE_FROM_VRF_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(out_is_in_rif_profile);

  for (; rif_id < SOC_DPP_CONFIG(unit)->l3.nof_rifs; ++ rif_id) {
    res = soc_mem_read(unit, IHP_IN_RIF_CONFIG_TABLEm, MEM_BLOCK_ANY, rif_id, data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    if ( soc_mem_field32_get(unit, IHP_IN_RIF_CONFIG_TABLEm, data, VRFf) == vrf_id ) {
      break;
    }
  }

  if (rif_id < SOC_DPP_CONFIG(unit)->l3.nof_rifs) { 
    *out_is_in_rif_profile = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, IHP_IN_RIF_CONFIG_TABLEm, data, L_3_VPN_DEFAULT_ROUTINGf));
  } else { 
    return ARAD_PP_RIF_VRF_NOT_FOUND_ERR;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_get_in_rif_profile_from_vrf_internal_unsafe()", rif_id, 0);
}


uint32
  arad_pp_set_in_rif_profile_to_vrf_internal_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID  vrf_id,
    SOC_SAND_IN  uint8   is_in_rif_profile
  )
{
  uint32  res = ARAD_PP_RIF_VRF_NOT_FOUND_ERR;
  uint32  err;
  uint32  data[ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE];
  int rif_id;
  uint32 value = SOC_SAND_NUM2BOOL(is_in_rif_profile);

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SET_IN_RIF_PROFILE_TO_VRF_UNSAFE);

  for (rif_id = 0; rif_id < SOC_DPP_CONFIG(unit)->l3.nof_rifs; ++ rif_id) {
    err = soc_mem_read(unit, IHP_IN_RIF_CONFIG_TABLEm, MEM_BLOCK_ANY, rif_id, data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 10, exit);
    if ( soc_mem_field32_get(unit, IHP_IN_RIF_CONFIG_TABLEm, data, VRFf) == vrf_id ) {
      soc_mem_field32_set(unit, IHP_IN_RIF_CONFIG_TABLEm, data, L_3_VPN_DEFAULT_ROUTINGf, value);
      err = soc_mem_write(unit, IHP_IN_RIF_CONFIG_TABLEm, MEM_BLOCK_ANY, rif_id, data);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 20, exit);
      res = SOC_SAND_OK;
    }
  }
  return res;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_set_in_rif_profile_to_vrf_internal_unsafe()", rif_id, 0);
}

#ifdef BCM_88660_A0

typedef struct {
  SOC_PPC_FRWRD_FEC_RPF_MODE mode;
  SOC_PPC_LIF_ENTRY_INFO *info;
  uint32 dev_id;
} SOC_PPC_RIF_INFO_SET_IMP_UNSAFE_DATA;


STATIC uint32 
  _arad_pp_rif_set_lif_rpf_mode(
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ID  member,
    SOC_SAND_INOUT void *opaque
  )
{
  bcm_port_t lif_id = member;
  uint32 res;
  SOC_PPC_RIF_INFO_SET_IMP_UNSAFE_DATA *data = opaque;
  uint32 *inlif_profile_ptr;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  
  res = soc_ppd_lif_table_entry_get(data->dev_id, lif_id, data->info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  
  switch (data->info->type) {
  case SOC_PPC_LIF_ENTRY_TYPE_AC:
    inlif_profile_ptr = &data->info->value.ac.lif_profile;
    break;
  case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
    inlif_profile_ptr = &data->info->value.ip_term_info.lif_profile;
    break;
  case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
    inlif_profile_ptr = &data->info->value.mpls_term_info.lif_profile;
    break;
  default:
    inlif_profile_ptr = NULL;
    break;
  }

  res = MBCM_PP_DRIVER_CALL(data->dev_id, mbcm_pp_occ_mgmt_app_set,(data->dev_id, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_STRICT_URPF, ((data->mode == SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT) ? 1 : 0), inlif_profile_ptr));
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  ARAD_DEVICE_CHECK(data->dev_id, exit);
  res = soc_ppd_lif_table_entry_update(data->dev_id, lif_id, data->info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_rif_set_lif_rpf_mode()", 0, 0);
}
#endif 
STATIC uint32
  arad_pp_rif_routing_enablers_profile_configure(SOC_SAND_IN int unit, SOC_SAND_IN uint32 routing_enablers_bm, SOC_SAND_IN uint8 routing_enablers_bm_id, uint8 is_strict)
{
  uint32
    res,
    entry_offset,
    class_id,
    dedicated_value,
    dedicated_bits,
    class_id_bits,
    tbl_data = 0,
    urpf_mask = (1 << ARAD_PP_RIF_RIF_PROFILE_STRUCTURE_URPF_ENABLE_BIT);
  uint64 val64;
  uint8 l3_ingress_urpf_enable = soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0);


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  if (routing_enablers_bm != 0) {
      tbl_data = SOC_PPC_RIF_PROFILE_TO_ROUTING_ENABLERS_VECTOR_BEFORE_INIT;
  }


  dedicated_bits = SOC_DPP_CONFIG(unit)->l3.nof_dedicated_in_rif_profile_bits;
  class_id_bits  = SOC_DPP_CONFIG(unit)->l3.bit_offset_of_the_routing_enablers_profile;

  if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV4UC_ENABLED(routing_enablers_bm)){
    tbl_data = tbl_data | 1<<SOC_PPC_ROUTING_ENABLE_IPV4UC_BIT;
  }

  if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV4MC_ENABLED(routing_enablers_bm)){
    tbl_data = tbl_data | 1<<SOC_PPC_ROUTING_ENABLE_IPV4MC_BIT;
  }

  if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV6UC_ENABLED(routing_enablers_bm)){
    tbl_data = tbl_data | 1<<SOC_PPC_ROUTING_ENABLE_IPV6UC_BIT;
  }

  if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV6MC_ENABLED(routing_enablers_bm)){
    tbl_data = tbl_data | 1<<SOC_PPC_ROUTING_ENABLE_IPV6MC_BIT;
  }

  if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_MPLS_ENABLED(routing_enablers_bm)){
    tbl_data = tbl_data | 1<<SOC_PPC_ROUTING_ENABLE_MPLS_UC_BIT;
    tbl_data = tbl_data | 1<<SOC_PPC_ROUTING_ENABLE_MPLS_MC_BIT;
  }

  

  COMPILER_64_ZERO(val64);
  res = READ_IHB_IN_RIF_UC_RPF_MODEL_IS_STRICTr(unit, SOC_CORE_ALL, &val64);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  for(dedicated_value = 0; dedicated_value < (1 << dedicated_bits); dedicated_value++) {
      for(class_id = 0; class_id < (1 << class_id_bits); class_id++) {
          entry_offset  = (dedicated_value << (SOC_DPP_NOF_IN_RIF_PROFILE_BITS - dedicated_bits)) | class_id;
          entry_offset |= (routing_enablers_bm_id << class_id_bits);
          res = arad_pp_ihp_map_rif_profile_to_routing_enable_tbl_set_unsafe(unit, entry_offset, tbl_data);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
          if (l3_ingress_urpf_enable && ((urpf_mask & entry_offset) > 0)) {
              
              if (is_strict) {
                  COMPILER_64_BITSET(val64, entry_offset);
              } else {
                  COMPILER_64_BITCLR(val64, entry_offset);
              }
          }
      }
  }

  if (l3_ingress_urpf_enable) {
      res = WRITE_IHB_IN_RIF_UC_RPF_MODEL_IS_STRICTr(unit, SOC_CORE_ALL, val64);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_routing_enablers_profile_configure()", 0, 0);
}


soc_error_t
 arad_pp_rif_global_urpf_mode_set(int unit, int index, int urpf_mode)
{
    ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA tbl_data;
    SOC_PPC_RIF_INFO  rif_info;

    SOCDNX_INIT_FUNC_DEFS

    SOC_PPC_RIF_INFO_clear(&rif_info);

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_in_rif_config_table_tbl_get_unsafe(unit, index, &tbl_data));

    arad_pp_rif_profile_to_rif_info_convert(unit, tbl_data.rif_profile,
                                       &(rif_info.uc_rpf_enable),
                                       &(rif_info.enable_default_routing),
                                       &(rif_info.custom_rif_bit),
                                       &(rif_info.intf_class),
                                       &(rif_info.routing_enablers_bm_id),
                                       &(rif_info.routing_enablers_bm),
                                       1); 
    if (urpf_mode) {
        rif_info.uc_rpf_enable = TRUE;
    }else {
        rif_info.uc_rpf_enable = FALSE;
    }
    tbl_data.rif_profile = 0;
    arad_pp_rif_info_to_rif_profile_convert(unit,rif_info.uc_rpf_enable,
                                    rif_info.enable_default_routing,
                                    rif_info.custom_rif_bit,
                                    rif_info.intf_class,
                                    rif_info.routing_enablers_bm_id,
                                    &(tbl_data.rif_profile));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_in_rif_config_table_tbl_set_unsafe(unit, index, &tbl_data));



exit:
  SOCDNX_FUNC_RETURN;
}




STATIC uint32
  arad_pp_rif_info_set_imp_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID    rif_id,
    SOC_SAND_IN  SOC_PPC_RIF_INFO  *rif_info
  )
{
  uint32                                    res;
  ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA  tbl_data;
  ARAD_PP_EGQ_TTL_SCOPE_TBL_DATA            egq_ttl_scope_tbl;  
  SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;  

  uint8 is_strict;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_VSID_MAP_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  tbl_data.rif_profile = 0;
  tbl_data.cos_profile = rif_info->cos_profile;
  tbl_data.vrf = rif_info->vrf_id;
  if (SOC_IS_JERICHO(unit)) {

    if (rif_info->is_configure_enabler_needed) {
      is_strict = ((rif_info->uc_rpf_mode == SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT) ? 1 : 0);
      res = arad_pp_rif_routing_enablers_profile_configure(unit, rif_info->routing_enablers_bm, rif_info->routing_enablers_bm_id, is_strict);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

    arad_pp_rif_info_to_rif_profile_convert(unit,rif_info->uc_rpf_enable,
                                            rif_info->enable_default_routing,
                                            rif_info->custom_rif_bit,
                                            rif_info->intf_class,
                                            rif_info->routing_enablers_bm_id,
                                            &(tbl_data.rif_profile)); 

  }else {
    tbl_data.uc_rpf_enable = SOC_SAND_NUM2BOOL(rif_info->uc_rpf_enable);
    tbl_data.enable_routing_mc = (rif_info->routing_enablers_bm & SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IP_MC) ? 1 : 0;
    tbl_data.enable_routing_uc = (rif_info->routing_enablers_bm & SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IP_UC) ? 1 : 0;
    tbl_data.enable_routing_mpls = (rif_info->routing_enablers_bm & SOC_PPC_RIF_ROUTE_ENABLE_TYPE_MPLS) ? 1 : 0;
    tbl_data.l_3_vpn_default_routing = SOC_SAND_BOOL2NUM(rif_info->enable_default_routing);    
  }

  res = arad_pp_ihp_in_rif_config_table_tbl_set_unsafe(unit, rif_id, &tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  egq_ttl_scope_tbl.ttl_scope_index = rif_info->ttl_scope_index;
  res = arad_pp_egq_ttl_scope_tbl_set_unsafe(unit, rif_id, &egq_ttl_scope_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
    SOC_PPC_RIF_INFO_SET_IMP_UNSAFE_DATA data;
    uint8 urpf_mode, urpf_mode_old;

    ARAD_ALLOC(lif_info, SOC_PPC_LIF_ENTRY_INFO, sizeof(SOC_PPC_LIF_ENTRY_INFO), "arad_pp_rif_info_set_imp_unsafe.lif_info");

    data.mode = rif_info->uc_rpf_mode;
    data.info = lif_info;

    
    data.dev_id = unit;

    
    urpf_mode = rif_info->uc_rpf_mode; 
    res = sw_state_access[unit].dpp.soc.arad.pp.rif_to_lif_group_map.rif_urpf_mode.get(unit, rif_id, &urpf_mode_old);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

    if ( urpf_mode_old != urpf_mode ) {    
      res = sw_state_access[unit].dpp.soc.arad.pp.rif_to_lif_group_map.rif_urpf_mode.set(unit, rif_id, urpf_mode);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

      
      
      res = arad_pp_sw_db_rif_to_lif_group_map_visit_lif_group(unit, rif_id, _arad_pp_rif_set_lif_rpf_mode, (void*)&data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }
#endif 

exit:
  ARAD_FREE(lif_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_info_set_imp_unsafe()", rif_id, 0);
}

STATIC uint32
  arad_pp_rif_info_get_imp_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID    rif_id,
    SOC_SAND_OUT SOC_PPC_RIF_INFO  *rif_info
  )
{
  uint32                                 res;
  ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA tbl_data;
  ARAD_PP_EGQ_TTL_SCOPE_TBL_DATA           egq_ttl_scope_tbl; 
  uint64 val64;
  uint32 mask;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_PPC_RIF_INFO_clear(rif_info);

  res = arad_pp_ihp_in_rif_config_table_tbl_get_unsafe(unit, rif_id, &tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  rif_info->vrf_id = tbl_data.vrf;
  rif_info->cos_profile = tbl_data.cos_profile;
  mask = soc_arad_rif_info_routing_enabler_bm_mask_get(unit);

  rif_info->routing_enablers_bm_rif_profile = tbl_data.rif_profile & mask;

  if (SOC_IS_JERICHO(unit)) {

    arad_pp_rif_profile_to_rif_info_convert(unit, tbl_data.rif_profile,
                                               &(rif_info->uc_rpf_enable),
                                               &(rif_info->enable_default_routing),
                                               &(rif_info->custom_rif_bit),
                                               &(rif_info->intf_class),
                                               &(rif_info->routing_enablers_bm_id),
                                               &(rif_info->routing_enablers_bm),
                                               1); 
    
    
    if (rif_info->uc_rpf_enable) {
      res = READ_IHB_IN_RIF_UC_RPF_MODEL_IS_STRICTr(unit, SOC_CORE_ALL, &val64);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

      if (COMPILER_64_BITTEST(val64, tbl_data.rif_profile)) {
           rif_info->uc_rpf_mode = SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT;
      } else {
          rif_info->uc_rpf_mode = SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE;
      }
    }
  } else {
      rif_info->uc_rpf_enable = SOC_SAND_NUM2BOOL(tbl_data.uc_rpf_enable);
      rif_info->routing_enablers_bm =
        ((tbl_data.enable_routing_mc == 0) ? 0 : SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IP_MC) |
        ((tbl_data.enable_routing_uc == 0) ? 0 : SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IP_UC) |
        ((tbl_data.enable_routing_mpls == 0) ? 0 : SOC_PPC_RIF_ROUTE_ENABLE_TYPE_MPLS);
      rif_info->enable_default_routing = SOC_SAND_NUM2BOOL(tbl_data.l_3_vpn_default_routing);
  }
  
  res = arad_pp_egq_ttl_scope_tbl_get_unsafe(unit, rif_id, &egq_ttl_scope_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  rif_info->ttl_scope_index = egq_ttl_scope_tbl.ttl_scope_index;

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
    uint8 urpf_mode;
     
    
    res = sw_state_access[unit].dpp.soc.arad.pp.rif_to_lif_group_map.rif_urpf_mode.get(unit, rif_id, &urpf_mode);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    rif_info->uc_rpf_mode = urpf_mode;
  }
#endif 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_info_get_imp_unsafe()", rif_id, 0);
}


uint32
  arad_pp_rif_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32 res = SOC_SAND_OK;
  uint32   reg32_val;
  uint32 inlif_profile_urpf_mask = 0;
  uint32 inlif_profile_index;
  uint32 in_lif_profile_strict_urpf_bitmap = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) { 
      
      res = READ_IHP_HIGH_VSI_CONFIGr(unit, &reg32_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      soc_reg_field_set(unit, IHP_HIGH_VSI_CONFIGr, &reg32_val, HIGH_VSI_IN_RIFf, 0);
      res = WRITE_IHP_HIGH_VSI_CONFIGr(unit, reg32_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }

#ifdef BCM_88660_A0
  
  if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
      
      res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set, (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_STRICT_URPF, 1,&inlif_profile_urpf_mask));
      SOC_SAND_CHECK_FUNC_RESULT(res, 99, exit);

      
      for (inlif_profile_index = 0; inlif_profile_index < SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_profile_index++) {
          if (inlif_profile_index & inlif_profile_urpf_mask) {
              in_lif_profile_strict_urpf_bitmap |= (1 << inlif_profile_index);
          }
      }

      res = WRITE_IHB_IN_LIF_UC_RPF_MODEL_IS_STRICTr(unit, in_lif_profile_strict_urpf_bitmap);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }
#endif 


    if(SOC_IS_JERICHO(unit)){        
        uint64 val64;    

        COMPILER_64_SET(val64, 0, 0);              
        res = WRITE_IHB_IN_RIF_UC_RPF_MODEL_IS_STRICTr(unit, SOC_CORE_ALL, val64);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    }    

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_init_unsafe()", 0, 0);
}


uint32
  arad_pp_rif_mpls_labels_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_RIF_MPLS_LABELS_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_labels_range);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_labels_range_set_unsafe()", 0, 0);
}

uint32
  arad_pp_rif_mpls_labels_range_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_RIF_MPLS_LABELS_RANGE_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_RIF_MPLS_LABELS_RANGE, rif_labels_range, 10, exit);

  
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_labels_range_set_verify()", 0, 0);
}

uint32
  arad_pp_rif_mpls_labels_range_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_RIF_MPLS_LABELS_RANGE_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_labels_range_get_verify()", 0, 0);
}


uint32
  arad_pp_rif_mpls_labels_range_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_RIF_MPLS_LABELS_RANGE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_labels_range);

  SOC_PPC_RIF_MPLS_LABELS_RANGE_clear(rif_labels_range);
  
  
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 20, exit);
  
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_labels_range_get_unsafe()", 0, 0);
}

uint32
arad_pp_rif_mpls_key_to_isem_key_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY          *mpls_key,
    SOC_SAND_OUT  ARAD_PP_ISEM_ACCESS_KEY            *isem_key)
{
    uint32                    res = SOC_SAND_OK;
    uint8 use_label_index = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(mpls_key);
    SOC_SAND_CHECK_NULL_INPUT(isem_key);

    ARAD_PP_CLEAR(isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);

    res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.mpls_info.mpls_termination_label_index_enable.get(unit,&use_label_index);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    
    if (mpls_key->flags & SOC_PPC_RIF_MPLS_LABEL_FRR) {
        
        if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode >= 20 && SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode <= 23) {
        	 isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L0;
        }
        else {
        	isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_FRR;
            if (mpls_key->flags & SOC_PPC_RIF_MPLS_LABEL_ELI) {
            
            SOC_SAND_SET_ERROR_CODE(
                    ARAD_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR, 20, exit);
             }
        }
    } else if (mpls_key->flags & SOC_PPC_RIF_MPLS_LABEL_TERM_BASED_PORT){
        isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_VLAN_DOMAIN_MPLS;
        isem_key->key_info.mpls.vlan_domain= mpls_key->vlan_domain;
    } else {
        if (use_label_index){
            switch (mpls_key->label_index) {
                case SOC_PPC_MPLS_LABEL_INDEX_FIRST:
                    isem_key->key_type =
                        (mpls_key->flags & SOC_PPC_RIF_MPLS_LABEL_ELI) ? \
                            ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_ELI : \
                                ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1;
                    break;
                case SOC_PPC_MPLS_LABEL_INDEX_SECOND:
                    isem_key->key_type =
                        (mpls_key->flags & SOC_PPC_RIF_MPLS_LABEL_ELI) ? \
                            ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2_ELI : \
                                ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2;
                    break;
                case SOC_PPC_MPLS_LABEL_INDEX_THIRD:
                    if ((mpls_key->flags & SOC_PPC_RIF_MPLS_LABEL_ELI)) {
                        
                        SOC_SAND_SET_ERROR_CODE(
                            ARAD_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR, 48, exit);
                    }
                    isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L3;
                    break;
                case SOC_PPC_MPLS_LABEL_INDEX_ZERO:
                    isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L0;
                    break;

                case SOC_PPC_MPLS_LABEL_INDEX_ALL:
                    
                    SOC_SAND_SET_ERROR_CODE(
                       ARAD_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR, 60, exit);
                    
                    break;
                default:
                    SOC_SAND_SET_ERROR_CODE(
                        ARAD_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR, 62, exit);
            }
        } else {
            isem_key->key_type =
                (mpls_key->flags & SOC_PPC_RIF_MPLS_LABEL_ELI) ? \
                        ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_ELI : ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS;
        }
    }


    isem_key->key_info.mpls.in_rif = 0;
    isem_key->key_info.mpls.label = mpls_key->label_id;
    isem_key->key_info.mpls.label2 = mpls_key->label_id_second;

    isem_key->key_info.mpls.is_bos =
        (mpls_key->flags & SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS) ? TRUE : FALSE;
    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
        isem_key->key_info.mpls.flags =
            (mpls_key->flags & SOC_PPC_RIF_MPLS_LABEL_EVPN_IML) ? SOC_PPC_MPLS_TERM_FLAG_EVPN_IML : 0;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(
        "error in arad_pp_rif_mpls_key_to_isem_key_unsafe()", 0, 0);
}


uint32
  arad_pp_rif_mpls_label_map_add_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY *mpls_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID             lif_index,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_INFO     *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO           *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success
  )
{
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_sw_db_lif_table_entry_use_set(unit, lif_index, SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF,0);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_rif_mpls_label_map_add_internal_unsafe(
          unit,
          mpls_key,
          (mpls_key->flags & SOC_PPC_RIF_MPLS_LABEL_LOOKUP_NONE),
          lif_index,
          term_info,
          rif_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_label_map_add_unsafe()", 0, 0);
}

uint32
  arad_pp_rif_mpls_label_map_add_internal_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY *mpls_key,
    SOC_SAND_IN  uint8                  ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID             lif_index,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_INFO     *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO           *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success
  )
{
  uint32                                             res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY                              isem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY                            isem_entry;
  ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_DATA sem_res_tbl_data;
  SOC_PPC_RIF_ID rif_id;
  uint32         reg32_val, protection_pointer = 0, disable_lsp_destination;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_ADD_INTERNAL_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(mpls_key);
    ARAD_PP_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
    ARAD_PP_CLEAR(&isem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);

    
    res = arad_pp_rif_mpls_key_to_isem_key_unsafe(unit, mpls_key, &isem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_lif_index_to_sem_result(unit, lif_index, term_info->ext_lif_id,
                                          &isem_entry.sem_result_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
    res = arad_pp_isem_access_entry_add_unsafe(unit, &isem_key, &isem_entry, success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if(*success != SOC_SAND_SUCCESS)
    {
      goto exit;
    }
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

  res = arad_pp_ihp_lif_table_label_protocol_or_lsp_tbl_get_unsafe(unit, lif_index, &sem_res_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_reg_above_64_field32_read(unit, IHP_VTT_GENERAL_CONFIGS_0r, SOC_CORE_ALL, 0, DISABLE_LSP_DESTINATIONf, &disable_lsp_destination);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

  sem_res_tbl_data.destination_valid = FALSE;
  sem_res_tbl_data.destination = 0;
  sem_res_tbl_data.service_type = ARAD_PP_RIF_ISEM_RES_SERVICE_TYPE;

  if (term_info->default_frwrd.default_frwd_type == SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF) {
      uint32 asd_buf;
      if (disable_lsp_destination) {
          SOC_SAND_SET_ERROR_CODE(ARAD_RIF_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR, 32, exit);
      }
      sem_res_tbl_data.destination_valid = TRUE;
      res = arad_pp_fwd_decision_in_buffer_build(unit,
                            ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
                            &term_info->default_frwrd.default_forwarding,
                            &sem_res_tbl_data.destination,
                            &asd_buf);
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

      if (term_info->service_type == SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P) {
          sem_res_tbl_data.service_type = ARAD_PP_RIF_ISEM_RES_SERVICE_TYPE_P2P;
      }
  } else if (disable_lsp_destination) {
      sem_res_tbl_data.destination_valid = (term_info->term_profile & ARAD_PP_RIF_MPLS_TERM_PROFILE_MSB_MASK) >> ARAD_PP_RIF_MPLS_TERM_PROFILE_LSB_NOF_BITS;
  }
  sem_res_tbl_data.orientation_is_hub = term_info->orientation;
  sem_res_tbl_data.da_not_found_profile = term_info->default_forward_profile;
  sem_res_tbl_data.in_lif_profile = term_info->lif_profile;
  sem_res_tbl_data.oam_lif_set = SOC_SAND_NUM2BOOL(term_info->oam_valid);
  sem_res_tbl_data.protection_path = SOC_SAND_NUM2BOOL(term_info->protection_pass_value);
  
  protection_pointer = (term_info->protection_pointer == 0 && SOC_IS_JERICHO(unit)) ? -1 : term_info->protection_pointer;
  
  arad_pp_l2_lif_convert_protection_fields_to_hw(unit, protection_pointer, sem_res_tbl_data.protection_path,
                                                 &sem_res_tbl_data.protection_pointer, &sem_res_tbl_data.protection_path);
  sem_res_tbl_data.tpid_profile = term_info->tpid_profile;
  sem_res_tbl_data.tt_learn_enable = SOC_SAND_NUM2BOOL(term_info->learn_enable);
  sem_res_tbl_data.vsi = term_info->vsi;
  sem_res_tbl_data.vsi_assignment_mode = term_info->vsi_assignment_mode;
  sem_res_tbl_data.action_profile_index = term_info->trap_code_index;
  sem_res_tbl_data.termination_profile = term_info->term_profile & ARAD_PP_RIF_MPLS_TERM_PROFILE_LSB_MASK;


  sem_res_tbl_data.cos_profile = term_info->cos_profile;
  if(term_info->rif != SOC_PPC_RIF_NULL)
  {
    sem_res_tbl_data.in_rif = term_info->rif;
    sem_res_tbl_data.in_rif_valid = 1;
  }
  else
  {
    sem_res_tbl_data.in_rif_valid = 0;
  }

  sem_res_tbl_data.type =
    ARAD_PP_RIF_ISEM_RES_MPLS_TYPE_PROTOCOL_OR_LSP;

  sem_res_tbl_data.in_lif_valid = 1;
  if (SOC_IS_JERICHO(unit)) {
    sem_res_tbl_data.sys_in_lif = term_info->global_lif;
  }
  sem_res_tbl_data.model_is_pipe = (term_info->processing_type == SOC_PPC_MPLS_TERM_MODEL_PIPE) ? 1 : 0;
  sem_res_tbl_data.forwarding_code = term_info->forwarding_code;
  sem_res_tbl_data.expect_bos = (term_info->forwarding_code != SOC_TMC_PKT_FRWRD_TYPE_MPLS) ? 1:0;

#ifdef BCM_88660_A0

  
  if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) {
    sem_res_tbl_data.expect_bos = (mpls_key->flags & SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS) ? 1 : 0;
  }

  if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
    uint8 urpf_mode;

    if (sem_res_tbl_data.vsi < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers)) {
      if ( sem_res_tbl_data.vsi == 0 && sem_res_tbl_data.in_rif_valid ) {
          rif_id = term_info->rif;
      }
      else {
          rif_id = sem_res_tbl_data.vsi;
      }
    } else { 
      res =READ_IHP_HIGH_VSI_CONFIGr(unit, &reg32_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      rif_id = soc_reg_field_get(unit, IHP_HIGH_VSI_CONFIGr, reg32_val, HIGH_VSI_IN_RIFf);
    }
    
    res = arad_pp_sw_db_rif_to_lif_group_map_add_lif_to_rif(
       unit,
       rif_id,
       lif_index
       );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    
    res = sw_state_access[unit].dpp.soc.arad.pp.rif_to_lif_group_map.rif_urpf_mode.get(unit, rif_id, &urpf_mode);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

    res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,(unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_STRICT_URPF, 
	                                                                       ((urpf_mode == SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT) ? 1 : 0), &sem_res_tbl_data.in_lif_profile));
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  }

#endif 

  ARAD_DEVICE_CHECK(unit, exit);
  res = arad_pp_ihp_lif_table_label_protocol_or_lsp_tbl_set_unsafe(unit, lif_index, &sem_res_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  
  if(term_info->rif != SOC_PPC_RIF_NULL && rif_info != NULL)
  {
    res = arad_pp_rif_info_set_imp_unsafe(unit, term_info->rif, rif_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

  
  if (SOC_IS_JERICHO(unit) && term_info->is_extended)
  {
     res = arad_pp_lif_additional_data_set(unit,lif_index,TRUE,term_info->additional_data,FALSE);
     SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_label_map_add_internal_unsafe()", 0, 0);
}

uint32
  arad_pp_rif_mpls_label_map_add_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY *mpls_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID             lif_index,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_INFO     *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO           *rif_info
  )
{
  uint32                    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_ADD_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MPLS_LABEL_RIF_KEY, mpls_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(lif_index, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 20, exit);
  res = SOC_PPC_MPLS_TERM_INFO_verify(unit, term_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  res = SOC_PPC_RIF_INFO_verify(unit, rif_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (mpls_key->vsid != 0)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_label_map_add_verify()", 0, 0);
}


uint32
  arad_pp_rif_mpls_label_map_remove_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN    SOC_PPC_MPLS_LABEL_RIF_KEY *mpls_key,
    SOC_SAND_INOUT SOC_PPC_LIF_ID             *lif_index
  )
{
  uint32                  res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY   isem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY isem_entry;
  uint8                     success;
  uint8                     ignore_key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  ignore_key = mpls_key->flags & SOC_PPC_RIF_MPLS_LABEL_LOOKUP_NONE;

  
  if (!ignore_key)
  {
      res = arad_pp_rif_mpls_key_to_isem_key_unsafe(unit, mpls_key, &isem_key);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      
      res = arad_pp_isem_access_entry_get_unsafe(unit, &isem_key, &isem_entry, &success);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  } 
  else 
  {
      success = TRUE;
  }
  

  if(success == TRUE)
  {

    
    if (!ignore_key) 
    {
      *lif_index =arad_pp_sem_result_to_lif_index(unit, isem_entry.sem_result_ndx); 
      res = arad_pp_isem_access_entry_remove_unsafe(unit, &isem_key);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
      res = arad_pp_sw_db_rif_to_lif_group_map_remove_lif_from_rif(unit, *lif_index);
      SOC_SAND_CHECK_FUNC_RESULT(res, 171, exit);
    }
#endif 

    
    res = arad_pp_sw_db_lif_table_entry_use_set(unit, *lif_index, SOC_PPC_LIF_ENTRY_TYPE_EMPTY,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  else
  {
    *lif_index = ARAD_PP_LIF_NULL;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_label_map_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_rif_mpls_label_map_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_REMOVE_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MPLS_LABEL_RIF_KEY, mpls_key, 10, exit);
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_label_map_remove_verify()", 0, 0);
}


uint32
  arad_pp_rif_mpls_label_map_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY *mpls_key,
    SOC_SAND_OUT  SOC_PPC_LIF_ID            *lif_index,
    SOC_SAND_OUT  SOC_PPC_MPLS_TERM_INFO    *term_info,
    SOC_SAND_OUT  SOC_PPC_RIF_INFO          *rif_info,
    SOC_SAND_OUT uint8                  *found
  )
{
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  
  res = arad_pp_rif_mpls_label_map_get_internal_unsafe(
          unit,
          mpls_key,
          FALSE,
          lif_index,
          term_info,
          rif_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_label_map_get_unsafe()", 0, 0);
}


uint32
  arad_pp_rif_mpls_label_map_get_internal_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY *mpls_key,
    SOC_SAND_IN  uint8                  ignore_key,
    SOC_SAND_OUT  SOC_PPC_LIF_ID            *lif_index,
    SOC_SAND_OUT  SOC_PPC_MPLS_TERM_INFO    *term_info,
    SOC_SAND_OUT  SOC_PPC_RIF_INFO          *rif_info,
    SOC_SAND_OUT uint8                  *found
  )
{
  uint32                                 res = SOC_SAND_OK, protection_pointer = 0;
  ARAD_PP_ISEM_ACCESS_KEY                  isem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY                isem_entry;
  ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_DATA sem_res_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_GET_INTERNAL_UNSAFE);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  }
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPC_MPLS_TERM_INFO_clear(term_info);
  SOC_PPC_RIF_INFO_clear(rif_info);

  if (!ignore_key)
  {
    ARAD_PP_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
    ARAD_PP_CLEAR(&isem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
    
    
    res = arad_pp_rif_mpls_key_to_isem_key_unsafe(unit, mpls_key, &isem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_isem_access_entry_get_unsafe(unit, &isem_key, &isem_entry, found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    *found = TRUE;
    isem_entry.sem_result_ndx = *lif_index;
  }

  if(!*found)
  {
    goto exit;
  }

  *lif_index = arad_pp_sem_result_to_lif_index(unit,isem_entry.sem_result_ndx);
  res = arad_pp_ihp_lif_table_label_protocol_or_lsp_tbl_get_unsafe(unit, *lif_index, &sem_res_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      
      if (sem_res_tbl_data.in_lif_valid == 0)
      {
        *found = FALSE;
        goto exit;
      }
  }

  term_info->cos_profile = sem_res_tbl_data.cos_profile;

  if(sem_res_tbl_data.in_rif_valid)
  {
    term_info->rif = sem_res_tbl_data.in_rif;
  }
  else
  {
    term_info->rif = SOC_PPC_RIF_NULL;
  }

  term_info->forwarding_code = sem_res_tbl_data.forwarding_code;

  term_info->processing_type = sem_res_tbl_data.model_is_pipe ? SOC_PPC_MPLS_TERM_MODEL_PIPE : SOC_PPC_MPLS_TERM_MODEL_UNIFORM;
  term_info->default_forward_profile = sem_res_tbl_data.da_not_found_profile;
  term_info->lif_profile = sem_res_tbl_data.in_lif_profile;
  term_info->oam_valid = sem_res_tbl_data.oam_lif_set;
  
  arad_pp_l2_lif_convert_protection_fields_from_hw(unit, sem_res_tbl_data.protection_pointer, sem_res_tbl_data.protection_path,
                                                   &(protection_pointer), &(term_info->protection_pass_value));
  
  term_info->protection_pointer = (protection_pointer == ARAD_PP_LIF_PROTECTION_POINTER_DEFAULT_VALUE && SOC_IS_JERICHO(unit)) ? 0 : protection_pointer;
  term_info->tpid_profile = sem_res_tbl_data.tpid_profile;
  term_info->learn_enable = sem_res_tbl_data.tt_learn_enable;
  term_info->vsi = sem_res_tbl_data.vsi;
  term_info->vsi_assignment_mode = sem_res_tbl_data.vsi_assignment_mode;
  term_info->trap_code_index = sem_res_tbl_data.action_profile_index;
  term_info->term_profile = sem_res_tbl_data.termination_profile;
  term_info->global_lif = sem_res_tbl_data.sys_in_lif;
  term_info->orientation = sem_res_tbl_data.orientation_is_hub;

  if (sem_res_tbl_data.destination_valid) {
        uint32   flags, asd_buf;
        term_info->default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;
        term_info->service_type= SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P;

        flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
        asd_buf = 0;
        res = arad_pp_fwd_decision_in_buffer_parse(unit,
                            sem_res_tbl_data.destination,
                            asd_buf,
                            flags,
                            &(term_info->default_frwrd.default_forwarding));
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  
  if(term_info->rif != SOC_PPC_RIF_NULL)
  {
    res = arad_pp_rif_info_get_imp_unsafe(unit, term_info->rif, rif_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_label_map_get_internal_unsafe()", 0, 0);
}

uint32
  arad_pp_rif_mpls_label_map_get_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY *mpls_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MPLS_LABEL_RIF_KEY, mpls_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_label_map_get_verify()", 0, 0);
}



uint32
  arad_pp_rif_ip_tunnel_term_add_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY  *term_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID           lif_index,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_INFO *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO         *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  )
{
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_rif_ip_tunnel_term_add_internal_unsafe(
          unit,
          term_key,
          FALSE,
          lif_index,
          term_info,
          rif_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 
  if (!(term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY)
       && (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2))
  {
    res = arad_pp_sw_db_lif_table_entry_use_set(
            unit,
            lif_index,
            SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF,
            0
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_add_unsafe()", 0, 0);
}


void
  arad_pp_rif_ip_tunnel_term_lookups_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY     *term_key,
    SOC_SAND_OUT  uint8                      *dip_only,
    SOC_SAND_OUT  uint8                      *sip_only,
    SOC_SAND_OUT  uint8                      *dip_sip_tcam,
    SOC_SAND_OUT  uint8                      *dip_sip_tcam_next_protocol,
    SOC_SAND_OUT  uint8                      *dip_sip_vrf_tcam, 
    SOC_SAND_OUT  uint8                      *my_vtep_index_sip_vrf_sem
  )
{
    *dip_only = ((SOC_DPP_CONFIG(unit)->pp.ingress_ipv4_tunnel_term_mode == SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_SIP_DIP_SEPERATED) ||
        ( SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DEFAULT) ||
        (SOC_IS_DIP_SIP_VRF_WITH_MY_VTEP_INDEX_ENABLE(unit) && (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY)));
    *sip_only = (SOC_DPP_CONFIG(unit)->pp.ingress_ipv4_tunnel_term_mode == SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_SIP_DIP_SEPERATED);
    *dip_sip_tcam = ((SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP)
                                             || (SOC_DPP_CONFIG(unit)->pp.ingress_ipv4_tunnel_term_mode == SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_SIP_DIP_JOIN));
    *dip_sip_tcam_next_protocol = ((SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_NEXT_PROTOCOL));
    *dip_sip_vrf_tcam = ((SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF) || 
                         (SOC_IS_DIP_SIP_VRF_WITH_MY_VTEP_INDEX_ENABLE(unit) 
                             && !(term_key->flags & (SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY | SOC_PPC_RIF_IP_TERM_FLAG_MY_VTEP_INDEX_IS_VALID)))); 
    *my_vtep_index_sip_vrf_sem =  (SOC_IS_DIP_SIP_VRF_WITH_MY_VTEP_INDEX_ENABLE(unit) 
                                   && (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_MY_VTEP_INDEX_IS_VALID));
}


uint32
  arad_pp_rif_ip_tunnel_term_key_to_sem_key(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY     *term_key,
    SOC_SAND_OUT  ARAD_PP_ISEM_ACCESS_KEY    *isem_key
  )
{
    uint8   dip_exact_enabled;
    uint8   sip_exact_enabled;
    uint8   dip_sip_tcam_enabled;
    uint8   dip_sip_tcam_next_protocol_enabled;
    uint8   dip_sip_vrf_tcam_enabled; 
    uint8   my_vtep_sip_vrf_enabled; 
    uint8   exact_match = 1;
    uint8   exact_blocked = 0;
    uint8   nof_fields = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_INTERNAL_UNSAFE);


    
    if((term_key->flags &  SOC_PPC_RIF_IP_TERM_IPMC_BIDIR)){
        isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_RPA;
        isem_key->key_info.rpa.dip = term_key->dip;
        isem_key->key_info.rpa.vrf = term_key->vrf;
        goto exit;
    }



    
    if((term_key->flags &  SOC_PPC_RIF_IP_TERM_FLAG_IPV6)){
        if (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY) {
            if (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_BUD) {
                isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP_AFTER_RECYCLE;
            } else {
                isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP;
            }
        } else if (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_MY_VTEP_INDEX_IS_VALID) {            
            isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_SIP;
        } else {
            if (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_BUD) {
                isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP_AFTER_RECYCLE;
            } else {
                isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP;
            }
        }
        
        switch(isem_key->key_type) {
        case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP:
        case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP_AFTER_RECYCLE:
            sal_memcpy(&isem_key->key_info.ip_tunnel.dip6,&term_key->dip6, sizeof(SOC_SAND_PP_IPV6_SUBNET));
            if (term_key->ipv4_next_protocol_prefix_len != 0) {
                isem_key->key_info.ip_tunnel.ipv4_next_protocol = term_key->ipv4_next_protocol;
                isem_key->key_info.ip_tunnel.ipv4_next_protocol_prefix_len = term_key->ipv4_next_protocol_prefix_len;  
            }
            if (term_key->qualifier_next_protocol_prefix_len != 0) {
                isem_key->key_info.ip_tunnel.qualifier_next_protocol = term_key->qualifier_next_protocol; 
                isem_key->key_info.ip_tunnel.qualifier_next_protocol_prefix_len = term_key->qualifier_next_protocol_prefix_len; 
            }
            break;
        case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_SIP:
            sal_memcpy(&isem_key->key_info.ip_tunnel.sip6,&term_key->sip6, sizeof(SOC_SAND_PP_IPV6_SUBNET));
            isem_key->key_info.ip_tunnel.vrf_prefix_len = 0;
            if (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_VRF_IS_VALID) {
                isem_key->key_info.ip_tunnel.vrf = term_key->vrf;
                isem_key->key_info.ip_tunnel.vrf_prefix_len = SOC_DPP_DEFS_GET(unit, vrf_nof_bits);
            }
            isem_key->key_info.ip_tunnel.my_vtep_index = term_key->my_vtep_index; 
            isem_key->key_info.ip_tunnel.port_property = term_key->port_property;
            isem_key->key_info.ip_tunnel.port_property_en = term_key->port_property_en;
            break;
        case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP:
        case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP_AFTER_RECYCLE:
            sal_memcpy(&isem_key->key_info.ip_tunnel.dip6,&term_key->dip6, sizeof(SOC_SAND_PP_IPV6_SUBNET));
            isem_key->key_info.ip_tunnel.vrf_prefix_len = 0;
            if (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_VRF_IS_VALID) {
                isem_key->key_info.ip_tunnel.vrf = term_key->vrf;
                isem_key->key_info.ip_tunnel.vrf_prefix_len = (SOC_DPP_CONFIG(unit))->pp.ip6_tunnel_term_in_tcam_vrf_nof_bits;
            }
            if (term_key->ipv4_next_protocol_prefix_len != 0) {
                isem_key->key_info.ip_tunnel.ipv4_next_protocol = term_key->ipv4_next_protocol;
                isem_key->key_info.ip_tunnel.ipv4_next_protocol_prefix_len = term_key->ipv4_next_protocol_prefix_len;  
            }
            isem_key->key_info.ip_tunnel.ip_gre_tunnel = term_key->ip_gre_tunnel; 
            if (term_key->qualifier_next_protocol_prefix_len != 0) {                
                isem_key->key_info.ip_tunnel.qualifier_next_protocol = term_key->qualifier_next_protocol; 
                isem_key->key_info.ip_tunnel.qualifier_next_protocol_prefix_len = term_key->qualifier_next_protocol_prefix_len; 
            } 
            isem_key->key_info.ip_tunnel.port_property = term_key->port_property;
            isem_key->key_info.ip_tunnel.port_property_en = term_key->port_property_en;
            break;
         default:
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_RIF_IP_TUNNEL_TERM_KEY_ILLEGAL_ERR, 510, exit);
            break;
        } 
        goto exit; 
    }
    
    if(term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_DUMMY) {
        isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_DIP;
        isem_key->key_info.ip_tunnel.flags = term_key->flags;
        goto exit;
    }

    
    if(term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_EXPLICIT_NULL_DUMMY) {
        isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_DIP;
        isem_key->key_info.ip_tunnel.flags = term_key->flags;
        isem_key->key_info.mpls.ttl = term_key->ttl;

        goto exit;
    }


    arad_pp_rif_ip_tunnel_term_lookups_get(unit,term_key, &dip_exact_enabled,&sip_exact_enabled,&dip_sip_tcam_enabled,&dip_sip_tcam_next_protocol_enabled,&dip_sip_vrf_tcam_enabled,&my_vtep_sip_vrf_enabled);

    
    if (term_key->dip_prefix_len == 32) {
        ++nof_fields;
        if (!dip_exact_enabled) {
            exact_blocked = 1;
        }
    }
    
    else if (term_key->dip_prefix_len != 0) {
        ++nof_fields;
        exact_match = 0;
    }
    
    if (term_key->sip_prefix_len == 32) {
        ++nof_fields;
        if (!sip_exact_enabled) {
            exact_blocked = 1;
        }
    }
    
    else if (term_key->sip_prefix_len != 0) {
        ++nof_fields;
        exact_match = 0;
    }
    else if (term_key->ipv4_next_protocol_prefix_len != 0) {
        ++nof_fields;
        exact_match = 0;
    }
    
    if (my_vtep_sip_vrf_enabled) {
        isem_key->key_info.ip_tunnel.sip = term_key->sip; 
        isem_key->key_info.ip_tunnel.vrf = term_key->vrf;
        
        isem_key->key_info.ip_tunnel.vrf_prefix_len = SOC_DPP_DEFS_GET(unit, vrf_nof_bits); 
        isem_key->key_info.ip_tunnel.my_vtep_index = term_key->my_vtep_index; 
        isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_MY_VTEP_INDEX_SIP_VRF; 
    }
    
    else if(nof_fields != 1 || !exact_match || exact_blocked) {
        if (!dip_sip_tcam_enabled && !dip_sip_tcam_next_protocol_enabled && !dip_sip_vrf_tcam_enabled) { 
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_RIF_IP_TUNNEL_TERM_KEY_ILLEGAL_ERR, 20, exit);
        }  
              
        if (dip_sip_vrf_tcam_enabled) {
            isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP_DIP_VRF;
        } else {
            isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP_DIP;
        }

        isem_key->key_info.ip_tunnel.dip = term_key->dip;
        isem_key->key_info.ip_tunnel.dip_prefix_len = term_key->dip_prefix_len;
        isem_key->key_info.ip_tunnel.sip = term_key->sip;
        isem_key->key_info.ip_tunnel.sip_prefix_len = term_key->sip_prefix_len;
        isem_key->key_info.ip_tunnel.ipv4_next_protocol = term_key->ipv4_next_protocol;
        isem_key->key_info.ip_tunnel.ipv4_next_protocol_prefix_len = term_key->ipv4_next_protocol_prefix_len;        
        isem_key->key_info.ip_tunnel.port_property = term_key->port_property;
        isem_key->key_info.ip_tunnel.port_property_en = term_key->port_property_en;

        
        if (dip_sip_vrf_tcam_enabled) {
            
            if (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_VRF_IS_VALID) {
                isem_key->key_info.ip_tunnel.vrf = term_key->vrf;
                
                isem_key->key_info.ip_tunnel.vrf_prefix_len = SOC_DPP_DEFS_GET(unit, vrf_nof_bits); 
            }
            
            isem_key->key_info.ip_tunnel.gre_ethertype = term_key->gre_ethertype; 
            isem_key->key_info.ip_tunnel.gre_ethertype_len = term_key->gre_ethertype_prefix_len; 

            
            if (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_IP_GRE_TUNNEL_IS_VALID) {
                isem_key->key_info.ip_tunnel.ip_gre_tunnel = term_key->ip_gre_tunnel;
                isem_key->key_info.ip_tunnel.ip_gre_tunnel_en = 1; 

            }

        }
    }

    
    
    else if(term_key->dip_prefix_len == 32) {
        isem_key->key_info.ip_tunnel.dip = term_key->dip;
        isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_DIP;
    }
    
    else if(term_key->sip_prefix_len == 32) {
        isem_key->key_info.ip_tunnel.sip = term_key->sip;
        isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP;
    }
    else{
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_RIF_IP_TUNNEL_TERM_KEY_ILLEGAL_ERR, 520, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_key_to_sem_key()", 0, 0);
}

uint32
  arad_pp_rif_ip_tunnel_term_key_from_sem_key(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PPC_RIF_IP_TERM_KEY    *term_key,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY    *isem_key
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_INTERNAL_UNSAFE);

    SOC_PPC_RIF_IP_TERM_KEY_clear(term_key);

    
    if((isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP) ||
        (isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP_AFTER_RECYCLE)){
        sal_memcpy(&term_key->dip6,&isem_key->key_info.ip_tunnel.dip6, sizeof(SOC_SAND_PP_IPV6_SUBNET));
        term_key->vrf = isem_key->key_info.ip_tunnel.vrf;        
        term_key->ipv4_next_protocol_prefix_len = isem_key->key_info.ip_tunnel.ipv4_next_protocol_prefix_len;
        term_key->ipv4_next_protocol = isem_key->key_info.ip_tunnel.ipv4_next_protocol;
        term_key->qualifier_next_protocol_prefix_len = isem_key->key_info.ip_tunnel.qualifier_next_protocol_prefix_len;
        term_key->qualifier_next_protocol = isem_key->key_info.ip_tunnel.qualifier_next_protocol;
        term_key->port_property_en = isem_key->key_info.ip_tunnel.port_property_en;
        term_key->port_property = isem_key->key_info.ip_tunnel.port_property;
        term_key->flags = SOC_PPC_RIF_IP_TERM_FLAG_IPV6;
        if (isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP_AFTER_RECYCLE) {
            term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_BUD;
        }
    }
    
    else if((isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP) ||
        (isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP_AFTER_RECYCLE)){
        sal_memcpy(&term_key->dip6,&isem_key->key_info.ip_tunnel.dip6, sizeof(SOC_SAND_PP_IPV6_SUBNET));
        term_key->ipv4_next_protocol_prefix_len = isem_key->key_info.ip_tunnel.ipv4_next_protocol_prefix_len;
        term_key->ipv4_next_protocol = isem_key->key_info.ip_tunnel.ipv4_next_protocol;
        term_key->qualifier_next_protocol_prefix_len = isem_key->key_info.ip_tunnel.qualifier_next_protocol_prefix_len;
        term_key->qualifier_next_protocol = isem_key->key_info.ip_tunnel.qualifier_next_protocol;
        term_key->flags = SOC_PPC_RIF_IP_TERM_FLAG_IPV6; 
        if (isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP_AFTER_RECYCLE) {
            term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_BUD;
        }
    }
    
    else if(isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_SIP) {
        sal_memcpy(&term_key->sip6,&isem_key->key_info.ip_tunnel.sip6, sizeof(SOC_SAND_PP_IPV6_SUBNET));
        term_key->vrf = isem_key->key_info.ip_tunnel.vrf;    
        term_key->my_vtep_index = isem_key->key_info.ip_tunnel.my_vtep_index;
        term_key->port_property_en = isem_key->key_info.ip_tunnel.port_property_en;
        term_key->port_property = isem_key->key_info.ip_tunnel.port_property;        
        term_key->flags = SOC_PPC_RIF_IP_TERM_FLAG_IPV6; 
    }
    
    else if(isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP_DIP) {
        term_key->dip = isem_key->key_info.ip_tunnel.dip;
        term_key->dip_prefix_len = isem_key->key_info.ip_tunnel.dip_prefix_len;
        term_key->sip = isem_key->key_info.ip_tunnel.sip;
        term_key->sip_prefix_len = isem_key->key_info.ip_tunnel.sip_prefix_len;
        term_key->ipv4_next_protocol_prefix_len = isem_key->key_info.ip_tunnel.ipv4_next_protocol_prefix_len;
        term_key->ipv4_next_protocol = isem_key->key_info.ip_tunnel.ipv4_next_protocol;
        term_key->port_property_en = isem_key->key_info.ip_tunnel.port_property_en;
        term_key->port_property = isem_key->key_info.ip_tunnel.port_property;
    }
    
    else if(isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_DIP) {
        term_key->dip = isem_key->key_info.ip_tunnel.dip;
        term_key->dip_prefix_len = 32;
    }
    
    else if(isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP) {
        term_key->sip = isem_key->key_info.ip_tunnel.sip;
        term_key->sip_prefix_len = 32;
    }
    else{
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_RIF_IP_TUNNEL_TERM_KEY_ILLEGAL_ERR, 20, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_key_from_sem_key()", 0, 0);
}


#define ARAD_PP_RIF_SERVICE_TYPE_IP_TUNNEL (6)
#define ARAD_PP_RIF_SERVICE_TYPE_MPLS_EXP_TTL_REJECT (4)

uint32
  arad_pp_rif_ip_tunnel_term_add_internal_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY     *term_key,
    SOC_SAND_IN  uint8                ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID           lif_index,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_INFO *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO         *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  )
{
  uint32                               res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY              isem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY            isem_entry;
  ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_DATA sem_res_tbl_data;
  uint32                               additional_info = 0;
  SOC_PPC_RIF_ID rif_id;
  uint32         reg32_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_INTERNAL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  ARAD_PP_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
  ARAD_PP_CLEAR(&isem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
  
  if (!ignore_key)
  {
    

    
    res = arad_pp_rif_ip_tunnel_term_key_to_sem_key(unit,term_key,&isem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_lif_index_to_sem_result(unit, lif_index, term_info->ext_lif_id, &isem_entry.sem_result_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    
    res = arad_pp_isem_access_entry_add_unsafe(unit, &isem_key, &isem_entry, success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(*success != SOC_SAND_SUCCESS)
    {
      goto exit;
    }
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

  
  if (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY) {
      goto exit;
  }

  res = arad_pp_ihp_lif_table_ip_tt_tbl_get_unsafe(unit, lif_index, &sem_res_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  sem_res_tbl_data.model_is_pipe = (term_info->processing_type == SOC_PPC_MPLS_TERM_MODEL_PIPE)?1:0;

  if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
      sem_res_tbl_data.orientation_is_hub = term_info->orientation; 
  } else {
      sem_res_tbl_data.orientation_is_hub = (term_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB) ? 1 : 0;
  }
  sem_res_tbl_data.orientation_is_hub_valid = 0x1; 
  sem_res_tbl_data.da_not_found_profile = term_info->default_forward_profile;
  sem_res_tbl_data.in_lif_profile = term_info->lif_profile;
  
  

  
  if(term_info->learn_enable && 
       ((term_info->learn_record.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW) || 
       (term_info->learn_record.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC))
    ){
      res = arad_pp_fwd_decision_to_learn_buffer(
                  unit,
                  term_info->learn_record.type,
                  term_info->learn_record.dest_id,
                  SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF, 
                  &sem_res_tbl_data.learn_info,
                  &additional_info
                  );
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
  }
  else{
      sem_res_tbl_data.learn_info = 0;
  }
  sem_res_tbl_data.tt_learn_enable = SOC_SAND_NUM2BOOL(term_info->learn_enable);
  sem_res_tbl_data.oam_lif_set = SOC_SAND_NUM2BOOL(term_info->oam_valid);
  sem_res_tbl_data.protection_path = SOC_SAND_NUM2BOOL(term_info->protection_pass_value);
  sem_res_tbl_data.protection_pointer = term_info->protection_pointer;
  
  if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.explicit_null_support && 
       (term_info->global_lif != 0) &&
       ((term_info->global_lif == soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "logical_interface_for_explicit_null_ttl0", 0)) || 
        (term_info->global_lif == soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "logical_interface_for_explicit_null_ttl1", 0)))) {
      sem_res_tbl_data.service_type = ARAD_PP_RIF_SERVICE_TYPE_MPLS_EXP_TTL_REJECT; 
  } else {
      sem_res_tbl_data.service_type = ARAD_PP_RIF_SERVICE_TYPE_IP_TUNNEL; 
  } 
  sem_res_tbl_data.tpid_profile = term_info->tpid_profile;
  sem_res_tbl_data.vsi = term_info->vsi & 0x7fff;
  sem_res_tbl_data.vsi_assignment_mode = term_info->vsi_assignment_mode;
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    
    sem_res_tbl_data.use_in_lif = 1;
    if (SOC_IS_JERICHO(unit)) {
        sem_res_tbl_data.sys_in_lif = term_info->global_lif;
    }
  }
  if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {

    uint8 urpf_mode;

    if (sem_res_tbl_data.vsi < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers)) { 
      rif_id = sem_res_tbl_data.vsi;
    } else { 
      res =READ_IHP_HIGH_VSI_CONFIGr(unit, &reg32_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      rif_id = soc_reg_field_get(unit, IHP_HIGH_VSI_CONFIGr, reg32_val, HIGH_VSI_IN_RIFf);
    }
    
    res = arad_pp_sw_db_rif_to_lif_group_map_add_lif_to_rif(
       unit,
       rif_id,
       lif_index
       );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    
    res = sw_state_access[unit].dpp.soc.arad.pp.rif_to_lif_group_map.rif_urpf_mode.get(unit, rif_id, &urpf_mode);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

    res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,(unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_STRICT_URPF, 
	                                                                       ((urpf_mode == SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT) ? 1 : 0), &sem_res_tbl_data.in_lif_profile));
    SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  }

  ARAD_DEVICE_CHECK(unit, exit);
  if (SOC_IS_ARADPLUS(unit)) {
     
     sem_res_tbl_data.skip_ethernet = term_info->skip_ethernet; 
  }
  

#endif 

  sem_res_tbl_data.cos_profile = term_info->cos_profile;
  if(term_info->rif != SOC_PPC_RIF_NULL)
  {
    sem_res_tbl_data.in_rif = term_info->rif;
    sem_res_tbl_data.in_rif_valid = 1;
  }
  else
  {
    sem_res_tbl_data.in_rif_valid = 0;
  }
  
  res = arad_pp_ihp_lif_table_ip_tt_tbl_set_unsafe(
          unit,
          lif_index,
          &sem_res_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  
  if(term_info->rif != SOC_PPC_RIF_NULL && rif_info != NULL)
  {
    res = arad_pp_rif_info_set_imp_unsafe(unit, term_info->rif, rif_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  
  if (SOC_IS_JERICHO(unit) && term_info->is_extended)
  {
        res = arad_pp_lif_additional_data_set(unit,lif_index,TRUE,term_info->additional_data,FALSE);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_add_internal_unsafe()", 0, 0);
}

uint32
  arad_pp_rif_ip_tunnel_term_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                    flags,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY                *term_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_VERIFY);

  
  if (flags & SOC_PPC_RIF_IP_TUNNEL_TERM_ADD_LIF_ID_IS_FEC) {
      
      SOC_SAND_ERR_IF_ABOVE_NOF(lif_index, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  } 
  else if (flags & SOC_PPC_RIF_IP_TUNNEL_TERM_ADD_LIF_ID_IS_MY_VTEP_INDEX) {
      
      if (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_IPV6) {
          SOC_SAND_ERR_IF_ABOVE_NOF(soc_sand_msb_bit_on(lif_index), 6, SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 11, exit);
      } else {
          SOC_SAND_ERR_IF_ABOVE_NOF(soc_sand_msb_bit_on(lif_index), (SOC_DPP_CONFIG(unit))->pp.vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits, SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 12, exit);
      }      
  }
  
  else {
      SOC_SAND_ERR_IF_ABOVE_NOF(lif_index, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 20, exit);
  }

  res = SOC_PPC_RIF_IP_TERM_INFO_verify(unit, term_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if(rif_info) {
      res = SOC_PPC_RIF_INFO_verify(unit, rif_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_add_verify()", 0, 0);
}


uint32
  arad_pp_rif_ip_tunnel_term_remove_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY        *term_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                 *lif_index
  )
{
  uint32                  res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY   isem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY isem_entry;
  uint8                 success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  ARAD_PP_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
  
  
  
  res = arad_pp_rif_ip_tunnel_term_key_to_sem_key(unit,term_key,&isem_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_isem_access_entry_get_unsafe(unit, &isem_key, &isem_entry, &success);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if(success == TRUE)
  {
    
    *lif_index = arad_pp_sem_result_to_lif_index(unit, isem_entry.sem_result_ndx);
    res = arad_pp_isem_access_entry_remove_unsafe(unit, &isem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    if (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY) {
      goto exit;
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
      res = arad_pp_sw_db_rif_to_lif_group_map_remove_lif_from_rif(unit, *lif_index);
      SOC_SAND_CHECK_FUNC_RESULT(res, 171, exit);
    }
#endif 

    
    res = arad_pp_sw_db_lif_table_entry_use_set(unit, *lif_index, SOC_PPC_LIF_ENTRY_TYPE_EMPTY,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  }
  else
  {
    *lif_index = ARAD_PP_LIF_NULL;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_rif_ip_tunnel_term_remove_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY     *term_key
  )
{
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_remove_verify()", 0, 0);
}


uint32
  arad_pp_rif_ip_tunnel_term_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY      *term_key,
    SOC_SAND_OUT  SOC_PPC_LIF_ID              *lif_index,
    SOC_SAND_OUT  SOC_PPC_RIF_IP_TERM_INFO    *term_info,
    SOC_SAND_OUT  SOC_PPC_RIF_INFO            *rif_info,
    SOC_SAND_OUT uint8                    *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_rif_ip_tunnel_term_get_internal_unsafe(
          unit,
          term_key,
          FALSE,
          lif_index,
          term_info,
          rif_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_get_unsafe()", 0, 0);
}


uint32
  arad_pp_rif_ip_tunnel_term_get_internal_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY      *term_key,
    SOC_SAND_IN  uint8                 ignore_key,
    SOC_SAND_OUT  SOC_PPC_LIF_ID           *lif_index,
    SOC_SAND_OUT  SOC_PPC_RIF_IP_TERM_INFO *term_info,
    SOC_SAND_OUT  SOC_PPC_RIF_INFO         *rif_info,
    SOC_SAND_OUT uint8                 *found
  )
{
  uint32                             res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY              isem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY            isem_entry;
  ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_DATA sem_res_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_GET_INTERNAL_UNSAFE);

  if(!ignore_key)
  {
      SOC_SAND_CHECK_NULL_INPUT(term_key);
  }
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPC_RIF_INFO_clear(rif_info);

  if (!ignore_key)
  {
    ARAD_PP_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
    ARAD_PP_CLEAR(&isem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
    
    
    
    res = arad_pp_rif_ip_tunnel_term_key_to_sem_key(unit,term_key,&isem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_isem_access_entry_get_unsafe(unit, &isem_key, &isem_entry, found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if(!*found)
    {
      goto exit;
    }
    *lif_index = arad_pp_sem_result_to_lif_index(unit, isem_entry.sem_result_ndx);

    
    if (term_key->flags & SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY) {
        term_info->ext_lif_id = isem_entry.sem_result_ndx;
        goto exit;
    }

  }
  else
  {
    *found = TRUE;
  }

  
  res = arad_pp_ihp_lif_table_ip_tt_tbl_get_unsafe(unit, *lif_index, &sem_res_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  term_info->cos_profile = sem_res_tbl_data.cos_profile;
  if(sem_res_tbl_data.in_rif_valid)
  {
     term_info->rif = sem_res_tbl_data.in_rif;
  }
  else
  {
    term_info->rif = SOC_PPC_RIF_NULL;
  }

    
  if(term_info->rif != SOC_PPC_RIF_NULL)
  {
    res = arad_pp_rif_info_get_imp_unsafe(unit, term_info->rif, rif_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  term_info->default_forward_profile = sem_res_tbl_data.da_not_found_profile;
  term_info->lif_profile = sem_res_tbl_data.in_lif_profile;
  
  
  term_info->learn_enable = sem_res_tbl_data.tt_learn_enable;
  if (sem_res_tbl_data.learn_info != 0)
  {
  res = arad_pp_fwd_decision_from_learn_buffer(
                    unit,
                    sem_res_tbl_data.learn_info,
                    SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF, 
                    &term_info->learn_record
                    );
      SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
  }
  else{
      term_info->learn_record.additional_info.outlif.val = *lif_index;
      term_info->learn_record.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
      term_info->learn_record.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
  }
  term_info->oam_valid = sem_res_tbl_data.oam_lif_set;
  term_info->protection_pass_value = sem_res_tbl_data.protection_path;
  term_info->protection_pointer = sem_res_tbl_data.protection_pointer;
  term_info->tpid_profile = sem_res_tbl_data.tpid_profile;
  term_info->vsi = sem_res_tbl_data.vsi;
  term_info->vsi_assignment_mode = sem_res_tbl_data.vsi_assignment_mode;
  term_info->processing_type = (sem_res_tbl_data.model_is_pipe)?SOC_PPC_MPLS_TERM_MODEL_PIPE:SOC_PPC_MPLS_TERM_MODEL_UNIFORM;
  if (SOC_IS_JERICHO(unit)) {
    term_info->global_lif =  sem_res_tbl_data.sys_in_lif;
  }

  
  if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
      term_info->orientation = sem_res_tbl_data.orientation_is_hub; 
  } else {
      term_info->orientation = (sem_res_tbl_data.orientation_is_hub) ? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
  }

  
  #ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
      term_info->skip_ethernet = (uint8) sem_res_tbl_data.skip_ethernet;
    }
    #endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_get_internal_unsafe()", 0, 0);
}

uint32
  arad_pp_rif_ip_tunnel_term_get_verify(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY      *term_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_GET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_get_verify()", 0, 0);
}



uint32
  arad_pp_rif_vsid_map_set_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID    vsid_ndx,
    SOC_SAND_IN  SOC_PPC_RIF_INFO  *rif_info
  )
{
  uint32       res = SOC_SAND_OK;
  SOC_PPC_RIF_ID rif_id;
  uint32         reg32_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_VSID_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  if ((vsid_ndx < SOC_DPP_CONFIG(unit)->l3.nof_rifs) || ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) && _BCM_DPP_VLAN_IS_BVID(unit, vsid_ndx))) { 
    rif_id = vsid_ndx;
  } else { 
    res = READ_IHP_HIGH_VSI_CONFIGr(unit, &reg32_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    rif_id = soc_reg_field_get(unit, IHP_HIGH_VSI_CONFIGr, reg32_val, HIGH_VSI_IN_RIFf);
  }

  res = arad_pp_rif_info_set_imp_unsafe(unit, rif_id, rif_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_vsid_map_set_unsafe()", vsid_ndx, 0);
}

uint32
  arad_pp_rif_vsid_map_set_verify(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID    vsid_ndx,
    SOC_SAND_IN  SOC_PPC_RIF_INFO  *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_VSID_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsid_ndx, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_RIF_INFO_verify(unit, rif_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_vsid_map_set_verify()", vsid_ndx, 0);
}

uint32
  arad_pp_rif_vsid_map_get_verify(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID    vsid_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_VSID_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsid_ndx, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_vsid_map_get_verify()", vsid_ndx, 0);
}


uint32
  arad_pp_rif_vsid_map_get_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID    vsid_ndx,
    SOC_SAND_OUT SOC_PPC_RIF_INFO  *rif_info
  )
{
  uint32       res = SOC_SAND_OK;
  SOC_PPC_RIF_ID rif_id;
  uint32         reg32_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_VSID_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  SOC_PPC_RIF_INFO_clear(rif_info);

  if (vsid_ndx < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers)) { 
    rif_id = vsid_ndx;
  } else { 
    res =READ_IHP_HIGH_VSI_CONFIGr(unit, &reg32_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    rif_id = soc_reg_field_get(unit, IHP_HIGH_VSI_CONFIGr, reg32_val, HIGH_VSI_IN_RIFf);
  }

  res = arad_pp_rif_info_get_imp_unsafe(
          unit,
          rif_id,
          rif_info
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_vsid_map_get_unsafe()", vsid_ndx, 0);
}


uint32
  arad_pp_rif_info_set_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID    rif_ndx,
    SOC_SAND_IN  SOC_PPC_RIF_INFO  *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_RIF_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  res = arad_pp_rif_info_set_imp_unsafe(
           unit,
           rif_ndx,
           rif_info
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_info_set_unsafe()", rif_ndx, 0);
}

uint32
  arad_pp_rif_info_set_verify(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID    rif_ndx,
    SOC_SAND_IN  SOC_PPC_RIF_INFO  *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_RIF_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(rif_ndx, SOC_DPP_CONFIG(unit)->l3.nof_rifs, SOC_PPC_RIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_RIF_INFO_verify(unit, rif_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_info_set_verify()", rif_ndx, 0);
}

uint32
  arad_pp_rif_info_get_verify(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID    rif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_RIF_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(rif_ndx, SOC_DPP_CONFIG(unit)->l3.nof_rifs, SOC_PPC_RIF_ID_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_info_get_verify()", rif_ndx, 0);
}


uint32
  arad_pp_rif_info_get_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID    rif_ndx,
    SOC_SAND_OUT SOC_PPC_RIF_INFO  *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_RIF_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  SOC_PPC_RIF_INFO_clear(rif_info);

  res = arad_pp_rif_info_get_imp_unsafe(
          unit,
          rif_ndx,
          rif_info
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_info_get_unsafe()", rif_ndx, 0);
}


uint32
  arad_pp_rif_ttl_scope_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl_val
  )
{
  uint32  res;
  uint64 reg_val;
  uint32  field_32[2] = {0};
  uint32 ttl_val_lcl = ttl_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_TTL_SCOPE_SET_UNSAFE);

  COMPILER_64_ZERO(reg_val);

  res = READ_EGQ_TTL_SCOPEr(unit, REG_PORT_ANY, &reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  field_32[1] = COMPILER_64_HI(reg_val);
  field_32[0] = COMPILER_64_LO(reg_val);

  SHR_BITCOPY_RANGE(field_32,ttl_scope_ndx*8, &ttl_val_lcl, 0, 8);

  COMPILER_64_SET(reg_val, field_32[1], field_32[0] );


  res = WRITE_EGQ_TTL_SCOPEr(unit, REG_PORT_ANY, reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ttl_scope_get_unsafe()", ttl_scope_ndx, 0);
}

uint32
  arad_pp_rif_ttl_scope_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_TTL_SCOPE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(ttl_scope_ndx, ARAD_PP_RIF_TTL_SCOPE_INDEX_MAX, ARAD_PP_RIF_TTL_SCOPE_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ttl_scope_set_verify()", ttl_scope_ndx, 0);
}

uint32
  arad_pp_rif_ttl_scope_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_TTL_SCOPE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(ttl_scope_ndx, ARAD_PP_RIF_TTL_SCOPE_INDEX_MAX, ARAD_PP_RIF_TTL_SCOPE_INDEX_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ttl_scope_get_verify()", ttl_scope_ndx, 0);
}


uint32
  arad_pp_rif_ttl_scope_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl_val
  )
{
  uint32  res;
  uint64   scope_data;
  uint32  field_32[2] = {0};
  uint32  ttl_val_lcl = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_TTL_SCOPE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(ttl_val);
  COMPILER_64_ZERO(scope_data);
  *ttl_val = 0;

  res = READ_EGQ_TTL_SCOPEr(unit, REG_PORT_ANY, &scope_data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  field_32[1] = COMPILER_64_HI(scope_data);
  field_32[0] = COMPILER_64_LO(scope_data);


  SHR_BITCOPY_RANGE(&ttl_val_lcl,0, field_32, ttl_scope_ndx*8, 8);
  
  *ttl_val = ttl_val_lcl & 0xffff;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ttl_scope_get_unsafe()", ttl_scope_ndx, 0);
}



uint32
arad_pp_rif_native_routing_vlan_tags_set_unsafe(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPC_RIF_ID                         intf_id, 
   SOC_SAND_IN  uint8                                  native_routing_vlan_tags
   )
{
    uint32
       res = SOC_SAND_OK; 
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
    
    if (SOC_IS_ARADPLUS_A0(unit)) 
    {
        
        uint32 native_routing_vlan_bytes = 14 + native_routing_vlan_tags * 4; 
        res = WRITE_EPNI_CFG_NATIVE_ETHERNET_COMPENSATIONr(unit, REG_PORT_ANY, native_routing_vlan_bytes);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    } 
    else if (SOC_IS_JERICHO(unit)) 
    {
        
        uint32 data = 0;
        uint32 value = 0; 
        uint32 native_routing_vlan_tags_32 = native_routing_vlan_tags; 
        int entry_offset = intf_id/8;
        
        res = soc_mem_read(unit, EPNI_ROO_VSIm, MEM_BLOCK_ANY, entry_offset, &data);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        
        SHR_BITCOPY_RANGE(&value, ((intf_id %8) * 2), &(native_routing_vlan_tags_32), 0,2);
        soc_mem_field32_set(unit, EPNI_ROO_VSIm, &data, ROO_VSIf, value); 
        res = soc_mem_write(unit,EPNI_ROO_VSIm, MEM_BLOCK_ANY, entry_offset,  &data);
    }
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_native_routing_vlan_tags_set_unsafe()", 0, 0); 
    
}


uint32
arad_pp_rif_native_routing_vlan_tags_set_verify(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPC_RIF_ID                         intf_id, 
   SOC_SAND_IN  uint8                                  native_routing_vlan_tags
   )
{
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
    
    SOC_SAND_ERR_IF_ABOVE_MAX(native_routing_vlan_tags, SOC_PPC_VLAN_TAGS_MAX, ARAD_PP_VLAN_TAGS_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_NOF(intf_id, SOC_DPP_CONFIG(unit)->l3.nof_rifs, SOC_PPC_RIF_ID_OUT_OF_RANGE_ERR, 10, exit);
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_native_routing_vlan_tags_set_verify()", 0, 0); 
}




uint32
arad_pp_rif_native_routing_vlan_tags_get_unsafe(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPC_RIF_ID                         intf_id, 
   SOC_SAND_OUT  uint8                                  *native_routing_vlan_tags
   )
{
    uint32
       res = SOC_SAND_OK; 
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
    
    if (SOC_IS_ARADPLUS_A0(unit)) 
    {
        
        uint32 native_routing_vlan_bytes = 0; 
        res = READ_EPNI_CFG_NATIVE_ETHERNET_COMPENSATIONr(unit, REG_PORT_ANY, &native_routing_vlan_bytes);

        *native_routing_vlan_tags = (uint8) ((native_routing_vlan_bytes - 14 ) / 4);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else if (SOC_IS_JERICHO(unit)) 
    {
        
        uint32 data;
        uint32 native_routing_vlan_tags_32 = 0; 
        int entry_offset = intf_id/8;

        *native_routing_vlan_tags = 0;

        
        res = soc_mem_read(unit, EPNI_ROO_VSIm, MEM_BLOCK_ANY, entry_offset, &data);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        
        SHR_BITCOPY_RANGE(&native_routing_vlan_tags_32, 0, &data, ((intf_id %8) * 2), 2);
        *native_routing_vlan_tags = native_routing_vlan_tags_32; 
    }
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_native_routing_vlan_tags_set_unsafe()", 0, 0); 
    
}

uint32
arad_pp_rif_native_routing_vlan_tags_get_verify(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPC_RIF_ID                         intf_id, 
   SOC_SAND_IN  uint8                                  *native_routing_vlan_tags
   )
{
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
    
    SOC_SAND_ERR_IF_ABOVE_NOF(intf_id, SOC_DPP_CONFIG(unit)->l3.nof_rifs, SOC_PPC_RIF_ID_OUT_OF_RANGE_ERR, 10, exit);
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_native_routing_vlan_tags_set_verify()", 0, 0); 
}
    

uint32
  arad_pp_l2_rif_mpls_key_parse_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY              *isem_key,
    SOC_SAND_OUT SOC_PPC_MPLS_LABEL_RIF_KEY           *rif_key
  )
{
  uint8 use_label_index = FALSE;
  uint32                    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_MPLS_LABEL_RIF_KEY_clear(rif_key);

  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.mpls_info.mpls_termination_label_index_enable.get(unit,&use_label_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);


  switch (isem_key->key_type) {
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF_ELI:
    if (use_label_index)
    {
      rif_key->label_index = SOC_PPC_MPLS_LABEL_INDEX_FIRST;
    }
    rif_key->vsid = isem_key->key_info.mpls.in_rif;    
    if (isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF_ELI) 
    {
      rif_key->flags |=  SOC_PPC_RIF_MPLS_LABEL_ELI;
    }
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_ELI:
    rif_key->flags |= SOC_PPC_RIF_MPLS_LABEL_ELI;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_FRR:
    rif_key->flags |= SOC_PPC_RIF_MPLS_LABEL_FRR;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_ELI:
    rif_key->flags |= SOC_PPC_RIF_MPLS_LABEL_ELI;
    rif_key->label_index = SOC_PPC_MPLS_LABEL_INDEX_FIRST;
    break;  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1:
    rif_key->label_index = SOC_PPC_MPLS_LABEL_INDEX_FIRST;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2:
    rif_key->label_index = SOC_PPC_MPLS_LABEL_INDEX_SECOND;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2_ELI:
    rif_key->label_index = SOC_PPC_MPLS_LABEL_INDEX_SECOND;
    rif_key->flags |= SOC_PPC_RIF_MPLS_LABEL_ELI;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L3:
    rif_key->label_index = SOC_PPC_MPLS_LABEL_INDEX_THIRD;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS:
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR, 10, exit);
    break;
  }

  rif_key->label_id = isem_key->key_info.mpls.label;
  rif_key->label_id_second = isem_key->key_info.mpls.label2;
  if (isem_key->key_info.mpls.is_bos) 
  {
    rif_key->flags |= SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_rif_mpls_key_parse_unsafe()", unit, use_label_index);
}


uint32
  arad_pp_rif_ip_tunnel_key_parse_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY              *isem_key,
    SOC_SAND_OUT SOC_PPC_RIF_IP_TERM_KEY              *term_key
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_RIF_IP_TERM_KEY_clear(term_key);

  switch (isem_key->key_type) {
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_RPA:
    term_key->flags |= SOC_PPC_RIF_IP_TERM_IPMC_BIDIR;
    term_key->dip = isem_key->key_info.rpa.dip;
    term_key->vrf = isem_key->key_info.rpa.vrf;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP_AFTER_RECYCLE:
    sal_memcpy(&term_key->dip6,&isem_key->key_info.ip_tunnel.dip6, sizeof(SOC_SAND_PP_IPV6_SUBNET));
    term_key->vrf = isem_key->key_info.ip_tunnel.vrf;        
    term_key->ipv4_next_protocol_prefix_len = isem_key->key_info.ip_tunnel.ipv4_next_protocol_prefix_len;
    term_key->ipv4_next_protocol = isem_key->key_info.ip_tunnel.ipv4_next_protocol;
    term_key->qualifier_next_protocol_prefix_len = isem_key->key_info.ip_tunnel.qualifier_next_protocol_prefix_len;
    term_key->qualifier_next_protocol = isem_key->key_info.ip_tunnel.qualifier_next_protocol;
    term_key->port_property_en = isem_key->key_info.ip_tunnel.port_property_en;
    term_key->port_property = isem_key->key_info.ip_tunnel.port_property;
    term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_IPV6; 
    if (isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP_AFTER_RECYCLE) {
        term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_BUD;
    }
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP_AFTER_RECYCLE:
    sal_memcpy(&term_key->dip6,&isem_key->key_info.ip_tunnel.dip6, sizeof(SOC_SAND_PP_IPV6_SUBNET));
    term_key->ipv4_next_protocol_prefix_len = isem_key->key_info.ip_tunnel.ipv4_next_protocol_prefix_len;
    term_key->ipv4_next_protocol = isem_key->key_info.ip_tunnel.ipv4_next_protocol;
    term_key->qualifier_next_protocol_prefix_len = isem_key->key_info.ip_tunnel.qualifier_next_protocol_prefix_len;
    term_key->qualifier_next_protocol = isem_key->key_info.ip_tunnel.qualifier_next_protocol;
    term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_IPV6; 
    if (isem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP_AFTER_RECYCLE) {
        term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_BUD;
    }
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_SIP:
    sal_memcpy(&term_key->sip6,&isem_key->key_info.ip_tunnel.sip6, sizeof(SOC_SAND_PP_IPV6_SUBNET));
    term_key->vrf = isem_key->key_info.ip_tunnel.vrf;        
    term_key->my_vtep_index = isem_key->key_info.ip_tunnel.my_vtep_index;
    term_key->port_property_en = isem_key->key_info.ip_tunnel.port_property_en;
    term_key->port_property = isem_key->key_info.ip_tunnel.port_property;    
    term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_IPV6; 
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP_DIP:
    term_key->dip = isem_key->key_info.ip_tunnel.dip;
    term_key->dip_prefix_len = isem_key->key_info.ip_tunnel.dip_prefix_len;
    term_key->sip = isem_key->key_info.ip_tunnel.sip;
    term_key->sip_prefix_len = isem_key->key_info.ip_tunnel.sip_prefix_len;
    term_key->ipv4_next_protocol = isem_key->key_info.ip_tunnel.ipv4_next_protocol;
    term_key->ipv4_next_protocol_prefix_len = isem_key->key_info.ip_tunnel.ipv4_next_protocol_prefix_len;
    term_key->port_property_en = isem_key->key_info.ip_tunnel.port_property_en;
    term_key->port_property = isem_key->key_info.ip_tunnel.port_property;
    term_key->flags |= (SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP | SOC_PPC_RIF_IP_TERM_FLAG_USE_SIP);    
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_DIP:
    term_key->dip = isem_key->key_info.ip_tunnel.dip;
    term_key->dip_prefix_len = 32;
    term_key->flags |= (SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP);
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP:
    term_key->sip = isem_key->key_info.ip_tunnel.sip;
    term_key->sip_prefix_len = 32;
    term_key->flags |= (SOC_PPC_RIF_IP_TERM_FLAG_USE_SIP);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR, 10, exit);
    break;
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_rif_mpls_key_parse_unsafe()", unit, 0);
}



CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_rif_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_rif;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_rif_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_rif;
}

uint32
  SOC_PPC_RIF_MPLS_LABELS_RANGE_verify(
    SOC_SAND_IN  SOC_PPC_RIF_MPLS_LABELS_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_label, ARAD_PP_RIF_FIRST_LABEL_MAX, ARAD_PP_RIF_FIRST_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->last_label, ARAD_PP_RIF_LAST_LABEL_MAX, ARAD_PP_RIF_LAST_LABEL_OUT_OF_RANGE_ERR, 11, exit);
  if(info->first_label > info->last_label)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_RIF_MPLS_LABEL_INVALID_RANGE_ERR, 20, exit);
  }
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_RIF_MPLS_LABELS_RANGE_verify()",0,0);
}

uint32
  SOC_PPC_MPLS_LABEL_RIF_KEY_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->label_id, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsid, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->label_index, SOC_PPC_NOF_MPLS_LABEL_INDEX-1, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_LABEL_RIF_KEY_verify()",0,0);
}

uint32
  SOC_PPC_RIF_INFO_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PPC_RIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->vrf_id != 0)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->vrf_id, ARAD_PP_VRF_ID_MIN, SOC_DPP_DEFS_GET(unit, nof_vrfs) - 1, SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR, 11, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, ARAD_PP_RIF_COS_PROFILE_MAX, ARAD_PP_RIF_COS_PROFILE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl_scope_index, ARAD_PP_RIF_TTL_SCOPE_INDEX_MAX, ARAD_PP_RIF_TTL_SCOPE_INDEX_OUT_OF_RANGE_ERR, 13, exit);  
#ifdef BCM_88660_A0
  SOC_SAND_ERR_IF_ABOVE_MAX(info->uc_rpf_mode, SOC_PPC_NOF_FRWRD_FEC_RPF_MODES, ARAD_PP_RIF_ROUTING_ENABLERS_BM_OUT_OF_RANGE_ERR, 15, exit);
#endif 

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_RIF_INFO_verify()",0,0);
}

uint32
  SOC_PPC_RIF_IP_TERM_INFO_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX_AND_NOT_NULL(info->rif, SOC_DPP_CONFIG(unit)->l3.nof_rifs - 1, SOC_PPC_RIF_NULL, SOC_PPC_RIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, ARAD_PP_RIF_COS_PROFILE_MAX, ARAD_PP_RIF_COS_PROFILE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->lif_profile, ARAD_PP_LIF_PROFILE_MAX, ARAD_PP_RIF_LIF_PROFILE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile, ARAD_PP_TPID_PROFILE_MAX, ARAD_PP_RIF_TPID_PROFILE_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_forward_profile, ARAD_PP_RIF_DEFAULT_FORWARD_PROFILE_MAX, ARAD_PP_RIF_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 15, exit); 
  
  
  if(info->learn_enable) {
      if ( !( (info->learn_record.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC && info->learn_record.dest_id <= SOC_DPP_DEFS_GET(unit, nof_fecs) - 1) ||
              (info->learn_record.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW && !(info->learn_record.dest_id & 0x1fffc)) ) )
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_RIF_LEARN_RECORD_OUT_OF_RANGE_ERR, 16, exit);
      }
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi_assignment_mode, ARAD_RIF_VSI_ASSIGNMENT_MODE_MAX, ARAD_RIF_VSI_ASSIGNMENT_MODE_OUT_OF_RANGE_ERR, 21, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->protection_pointer, ARAD_RIF_PROTECTION_POINTER_MAX, ARAD_RIF_PROTECTION_POINTER_OUT_OF_RANGE_ERR, 22, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_RIF_IP_TERM_INFO_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 
