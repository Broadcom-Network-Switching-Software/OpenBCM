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

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS

#include <shared/bsl.h>



#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_ac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/mem.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap_access.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_eg_encap_access.h>

#include <soc/dpp/PPC/ppc_api_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>





#define SOC_PPC_EG_ENCAP_EEP_TYPE_NDX_MAX                        (SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD-1)
#define ARAD_PP_EG_ENCAP_DEPTH_MAX                               (SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD-1)
#define ARAD_PP_EG_ENCAP_PROFILE_NDX_MAX                         ((SOC_IS_JERICHO_B0_AND_ABOVE(unit)) ? (15) : (7))
#define ARAD_PP_EG_ENCAP_ENTRY_NDX_MAX                           (15)
#define ARAD_PP_EG_ENCAP_SRC_IP_MAX                              (15)
#define ARAD_PP_EG_ENCAP_IP_TNL_LIMIT_MIN                        (0) 
#define ARAD_PP_EG_ENCAP_SWAP_LABEL_MAX                          (1*1024*1024-1)
#define ARAD_PP_EG_ENCAP_LABEL_MAX                               (1*1024*1024-1)
#define ARAD_PP_EG_ENCAP_PUSH_PROFILE_MAX                        ((SOC_IS_JERICHO_B0_AND_ABOVE(unit)) ? (15) : (7))
#define ARAD_PP_EG_ENCAP_TPID_PROFILE_MAX                        (3)
#define ARAD_PP_EG_ENCAP_TUNNEL_LABEL_MAX                        (SOC_SAND_U32_MAX)
#define ARAD_PP_EG_ENCAP_NOF_TUNNELS_MAX                         (2)
#define ARAD_PP_EG_ENCAP_DEST_MAX                                (SOC_SAND_U32_MAX)
#define ARAD_PP_EG_ENCAP_SRC_INDEX_MAX                           (15)
#define ARAD_PP_EG_ENCAP_TTL_INDEX_MAX                           (3)
#define ARAD_PP_EG_ENCAP_TOS_INDEX_MAX                           (15)
#define ARAD_PP_EG_ENCAP_TPID_INDEX_MAX                          (3)
#define ARAD_PP_EG_LL_ENCAP_REMARK_PROFILE_INDEX_MAX             ((SOC_IS_JERICHO(unit)) ? (7) : (3))
#define ARAD_PP_EG_VSI_ENCAP_REMARK_PROFILE_INDEX_MAX            (15)
#define ARAD_PP_EG_ENCAP_MODEL_MAX                               (SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS-1)
#define SOC_PPC_EG_ENCAP_EXP_MARK_MODE_MAX                       (SOC_PPC_NOF_EG_ENCAP_EXP_MARK_MODES-1)
#define ARAD_PP_EG_ENCAP_CW_MAX                                  (SOC_SAND_U32_MAX)
#define SOC_PPC_EG_ENCAP_ENTRY_TYPE_MAX                          (SOC_PPC_NOF_EG_ENCAP_ENTRY_TYPES_ARAD-1)
#define SOC_PPC_EG_ENCAP_ACCESS_PHASE_MAX(unit)                  (SOC_PPC_NOF_EG_ENCAP_ACCESS_PHASE_TYPES(unit)-1)
#define ARAD_PP_EG_ENCAP_ADD_ENTROPY_LABEL_MAX                   (1)
#define ARAD_PP_EG_ENCAP_PROTECTION_PASS_VAL_MAX                 (1)

#define ARAD_PP_EG_ENCAP_SIP_NOF_BITS                            (32)
#define ARAD_PP_EG_ENCAP_TTL_NOF_BITS                            (8)
#define ARAD_PP_EG_ENCAP_TOS_NOF_BITS                            (8)
#define ARAD_PP_EG_ENCAP_REMARK_NOF_BITS                         (4)
#define ARAD_PP_EG_ENCAP_NEXT_EEP_INVALID                        (SOC_PPC_EG_ENCAP_NEXT_EEP_INVALID)

#define ARAD_PP_EG_ENCAP_NOF_MIRRORS                             (16)

#define ARAD_PP_EG_ENCAP_ERSPAN_VERSION                          (1)
#define ARAD_PP_EG_ENCAP_VXLAN_UDP_SRC_PORT                      (0x0050)

#define ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_POINTER_LSB           (0)
#define ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_POINTER_NOF_BITS      (15)
#define ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_PATH_LSB              (14)
#define ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_PATH_NOF_BITS         (1)

#define ARAD_PP_EG_ENCAP_IPV6_SIP_INDEX_SHIFT                    (4)
#define ARAD_PP_EG_ENCAP_IPV6_SIP_INDEX_MAX                      (15)
#define ARAD_PP_EG_ENCAP_IPV6_SIP_INDEX_MASK                     (0xF)
#define ARAD_PP_EG_ENCAP_IPV6_TYPE_MASK                          (0x30)
#define ARAD_PP_EG_ENCAP_IPV6_OUTLIF_PROFILE_MASK                (0x3F)








#define ARAD_PP_EG_ENCAP_NDX_OF_TYPE_VERIFY(ndx) \
    if (ndx % 2 != 0) {                                                     \
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_NDX_ILLEGAL_ERR, 550, exit); \
    }                                                                       \



#define ARAD_PP_EG_ENCAP_UPDATE_FIELD_DROP(entry, flags, val)   \
    if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_DROP) {            \
        entry.drop = val;                                       \
    }

#define ARAD_PP_EG_ENCAP_UPDATE_FIELD_OUT_LIF_PROFILE(entry, flags, val)   \
    if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_OUT_LIF_PROFILE) {            \
        entry.outlif_profile = val;                                       \
    }


#define ARAD_PP_EG_ENCAP_GET_FIELD_DROP(entry, flags, val)   \
    if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_DROP) {            \
        *val = entry.drop;                                       \
        goto exit;                                              \
    }

#define ARAD_PP_EG_ENCAP_GET_FIELD_NEXT_LIF(entry, flags, val)   \
    if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF) {            \
        *val = entry.next_outlif;                               \
        goto exit;                                              \
    }

#define ARAD_PP_EG_ENCAP_GET_FIELD_OUT_LIF_PROFILE(entry, flags, val)   \
    if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_OUT_LIF_PROFILE) {            \
        *val = entry.outlif_profile;                               \
        goto exit;                                              \
    }




#define ARAD_PP_EG_ENCAP_PRGE_MEM_ERSPAN_FORMAT_SET(__device_id, __prio, __trunc, __span_id, __data) \
    (__data)[1] = ( (ARAD_PP_EG_ENCAP_ERSPAN_VERSION << 28) | (__prio << 13) | (__trunc << 10) | __span_id ) ; \
     (__data)[0] = 0;

#define ARAD_PP_EG_ENCAP_PRGE_MEM_ERSPAN_FORMAT_SPAN_ID_GET(__span_id, __data) \
    (__span_id) = (((__data)[1]) & 0x03ff) ; 






typedef enum {
  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_NONE = 0,
  
  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_ETHER_IP = 1,

  #ifdef BCM_88660_A0     
  
  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_VXLAN = 1,
  #endif

  
      SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_BASIC_GRE = 2,
  
  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_ENHANCE_GRE = 3
} SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD; 






CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_eg_encap[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_RANGE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_RANGE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_RANGE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_RANGE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_RANGE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_RANGE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_RANGE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_RANGE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_AC_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_AC_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_AC_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_AC_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_PWE_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_PWE_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_PWE_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_PWE_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_VSI_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_VSI_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_VSI_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_VSI_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_MPLS_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_MPLS_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_MPLS_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_LL_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_LL_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_LL_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_LL_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ENTRY_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ENTRY_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_ENTRY_TYPE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_ENTRY_TYPE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_ENTRY_TYPE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_ENCAP_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_GET_ERRS_PTR),
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET_VERIFY),

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_LL_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_TUNNEL_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_LIF_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_DATA_ENTRY_GET_UNSAFE),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_eg_encap[] =
{
  
  {
    ARAD_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'lif_eep_ndx' is out of range. \n\r "
    "The range is: No min - 64K.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'next_eep' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'tunnel_eep_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'll_eep' is out of range. \n\r "
    "The range is: 0-64K.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_LL_EEP_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_LL_EEP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'll_eep_ndx' is out of range. \n\r "
    "The range is: 0 to range_info->ll_limit.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PPC_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'eep_type_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_EEP_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_EEP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'eep_ndx' is out of range. \n\r "
    "The range is: 0 - 64K.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_DEPTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_DEPTH_OUT_OF_RANGE_ERR",
    "The parameter 'depth' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'profile_ndx' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_NDX_ILLEGAL_ERR,
    "ARAD_PP_EG_ENCAP_NDX_ILLEGAL_ERR",
    "The parameter 'll_eep' is illegal. \n\r "
    "Must be even.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_INCOMPATIBLE_TYPE_ERR,
    "ARAD_PP_EG_ENCAP_INCOMPATIBLE_TYPE_ERR",
    "The requested eep_type_ndx is not comaptible with \n\r"
    "the existing entry at eep_ndx. \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_ENTRY_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_ENTRY_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'entry_ndx' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_SRC_IP_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_SRC_IP_OUT_OF_RANGE_ERR",
    "The parameter 'src_ip' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_LL_LIMIT_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_LL_LIMIT_OUT_OF_RANGE_ERR",
    "The parameter 'll_limit' is out of range. \n\r "
    "The range is: 0 - 4*1024.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_IP_TNL_LIMIT_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_IP_TNL_LIMIT_OUT_OF_RANGE_ERR",
    "The parameter 'ip_tnl_limit' is out of range. \n\r "
    "The range is: (ll_limit + 1) - (12K-1).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_NOF_EG_ENCAP_ACCESS_PHASE_OUT_OF_RANGE_ERROR,
    "ARAD_PP_NOF_EG_ENCAP_ACCESS_PHASE_OUT_OF_RANGE_ERROR",
    "The parameter 'bank_access_phase' is out of range. \n\r "
    "The range is: 0-3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_MPLS_TNL_LIMIT_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_MPLS_TNL_LIMIT_OUT_OF_RANGE_ERR",
    "The parameter 'mpls_tnl_limit' is out of range. \n\r "
    "The range is: ip_tnl_limit+1 - 12*1024.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_SWAP_LABEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_SWAP_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'swap_label' is out of range. \n\r "
    "The range is: 0 - 1*1024*1024.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_LABEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'label' is out of range. \n\r "
    "The range is: 0 - 1*1024*1024.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_PUSH_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_PUSH_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'push_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR",
    "The parameter 'out_vsi' is out of range. \n\r "
    "The range is: 0 - 4K-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_TPID_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_TPID_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_POP_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_POP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'pop_type' is out of range. \n\r "
    "Value should be an mpls pop command (Range: 8-14).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_TUNNEL_LABEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_TUNNEL_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'tunnel_label' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_NOF_TUNNELS_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_NOF_TUNNELS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_tunnels' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_ORIENTATION_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_ORIENTATION_OUT_OF_RANGE_ERR",
    "The parameter 'orientation' is out of range. \n\r "
    "The range is: SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS - SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_DEST_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_DEST_OUT_OF_RANGE_ERR",
    "The parameter 'dest' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_SRC_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_SRC_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'src_index' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'ttl_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_TOS_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_TOS_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tos_index' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_PCP_DEI_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_PCP_DEI_OUT_OF_RANGE_ERR",
    "The parameter 'pcp_dei' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_TPID_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_TPID_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_MODEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_MODEL_OUT_OF_RANGE_ERR",
    "The parameter 'model' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_EG_ENCAP_EXP_MARK_MODE_OUT_OF_RANGE_ERR,
    "SOC_PPC_EG_ENCAP_EXP_MARK_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'exp_mark_mode' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_EG_ENCAP_EXP_MARK_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_CW_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_CW_OUT_OF_RANGE_ERR",
    "The parameter 'cw' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_EG_ENCAP_ENTRY_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PPC_EG_ENCAP_ENTRY_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'entry_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_EG_ENCAP_ENTRY_TYPES_ARAD-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  {
    ARAD_PP_EG_ENCAP_LIF_EEP_NDX_AC_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_LIF_EEP_NDX_AC_OUT_OF_RANGE_ERR",
    "'lif_eep_ndx' is out of range. The range is: 0 to range_info->ll_limit,\n\r "
    "as configured using soc_ppd_eg_encap_range_info_set().\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_LIF_EEP_NDX_MPLS_TUNNEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_LIF_EEP_NDX_MPLS_TUNNEL_OUT_OF_RANGE_ERR",
    "'lif_eep_ndx' is out of range. The range is: range_info->ip_tunnel_limit+1\n\r "
    "to 12K-1, as configured using soc_ppd_eg_encap_range_info_set().\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_LIF_EEP_AND_AC_ID_NOT_EQUAL_ERR,
    "ARAD_PP_EG_ENCAP_LIF_EEP_AND_AC_ID_NOT_EQUAL_ERR",
    "Out ac should be equal to lif_eep.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_LIF_EEP_NDX_PWE_MCAST_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_LIF_EEP_NDX_PWE_MCAST_OUT_OF_RANGE_ERR",
    "'lif_eep_ndx' is out of range. \n\r"
    "The range is: MAX(range_info->ip_tunnel_limit+1,8K) to 12K-1 .\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_IP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_IP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR",
    "'PARAM_NAME ' is out of range. \n\r"
    "The range is: 0-64K, \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_LL_ENCAP_REMARK_PROFILE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_LL_ENCAP_REMARK_PROFILE_INDEX_OUT_OF_RANGE_ERR",
    "remark_profile_index is out of range. \n\r"
    "The range for ll encapsulation entry is: 0-3, \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_VSI_ENCAP_REMARK_PROFILE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_VSI_ENCAP_REMARK_PROFILE_INDEX_OUT_OF_RANGE_ERR",
    "remark_profile_index is out of range. \n\r"
    "The range for vsi encapsulation entry is: 0-15, \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_EG_ENCAP_POP_INFO_PKT_FRWRD_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PPC_EG_ENCAP_POP_INFO_PKT_FRWRD_TYPE_OUT_OF_RANGE_ERR",
    "pop_next_header is out of range. \n\r"
    "header must be of type ethernet/mpls/ipv4/ipv6. \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_ADD_ENTROPY_LABEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_ADD_ENTROPY_LABEL_OUT_OF_RANGE_ERR",
    "add_entropy_label is out of range. \n\r"
    "The range is: 0-1, \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_MIRROR_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_MIRROR_ID_OUT_OF_RANGE_ERR",
    "mirror_index is out of range. \n\r"
    "The range is: 0-15, \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_MIRROR_VLAN_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_MIRROR_VLAN_OUT_OF_RANGE_ERR",
    "mirror_index is out of range. \n\r"
    "The range is: 0 - 4*1024-1, \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_IS_ERSPAN_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_IS_ERSPAN_OUT_OF_RANGE_ERR",
    "is_erspan is out of range. \n\r"
    "The range is: 0 - 1, \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};




 uint32
  arad_pp_eg_encap_ipv6_entry_get_references(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_OUT  uint32                                 *sip_index,
    SOC_SAND_OUT  uint32                                 *dip_index,
    SOC_SAND_OUT  uint8                                  *found
  );





STATIC uint32 SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_TO_SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD( int unit, 
                                                                                                SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE encapsulation_mode, 
                                                                                                uint32* encapsulation_mode_hw) {
    uint32 res = SOC_SAND_OK;

    switch (encapsulation_mode) {
    case (SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_NONE) :
        *encapsulation_mode_hw = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_NONE;
    break;
    case (SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP) :
        *encapsulation_mode_hw = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_ETHER_IP;
    break;
    case (SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE) :
        *encapsulation_mode_hw = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_BASIC_GRE;
    break;
    case (SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE) :
        *encapsulation_mode_hw = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_ENHANCE_GRE;
    break;
    case (SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN) : 
        *encapsulation_mode_hw = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_VXLAN;
    break;
    default: 
        
        res = SOC_SAND_ERR;
        break;
    }
    return res;
}


STATIC uint32 
SOC_PPC_EG_ENCAP_SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_TO_ENCAPSULATION_MODE(int unit, 
                                                                                  uint32 encapsulation_mode_hw, 
                                                                                  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE* encapsulation_mode) {
    int vxlan_mode;
    uint32 res;

    res = SOC_SAND_OK;

    switch (encapsulation_mode_hw) {
    case (SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_NONE) :
        *encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_NONE;
    break;
    case (SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_ETHER_IP) : 
        
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) { 
            *encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP;
        }
        
        #ifdef BCM_88660_A0
        else {
            
            vxlan_mode = SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN);
            if (vxlan_mode) {
                *encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN;
            } else {
                *encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP;
            }
      }
      #endif
        
    break;
    case (SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_BASIC_GRE) :
        *encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE;
    break;
    case (SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_ENHANCE_GRE) :
        *encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE;
    break;
    default: 
        
        return SOC_SAND_ERR;
        break;
    }

    return res;
}



STATIC uint32
  arad_pp_eg_encap_data_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  data_eep_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                 *encap_info,
    SOC_SAND_OUT uint8                                   *next_eep_valid, 
    SOC_SAND_OUT uint32                                    *next_eep
  );


STATIC soc_error_t 
    arad_pp_eg_encap_trill_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  eep_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                 *encap_info,
    SOC_SAND_OUT uint32                                  *next_eep
  ); 


uint32
  arad_pp_eg_encap_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
      reg_val;
  uint32
    res = SOC_SAND_OK;
  uint32 
    fld_val;

  uint64
    reg_val_64;
  uint64 
    fld_val_64;
  uint8 is_erspan_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_PCID_LITE_SKIP(unit);


  res = arad_pp_eg_encap_access_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  reg_val = 0x88BE;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_IPV4_UNKNOWN_HEADER_CODE_CFGr, SOC_CORE_ALL, 0, IPV4_UNKNOWN_HEADER_CODE_ETHERNET_TYPEf,  reg_val));

   
  reg_val = 6;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_GLOBAL_CONFIGr, SOC_CORE_ALL, 0, TM_ACTION_IS_SNOOPf,  reg_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_GLOBAL_CONFIGr, SOC_CORE_ALL, 0, TM_ACTION_IS_SNOOPf,  reg_val));
  

  
  if(SOC_DPP_CONFIG(unit)->pp.ip_tunnel_defrag_set){
      reg_val = 1;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_IPV4_TOS_TO_FLAGSr, SOC_CORE_ALL, 0, IPV4_TOS_TO_FLAGSf,  reg_val));
  }

  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      if (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN | SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE) ) {
          
          fld_val = 25; 
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_BOUNCE_BACKr, SOC_CORE_ALL, 0, BOUNCE_BACK_THRESHOLDf,  fld_val)); 
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_BOUNCE_BACKr, SOC_CORE_ALL, 0, ENABLE_BOUNCE_BACK_FILTERf, &fld_val)); 
          fld_val |= 0x4; 
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_BOUNCE_BACKr, SOC_CORE_ALL, 0, ENABLE_BOUNCE_BACK_FILTERf,  fld_val)); 
      }
  }
  #ifdef BCM_88660_A0
  
  if (SOC_IS_ARADPLUS(unit) && SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN)) {
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,READ_EPNI_CFG_VXLAN_UDP_FIELDSr(unit, SOC_CORE_ALL, &reg_val_64));
      
      
      COMPILER_64_SET(fld_val_64, 0, 0x08);
      ARAD_FLD_TO_REG64(EPNI_CFG_VXLAN_UDP_FIELDSr, CFG_VXLAN_FLAGSf, fld_val_64 , reg_val_64, 100, exit);
      COMPILER_64_SET(fld_val_64, 0, 0x0);
      ARAD_FLD_TO_REG64(EPNI_CFG_VXLAN_UDP_FIELDSr, CFG_VXLAN_RESERVED_1f, fld_val_64, reg_val_64, 110, exit);
      COMPILER_64_SET(fld_val_64, 0, 0x0);
      ARAD_FLD_TO_REG64(EPNI_CFG_VXLAN_UDP_FIELDSr, CFG_VXLAN_RESERVED_2f, fld_val_64, reg_val_64, 120, exit);
      COMPILER_64_SET(fld_val_64, 0, ARAD_PP_EG_ENCAP_VXLAN_UDP_SRC_PORT);
      ARAD_FLD_TO_REG64(EPNI_CFG_VXLAN_UDP_FIELDSr, CFG_UDP_SRC_PORT_VXLANf, fld_val_64, reg_val_64, 130, exit);
      COMPILER_64_SET(fld_val_64, 0, SOC_DPP_CONFIG(unit)->pp.vxlan_udp_dest_port);
      ARAD_FLD_TO_REG64(EPNI_CFG_VXLAN_UDP_FIELDSr, CFG_UDP_DEST_PORT_VXLANf, fld_val_64, reg_val_64, 140, exit);
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,WRITE_EPNI_CFG_VXLAN_UDP_FIELDSr(unit, SOC_CORE_ALL,  reg_val_64));
  }
  #endif

  is_erspan_enable = soc_property_get(unit, spn_BCM886XX_ERSPAN_TUNNEL_ENABLE, 0) ? 1 : 0;
  if (is_erspan_enable == 1) {
      
      if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 180, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, EPNI_IPV4_SIZE_WITH_GRE_8_KEYr, SOC_CORE_ALL, 0, IPV4_SIZE_WITH_GRE_8_KEYf, &fld_val));
          fld_val += 8; 
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 190, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, EPNI_IPV4_SIZE_WITH_GRE_8_KEYr, SOC_CORE_ALL, 0, IPV4_SIZE_WITH_GRE_8_KEYf, fld_val));
      }
  }

    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_init_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_encap_entry_init_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_eg_encap_access_entry_init_unsafe(
          unit,
          lif_eep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_entry_init_unsafe()", lif_eep_ndx, 0);
}


uint32
  arad_pp_eg_encap_range_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_RANGE_INFO                 *range_info
  )
{
  uint32
    bank_ndx,
    res = SOC_SAND_OK,
    tmp,
    nof_bits_per_bank;
  soc_reg_above_64_val_t 
      reg_val, 
      fld_val;
  soc_error_t
    rv = SOC_E_NONE;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_RANGE_INFO_SET_UNSAFE);
  
  SOC_SAND_CHECK_NULL_INPUT(range_info);

  SOC_REG_ABOVE_64_CLEAR(reg_val);
  SOC_REG_ABOVE_64_CLEAR(fld_val);

  nof_bits_per_bank = SOC_DPP_DEFS_GET(unit, eg_encap_phase_nof_bits);

  for (bank_ndx=0; bank_ndx < SOC_PPC_EG_ENCAP_NOF_BANKS(unit); bank_ndx++) {
    if (range_info->bank_access_phase[bank_ndx] < SOC_PPC_NOF_EG_ENCAP_ACCESS_PHASE_TYPES(unit)) {
        tmp = range_info->bank_access_phase[bank_ndx];
        res = soc_sand_bitstream_set_any_field(&tmp, bank_ndx * nof_bits_per_bank, nof_bits_per_bank, fld_val);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20+bank_ndx, exit);
    }
  }
    
  soc_reg_above_64_field_set(unit, EPNI_EEDB_OUTLIF_ACCESSr, reg_val, EEDB_OUTLIF_ACCESSf, fld_val);
  SOC_DPP_ALLOW_WARMBOOT_WRITE(soc_reg_above_64_set(unit, EPNI_EEDB_OUTLIF_ACCESSr, REG_PORT_ANY, 0, reg_val), rv);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);

  if (SOC_IS_JERICHO_PLUS(unit)) {
      
      SOC_DPP_ALLOW_WARMBOOT_WRITE(soc_reg_above_64_set(unit, EDB_EEDB_OUTLIF_ACCESSr, REG_PORT_ANY, 0, reg_val), rv);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 40, exit);
  }

exit:
  SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX(rv);
  if(rv != SOC_E_NONE) {
    LOG_ERROR(BSL_LS_SOC_EGRESS,
              (BSL_META_U(unit,
                          " Failed while executing the macro SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX.\n")));
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_range_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_encap_range_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_RANGE_INFO                 *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_RANGE_INFO_SET_VERIFY);

  res = SOC_PPC_EG_ENCAP_RANGE_INFO_verify(unit, range_info);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_range_info_set_verify()", 0, 0);
}

uint32
  arad_pp_eg_encap_range_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_RANGE_INFO_GET_VERIFY);

  
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_range_info_get_verify()", 0, 0);
}



uint32
  arad_pp_eg_encap_range_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_RANGE_INFO                 *range_info
  )
{
  uint32
    ndx,
    res = SOC_SAND_OK;
  uint32
      nof_bits_per_bank;
  soc_reg_above_64_val_t reg_val, fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_RANGE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  SOC_PPC_EG_ENCAP_RANGE_INFO_clear(range_info);

  SOC_REG_ABOVE_64_CLEAR(reg_val);
  SOC_REG_ABOVE_64_CLEAR(fld_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg_above_64_get(unit, EPNI_EEDB_OUTLIF_ACCESSr, REG_PORT_ANY, 0, reg_val));

  nof_bits_per_bank = SOC_DPP_DEFS_GET(unit, eg_encap_phase_nof_bits);
 
  soc_reg_above_64_field_get(unit, EPNI_EEDB_OUTLIF_ACCESSr, reg_val, EEDB_OUTLIF_ACCESSf, fld_val); 

  for (ndx=0; ndx < SOC_PPC_EG_ENCAP_NOF_BANKS(unit); ndx++) {

      res = soc_sand_bitstream_get_any_field(fld_val, ndx*nof_bits_per_bank, nof_bits_per_bank, &range_info->bank_access_phase[ndx]);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20+ndx, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_range_info_get_unsafe()", 0, 0);
}



uint32 arad_pp_eg_encap_protection_info_set_verify(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PROTECTION_INFO       *protection_info)
{
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_PROTECTION_INFO, unit, protection_info, 10, exit);
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_protection_info_set_verify()", 0, 0);
}



uint32 arad_pp_eg_encap_protection_info_extension_buffer_set(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  uint8                                  is_full_entry_extension,
    SOC_SAND_IN  uint8                                  is_coupled,
    SOC_SAND_IN SOC_PPC_EG_ENCAP_PROTECTION_INFO        *protection_info, 
    SOC_SAND_OUT uint64                                 *protection_buffer)
{

    uint32 res = SOC_SAND_OK;
    uint32 data[sizeof(*protection_buffer)/sizeof(uint32)], tmp_val; 
    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_os_memset(data, 0x0, sizeof(*protection_buffer));

    
    res = soc_sand_bitstream_set_any_field(&(protection_info->protection_pointer), 
                                           ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_POINTER_LSB, 
                                           ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_POINTER_NOF_BITS, 
                                           data);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    if (is_coupled && is_full_entry_extension) {

        
        tmp_val = SOC_SAND_NUM2BOOL(protection_info->protection_pass_value);
        res = soc_sand_bitstream_set_any_field(&tmp_val, 
                                               ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_PATH_LSB, 
                                               ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_PATH_NOF_BITS, 
                                               data);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }

    
    COMPILER_64_SET(*protection_buffer, data[1], data[0]);

exit:
  SOCDNX_FUNC_RETURN; 
}



uint32 arad_pp_eg_encap_protection_info_extension_buffer_get(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  uint8                                  is_full_entry_extension,
    SOC_SAND_IN  uint8                                  is_coupled,
    SOC_SAND_IN int                                     out_lif_id,
    SOC_SAND_IN  uint64                                 protection_buffer,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PROTECTION_INFO       *protection_info)
{

    uint32 res = SOC_SAND_OK;
    uint32 data[sizeof(protection_buffer)/sizeof(uint32)], tmp_val;
    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_os_memset(data, 0x0, sizeof(protection_buffer));

    
    COMPILER_64_TO_32_LO(data[0], protection_buffer); 
    COMPILER_64_TO_32_HI(data[1], protection_buffer); 

    
    res = soc_sand_bitstream_get_any_field(data,
                                           ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_POINTER_LSB,
                                           ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_POINTER_NOF_BITS,
                                           &(protection_info->protection_pointer));
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    if (is_coupled && is_full_entry_extension) {

        
        res = soc_sand_bitstream_get_any_field(data,
                                               ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_PATH_LSB, 
                                               ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_PATH_NOF_BITS, 
                                               &tmp_val);
        SOCDNX_SAND_IF_ERR_EXIT(res);
        protection_info->protection_pass_value = SOC_SAND_BOOL2NUM(tmp_val);

        
        SHR_BITCLR(&(protection_info->protection_pointer),
                   (ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_POINTER_NOF_BITS + ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_POINTER_LSB - 1));
    } else {
        protection_info->protection_pass_value = 0;

        
        if (!is_full_entry_extension) {

            if (is_coupled) {
                
                protection_info->protection_pass_value = SOC_SAND_NUM2BOOL_INVERSE(out_lif_id % 2);
            } else {
                
                SHR_BITCLR(&(protection_info->protection_pointer), ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_POINTER_LSB);
                protection_info->protection_pointer |= (out_lif_id % 2) << ARAD_PP_EG_ENCAP_ACCESS_PROTECTION_POINTER_LSB;
            }
        }
    }

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 arad_pp_eg_encap_protection_info_set_unsafe(
    SOC_SAND_IN int                                 unit,
    SOC_SAND_IN int                                 out_lif_id,
    SOC_SAND_IN uint8                               is_full_entry_extension,
    SOC_SAND_IN SOC_PPC_EG_ENCAP_PROTECTION_INFO    *protection_info)
{
    uint8 is_coupled, is_ingress = FALSE;
    uint32 res = SOC_SAND_OK;
    uint64 protection_buffer;
    SOCDNX_INIT_FUNC_DEFS

    if (protection_info->is_protection_valid) {

        
        is_coupled = SOC_DPP_IS_PROTECTION_EGRESS_COUPLED(unit);
        res = arad_pp_eg_encap_protection_info_extension_buffer_set(
                unit, is_full_entry_extension, is_coupled, protection_info, &protection_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        
        res = arad_pp_lif_additional_data_set(unit, out_lif_id, is_ingress, protection_buffer, FALSE);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN; 
}



uint32 arad_pp_eg_encap_protection_info_get_unsafe(
    SOC_SAND_IN int                                 unit,
    SOC_SAND_IN int                                 out_lif_id,
    SOC_SAND_IN uint8                               is_full_entry_extension,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PROTECTION_INFO   *protection_info)
{
    uint8 is_coupled, is_ingress = FALSE, is_egress_wide_entry, ext_type;
    uint32 res = SOC_SAND_OK;
    uint64 protection_buffer;
    SOCDNX_INIT_FUNC_DEFS

    
    res = arad_pp_lif_is_wide_entry(unit, out_lif_id, FALSE, &is_egress_wide_entry, &ext_type);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if (is_egress_wide_entry && (ext_type == 0)) {

        
        res = arad_pp_lif_additional_data_get(unit, out_lif_id, is_ingress, &protection_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        
        is_coupled = SOC_DPP_IS_PROTECTION_EGRESS_COUPLED(unit);
        res = arad_pp_eg_encap_protection_info_extension_buffer_get(
                unit, is_full_entry_extension, is_coupled, out_lif_id, protection_buffer, protection_info);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        protection_info->is_protection_valid = TRUE;
    } else {
        SOC_PPC_EG_ENCAP_PROTECTION_INFO_clear(protection_info);
    }

exit:
    SOCDNX_FUNC_RETURN; 
}



uint32
  arad_pp_eg_encap_null_lif_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_null_lif_entry_add_unsafe()", lif_eep_ndx, 0);
}

uint32
  arad_pp_eg_encap_null_lif_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_null_lif_entry_add_verify()", lif_eep_ndx, 0);
}




uint32  arad_pp_eg_encap_lif_field_get_unsafe(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                                lif_eep_ndx,
      SOC_SAND_IN  uint32                                flags,
      SOC_SAND_OUT  uint32                               *val 
  )
{
    uint32
     res = SOC_SAND_OK;
    ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
      eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(val);

    res = arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, lif_eep_ndx, &eep_type);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    
    if(eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC) {
        ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT
            entry;
        res = arad_pp_eg_encap_access_out_ac_entry_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);
        if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF) {
            
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
        }
        ARAD_PP_EG_ENCAP_GET_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_GET_FIELD_OUT_LIF_PROFILE(entry,flags,val);
    }
    else if(eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_TRILL) {
        ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT
            entry;
        res = arad_pp_eg_encap_access_trill_entry_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        ARAD_PP_EG_ENCAP_GET_FIELD_NEXT_LIF(entry,flags,val);
        ARAD_PP_EG_ENCAP_GET_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_GET_FIELD_OUT_LIF_PROFILE(entry,flags,val);
    }
    else if(eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER) {
        ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT
            entry;
        res = arad_pp_eg_encap_access_link_layer_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF){
            *val = (entry.next_outlif_valid == TRUE) ? entry.next_outlif_lsb : 0xFFFFFFFF;
        }
        ARAD_PP_EG_ENCAP_GET_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_GET_FIELD_OUT_LIF_PROFILE(entry,flags,val);
    }
    else if(eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF) {
        ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT
                entry;
        if (SOC_IS_JERICHO_PLUS(unit)) {
            
            if((flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_OUT_LIF_PROFILE) == FALSE &&
                (flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_DROP) == FALSE )
            { 
               SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);  
            }
            res = qax_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        
        } else { 
            res = arad_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            ARAD_PP_EG_ENCAP_GET_FIELD_NEXT_LIF(entry,flags,val);
            
        }
        ARAD_PP_EG_ENCAP_GET_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_GET_FIELD_OUT_LIF_PROFILE(entry,flags,val);
    }
    else if(eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA || eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_IPV6) {
        ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT
            entry;
        res = arad_pp_eg_encap_access_data_entry_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        ARAD_PP_EG_ENCAP_GET_FIELD_NEXT_LIF(entry,flags,val);
        ARAD_PP_EG_ENCAP_GET_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_GET_FIELD_OUT_LIF_PROFILE(entry,flags,val);
    }
    else if(eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_IP) {
        ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT
            entry;
        res = arad_pp_eg_encap_access_ip_tunnel_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        ARAD_PP_EG_ENCAP_GET_FIELD_NEXT_LIF(entry,flags,val);
        ARAD_PP_EG_ENCAP_GET_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_GET_FIELD_OUT_LIF_PROFILE(entry,flags,val);
    }
    else if( eep_type ==  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_TUNNEL ||
        eep_type ==  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_SWAP ||
        eep_type ==  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_POP
      ) 
      {
        ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT  
            entry;
        res = arad_pp_eg_encap_access_mpls_tunnel_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        ARAD_PP_EG_ENCAP_GET_FIELD_NEXT_LIF(entry,flags,val);
        ARAD_PP_EG_ENCAP_GET_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_GET_FIELD_OUT_LIF_PROFILE(entry,flags,val);
    }

    else{
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_next_outlif_update_unsafe()", lif_eep_ndx, 0);
}



uint32 arad_pp_eg_encap_lif_field_set_unsafe(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                                lif_eep_ndx,
      SOC_SAND_IN  uint32                                flags,
      SOC_SAND_IN  uint32                                val
  )
{
    uint32
     res = SOC_SAND_OK;
    ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
      eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_UNSAFE);

    res = arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, lif_eep_ndx, &eep_type);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    
    if(eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC) {
        ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT
            entry;
        res = arad_pp_eg_encap_access_out_ac_entry_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);
        if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF) {
            
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
        }
        ARAD_PP_EG_ENCAP_UPDATE_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_UPDATE_FIELD_OUT_LIF_PROFILE(entry,flags,val);

        res = arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

    }
    else if(eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_TRILL) {
        ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT
            entry;
        res = arad_pp_eg_encap_access_trill_entry_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF) {
            entry.next_outlif = val;
            entry.next_outlif_valid = val != 0;
        }
        ARAD_PP_EG_ENCAP_UPDATE_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_UPDATE_FIELD_OUT_LIF_PROFILE(entry,flags,val);
        
        res = arad_pp_eg_encap_access_trill_entry_format_tbl_set_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else if(eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER) {
        ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT
            entry;
        res = arad_pp_eg_encap_access_link_layer_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF) {
            entry.next_outlif_lsb = val;
            entry.next_outlif_valid = (val != 0xFFFFFFFF);
        }
        ARAD_PP_EG_ENCAP_UPDATE_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_UPDATE_FIELD_OUT_LIF_PROFILE(entry,flags,val);
        
        res = arad_pp_eg_encap_access_link_layer_format_tbl_set_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else if(eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF) {
        ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT
                entry;
        if (SOC_IS_JERICHO_PLUS(unit)) {
            
            if((flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_OUT_LIF_PROFILE) == FALSE &&
                (flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_DROP) == FALSE )
            { 
               SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);  
            }
            res = qax_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            ARAD_PP_EG_ENCAP_UPDATE_FIELD_OUT_LIF_PROFILE(entry,flags,val);
            ARAD_PP_EG_ENCAP_UPDATE_FIELD_DROP(entry,flags,val);
            res = qax_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe(unit,lif_eep_ndx,&entry);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        } else {
            res = arad_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF){
                entry.next_outlif = val;
                entry.next_outlif_valid = val != 0;
            }
            ARAD_PP_EG_ENCAP_UPDATE_FIELD_DROP(entry,flags,val);
            ARAD_PP_EG_ENCAP_UPDATE_FIELD_OUT_LIF_PROFILE(entry,flags,val);

            res = arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe(unit,lif_eep_ndx,&entry);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
    }
    else if(eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA || eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_IPV6) {
        ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT
            entry;
        res = arad_pp_eg_encap_access_data_entry_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF){
            entry.next_outlif = val;
            entry.next_outlif_valid = val != 0;
        }
        ARAD_PP_EG_ENCAP_UPDATE_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_UPDATE_FIELD_OUT_LIF_PROFILE(entry,flags,val);
        
        res = arad_pp_eg_encap_access_data_entry_format_tbl_set_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else if(eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_IP) {
        ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT
            entry;
        res = arad_pp_eg_encap_access_ip_tunnel_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF){
            entry.next_outlif = val;
        }
        ARAD_PP_EG_ENCAP_UPDATE_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_UPDATE_FIELD_OUT_LIF_PROFILE(entry,flags,val);
        
        res = arad_pp_eg_encap_access_ip_tunnel_format_tbl_set_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else if( eep_type ==  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_TUNNEL ||
        eep_type ==  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_SWAP ||
        eep_type ==  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_POP
      ) 
      {
        ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT  
            entry;
        res = arad_pp_eg_encap_access_mpls_tunnel_format_tbl_get_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        if(flags & SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF){
            entry.next_outlif = val;
        }
        ARAD_PP_EG_ENCAP_UPDATE_FIELD_DROP(entry,flags,val);
        ARAD_PP_EG_ENCAP_UPDATE_FIELD_OUT_LIF_PROFILE(entry,flags,val);
        
        res = arad_pp_eg_encap_access_mpls_tunnel_format_tbl_set_unsafe(unit,lif_eep_ndx,&entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

    else{
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_next_outlif_update_unsafe()", lif_eep_ndx, 0);
}


uint32
  arad_pp_eg_encap_lif_field_set_verify(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                                lif_eep_ndx,
      SOC_SAND_IN  uint32                                flags,
      SOC_SAND_IN  uint32                                val
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_next_outlif_update_verify()", lif_eep_ndx, 0);
}


uint32
  arad_pp_eg_encap_data_local_to_global_cud_init_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_INIT_VERIFY);

  if (!(SOC_IS_ARADPLUS(unit)) || 
      ((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_CUD_EXTENSION_MODE) == FALSE)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_UNSUPPORTED, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_data_local_to_global_cud_init_verify()", 0, 0);
}

uint32
  arad_pp_eg_encap_data_local_to_global_cud_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK,
      eedb_mem_index = 0,
      eedb_array_index = 0,
      eedb_data_index = 0,
      in_out_cud;
  int
      dma_able, 
      epni_eedb_bank_buf_size,
      epni_eedb_bank_nof_elements,
      epni_eedb_bank_nof_bit_in_elment;
  uint32 
      flags=0;
  uint32 
      *epni_eedb_bank_buf;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_INIT_UNSAFE);

  epni_eedb_bank_nof_elements = 2048;
  epni_eedb_bank_nof_bit_in_elment = 96;
  epni_eedb_bank_buf_size = (epni_eedb_bank_nof_elements * (epni_eedb_bank_nof_bit_in_elment/32));


  dma_able = soc_mem_dmaable(unit, EPNI_EEDB_BANKm, 0);

  if (dma_able) {
     epni_eedb_bank_buf = (uint32 *)soc_cm_salloc(unit,epni_eedb_bank_buf_size*sizeof(uint32), "EEDB DMA buffer");
  } else {
      epni_eedb_bank_buf = (uint32 *)sal_alloc(epni_eedb_bank_buf_size*sizeof(uint32), "EEDB DMA buffer");
  }
  
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (eedb_array_index = 0; eedb_array_index < (1 << ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_ARRAY_INDEX_NOF_BITS) ; eedb_array_index++) 
  {
      res = soc_sand_os_memset(epni_eedb_bank_buf, 0x0, epni_eedb_bank_buf_size*sizeof(uint32));
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      

      for(eedb_mem_index = 0 ,in_out_cud = 0 ;
          eedb_mem_index < (1 << ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_MEM_INDEX_NOF_BITS) ;
          eedb_mem_index++, in_out_cud = 0) 
      {
          for(eedb_data_index = 0 ;
              eedb_data_index <  (1 << ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_DATA_INDEX_NOF_BITS) ;
              eedb_data_index++) 
          {
              
              
              in_out_cud  = eedb_data_index;
              in_out_cud |= (eedb_mem_index << ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_MEM_INDEX_START);
              in_out_cud |= (eedb_array_index << ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_ARRAY_INDEX_START);
              
              res = soc_sand_bitstream_set_any_field(&in_out_cud ,
                                                    (eedb_mem_index * epni_eedb_bank_nof_bit_in_elment) + (eedb_data_index*ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_EEDB) ,
                                                     ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_EEDB ,
                                                     epni_eedb_bank_buf);
              SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          }
      }
      res = soc_mem_array_write_range(unit, flags, EPNI_EEDB_BANKm, eedb_array_index, MEM_BLOCK_ALL, 0, (epni_eedb_bank_nof_elements - 1),  epni_eedb_bank_buf);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  

exit:
  if (dma_able) {
      soc_cm_sfree(unit, epni_eedb_bank_buf);
  } else {
      sal_free(epni_eedb_bank_buf);
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_data_local_to_global_cud_init_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_encap_data_local_to_global_cud_set_verify(
     SOC_SAND_IN  int                                  unit,
     SOC_SAND_IN  uint32                                  in_cud,
     SOC_SAND_OUT uint32                                  out_cud
   )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET_VERIFY);
    if (!(SOC_IS_ARADPLUS(unit)) || 
        ((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_CUD_EXTENSION_MODE) == FALSE)) {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_UNSUPPORTED, 5, exit);
    }
    SOC_SAND_ERR_IF_ABOVE_MAX(in_cud , (1 << ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_IN_CUD_NOF_BITS) - 1, ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(out_cud ,(1 << ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_OUT_CUD_NOF_BITS) - 1, ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_OUT_OF_RANGE_ERR, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_data_local_to_global_cud_set_verify()", in_cud, 0);
}


uint32
  arad_pp_eg_encap_data_local_to_global_cud_set_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  in_cud,
    SOC_SAND_IN  uint32                                  out_cud
  )
{
    uint32 
        res = SOC_SAND_OK,
        eedb_mem_index = 0,
        eedb_array_index = 0,
        eedb_data_index = 0,
        eedb_data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE],
        dscp_data_index = 0,
        dscp_mem_index = 0,
        dscp_out_cud = 0;
    ARAD_PP_EPNI_DSCP_REMARK_TBL_DATA
        dscp_tbl_data;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET_UNSAFE);
    
    
    res = soc_sand_bitstream_get_any_field(&in_cud , 
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_MEM_INDEX_START, 
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_MEM_INDEX_NOF_BITS, 
                                           &eedb_mem_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    res = soc_sand_bitstream_get_any_field(&in_cud, 
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_ARRAY_INDEX_START, 
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_ARRAY_INDEX_NOF_BITS, 
                                           &eedb_array_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    
    res = soc_sand_bitstream_get_any_field(&in_cud,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_DATA_INDEX_START,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_DATA_INDEX_NOF_BITS,
                                           &eedb_data_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
    

    res = READ_EDB_EEDB_BANKm(unit, eedb_array_index, MEM_BLOCK_ANY, eedb_mem_index, eedb_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    
    res = soc_sand_bitstream_set_any_field(&out_cud , (eedb_data_index*ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_EEDB) , ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_EEDB , eedb_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    res = WRITE_EDB_EEDB_BANKm(unit, eedb_array_index, MEM_BLOCK_ANY, eedb_mem_index, eedb_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);


    
    res = soc_sand_bitstream_get_any_field(&in_cud,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_MEM_INDEX_START,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_MEM_INDEX_NOF_BITS,
                                           &dscp_mem_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    
    res = soc_sand_bitstream_get_any_field(&in_cud,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_DATA_INDEX_START,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_DATA_INDEX_NOF_BITS,
                                           &dscp_data_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);


    res = arad_pp_epni_dscp_remark_tbl_get_unsafe(unit,dscp_mem_index,&dscp_tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    
    res = soc_sand_bitstream_get_any_field(&out_cud , ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_EEDB , ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_DSCP , &dscp_out_cud);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    res = soc_sand_bitstream_set_any_field(&dscp_out_cud , (dscp_data_index*ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_DSCP), ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_DSCP, &(dscp_tbl_data.dscp_remark_data));
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
    
    res =  arad_pp_epni_dscp_remark_tbl_set_unsafe(unit,dscp_mem_index,&dscp_tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_data_local_to_global_cud_set_unsafe()", in_cud, out_cud);

}


uint32
  arad_pp_eg_encap_data_local_to_global_cud_get_verify(
     SOC_SAND_IN  int                                  unit,
     SOC_SAND_IN  uint32                                  in_cud,
     SOC_SAND_OUT uint32*                                 out_cud
   )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET_VERIFY);
    if (!(SOC_IS_ARADPLUS(unit)) || 
        ((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_CUD_EXTENSION_MODE) == FALSE)) {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_UNSUPPORTED, 5, exit);
    }
    SOC_SAND_ERR_IF_ABOVE_MAX(in_cud , (1 << ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_IN_CUD_NOF_BITS) - 1 , ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_OUT_OF_RANGE_ERR, 20, exit);

exit:    
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_data_local_to_global_cud_get_verify()", in_cud, 0);
}

uint32
  arad_pp_eg_encap_data_local_to_global_cud_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  in_cud,
    SOC_SAND_OUT uint32*                                 out_cud
  )
{
    uint32 
        res = SOC_SAND_OK,
        eedb_mem_index = 0,
        eedb_array_index = 0,
        eedb_data_index = 0,
        eedb_data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE],
        dscp_data_index = 0,
        dscp_mem_index = 0,
        dscp_out_cud = 0,
        out_cud_lcl[1];
    ARAD_PP_EPNI_DSCP_REMARK_TBL_DATA
        dscp_tbl_data;        

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET_UNSAFE);
    
    *out_cud_lcl = 0x0;
    
    res = soc_sand_bitstream_get_any_field(&in_cud,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_MEM_INDEX_START,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_MEM_INDEX_NOF_BITS,
                                           &eedb_mem_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    res = soc_sand_bitstream_get_any_field(&in_cud,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_ARRAY_INDEX_START,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_ARRAY_INDEX_NOF_BITS,
                                           &eedb_array_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    
    res = soc_sand_bitstream_get_any_field(&in_cud,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_DATA_INDEX_START, 
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_DATA_INDEX_NOF_BITS, 
                                           &eedb_data_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

    res = READ_EDB_EEDB_BANKm(unit, eedb_array_index, MEM_BLOCK_ANY, eedb_mem_index, eedb_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    
    res = soc_sand_bitstream_get_any_field(eedb_data, 
                                           (eedb_data_index * ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_EEDB), 
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_EEDB,
                                           out_cud_lcl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    res = soc_sand_bitstream_get_any_field(&in_cud,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_MEM_INDEX_START, 
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_MEM_INDEX_NOF_BITS,
                                           &dscp_mem_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    
    res = soc_sand_bitstream_get_any_field(&in_cud,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_DATA_INDEX_START,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_DATA_INDEX_NOF_BITS,
                                           &dscp_data_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    res = arad_pp_epni_dscp_remark_tbl_get_unsafe(unit,dscp_mem_index,&dscp_tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    
    res = soc_sand_bitstream_get_any_field(&(dscp_tbl_data.dscp_remark_data) , (dscp_data_index*ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_DSCP), ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_DSCP, &dscp_out_cud);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    res = soc_sand_bitstream_set_any_field(&dscp_out_cud,
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_EEDB, 
                                           ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_DSCP, 
                                           out_cud_lcl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
    *out_cud = *out_cud_lcl;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_data_local_to_global_cud_get_unsafe()", in_cud, 0);

}


uint32
  arad_pp_eg_encap_data_lif_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO              *data_info,
    SOC_SAND_IN  uint8                                   next_eep_valid,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK,
    ind;
  ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT
    tbl_data;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_DATA_LIF_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data_info);

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

  for (ind = 0; ind < SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE; ind++) {
    tbl_data.data[ind] = data_info->data_entry[ind];
  }
  
  tbl_data.next_outlif = next_eep;
  tbl_data.next_outlif_valid = SOC_SAND_BOOL2NUM(next_eep_valid);
  tbl_data.oam_lif_set = SOC_SAND_BOOL2NUM(data_info->oam_lif_set);
  tbl_data.drop = SOC_SAND_BOOL2NUM(data_info->drop);
  tbl_data.outlif_profile = data_info->outlif_profile;
  tbl_data.rsv_bits= data_info->rsv_bits;

  res = arad_pp_eg_encap_access_data_entry_format_tbl_set_unsafe(
          unit,
          lif_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_data_lif_entry_add_unsafe()", lif_eep_ndx, 0);
}

uint32
  arad_pp_eg_encap_data_lif_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO              *data_info,
    SOC_SAND_IN  uint8                                   next_eep_valid,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_DATA_LIF_ENTRY_ADD_VERIFY);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, lif_eep_ndx, ARAD_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_ENCAP_DATA_INFO, data_info, 20, exit);
  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, next_eep, ARAD_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_data_lif_entry_add_verify()", lif_eep_ndx, next_eep);
}


uint32
  arad_pp_eg_encap_swap_command_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(swap_info);

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

  tbl_data.mpls1_command = ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val;
  tbl_data.mpls1_label = swap_info->swap_label;
  tbl_data.next_outlif = next_eep;
  tbl_data.next_vsi_lsb = swap_info->out_vsi;
  tbl_data.oam_lif_set = SOC_SAND_BOOL2NUM(swap_info->oam_lif_set);
  tbl_data.drop = SOC_SAND_BOOL2NUM(swap_info->drop);
  tbl_data.mpls_swap_remark_profile = swap_info->remark_profile;
  tbl_data.outlif_profile = swap_info->outlif_profile;

  res = arad_pp_eg_encap_access_mpls_tunnel_format_tbl_set_unsafe(
          unit,
          lif_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_swap_command_entry_add_unsafe()", lif_eep_ndx, 0);
}

uint32
  arad_pp_eg_encap_swap_command_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_VERIFY);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, lif_eep_ndx, ARAD_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_ENCAP_SWAP_INFO, swap_info, 20, exit);
  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, next_eep, ARAD_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_swap_command_entry_add_verify()", lif_eep_ndx, 0);
}


uint32
  arad_pp_eg_encap_pwe_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT
    tbl_data;
  uint8 is_full_entry_extension = TRUE;

  SOC_PPC_EG_ENCAP_ENTRY_INFO               encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
  uint32 next_eep_dummy[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
  uint32 nof_entries;
  uint32 default_label_value = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PWE_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

  
  default_label_value = SOC_IS_JERICHO_B0_AND_ABOVE(unit) ? soc_property_get(unit, spn_MPLS_ENCAP_INVALID_VALUE, 0) : 0;

  
  res = arad_pp_eg_encap_entry_get_unsafe(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, lif_eep_ndx,
                                          2, encap_entry_info, next_eep_dummy, &nof_entries);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if (!soc_property_get(unit, spn_MPLS_BIND_PWE_WITH_MPLS_ONE_CALL, 0)) {
      if ((nof_entries >= 1) && (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP) &&
          encap_entry_info[0].entry_val.mpls_encap_info.nof_tunnels > 1) {

          tbl_data.mpls2_label = encap_entry_info[0].entry_val.mpls_encap_info.tunnels[1].tunnel_label;
          tbl_data.mpls2_command = encap_entry_info[0].entry_val.mpls_encap_info.tunnels[1].push_profile;
      } else if (SOC_IS_JERICHO_B0_AND_ABOVE(unit)) {
          
          tbl_data.mpls2_label = default_label_value;
      }
  } else { 
      if (pwe_info->egress_tunnel_label_info.nof_tunnels == 1) { 
          tbl_data.mpls2_label = pwe_info->egress_tunnel_label_info.tunnels[0].tunnel_label;
          tbl_data.mpls2_command = SOC_PPC_MPLS_COMMAND_TYPE_PUSH + pwe_info->egress_tunnel_label_info.tunnels[0].push_profile;
      } else {
          tbl_data.mpls2_label = default_label_value;
          tbl_data.mpls2_command = 0;
      }
  }



 
  tbl_data.mpls1_command = ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_val + pwe_info->push_profile;
  tbl_data.mpls1_label = pwe_info->label;
  tbl_data.next_outlif = next_eep;
  tbl_data.next_vsi_lsb = 0;
  tbl_data.oam_lif_set = SOC_SAND_BOOL2NUM(pwe_info->oam_lif_set);
  tbl_data.drop = SOC_SAND_BOOL2NUM(pwe_info->drop);
  tbl_data.outlif_profile = pwe_info->outlif_profile;
  
  res = arad_pp_eg_encap_access_mpls_tunnel_format_tbl_set_unsafe(
          unit,
          lif_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if (SOC_IS_JERICHO(unit)) {
      res = arad_pp_eg_encap_protection_info_set_unsafe(
                unit, lif_eep_ndx, is_full_entry_extension, &(pwe_info->protection_info));
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_pwe_entry_add_unsafe()", lif_eep_ndx, 0);
}

uint32
  arad_pp_eg_encap_pwe_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PWE_ENTRY_ADD_VERIFY);
  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, lif_eep_ndx, ARAD_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_PWE_INFO, unit, pwe_info, 20, exit);
  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, next_eep, ARAD_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_pwe_entry_add_verify()", lif_eep_ndx, next_eep);
}


uint32
  arad_pp_eg_encap_pop_command_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pop_info);

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

  tbl_data.model_is_pipe = pop_info->model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE ? 0x1 : 0x0;
  tbl_data.upper_layer_protocol = pop_info->pop_next_header;
  tbl_data.mpls1_command = ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val;
  tbl_data.next_outlif = next_eep;
  tbl_data.next_vsi_lsb = pop_info->out_vsi;
  tbl_data.oam_lif_set = SOC_SAND_BOOL2NUM(pop_info->oam_lif_set);
  tbl_data.drop = SOC_SAND_BOOL2NUM(pop_info->drop);

  tbl_data.has_cw = SOC_SAND_BOOL2NUM(pop_info->ethernet_info.has_cw);
  tbl_data.tpid_profile = pop_info->ethernet_info.tpid_profile;
  
  res = arad_pp_eg_encap_access_mpls_tunnel_format_tbl_set_unsafe(
          unit,
          lif_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_pop_command_entry_add_unsafe()", lif_eep_ndx, next_eep);
}

uint32
  arad_pp_eg_encap_pop_command_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_VERIFY);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, lif_eep_ndx, ARAD_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_POP_INFO, unit, pop_info, 20, exit);
  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, next_eep, ARAD_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR, 30, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_pop_command_entry_add_verify()", lif_eep_ndx, 0);
}


uint32
  arad_pp_eg_encap_vsi_entry_add_verify(
    SOC_SAND_IN int                              unit, 
    SOC_SAND_IN uint32                               lif_eep_ndx, 
    SOC_SAND_IN SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO      *vsi_info, 
    SOC_SAND_IN uint8                              next_eep_valid,
    SOC_SAND_IN uint32                               next_eep 
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_VSI_ENTRY_ADD_VERIFY);

  res = SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_verify(unit, vsi_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, lif_eep_ndx, ARAD_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR, 30, exit);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, next_eep, ARAD_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR, 40, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_vsi_entry_add_verify()", lif_eep_ndx, 0);
}


uint32
  arad_pp_eg_encap_vsi_entry_add_unsafe(
    SOC_SAND_IN int                              unit, 
    SOC_SAND_IN uint32                               lif_eep_ndx, 
    SOC_SAND_IN SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO          *vsi_info, 
    SOC_SAND_IN uint8                              next_eep_valid,                          
    SOC_SAND_IN uint32                               next_eep 
  )
{ 
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT
    tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_VSI_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));


  tbl_data.next_outlif_valid  = SOC_SAND_BOOL2NUM(next_eep_valid);
  tbl_data.next_outlif = (tbl_data.next_outlif_valid) ? next_eep:0;
  tbl_data.next_vsi_lsb = vsi_info->out_vsi;
  tbl_data.remark_profile = vsi_info->remark_profile;
  tbl_data.oam_lif_set = SOC_SAND_BOOL2NUM(vsi_info->oam_lif_set);
  tbl_data.drop = SOC_SAND_BOOL2NUM(vsi_info->drop);
  if (SOC_IS_JERICHO(unit)) {
      tbl_data.outlif_profile = vsi_info->outlif_profile;
  }

  if (SOC_IS_JERICHO_PLUS(unit)) {
      tbl_data.outrif_profile_index = vsi_info->outrif_profile_index;
      res = qax_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe(
              unit,
              lif_eep_ndx,
              &tbl_data
            );
  } else {
      res = arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe(
              unit,
              lif_eep_ndx,
              &tbl_data
            );
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_vsi_entry_add_unsafe()", lif_eep_ndx, 0);
}


uint32
  arad_pp_eg_encap_mpls_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  )
{
  uint32
    res = SOC_SAND_OK, default_label_value = 0;
  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT
    ee_entry_tbl_data;
  uint8 is_full_entry_extension = TRUE;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_MPLS_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mpls_encap_info);

  soc_sand_os_memset(&ee_entry_tbl_data, 0x0, sizeof(ee_entry_tbl_data));

  
  default_label_value = SOC_IS_JERICHO_B0_AND_ABOVE(unit) ? soc_property_get(unit, spn_MPLS_ENCAP_INVALID_VALUE, 0) : 0;

  ee_entry_tbl_data.next_vsi_lsb = mpls_encap_info->out_vsi;
  ee_entry_tbl_data.next_outlif = ll_eep;
  ee_entry_tbl_data.oam_lif_set = SOC_SAND_BOOL2NUM(mpls_encap_info->oam_lif_set);
  ee_entry_tbl_data.drop = SOC_SAND_BOOL2NUM(mpls_encap_info->drop);
  ee_entry_tbl_data.outlif_profile = mpls_encap_info->outlif_profile;

  if (mpls_encap_info->nof_tunnels > 0)
  {
    
    ee_entry_tbl_data.mpls1_command = ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_val + mpls_encap_info->tunnels[0].push_profile; 
    ee_entry_tbl_data.mpls1_label = mpls_encap_info->tunnels[0].tunnel_label;
  }
  else
  {
    ee_entry_tbl_data.mpls1_command = 0;
    ee_entry_tbl_data.mpls1_label = 0;
  }

  if (mpls_encap_info->nof_tunnels > 1)
  {
    
    ee_entry_tbl_data.mpls2_command = SOC_PPC_MPLS_COMMAND_TYPE_PUSH + mpls_encap_info->tunnels[1].push_profile;
    ee_entry_tbl_data.mpls2_label = mpls_encap_info->tunnels[1].tunnel_label;
  }
  else 
  {
    ee_entry_tbl_data.mpls2_command = 0;
    ee_entry_tbl_data.mpls2_label = default_label_value;
  }

  res = arad_pp_eg_encap_access_mpls_tunnel_format_tbl_set_unsafe(
          unit,
          tunnel_eep_ndx,
          &ee_entry_tbl_data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if (SOC_IS_JERICHO(unit)) {
      res = arad_pp_eg_encap_protection_info_set_unsafe(
                unit, tunnel_eep_ndx, is_full_entry_extension, &(mpls_encap_info->protection_info));
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mpls_entry_add_unsafe()", tunnel_eep_ndx, ll_eep);
}

uint32
  arad_pp_eg_encap_mpls_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_MPLS_ENTRY_ADD_VERIFY);

  ARAD_PP_EG_ENCAP_NDX_OF_TYPE_VERIFY(tunnel_eep_ndx);
  
  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, tunnel_eep_ndx, ARAD_PP_EG_ENCAP_IP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO, unit, mpls_encap_info, 20, exit);
  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, ll_eep, ARAD_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR, 30, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mpls_entry_add_verify()", tunnel_eep_ndx, 0);
}




uint32
  arad_pp_eg_encap_ipv4_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ipv4_encap_info);

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

  tbl_data.next_outlif = ll_eep;
  tbl_data.next_vsi_lsb = ipv4_encap_info->out_vsi;
  if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && (SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE == 1)) {
      tbl_data.encapsulation_mode = ipv4_encap_info->dest.encapsulation_mode_index; 
  } else {
      SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_TO_SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD(unit, ipv4_encap_info->dest.encapsulation_mode, &(tbl_data.encapsulation_mode));
  }
  tbl_data.ipv4_tos_index = ipv4_encap_info->dest.tos_index;
  tbl_data.ipv4_ttl_index = ipv4_encap_info->dest.ttl_index;
  tbl_data.ipv4_src_index = ipv4_encap_info->dest.src_index;
  tbl_data.ipv4_dst = ipv4_encap_info->dest.dest;
  tbl_data.oam_lif_set = SOC_SAND_BOOL2NUM(ipv4_encap_info->oam_lif_set);
  tbl_data.drop = SOC_SAND_BOOL2NUM(ipv4_encap_info->drop);
  tbl_data.outlif_profile = ipv4_encap_info->outlif_profile;

  res = arad_pp_eg_encap_access_ip_tunnel_format_tbl_set_unsafe(
          unit,
          tunnel_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_entry_add_unsafe()", tunnel_eep_ndx, 0);
}

uint32
  arad_pp_eg_encap_ipv4_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_ENTRY_ADD_VERIFY);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, tunnel_eep_ndx, ARAD_PP_EG_ENCAP_IP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR, 5, exit);

  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO, unit, ipv4_encap_info, 10, exit);

  ARAD_PP_EG_ENCAP_NDX_OF_TYPE_VERIFY(tunnel_eep_ndx);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, ll_eep, ARAD_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_entry_add_verify()", tunnel_eep_ndx, 0);
}


STATIC uint32
  arad_pp_eg_encap_ipv6_sip_entry_to_prge_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO       *ipv6_encap_info,
    SOC_SAND_OUT  soc_reg_above_64_val_t                 prge_data_entry
  )
{
    switch (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode) 
    {
    case SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LARGE:
    case SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL:
        
        SHR_BITCOPY_RANGE(prge_data_entry,0,ipv6_encap_info->tunnel.src.address,0,128);
        break;
    default:
        
        SHR_BITCOPY_RANGE(prge_data_entry,0,ipv6_encap_info->tunnel.src.address,32,96);

        
        prge_data_entry[96/32] = ipv6_encap_info->tunnel.hop_limit << (96% 32);

        if (SOC_DPP_CONFIG(unit)->pp.lawful_interception_enable) {
            
            prge_data_entry[104/32] |= ipv6_encap_info->tunnel.traffic_class << (104 % 32);
        }
        else {
            
            prge_data_entry[104/32] |= ipv6_encap_info->tunnel.next_header << (104 % 32);
        }

        
        prge_data_entry[112/32] |= ipv6_encap_info->tunnel.flow_label << (112 % 32);
    }

    return SOC_SAND_OK;
}

STATIC uint32
  arad_pp_eg_encap_ipv6_sip_entry_from_prge_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO       *ipv6_encap_info,
    SOC_SAND_IN  soc_reg_above_64_val_t                 prge_data_entry
  )
{
    switch (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode) 
    {
    case SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LARGE:
    case SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL:
        
        SHR_BITCOPY_RANGE(ipv6_encap_info->tunnel.src.address,0,prge_data_entry,0,128);
        break;
    default:    
        
        SHR_BITCOPY_RANGE(ipv6_encap_info->tunnel.src.address,32,prge_data_entry,0,96);

        
        ipv6_encap_info->tunnel.hop_limit = (prge_data_entry[96/32] >> (96 % 32)) & 0xff;

        if (SOC_DPP_CONFIG(unit)->pp.lawful_interception_enable) {
            
            ipv6_encap_info->tunnel.traffic_class = (prge_data_entry[104/32] >> (104 % 32)) & 0xff;
        }
        else {
            
            ipv6_encap_info->tunnel.next_header = (prge_data_entry[104/32] >> (104 % 32)) & 0xff;
        }

        
        ipv6_encap_info->tunnel.flow_label = (prge_data_entry[112/32] >> (112 % 32)) & 0xffff;
        break;
    }

    return SOC_SAND_OK;
}

STATIC uint32
  arad_pp_eg_encap_ipv6_entry_to_data_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 sip_index,
    SOC_SAND_IN  uint32                                 dip_index,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO       *ipv6_encap_info,
    SOC_SAND_OUT  SOC_PPC_EG_ENCAP_DATA_INFO            *data_info
  )
{  
    uint32 res = SOC_SAND_OK;    
    uint32 entry_lsb = ARAD_PRGE_DATA_ENTRY_LSBS_IPV6_TUNNEL;    
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) {
        
        SHR_BITCOPY_RANGE(data_info->data_entry,0,ipv6_encap_info->tunnel.dest.address,0,56);
        data_info->oam_lif_set = 0;
        data_info->drop = 0;        
        res = arad_pp_occ_mgmt_app_set(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_IPV6_TUNNEL,1, &(data_info->outlif_profile));
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    } else if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LEGACY) {
        
        
        SHR_BITCOPY_RANGE(data_info->data_entry,0,&entry_lsb,0,8);

        
        SHR_BITCOPY_RANGE(data_info->data_entry,8,&dip_index,0,8);

        
        SHR_BITCOPY_RANGE(data_info->data_entry,16,&sip_index,0,8);

        
        SHR_BITCOPY_RANGE(data_info->data_entry,24,ipv6_encap_info->tunnel.src.address,0,32);

        data_info->oam_lif_set = 0;
        data_info->drop = 0;
    }
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv6_entry_to_data_buffer()", 0, 0);
}

STATIC uint32
  arad_pp_eg_encap_ipv6_entry_from_data_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  uint32                                 *sip_index,
    SOC_SAND_OUT  uint32                                 *dip_index,
    SOC_SAND_OUT  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO       *ipv6_encap_info,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO               *data_info
  )
{
    uint32 dip_index_lcl[1] = {0};
    uint32 sip_index_lcl[1] = {0};

    if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) {
        
        SHR_BITCOPY_RANGE(ipv6_encap_info->tunnel.dest.address, 0, data_info->data_entry,0,56);
    } else if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LEGACY) { 
            
        
        SHR_BITCOPY_RANGE(dip_index_lcl,0,data_info->data_entry,8,8);
        *dip_index = dip_index_lcl[0];

        
        SHR_BITCOPY_RANGE(sip_index_lcl, 0, data_info->data_entry,16,8);
        *sip_index = sip_index_lcl[0];

        
        SHR_BITCOPY_RANGE(ipv6_encap_info->tunnel.src.address, 0, data_info->data_entry,24,32);
    }

    return SOC_SAND_OK;
}


STATIC uint32
  arad_pp_eg_encap_mirror_erspan_data_to_prge_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO    *mirror_encap_info,
    SOC_SAND_OUT soc_reg_above_64_val_t                 prge_data_entry
  )
{
    soc_reg_above_64_val_t erspan_hdr;
    uint32 eth_hdr_size;

    soc_sand_os_memset(prge_data_entry, 0x0, sizeof(soc_reg_above_64_val_t));

    SOC_REG_ABOVE_64_CLEAR(erspan_hdr);
    
    
    ARAD_PP_EG_ENCAP_PRGE_MEM_ERSPAN_FORMAT_SET(unit, SOC_PPC_EG_ENCAP_ERSPAN_PRIO_VAL, SOC_PPC_EG_ENCAP_ERSPAN_TRUNC_VAL, mirror_encap_info->tunnel.erspan_id, erspan_hdr);

    
    if(0 == mirror_encap_info->tunnel.tpid) 
    {
        eth_hdr_size = 12; 
    }
    else 
    {
        eth_hdr_size = 16; 
    }

    

    
    SHR_BITCOPY_RANGE(prge_data_entry, 0, erspan_hdr, 0, 64);

    
    prge_data_entry[64/32] |= eth_hdr_size << (64 % 32);

    
    SHR_BITCLR_RANGE(prge_data_entry, 72, 40);

    
    prge_data_entry[112/32] |= ((uint32)mirror_encap_info->tunnel.ether_type) << (112 % 32);


   return SOC_SAND_OK;
}


STATIC uint32
  arad_pp_eg_encap_mirror_erspan_entry_from_prge_buffer(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  soc_reg_above_64_val_t                prge_data_entry,
    SOC_SAND_OUT uint8                                 *eth_hdr_size,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO    *mirror_encap_info
  )
{
    soc_reg_above_64_val_t erspan_hdr;

    SOC_REG_ABOVE_64_CLEAR(erspan_hdr);

    

    
    SHR_BITCOPY_RANGE(erspan_hdr, 0, prge_data_entry, 0, 64);
    ARAD_PP_EG_ENCAP_PRGE_MEM_ERSPAN_FORMAT_SPAN_ID_GET(mirror_encap_info->tunnel.erspan_id, erspan_hdr);

    
    *eth_hdr_size = (prge_data_entry[64/32] >> (64 % 32)) & 0xff;

    

    
    mirror_encap_info->tunnel.ether_type = (prge_data_entry[112/32] >> (112 % 32)) & 0xffff;

    return SOC_SAND_OK;
}


STATIC uint32
  arad_pp_eg_encap_mirror_l2_data_to_prge_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO    *mirror_encap_info,
    SOC_SAND_OUT soc_reg_above_64_val_t                 prge_data_entry
  )
{
    uint32 offset = 0;
    uint32 res = SOC_SAND_OK;
    uint32 mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S] = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_os_memset(prge_data_entry, 0x0, sizeof(soc_reg_above_64_val_t));

    
 
    
    if(0 != mirror_encap_info->tunnel.tpid)
    {
        
        prge_data_entry[ 0/32] |= ((uint32)mirror_encap_info->tunnel.vid)  << (0 % 32);

        
        prge_data_entry[12/32] |= 0 << (12 % 32);

        
        prge_data_entry[13/32] |= ((uint32)mirror_encap_info->tunnel.pcp)  << (13 % 32);

        
        prge_data_entry[16/32] |= ((uint32)mirror_encap_info->tunnel.tpid) << (16 % 32);

        offset = 32;
    }

    
       
    res = soc_sand_pp_mac_address_struct_to_long(&(mirror_encap_info->tunnel.src), mac_in_longs);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    SHR_BITCOPY_RANGE(prge_data_entry, offset, mac_in_longs, 0, 48);

    offset += 48;

    
       
    res = soc_sand_pp_mac_address_struct_to_long(&(mirror_encap_info->tunnel.dest), mac_in_longs);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    SHR_BITCOPY_RANGE(prge_data_entry, offset, mac_in_longs, 0, 48);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mirror_l2_data_to_prge_buffer()", 0, 0);
}



STATIC uint32
  arad_pp_eg_encap_mirror_l2_entry_from_prge_buffer(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  soc_reg_above_64_val_t                prge_data_entry,
    SOC_SAND_IN  uint8                                 eth_hdr_size,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO    *mirror_encap_info
  )
{
    uint32 offset = 0;
    uint32 res = SOC_SAND_OK;
    uint32 mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S] = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (eth_hdr_size == 12) {
        mirror_encap_info->tunnel.vid = 0;
        mirror_encap_info->tunnel.pcp = 0;
        mirror_encap_info->tunnel.tpid = 0;
    } else {
        
        mirror_encap_info->tunnel.vid  = (prge_data_entry[ 0/32] >> ( 0 % 32)) & 0xfff;

        

        
        mirror_encap_info->tunnel.pcp  = (prge_data_entry[13/32] >> (13 % 32)) & 0x7;

        
        mirror_encap_info->tunnel.tpid = (prge_data_entry[16/32] >> (16 % 32)) & 0xffff;

        offset = 32;
    }

    
    SHR_BITCOPY_RANGE(mac_in_longs, 0, prge_data_entry, offset, 48);

    offset += 48;
    
       
    res = soc_sand_pp_mac_address_long_to_struct(mac_in_longs, &(mirror_encap_info->tunnel.src));
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    SHR_BITCOPY_RANGE(mac_in_longs, 0, prge_data_entry, offset, 48);

    
       
    res = soc_sand_pp_mac_address_long_to_struct(mac_in_longs, &(mirror_encap_info->tunnel.dest));
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mirror_l2_entry_from_prge_buffer()", 0, 0);
 }


STATIC uint32
arad_pp_eg_encap_overlay_arp_data_to_prge_buffer(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *overlay_arp_info, 
    SOC_SAND_OUT soc_reg_above_64_val_t                 prge_data_entry
   ) 
{
    uint32 size_of_header_without_da;  
    uint32 is_ethernet_1_tag;

    uint32 start_index;
    uint32 field_length;
    uint32 field_value;

    uint32 field_mac[2];


    
    soc_sand_os_memset(prge_data_entry, 0x0, sizeof(soc_reg_above_64_val_t));

    start_index = 0;

    
    is_ethernet_1_tag = (overlay_arp_info->outer_tpid != 0);

    
    if (is_ethernet_1_tag) {
        size_of_header_without_da = 12; 
    } else {
        size_of_header_without_da = 8; 
    }
    field_length = 4;
    field_value = size_of_header_without_da;
    SHR_BITCOPY_RANGE(prge_data_entry, start_index, &field_value, 0, field_length);

    start_index += field_length;

    
    field_length = 16;
    field_value = overlay_arp_info->ether_type;
    SHR_BITCOPY_RANGE(prge_data_entry, start_index, &field_value, 0, field_length);

    start_index += field_length;

    if (is_ethernet_1_tag) {
        
        field_length = 12;
        field_value = overlay_arp_info->out_vid;
        SHR_BITCOPY_RANGE(prge_data_entry, start_index, &field_value, 0, field_length);

        start_index += field_length;

        
        field_length = 4;
        field_value = overlay_arp_info->pcp_dei;
        SHR_BITCOPY_RANGE(prge_data_entry, start_index, &field_value, 0, field_length);

        start_index += field_length;

        
        field_length = 16;
        field_value = overlay_arp_info->outer_tpid;
        SHR_BITCOPY_RANGE(prge_data_entry, start_index, &field_value, 0, field_length);

        start_index += field_length;
    }

    
    field_length = 48;


    soc_sand_pp_mac_address_struct_to_long(&overlay_arp_info->src_mac, field_mac);


    SHR_BITCOPY_RANGE(prge_data_entry, start_index, field_mac, 0, field_length);

    return SOC_SAND_OK;
}



STATIC uint32
arad_pp_eg_encap_overlay_arp_data_from_prge_buffer(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *overlay_arp_info, 
    SOC_SAND_IN soc_reg_above_64_val_t                 prge_data_entry
   ) 
{
    uint32 field_mac[2];

    uint32 size_of_header_without_da = 0;  
    uint32 is_ethernet_1_tag = 0;

    uint32 start_index = 0;
    uint32 field_length = 0;
    uint32 field_value = 0; 
    

    start_index = 0;

    
    field_length = 4;
    SHR_BITCOPY_RANGE(&size_of_header_without_da, 0, prge_data_entry, start_index, field_length);

    start_index += field_length;

    
    is_ethernet_1_tag = (size_of_header_without_da == 12); 

    
    field_length = 16;
    SHR_BITCOPY_RANGE(&field_value, 0, prge_data_entry, start_index, field_length);
    overlay_arp_info->ether_type = (uint16) field_value; 
    start_index += field_length;


    if (is_ethernet_1_tag) {
        
        field_length = 12;
        SHR_BITCOPY_RANGE(&overlay_arp_info->out_vid, 0, prge_data_entry, start_index, field_length);

        start_index += field_length;

        
        field_value = 0;
        field_length = 4;
        SHR_BITCOPY_RANGE(&field_value, 0, prge_data_entry, start_index, field_length);
        overlay_arp_info->pcp_dei = (uint8) field_value; 

        start_index += field_length;

        
        field_value = 0;
        field_length = 16;
        SHR_BITCOPY_RANGE(&field_value, 0, prge_data_entry, start_index, field_length);
        overlay_arp_info->outer_tpid = (uint16) field_value;

        start_index += field_length;
    }

    
    field_length = 48;
    SHR_BITCOPY_RANGE(field_mac, 0, prge_data_entry, start_index, field_length);
    soc_sand_pp_mac_address_long_to_struct(field_mac, &overlay_arp_info->src_mac);

    return SOC_SAND_OK;
}




 
STATIC uint32
arad_pp_eg_encap_overlay_arp_data_to_data_entry_buffer(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *overlay_arp_info, 
    SOC_SAND_OUT  SOC_PPC_EG_ENCAP_DATA_INFO            *data_info
    ) 
{
    uint32 field_value; 
    uint32 field_mac[2];

    
    field_value = ARAD_PRGE_DATA_ENTRY_LSBS_ROO_VXLAN;
    SHR_BITCOPY_RANGE(data_info->data_entry, 0, &field_value, 0, 2);
    

    
    field_value = overlay_arp_info->ll_vsi_pointer;
    SHR_BITCOPY_RANGE(data_info->data_entry, 4, &field_value, 0, 4);

    
    soc_sand_pp_mac_address_struct_to_long(&overlay_arp_info->dest_mac, field_mac);

    SHR_BITCOPY_RANGE(data_info->data_entry, 8, field_mac, 0, 48);

    return SOC_SAND_OK;

}

 
static uint32 
arad_pp_eg_encap_overlay_arp_data_from_data_entry_buffer(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *overlay_arp_info, 
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO              *data_info
   ) 
{
    uint32 field_mac[2];
    uint32 ll_vsi_pointer_lcl[1] = {0};

    
    SHR_BITCOPY_RANGE(ll_vsi_pointer_lcl, 0, data_info->data_entry, 4, 4);
    overlay_arp_info->ll_vsi_pointer = (uint8) ll_vsi_pointer_lcl[0];

    
    SHR_BITCOPY_RANGE(field_mac, 0, data_info->data_entry, 8, 48);
    soc_sand_pp_mac_address_long_to_struct(field_mac, &overlay_arp_info->dest_mac);

    return SOC_SAND_OK;
}




STATIC SOC_PPC_EG_ENCAP_ENTRY_TYPE
  arad_pp_eg_encap_data_entry_exact_type_get_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO          *tbl_data
  )
{
    uint32 res = SOC_SAND_OK;    
    uint32 outlif_profile;
    uint32 outlif_profile_value;
    int data_entry_lsbs = tbl_data->data_entry[0] & 0x3; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
    outlif_profile = tbl_data->outlif_profile;
    res = arad_pp_occ_mgmt_app_get(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_IPV6_TUNNEL, &outlif_profile, &outlif_profile_value);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) {
       if (outlif_profile_value == 1) {
          return SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP;
        } else {
          return SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA;
        }
    } else {
        switch (data_entry_lsbs) 
        {
        case ARAD_PRGE_DATA_ENTRY_LSBS_IPV6_TUNNEL:
            return SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP;
            break;
        case ARAD_PRGE_DATA_ENTRY_LSBS_ROO_VXLAN: 
            return SOC_PPC_EG_ENCAP_ENTRY_TYPE_OVERLAY_ARP_ENCAP; 
            break;
        default: 
            return SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA;
            break;
        }
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_data_entry_exact_type_get_unsafe()", 0, 0);

}


  

STATIC uint32
  arad_pp_eg_encap_data_entry_data_exact_info_get_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO                  *data_info,
    SOC_SAND_OUT  SOC_PPC_EG_ENCAP_ENTRY_INFO                *exact_data_info
  )
{
    uint32
      res = SOC_SAND_OK;
    SOC_PPC_EG_ENCAP_ENTRY_TYPE encap_entry_type; 

    uint32 sip_index=0;
    uint32 dip_index=0;
    ARAD_PP_EPNI_PRGE_DATA_TBL_DATA prge_data_sip, prge_data_dip;
    SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO ipv6_encap_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    encap_entry_type = arad_pp_eg_encap_data_entry_exact_type_get_unsafe(unit,data_info);

    switch (encap_entry_type) 
    { 
    case (SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP): 
        SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_clear(&ipv6_encap_info);

        
        res = arad_pp_eg_encap_ipv6_entry_from_data_buffer(unit,&sip_index,&dip_index,&ipv6_encap_info,data_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LEGACY) {
            
            res = arad_pp_epni_prge_data_tbl_get_unsafe(unit,sip_index,&prge_data_sip);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            
            res = arad_pp_epni_prge_data_tbl_get_unsafe(unit,dip_index,&prge_data_dip);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            
            arad_pp_eg_encap_ipv6_sip_entry_from_prge_buffer(unit,&ipv6_encap_info,prge_data_sip.prge_data_entry);
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

            
            soc_sand_os_memcpy(ipv6_encap_info.tunnel.dest.address,
                               prge_data_dip.prge_data_entry,
                               SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S * sizeof(uint32));
        }
                                      
        exact_data_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP;
        soc_sand_os_memcpy(&exact_data_info->entry_val.ipv6_encap_info,&ipv6_encap_info,sizeof(SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO));

        break;

    default: 
        exact_data_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA;
        break;
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_data_entry_data_exact_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_encap_ipv6_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO     *ipv6_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep 
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EPNI_PRGE_DATA_TBL_DATA
      prge_data_dip,
      prge_data_sip;
  SOC_SAND_SUCCESS_FAILURE
      add_dip_success,
      add_sip_success;
  uint32
      dip_index,sip_index,
      old_dip_index,old_sip_index;
  uint8
      dip_first_appear,
      sip_first_appear,
      dip_last_appear,
      sip_last_appear,
      found;
  SOC_PPC_EG_ENCAP_DATA_INFO
      data_info;  
  uint32 data_32;
  uint64 add_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV6_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ipv6_encap_info);

  if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LEGACY) {  
      soc_sand_os_memset(&prge_data_dip, 0x0, sizeof(prge_data_dip));
      soc_sand_os_memset(&prge_data_sip, 0x0, sizeof(prge_data_sip));

      SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);

      
      
      
      res = arad_pp_eg_encap_ipv6_entry_get_references(unit,tunnel_eep_ndx,&old_sip_index,&old_dip_index,&found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      
      
      soc_sand_os_memcpy(&prge_data_dip.prge_data_entry, ipv6_encap_info->tunnel.dest.address, sizeof(uint32) * SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S);
      
      
      res = arad_sw_db_multiset_add(
            unit,
    		ARAD_SW_DB_CORE_ANY,
            ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY,
            prge_data_dip.prge_data_entry,
            &dip_index,
            &dip_first_appear,
            &add_dip_success
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      if(add_dip_success != SOC_SAND_SUCCESS) {
          goto exit;
      }

      
      res = arad_pp_eg_encap_ipv6_sip_entry_to_prge_buffer(unit,ipv6_encap_info,prge_data_sip.prge_data_entry);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      
      res = arad_sw_db_multiset_add(
            unit,
    		ARAD_SW_DB_CORE_ANY,
            ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY,
            prge_data_sip.prge_data_entry,
            &sip_index,
            &sip_first_appear,
            &add_sip_success
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      if(add_sip_success != SOC_SAND_SUCCESS) {

          
          res = arad_sw_db_multiset_remove_by_index(
                unit,
                ARAD_SW_DB_CORE_ANY,
                ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY,
                dip_index,
                &dip_last_appear
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
          goto exit;
      }

      

      
      if(found) {
          ARAD_PP_EPNI_PRGE_DATA_TBL_DATA prge_reset_data;
          soc_sand_os_memset(&prge_reset_data, 0x0, sizeof(prge_reset_data));

          res = arad_sw_db_multiset_remove_by_index(
                unit,
                ARAD_SW_DB_CORE_ANY,
                ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY,
                old_dip_index,
                &dip_last_appear
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

          if (dip_last_appear) {
              SOC_REG_ABOVE_64_CLEAR(&prge_reset_data.prge_data_entry);
              res = arad_pp_epni_prge_data_tbl_set_unsafe(unit, old_dip_index, &prge_reset_data);
              SOC_SAND_CHECK_FUNC_RESULT(res, 81, exit);
          }

          res = arad_sw_db_multiset_remove_by_index(
                unit,
                ARAD_SW_DB_CORE_ANY,
                ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY,
                old_sip_index,
                &sip_last_appear
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

          if (sip_last_appear) {
              SOC_REG_ABOVE_64_CLEAR(&prge_reset_data.prge_data_entry);
              res = arad_pp_epni_prge_data_tbl_set_unsafe(unit, old_sip_index, &prge_reset_data);
              SOC_SAND_CHECK_FUNC_RESULT(res, 91, exit);
          }
      }

      

      
      if(dip_first_appear) {
          
          res = arad_pp_epni_prge_data_tbl_set_unsafe(unit,dip_index,&prge_data_dip);
          SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      }

      if(sip_first_appear) {
          
          res = arad_pp_epni_prge_data_tbl_set_unsafe(unit,sip_index,&prge_data_sip);
          SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
      }
      
      
      res = arad_pp_eg_encap_ipv6_entry_to_data_buffer(unit,sip_index,dip_index,ipv6_encap_info,&data_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

      res = arad_pp_eg_encap_data_lif_entry_add_unsafe(unit,tunnel_eep_ndx,&data_info,ll_eep!=0,ll_eep);
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  } else {
        soc_sand_os_memset(&prge_data_sip, 0x0, sizeof(prge_data_sip));
       SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);

       
       res = arad_pp_eg_encap_ipv6_entry_get_references(unit,tunnel_eep_ndx,&old_sip_index,&old_dip_index,&found);
       SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

       
       if(found) {
           ARAD_PP_EPNI_PRGE_DATA_TBL_DATA prge_reset_data;
           soc_sand_os_memset(&prge_reset_data, 0x0, sizeof(prge_reset_data)); 
 
           res = arad_sw_db_multiset_remove_by_index(
                 unit,
                 ARAD_SW_DB_CORE_ANY,
                 ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY,
                 old_sip_index,
                 &sip_last_appear
               );
           SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
 
           if (sip_last_appear) {
               SOC_REG_ABOVE_64_CLEAR(&prge_reset_data.prge_data_entry);
               res = arad_pp_epni_prge_data_tbl_set_unsafe(unit, old_sip_index, &prge_reset_data);
               SOC_SAND_CHECK_FUNC_RESULT(res, 91, exit);
           }
       }
  
       
       res = arad_pp_eg_encap_ipv6_sip_entry_to_prge_buffer(unit,ipv6_encap_info,prge_data_sip.prge_data_entry);
       SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  
       
       res = arad_sw_db_multiset_add(
             unit,
             ARAD_SW_DB_CORE_ANY,
             ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY,
             prge_data_sip.prge_data_entry,
             &sip_index,
             &sip_first_appear,
             &add_sip_success
           );
       SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

       
       if (sip_index > ARAD_PP_EG_ENCAP_IPV6_SIP_INDEX_MAX) {
           
          res = arad_sw_db_multiset_remove_by_index(
                unit,
                ARAD_SW_DB_CORE_ANY,
                ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY,
                sip_index,
                &sip_last_appear
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

          goto exit;
       }       
  
       if(sip_first_appear) {
           
           res = arad_pp_epni_prge_data_tbl_set_unsafe(unit,sip_index,&prge_data_sip);
           SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
       }
       
       ipv6_encap_info->tunnel.sip_index = sip_index;
       
       if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) {        
           
           res = arad_pp_eg_encap_ipv6_entry_to_data_buffer(unit,sip_index,-1,ipv6_encap_info,&data_info);
           SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
      
           res = arad_pp_eg_encap_data_lif_entry_add_unsafe(unit,tunnel_eep_ndx,&data_info,ll_eep!=0,ll_eep);
           SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

           
           COMPILER_64_ZERO(add_data);
           data_32 = 0;       
           SHR_BITCOPY_RANGE(&data_32,0,ipv6_encap_info->tunnel.dest.address,56,20);
           COMPILER_64_SET(add_data, 0, data_32);
           res = arad_pp_lif_additional_data_set(unit, tunnel_eep_ndx, FALSE, add_data, FALSE);
           SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
       }
    }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv6_entry_add_unsafe()", tunnel_eep_ndx, 0);
}

uint32
  arad_pp_eg_encap_ipv6_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO            *ipv6_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV6_ENTRY_ADD_VERIFY);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, tunnel_eep_ndx, ARAD_PP_EG_ENCAP_IP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR, 5, exit);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO, ipv6_encap_info, 10, exit);

  ARAD_PP_EG_ENCAP_NDX_OF_TYPE_VERIFY(tunnel_eep_ndx);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, ll_eep, ARAD_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv6_entry_add_verify()", tunnel_eep_ndx, ll_eep);
}



uint32
  arad_pp_eg_encap_mirror_entry_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EPNI_PRGE_DATA_TBL_DATA
      prge_data_erspan,
      prge_data_l2;
  uint32 prge_tbl_mirror_base;
  ARAD_PP_EPNI_ISID_TABLE_TBL_DATA
      isid_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_MIRROR_ENTRY_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mirror_encap_info);


  
  res = arad_pp_eg_encap_mirror_erspan_data_to_prge_buffer(unit, mirror_encap_info, prge_data_erspan.prge_data_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = arad_pp_eg_encap_mirror_l2_data_to_prge_buffer(unit, mirror_encap_info, prge_data_l2.prge_data_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  res = arad_sw_db_eg_encap_prge_tbl_nof_dynamic_entries_get(unit, &prge_tbl_mirror_base);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  res = arad_pp_epni_prge_data_tbl_set_unsafe(unit, prge_tbl_mirror_base + mirror_ndx, &prge_data_erspan);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  res = arad_pp_epni_prge_data_tbl_set_unsafe(unit, prge_tbl_mirror_base + mirror_ndx + ARAD_PP_EG_ENCAP_NOF_MIRRORS, &prge_data_l2);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  
  isid_tbl_data.isid = mirror_encap_info->tunnel.encap_id;
  res = arad_pp_epni_isid_table_tbl_set_unsafe(unit, mirror_encap_info->tunnel.encap_id, &isid_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mirror_entry_set_unsafe()", mirror_ndx, 0);
}


uint32
  arad_pp_eg_encap_mirror_entry_set_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_MIRROR_ENTRY_SET_VERIFY);


  SOC_SAND_ERR_IF_ABOVE_MAX(mirror_ndx, DPP_MIRROR_ACTION_NDX_MAX, ARAD_PP_EG_ENCAP_MIRROR_ID_OUT_OF_RANGE_ERR, 5, exit);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO, mirror_encap_info, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mirror_entry_set_verify()", mirror_ndx, 0);
}




uint32
  arad_pp_eg_encap_mirror_entry_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  )
{
  uint8
    eth_hdr_size = 0;
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EPNI_PRGE_DATA_TBL_DATA
      prge_data_erspan,
      prge_data_l2;
  uint32 prge_tbl_mirror_base;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_MIRROR_ENTRY_GET_UNSAFE);

  
  res = arad_sw_db_eg_encap_prge_tbl_nof_dynamic_entries_get(unit, &prge_tbl_mirror_base);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = arad_pp_epni_prge_data_tbl_get_unsafe(unit, prge_tbl_mirror_base + mirror_ndx, &prge_data_erspan);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  res = arad_pp_epni_prge_data_tbl_get_unsafe(unit, prge_tbl_mirror_base + mirror_ndx + ARAD_PP_EG_ENCAP_NOF_MIRRORS, &prge_data_l2);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  res = arad_pp_eg_encap_mirror_erspan_entry_from_prge_buffer(unit, prge_data_erspan.prge_data_entry, &eth_hdr_size, mirror_encap_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  res = arad_pp_eg_encap_mirror_l2_entry_from_prge_buffer(unit, prge_data_l2.prge_data_entry, eth_hdr_size, mirror_encap_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mirror_entry_get_unsafe()", mirror_ndx, 0);
}



uint32
  arad_pp_eg_encap_port_erspan_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  is_erspan
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EPNI_PP_PCT_TBL_DATA
    pp_pct_tbl_data;
  int set_table = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PORT_ERSPAN_SET_UNSAFE);


  res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core_id, local_port_ndx, &pp_pct_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

                
  if(1 == is_erspan) {
      
      if(ARAD_PRGE_PP_SELECT_PP_COPY_HEADER_SPAN == pp_pct_tbl_data.prge_profile) {
          pp_pct_tbl_data.prge_profile = ARAD_PRGE_PP_SELECT_PP_COPY_HEADER_ERSPAN;
          set_table = 1;
      }
      else if(ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FTMH_SPAN == pp_pct_tbl_data.prge_profile) {
          pp_pct_tbl_data.prge_profile = ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FTMH_ERSPAN;
          set_table = 1;
      }
  }
  else {
      
      if(ARAD_PRGE_PP_SELECT_PP_COPY_HEADER_ERSPAN == pp_pct_tbl_data.prge_profile) {
          pp_pct_tbl_data.prge_profile = ARAD_PRGE_PP_SELECT_PP_COPY_HEADER_SPAN;
          set_table = 1;
      }
      else if(ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FTMH_ERSPAN == pp_pct_tbl_data.prge_profile) {
          pp_pct_tbl_data.prge_profile = ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FTMH_SPAN;
          set_table = 1;
      }
  }

  if(1 == set_table) {
      res = arad_pp_epni_pp_pct_tbl_set_unsafe(unit, core_id, local_port_ndx, &pp_pct_tbl_data);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mirror_entry_get_unsafe()", local_port_ndx, is_erspan);
}


uint32
  arad_pp_eg_encap_port_erspan_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *is_erspan
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EPNI_PP_PCT_TBL_DATA
    pp_pct_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PORT_ERSPAN_GET_UNSAFE);

  res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core_id, local_port_ndx, &pp_pct_tbl_data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit); 

  if(ARAD_PRGE_PP_SELECT_PP_COPY_HEADER_ERSPAN   == pp_pct_tbl_data.prge_profile ||
     ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FTMH_ERSPAN == pp_pct_tbl_data.prge_profile) {
      *is_erspan = 1;
  }
  else {
      *is_erspan = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mirror_entry_get_unsafe()", local_port_ndx, 0);
}



uint32
  arad_pp_eg_encap_ll_entry_add_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                ll_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_LL_INFO                 *ll_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_LL_ENTRY_ADD_UNSAFE);
  
  SOC_SAND_CHECK_NULL_INPUT(ll_encap_info);

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

  
     
  res = soc_sand_pp_mac_address_struct_to_long(
          &ll_encap_info->dest_mac,
          tbl_data.dest_mac
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  tbl_data.vid = ll_encap_info->out_vid;
  tbl_data.vid_valid = ll_encap_info->out_vid_valid;
  tbl_data.next_outlif_lsb = ll_encap_info->out_ac_lsb;
  tbl_data.next_outlif_valid = ll_encap_info->out_ac_valid;
  tbl_data.remark_profile = ll_encap_info->ll_remark_profile;
  tbl_data.oam_lif_set = SOC_SAND_BOOL2NUM(ll_encap_info->oam_lif_set);
  tbl_data.drop = SOC_SAND_BOOL2NUM(ll_encap_info->drop);
  if (SOC_IS_JERICHO(unit)) {
      tbl_data.outlif_profile = ll_encap_info->outlif_profile; 
  }
  if (SOC_IS_JERICHO_PLUS(unit) && !SOC_IS_QAX_A0(unit)) {
      tbl_data.native_ll = ll_encap_info->native_ll;
  }
  res = arad_pp_eg_encap_access_link_layer_format_tbl_set_unsafe(
          unit,
          ll_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ll_entry_add_unsafe()", ll_eep_ndx, 0);
}


uint32
  arad_pp_eg_encap_ll_entry_add_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                ll_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_LL_INFO                  *ll_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_LL_ENTRY_ADD_VERIFY);

  ARAD_PP_EG_ENCAP_NDX_OF_TYPE_VERIFY(ll_eep_ndx);

  res = SOC_PPC_EG_ENCAP_LL_INFO_verify(unit, ll_encap_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, ll_eep_ndx, ARAD_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ll_entry_add_verify()", ll_eep_ndx, 0);
}



uint32
  arad_pp_eg_encap_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD];
  uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD];
  uint32 nof_entries;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ENTRY_REMOVE_UNSAFE);

  soc_sand_os_memset(next_eep, 0x0, sizeof(next_eep)); 
  res = arad_pp_eg_encap_entry_get_unsafe(unit, eep_type_ndx, eep_ndx, 1, encap_entry_info, next_eep, &nof_entries);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (SOC_IS_ARADPLUS_A0(unit)) {
      if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_OVERLAY_ARP_ENCAP) {
          uint32  prge_data_relative_index;
          uint32 prge_data_base_index;
          uint32 prge_data_absolute_index;
          uint8 prge_data_last_appear; 
          ARAD_PP_EPNI_PRGE_DATA_TBL_DATA prge_data;

          
          prge_data_relative_index = encap_entry_info[0].entry_val.overlay_arp_encap_info.ll_vsi_pointer;

          
          res  = arad_sw_db_eg_encap_prge_tbl_overlay_arp_entries_base_index_get(unit, &prge_data_base_index);
          SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit); 

          
          prge_data_absolute_index = prge_data_base_index + prge_data_relative_index;

          
          res = arad_sw_db_multiset_remove_by_index(unit, 
                                                    ARAD_SW_DB_CORE_ANY,
                                                    ARAD_PP_SW_DB_MULTI_SET_ENTRY_OVERLAY_ARP_PROG_DATA_ENTRY, 
                                                    prge_data_relative_index ,
                                                    &prge_data_last_appear); 
          SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit); 

          
          SOC_REG_ABOVE_64_CLEAR(&prge_data.prge_data_entry);
          res = arad_pp_epni_prge_data_tbl_set_unsafe(unit, prge_data_absolute_index, &prge_data); 
          SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit); 
      }
  } 
  
  else if (SOC_IS_JERICHO(unit)) {

      if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP) {
        uint64 additional_data;

          
          SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO eth_type_index_entry;
          soc_sand_os_memset(
               &eth_type_index_entry, 0x0, sizeof(SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO)); 
          soc_jer_eg_encap_ether_type_index_get(
             unit, encap_entry_info[0].entry_val.overlay_arp_encap_info.eth_type_index, &eth_type_index_entry); 

          COMPILER_64_SET(additional_data,0,0);

            
            if (eth_type_index_entry.tpid_0 != 0) encap_entry_info[0].entry_val.overlay_arp_encap_info.nof_tags++;
            if (eth_type_index_entry.tpid_1 != 0) encap_entry_info[0].entry_val.overlay_arp_encap_info.nof_tags++;

          
          res = arad_pp_lif_additional_data_set(unit, eep_ndx, 0, additional_data, FALSE);
          SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
      }
  }

  
  if (SOC_IS_ARADPLUS(unit)) {

      if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP) {
          if ((SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) && 
              (encap_entry_info[1].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP)) {
              uint64 additional_data; 

              
              COMPILER_64_SET(additional_data,0,0);              
              res = arad_pp_lif_additional_data_set(unit, eep_ndx, 0, additional_data, FALSE);
              SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
              
              res = arad_pp_eg_encap_access_entry_init_unsafe(
                      unit,
                      next_eep[0]
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
              
              COMPILER_64_SET(additional_data,0,0);              
              res = arad_pp_lif_additional_data_set(unit, next_eep[0], 0, additional_data, FALSE);
              SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
              
              if (nof_entries >= 2 && next_eep[1]) {
                  res = arad_pp_eg_encap_access_entry_init_unsafe(
                          unit,
                          next_eep[1]
                        );
                  SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);
              }
          } else {
              uint8 prge_data_last_appear, found;
              uint32 old_dip_index, old_sip_index;
              ARAD_PP_EPNI_PRGE_DATA_TBL_DATA prge_reset_data;
    
              soc_sand_os_memset(&prge_reset_data, 0x0, sizeof(prge_reset_data));
    
              res = arad_pp_eg_encap_ipv6_entry_get_references(unit,eep_ndx,&old_sip_index,&old_dip_index,&found);
              SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    
              if (found) {
                  
                  res = arad_sw_db_multiset_remove_by_index(unit,
                                                            ARAD_SW_DB_CORE_ANY,
                                                            ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY,
                                                            old_sip_index,
                                                            &prge_data_last_appear);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);
    
                  
                  if (prge_data_last_appear) {
                      SOC_REG_ABOVE_64_CLEAR(&prge_reset_data.prge_data_entry);
                      res = arad_pp_epni_prge_data_tbl_set_unsafe(unit, old_sip_index, &prge_reset_data);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
                  }
    
                  
                  res = arad_sw_db_multiset_remove_by_index(unit,
                                                            ARAD_SW_DB_CORE_ANY,
                                                            ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY,
                                                            old_dip_index,
                                                            &prge_data_last_appear);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);
    
                  
                  if (prge_data_last_appear) {
                      SOC_REG_ABOVE_64_CLEAR(&prge_reset_data.prge_data_entry);
                      res = arad_pp_epni_prge_data_tbl_set_unsafe(unit, old_dip_index, &prge_reset_data);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
                  }
              }
          }
      }
  }

  res = arad_pp_eg_encap_access_entry_init_unsafe(
          unit,
          eep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_entry_remove_unsafe()", eep_type_ndx, eep_ndx);
}


uint32
  arad_pp_eg_encap_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ENTRY_REMOVE_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(eep_type_ndx, SOC_PPC_EG_ENCAP_EEP_TYPE_NDX_MAX, SOC_PPC_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, eep_ndx, ARAD_PP_EG_ENCAP_EEP_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_entry_remove_verify()", 0, eep_ndx);
}


STATIC uint32
  arad_pp_eg_encap_ll_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  ll_eep_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                 *encap_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT
    tbl_data;
  SOC_PPC_EG_ENCAP_EEP_TYPE
    eep_type_ndx = SOC_PPC_EG_ENCAP_EEP_TYPE_LL;
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
    cur_eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_LL_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(encap_info);

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

  SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(encap_info);

  res = arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, ll_eep_ndx, &cur_eep_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_PP_EG_ENCAP_VERIFY_EEP_TYPE_COMPATIBLE_TO_ACCESS_TYPE(ll_eep_ndx,eep_type_ndx, cur_eep_type);

  res = arad_pp_eg_encap_access_link_layer_format_tbl_get_unsafe(
          unit,
          ll_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
     
  res = soc_sand_pp_mac_address_long_to_struct(
          tbl_data.dest_mac,
          &encap_info->entry_val.ll_info.dest_mac
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  encap_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP;
  encap_info->entry_val.ll_info.out_vid = tbl_data.vid;
  encap_info->entry_val.ll_info.out_vid_valid = SOC_SAND_NUM2BOOL(tbl_data.vid_valid);
  encap_info->entry_val.ll_info.ll_remark_profile = tbl_data.remark_profile;
  encap_info->entry_val.ll_info.oam_lif_set = SOC_SAND_BOOL2NUM(tbl_data.oam_lif_set);
  encap_info->entry_val.ll_info.drop = SOC_SAND_BOOL2NUM(tbl_data.drop);
  encap_info->entry_val.ll_info.out_ac_valid = SOC_SAND_NUM2BOOL(tbl_data.next_outlif_valid);
  if (encap_info->entry_val.ll_info.out_ac_valid)
  {
      encap_info->entry_val.ll_info.out_ac_lsb = tbl_data.next_outlif_lsb;    
  }
  if (SOC_IS_JERICHO(unit)) {
      encap_info->entry_val.ll_info.outlif_profile = tbl_data.outlif_profile; 
  }
  if (SOC_IS_JERICHO_PLUS(unit) && !SOC_IS_QAX_A0(unit)) {
      encap_info->entry_val.ll_info.native_ll = tbl_data.native_ll; 
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ll_entry_get_unsafe()", ll_eep_ndx, 0);
}

  


STATIC uint32
  arad_pp_eg_encap_data_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  data_eep_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                 *encap_info,
    SOC_SAND_OUT uint8                                   *next_eep_valid, 
    SOC_SAND_OUT uint32                                    *next_eep
  )
{
  uint32
    res = SOC_SAND_OK,
    ind;
  ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT
    tbl_data;
  SOC_PPC_EG_ENCAP_EEP_TYPE
    eep_type_ndx = SOC_PPC_EG_ENCAP_EEP_TYPE_DATA;
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
    cur_eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_DATA_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(encap_info);

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

  SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(encap_info);

  arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, data_eep_ndx, &cur_eep_type);
  ARAD_PP_EG_ENCAP_VERIFY_EEP_TYPE_COMPATIBLE_TO_ACCESS_TYPE(data_eep_ndx,eep_type_ndx, cur_eep_type); 

  res = arad_pp_eg_encap_access_data_entry_format_tbl_get_unsafe(
          unit,
          data_eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for (ind = 0; ind < SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE; ind++) {
    encap_info->entry_val.data_info.data_entry[ind] = tbl_data.data[ind];
  }

  encap_info->entry_val.data_info.oam_lif_set = SOC_SAND_BOOL2NUM(tbl_data.oam_lif_set);
  encap_info->entry_val.data_info.drop = SOC_SAND_BOOL2NUM(tbl_data.drop);
  encap_info->entry_val.data_info.outlif_profile = tbl_data.outlif_profile;
  encap_info->entry_val.data_info.rsv_bits = tbl_data.rsv_bits;

  *next_eep_valid = SOC_SAND_NUM2BOOL(tbl_data.next_outlif_valid);
  if (*next_eep_valid)
  {
      *next_eep = tbl_data.next_outlif;   
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_data_entry_get_unsafe()", data_eep_ndx, 0);
}


STATIC uint32
  arad_pp_eg_encap_vsi_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                 *encap_info,
    SOC_SAND_OUT uint8                                   *next_eep_valid, 
    SOC_SAND_OUT uint32                                    *next_eep
  )
{ 
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT
    tbl_data;
  SOC_PPC_EG_ENCAP_EEP_TYPE
    eep_type_ndx = SOC_PPC_EG_ENCAP_EEP_TYPE_VSI;
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
    cur_eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_VSI_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(encap_info);

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

  SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(encap_info);

  arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, lif_eep_ndx, &cur_eep_type);
  ARAD_PP_EG_ENCAP_VERIFY_EEP_TYPE_COMPATIBLE_TO_ACCESS_TYPE(lif_eep_ndx,eep_type_ndx, cur_eep_type); 

  if (SOC_IS_JERICHO_PLUS(unit)) {
      res = qax_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(
              unit,
              lif_eep_ndx,
              &tbl_data
            );
  } else {
      res = arad_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(
              unit,
              lif_eep_ndx,
              &tbl_data
            );
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  encap_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_VSI;
  encap_info->entry_val.vsi_info.out_vsi = tbl_data.next_vsi_lsb;
  encap_info->entry_val.vsi_info.remark_profile = tbl_data.remark_profile;
  encap_info->entry_val.vsi_info.oam_lif_set = SOC_SAND_BOOL2NUM(tbl_data.oam_lif_set);
  encap_info->entry_val.vsi_info.drop = SOC_SAND_BOOL2NUM(tbl_data.drop);
  if (SOC_IS_JERICHO(unit)) {
      encap_info->entry_val.vsi_info.outlif_profile = tbl_data.outlif_profile;
  }
  if (SOC_IS_JERICHO_PLUS(unit)) {
      encap_info->entry_val.vsi_info.outrif_profile_index = tbl_data.outrif_profile_index;
  }

  *next_eep_valid = SOC_SAND_NUM2BOOL(tbl_data.next_outlif_valid);
  if (*next_eep_valid)
  {
      *next_eep = tbl_data.next_outlif;   
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_vsi_entry_get_unsafe()", lif_eep_ndx, 0);
}


STATIC uint32
  arad_pp_eg_encap_lif_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  eep_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                 *encap_info,
    SOC_SAND_OUT uint32                                  *next_eep
  )
{
  uint32
    res;
  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT
    tbl_data;
  SOC_PPC_EG_ENCAP_EEP_TYPE
    eep_type_ndx = SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP;
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
    cur_eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
  uint8 is_full_entry_extension = TRUE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_LIF_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(encap_info);
  SOC_SAND_CHECK_NULL_INPUT(next_eep);

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

  arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, eep_ndx, &cur_eep_type);
  ARAD_PP_EG_ENCAP_VERIFY_EEP_TYPE_COMPATIBLE_TO_ACCESS_TYPE(eep_ndx,eep_type_ndx, cur_eep_type); 

  res = arad_pp_eg_encap_access_mpls_tunnel_format_tbl_get_unsafe(
          unit,
          eep_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (tbl_data.mpls1_command == ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val)
  {
    
    encap_info->entry_val.swap_info.swap_label = tbl_data.mpls1_label;
    encap_info->entry_val.swap_info.oam_lif_set = tbl_data.oam_lif_set;
    encap_info->entry_val.swap_info.drop = tbl_data.drop;
    encap_info->entry_val.swap_info.out_vsi = tbl_data.next_vsi_lsb;
    encap_info->entry_val.swap_info.outlif_profile = SOC_IS_JERICHO(unit) ? tbl_data.outlif_profile : 0;
    encap_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND;
    encap_info->entry_val.swap_info.remark_profile = tbl_data.mpls_swap_remark_profile;
  }
  else if (tbl_data.mpls1_command == ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val)
  {
    
    encap_info->entry_val.pop_info.oam_lif_set = tbl_data.oam_lif_set;
    encap_info->entry_val.pop_info.drop = tbl_data.drop;
    encap_info->entry_val.pop_info.pop_type = SOC_PPC_MPLS_COMMAND_TYPE_POP;
    encap_info->entry_val.pop_info.out_vsi = tbl_data.next_vsi_lsb;
    encap_info->entry_val.pop_info.ethernet_info.has_cw = SOC_SAND_NUM2BOOL(tbl_data.has_cw);
    encap_info->entry_val.pop_info.pop_next_header = tbl_data.upper_layer_protocol;
    encap_info->entry_val.pop_info.ethernet_info.tpid_profile = tbl_data.tpid_profile;
    encap_info->entry_val.pop_info.model = (tbl_data.model_is_pipe )? SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE : SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
    encap_info->entry_val.pop_info.outlif_profile = SOC_IS_JERICHO(unit) ? tbl_data.outlif_profile : 0;
    encap_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND;
  }
  else
  {
    
    uint32 default_label_value = 0;
    default_label_value = SOC_IS_JERICHO_B0_AND_ABOVE(unit) ? soc_property_get(unit, spn_MPLS_ENCAP_INVALID_VALUE, 0) : 0;

    
    encap_info->entry_val.pwe_info.push_profile = tbl_data.mpls1_command - ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_val;
    encap_info->entry_val.pwe_info.label = tbl_data.mpls1_label;
    encap_info->entry_val.pwe_info.oam_lif_set = tbl_data.oam_lif_set;
    encap_info->entry_val.pwe_info.drop = tbl_data.drop;
    encap_info->entry_val.pwe_info.outlif_profile = SOC_IS_JERICHO(unit) ? tbl_data.outlif_profile : 0;
     
    if (soc_property_get(unit, spn_MPLS_BIND_PWE_WITH_MPLS_ONE_CALL, 0)) {
        encap_info->entry_val.pwe_info.egress_tunnel_label_info.tunnels[0].tunnel_label = (tbl_data.mpls2_label == default_label_value) ? 0 : tbl_data.mpls2_label;
        encap_info->entry_val.pwe_info.egress_tunnel_label_info.tunnels[0].push_profile = tbl_data.mpls2_command;
        encap_info->entry_val.pwe_info.egress_tunnel_label_info.nof_tunnels = tbl_data.mpls2_label == default_label_value ? 0 : 1;
    }
    encap_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE;


    
    SOC_PPC_EG_ENCAP_PROTECTION_INFO_clear(&(encap_info->entry_val.pwe_info.protection_info));
    if (SOC_IS_JERICHO(unit)) {
        res = arad_pp_eg_encap_protection_info_get_unsafe(
                unit, eep_ndx, is_full_entry_extension, &(encap_info->entry_val.pwe_info.protection_info));
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

  *next_eep = tbl_data.next_outlif;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_tunnel_entry_get_unsafe()", eep_ndx, 0);
}


STATIC uint32
  arad_pp_eg_encap_tunnel_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  eep_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                 *encap_info,
    SOC_SAND_OUT uint32                                  *next_eep
  )
{
  uint32
    res = SOC_SAND_OK, default_label_value = 0;
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
    tbl_data_entry_type;
  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT
    ee_entry_tbl_data;
  ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT
    ip_tunnel_format_tbl_data;
  uint8 is_full_entry_extension = TRUE;
  uint32 push_command_upper_limit_val = 0;
  uint32 push_profile = 0;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_TUNNEL_ENTRY_GET_UNSAFE);

  soc_sand_os_memset(&tbl_data_entry_type, 0x0, sizeof(tbl_data_entry_type));
  soc_sand_os_memset(&ip_tunnel_format_tbl_data, 0x0, sizeof(ip_tunnel_format_tbl_data));

  SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(encap_info);

  arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, eep_ndx, &tbl_data_entry_type);

  switch(tbl_data_entry_type)
  {
    case (ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_IP):
        
        res = arad_pp_eg_encap_access_ip_tunnel_format_tbl_get_unsafe(
                unit,
                eep_ndx,
                &ip_tunnel_format_tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        encap_info->entry_val.ipv4_encap_info.dest.dest = ip_tunnel_format_tbl_data.ipv4_dst;
        encap_info->entry_val.ipv4_encap_info.dest.src_index = ip_tunnel_format_tbl_data.ipv4_src_index;
        encap_info->entry_val.ipv4_encap_info.dest.tos_index = (uint8)ip_tunnel_format_tbl_data.ipv4_tos_index;
        encap_info->entry_val.ipv4_encap_info.dest.ttl_index = (uint8)ip_tunnel_format_tbl_data.ipv4_ttl_index;

        if (SOC_IS_JERICHO_B0_AND_ABOVE(unit)&& (SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE == 1)) {
            encap_info->entry_val.ipv4_encap_info.dest.encapsulation_mode_index = ip_tunnel_format_tbl_data.encapsulation_mode; 
        } else {
            SOC_PPC_EG_ENCAP_SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_HW_FLD_TO_ENCAPSULATION_MODE(unit, 
                                                                                              ip_tunnel_format_tbl_data.encapsulation_mode, 
                                                                                              &(encap_info->entry_val.ipv4_encap_info.dest.encapsulation_mode));
        }
        encap_info->entry_val.ipv4_encap_info.out_vsi = ip_tunnel_format_tbl_data.next_vsi_lsb;
        encap_info->entry_val.ipv4_encap_info.oam_lif_set = SOC_SAND_BOOL2NUM(ip_tunnel_format_tbl_data.oam_lif_set);
        encap_info->entry_val.ipv4_encap_info.drop = SOC_SAND_BOOL2NUM(ip_tunnel_format_tbl_data.drop);
        encap_info->entry_val.ipv4_encap_info.outlif_profile = SOC_IS_JERICHO(unit) ? ip_tunnel_format_tbl_data.outlif_profile : 0;

        *next_eep = ip_tunnel_format_tbl_data.next_outlif;

        encap_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP;
        break;
   case (ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_TUNNEL):
        
		
        default_label_value = SOC_IS_JERICHO_B0_AND_ABOVE(unit) ? soc_property_get(unit, spn_MPLS_ENCAP_INVALID_VALUE, 0) : 0;

        push_command_upper_limit_val = SOC_IS_JERICHO_B0_AND_ABOVE(unit) ? ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_JER_B0_UPPER_LIMIT_val : ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_UPPER_LIMIT_val;

        res = arad_pp_eg_encap_access_mpls_tunnel_format_tbl_get_unsafe(
                unit,
                eep_ndx,
                &ee_entry_tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        encap_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP;
        encap_info->entry_val.mpls_encap_info.out_vsi = ee_entry_tbl_data.next_vsi_lsb;
        encap_info->entry_val.mpls_encap_info.oam_lif_set = SOC_SAND_BOOL2NUM(ee_entry_tbl_data.oam_lif_set);
        encap_info->entry_val.mpls_encap_info.drop = SOC_SAND_BOOL2NUM(ee_entry_tbl_data.drop);
        encap_info->entry_val.mpls_encap_info.outlif_profile = SOC_IS_JERICHO(unit) ? ee_entry_tbl_data.outlif_profile : 0 ;

        *next_eep = ee_entry_tbl_data.next_outlif;

        encap_info->entry_val.mpls_encap_info.nof_tunnels = 0;

        
        encap_info->entry_val.mpls_encap_info.tunnels[0].push_profile =
          ee_entry_tbl_data.mpls1_command - ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_val;
        encap_info->entry_val.mpls_encap_info.tunnels[0].tunnel_label =
          ee_entry_tbl_data.mpls1_label;

        
        encap_info->entry_val.mpls_encap_info.tunnels[1].push_profile =
          ee_entry_tbl_data.mpls2_command - ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_val;
        encap_info->entry_val.mpls_encap_info.tunnels[1].tunnel_label =
          ee_entry_tbl_data.mpls2_label == default_label_value ? 0 : ee_entry_tbl_data.mpls2_label;

        push_profile = encap_info->entry_val.mpls_encap_info.tunnels[0].push_profile;

        
        if (push_profile <= push_command_upper_limit_val && push_profile != ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val && \
            push_profile != ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val)
        {
          ++encap_info->entry_val.mpls_encap_info.nof_tunnels;
        }

        push_profile = encap_info->entry_val.mpls_encap_info.tunnels[1].push_profile;

        if ((push_profile <= push_command_upper_limit_val && push_profile != ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val \
              && push_profile != ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val)  && (ee_entry_tbl_data.mpls2_label != default_label_value))
        {
          ++encap_info->entry_val.mpls_encap_info.nof_tunnels;
        }
        
        
        SOC_PPC_EG_ENCAP_PROTECTION_INFO_clear(&(encap_info->entry_val.mpls_encap_info.protection_info));
        if (SOC_IS_JERICHO(unit)) {
            res = arad_pp_eg_encap_protection_info_get_unsafe(
                    unit, eep_ndx, is_full_entry_extension, &(encap_info->entry_val.mpls_encap_info.protection_info));
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }

        break;
  case (ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE):
      ARAD_PP_DO_NOTHING_AND_EXIT;
      break;
  default: 
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_tunnel_entry_get_unsafe()", eep_ndx, 0);
}

STATIC uint32
  arad_pp_eg_encap_eedb_type_to_entry_type(
    SOC_SAND_IN  int                             unit, 
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE  eep_type,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_TYPE        *entry_type
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch(eep_type) {
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE:
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ISID:
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_TRILL:
    *entry_type =  SOC_PPC_EG_ENCAP_ENTRY_TYPE_NULL;
    break;
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS:
    *entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE;
    break;
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_IPV6:
    *entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP;
    break;
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA:
    *entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA;
    break;
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER:
    *entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP;
    break;
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_SWAP:
    *entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND;
    break;
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_TUNNEL:
    *entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP;
    break;
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_IP:
    *entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP;
    break;
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF:
    *entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_VSI;
    break;
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC:
    *entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_AC;
    break;
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_POP:
    *entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND;
    break;
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_ARP_OVERLAY: 
    *entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_OVERLAY_ARP_ENCAP;
    break;
  case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ROO_LINK_LAYER:
    *entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP; 
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR, 40, exit);
  }
   
  ARAD_DO_NOTHING_AND_EXIT;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_entry_type_get_unsafe()", 0, eep_type);
}
  


uint32
  arad_pp_eg_encap_entry_type_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 eep_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_TYPE            *entry_type
  )
{
  uint32
   res = SOC_SAND_OK;
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
    eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_ENTRY_TYPE_GET_UNSAFE);
 
  res = arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, eep_ndx, &eep_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_pp_eg_encap_eedb_type_to_entry_type(
          unit, 
          eep_type,
          entry_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_entry_type_get_unsafe()", 0, eep_ndx);
}


uint32
  arad_pp_eg_encap_entry_type_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  eep_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_ENTRY_TYPE_GET_VERIFY);

  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, eep_ndx, ARAD_PP_EG_ENCAP_EEP_NDX_OUT_OF_RANGE_ERR, 20, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_entry_type_get_verify()", 0, eep_ndx);
}



uint32
  arad_pp_eg_encap_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE                 eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx,
    SOC_SAND_IN  uint32                                  depth,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO               encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD],
    SOC_SAND_OUT uint32                                  next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD],
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
   res = SOC_SAND_OK,
   depth_i,
   cur_eep_ndx;
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
    cur_eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
  uint8
    ll_out_ac_valid;  
  uint8 
    is_wide_entry; 
  uint8 
    ext_type_dummy;
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO *mpls_encap_info;
  
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info;
  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO *ipv6_encap_info;
  uint64 add_data;
  uint32 data_32;
  uint32 index;
  ARAD_PP_EPNI_PRGE_DATA_TBL_DATA prge_data_sip;
  SOC_SAND_PP_IPV4_TOS tos_encoded;
  SOC_SAND_PP_IP_TTL ttl_encoded;
  uint32 outlif_value = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(nof_entries);
 
  SOC_SAND_ERR_IF_ABOVE_MAX(depth, SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD - 1, ARAD_PP_EG_ENCAP_DEPTH_OUT_OF_RANGE_ERR, 5, exit);

  
  cur_eep_ndx = eep_ndx;
  *nof_entries = 0; 

  for (depth_i = 0; depth_i <= depth; ++depth_i)
  {
    
    
    SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(&encap_entry_info[depth_i]);
    
    
    next_eep[depth_i] = ARAD_PP_EG_ENCAP_NEXT_EEP_INVALID;
    if (cur_eep_ndx == ARAD_PP_EG_ENCAP_NEXT_EEP_INVALID) {
        break;
    } else {
      arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, cur_eep_ndx, &cur_eep_type);

      switch (cur_eep_type)
      {
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_POP:    
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_SWAP:    
        res = arad_pp_eg_encap_lif_entry_get_unsafe(
                  unit,
                  cur_eep_ndx,
                  &encap_entry_info[depth_i],
                  &next_eep[depth_i]
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          break;
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_TUNNEL:
          
          if ((eep_type_ndx == SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP) && (depth_i == 0)) {
              res = arad_pp_eg_encap_lif_entry_get_unsafe(
                        unit,
                        cur_eep_ndx,
                        &encap_entry_info[depth_i],
                        &next_eep[depth_i]
                      );
              SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
          } else {
              res = arad_pp_eg_encap_tunnel_entry_get_unsafe(
                        unit,
                        cur_eep_ndx,
                        &encap_entry_info[depth_i],
                        &next_eep[depth_i]
                      );
              SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          }
                     
          res = arad_pp_lif_is_wide_entry(unit, cur_eep_ndx, FALSE, &is_wide_entry, &ext_type_dummy);
          if (is_wide_entry) {
              mpls_encap_info = &(encap_entry_info[depth_i].entry_val.mpls_encap_info);
              res = arad_pp_lif_additional_data_get(unit, cur_eep_ndx, FALSE, &add_data);
              SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
              COMPILER_64_TO_32_LO(data_32, add_data);
              
              mpls_encap_info->addition_label = data_32;
          }
          break;
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_IP: 
        res = arad_pp_eg_encap_tunnel_entry_get_unsafe(
                  unit,
                  cur_eep_ndx,
                  &encap_entry_info[depth_i],
                  &next_eep[depth_i]
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        res = arad_pp_occ_mgmt_app_get(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_IPV4_TUNNEL_TOR,&(encap_entry_info[depth_i].entry_val.ipv4_encap_info.outlif_profile), &outlif_value);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

        
        if (outlif_value != 0) { 
            ipv4_encap_info = &(encap_entry_info[depth_i].entry_val.ipv4_encap_info);
            
                       
            res = arad_pp_lif_is_wide_entry(unit, cur_eep_ndx, FALSE, &is_wide_entry, &ext_type_dummy);
            SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

            
            if (is_wide_entry) {
                res = arad_pp_lif_additional_data_get(unit, cur_eep_ndx, FALSE, &add_data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
                COMPILER_64_TO_32_LO(data_32, add_data);
                
                SHR_BITCOPY_RANGE(&ipv4_encap_info->dest.dest, 0, &data_32, 0, 16);
            }
        }

        res = arad_pp_occ_mgmt_app_get(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_IPV6_TUNNEL_TOR,&(encap_entry_info[depth_i].entry_val.ipv4_encap_info.outlif_profile), &outlif_value);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

        
        if (outlif_value != 0) { 
            encap_entry_info[depth_i].entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP;
            ipv4_encap_info = &(encap_entry_info[depth_i].entry_val.ipv4_encap_info);
            ipv6_encap_info = &(encap_entry_info[depth_i].entry_val.ipv6_encap_info);

                       
            res = arad_pp_lif_is_wide_entry(unit, cur_eep_ndx, FALSE, &is_wide_entry, &ext_type_dummy);
            SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

            
            if (is_wide_entry) {
                res = arad_pp_lif_additional_data_get(unit, cur_eep_ndx, FALSE, &add_data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
                COMPILER_64_TO_32_LO(data_32, add_data);
                SHR_BITCOPY_RANGE(ipv6_encap_info->tunnel.dest.address, 92, &data_32, 16, 4);
                
                SHR_BITCOPY_RANGE(ipv6_encap_info->tunnel.dest.address, 32, &data_32, 0, 16);
            }
            
            SHR_BITCOPY_RANGE(ipv6_encap_info->tunnel.dest.address, 96, &(ipv4_encap_info->dest.dest), 0, 32);

            
            
            index = ipv4_encap_info->dest.src_index;                
            res = arad_pp_epni_prge_data_tbl_get_unsafe(unit,index,&prge_data_sip);
            SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
            arad_pp_eg_encap_ipv6_sip_entry_from_prge_buffer(unit,ipv6_encap_info,prge_data_sip.prge_data_entry);
            SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

            
            index = ipv4_encap_info->dest.ttl_index;
            res = arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_unsafe(unit, index, &ttl_encoded);
            SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
            ipv6_encap_info->tunnel.hop_limit = ttl_encoded;

            
            index = ipv4_encap_info->dest.tos_index;
            res = arad_pp_eg_encap_ipv4_tunnel_glbl_tos_get_unsafe(unit, index, &tos_encoded);
            SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
            ipv6_encap_info->tunnel.flow_label = tos_encoded;
        }
            
        break;
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF: 
        res = arad_pp_eg_encap_vsi_entry_get_unsafe(
                unit,
                cur_eep_ndx,
                &encap_entry_info[depth_i],
                &ll_out_ac_valid,
                &next_eep[depth_i]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        break;
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER:
        res = arad_pp_eg_encap_ll_entry_get_unsafe(
                unit,
                cur_eep_ndx,
                &encap_entry_info[depth_i]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
        
        if (encap_entry_info[depth_i].entry_val.ll_info.out_ac_valid)
        {
          next_eep[depth_i] = encap_entry_info[depth_i].entry_val.ll_info.out_ac_lsb;
        }
        else
        {
          next_eep[depth_i] = ARAD_PP_EG_ENCAP_NEXT_EEP_INVALID;
        }
        break;
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA:
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_IPV6:
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_ARP_OVERLAY:
        res = arad_pp_eg_encap_data_entry_get_unsafe(
                unit,
                cur_eep_ndx,
                &encap_entry_info[depth_i],
                &ll_out_ac_valid, 
                &next_eep[depth_i]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

        
        res = arad_pp_eg_encap_data_entry_data_exact_info_get_unsafe(unit,&(encap_entry_info[depth_i].entry_val.data_info),&encap_entry_info[depth_i]);
        SOC_SAND_CHECK_FUNC_RESULT(res, 83, exit);

                   
        res = arad_pp_lif_is_wide_entry(unit, cur_eep_ndx, FALSE, &is_wide_entry, &ext_type_dummy);
        SOC_SAND_CHECK_FUNC_RESULT(res, 83, exit);

        if (is_wide_entry && (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL)) {
            ipv6_encap_info = &encap_entry_info[depth_i].entry_val.ipv6_encap_info;            
            
            
            res = arad_pp_lif_additional_data_get(unit, cur_eep_ndx, FALSE, &add_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
            COMPILER_64_TO_32_LO(data_32, add_data);
            SHR_BITCOPY_RANGE(ipv6_encap_info->tunnel.dest.address, 56, &data_32, 0, 20);

            
            res = arad_pp_eg_encap_tunnel_entry_get_unsafe(
                  unit,
                  next_eep[depth_i],
                  &encap_entry_info[depth_i + 1],
                  &next_eep[depth_i + 1]
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

            if (encap_entry_info[depth_i + 1].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
                ipv4_encap_info = &encap_entry_info[depth_i + 1].entry_val.ipv4_encap_info;

                           
                res = arad_pp_lif_is_wide_entry(unit, next_eep[depth_i], FALSE, &is_wide_entry, &ext_type_dummy);
                SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

                
                if (is_wide_entry) {
                    res = arad_pp_lif_additional_data_get(unit, next_eep[depth_i], FALSE, &add_data);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
                    COMPILER_64_TO_32_LO(data_32, add_data);
                    SHR_BITCOPY_RANGE(ipv6_encap_info->tunnel.dest.address, 76, &data_32, 0, 20);
                }
                
                SHR_BITCOPY_RANGE(ipv6_encap_info->tunnel.dest.address, 96, &(ipv4_encap_info->dest.dest), 0, 32);

                
                
                index = ipv4_encap_info->dest.src_index;                
                res = arad_pp_epni_prge_data_tbl_get_unsafe(unit,index,&prge_data_sip);
                SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
                arad_pp_eg_encap_ipv6_sip_entry_from_prge_buffer(unit,ipv6_encap_info,prge_data_sip.prge_data_entry);
                SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

                
                index = ipv4_encap_info->dest.ttl_index;
                res = arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_unsafe(unit, index, &ttl_encoded);
                SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
                ipv6_encap_info->tunnel.hop_limit = ttl_encoded;

                
                index = ipv4_encap_info->dest.tos_index;
                res = arad_pp_eg_encap_ipv4_tunnel_glbl_tos_get_unsafe(unit, index, &tos_encoded);
                SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
                ipv6_encap_info->tunnel.flow_label = tos_encoded;

                depth_i = depth_i + 1;
                if (next_eep[depth_i]) {
                    ++(*nof_entries);
                }
            }
                
        }
        break;
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_TRILL:
        res = arad_pp_eg_encap_trill_entry_get(
                unit,
                cur_eep_ndx,
                &encap_entry_info[depth_i],
                &next_eep[depth_i]
           ); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 84, exit);
        break; 
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC:
        encap_entry_info[depth_i].entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_AC;
        res = arad_pp_eg_ac_info_get_unsafe(unit, cur_eep_ndx, &(encap_entry_info[depth_i].entry_val.out_ac));
        SOC_SAND_CHECK_FUNC_RESULT(res, 86, exit);
        break;
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ROO_LINK_LAYER:
        if (SOC_IS_JERICHO(unit)) {
            res = soc_jer_eg_encap_roo_ll_entry_get(unit, cur_eep_ndx, &encap_entry_info[depth_i]);
            SOC_SAND_CHECK_FUNC_RESULT(res, 91, exit);                                             
        }
        break; 
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ISID:
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 90, exit);
		  break;
      case ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE:
        ARAD_PP_DO_NOTHING_AND_EXIT;
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PPC_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR, 100, exit);
        break;
      }
    }

    if (depth_i != SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD)
    {
      
      cur_eep_ndx = next_eep[depth_i];      
    }

    ++(*nof_entries);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_entry_get_unsafe()", 0, eep_ndx);
}

uint32
  arad_pp_eg_encap_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx,
    SOC_SAND_IN  uint32                                  depth
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ENTRY_GET_VERIFY);

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      SOC_SAND_ERR_IF_ABOVE_MAX(eep_type_ndx, SOC_PPC_EG_ENCAP_EEP_TYPE_NDX_MAX, SOC_PPC_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  } 
  
  else {
      SOC_SAND_ERR_IF_ABOVE_MAX(eep_type_ndx, SOC_PPC_EG_ENCAP_EEP_TYPE_NDX_MAX, SOC_PPC_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }
  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, eep_ndx, ARAD_PP_EG_ENCAP_EEP_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(depth, ARAD_PP_EG_ENCAP_DEPTH_MAX, ARAD_PP_EG_ENCAP_DEPTH_OUT_OF_RANGE_ERR, 30, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_entry_get_verify()", 0, eep_ndx);
}





uint32
arad_pp_eg_encap_entry_data_info_to_overlay_arp_encap_info_unsafe(
 SOC_SAND_IN  int                                    unit, 
   SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO          *exact_data_info
   )
{
    uint32
       res = SOC_SAND_OK; 
    
    uint32 prge_data_relative_index; 
    uint32 prge_data_base_index;     
    uint32 prge_data_absolute_index; 
    ARAD_PP_EPNI_PRGE_DATA_TBL_DATA prge_data; 
    SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO overlay_arp_encap_info; 

    SOC_PPC_EG_ENCAP_DATA_INFO *data_info; 

    

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    data_info = &(exact_data_info->entry_val.data_info); 

    SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO_clear(&overlay_arp_encap_info);

    
    arad_pp_eg_encap_overlay_arp_data_from_data_entry_buffer(unit, 
                                                             &overlay_arp_encap_info, 
                                                             data_info);
    
    
    prge_data_relative_index = overlay_arp_encap_info.ll_vsi_pointer;

    
    res  = arad_sw_db_eg_encap_prge_tbl_overlay_arp_entries_base_index_get(unit, &prge_data_base_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    
    prge_data_absolute_index = prge_data_base_index + prge_data_relative_index;

    
    res = arad_pp_epni_prge_data_tbl_get_unsafe(unit, prge_data_absolute_index, &prge_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

    
    res = arad_pp_eg_encap_overlay_arp_data_from_prge_buffer(unit, &overlay_arp_encap_info, prge_data.prge_data_entry);

    
    exact_data_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_OVERLAY_ARP_ENCAP;

    soc_sand_os_memcpy(&exact_data_info->entry_val.overlay_arp_encap_info, 
                       &overlay_arp_encap_info, 
                       sizeof(SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO));



    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_entry_data_info_to_overlay_arp_encap_info_unsafe()", 0, 0); 
    
}


uint32
  arad_pp_eg_encap_ipv6_entry_get_references(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_OUT  uint32                                 *sip_index,
    SOC_SAND_OUT  uint32                                 *dip_index,
    SOC_SAND_OUT  uint8                                  *found
  )
{
    uint32
      res = SOC_SAND_OK;
    SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO       
        ipv6_encap_info;
    SOC_PPC_EG_ENCAP_ENTRY_INFO
        encap_entry_info[2];
    uint8
        next_eep_valid;
    uint32
        next_eep[2];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found = 0;

    if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LARGE) {
        
        res = arad_pp_eg_encap_tunnel_entry_get_unsafe(
                  unit,
                  tunnel_eep_ndx,
                  &encap_entry_info[0],
                  &next_eep[0]
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);            
        if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
            uint32 outlif_value = 0;
            
            res = arad_pp_occ_mgmt_app_get(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_IPV6_TUNNEL_TOR,&(encap_entry_info[0].entry_val.ipv4_encap_info.outlif_profile), &outlif_value);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
            if (outlif_value != 0) { 
                *found  = 1;
                *sip_index = encap_entry_info[0].entry_val.ipv4_encap_info.dest.src_index;
            }
        }
    } else {
        
        res = arad_pp_eg_encap_data_entry_get_unsafe(
                unit,
                tunnel_eep_ndx,
                &encap_entry_info[0],
                &next_eep_valid, 
                &next_eep[0]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        

        
        if(arad_pp_eg_encap_data_entry_exact_type_get_unsafe(unit,&(encap_entry_info[0].entry_val.data_info)) == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP) {
            if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LEGACY) {
                
                *found  = 1;

                SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_clear(&ipv6_encap_info);       

                
                res = arad_pp_eg_encap_ipv6_entry_from_data_buffer(unit,sip_index,dip_index,&ipv6_encap_info,&encap_entry_info[0].entry_val.data_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            } else {
                
                res = arad_pp_eg_encap_tunnel_entry_get_unsafe(
                      unit,
                      next_eep[0],
                      &encap_entry_info[1],
                      &next_eep[1]
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);            
                if (encap_entry_info[1].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
                    *found  = 1;
                    *sip_index = encap_entry_info[1].entry_val.ipv4_encap_info.dest.src_index;
                }
            }            
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv6_entry_get_references()", tunnel_eep_ndx, 0);
}

#define ARAD_PP_EPNI_MPLS_CMD_TABLE_ENTRY_SIZE                      (1)


uint32
  arad_pp_eg_encap_push_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  )
{
  uint32
    mpls_is_pipe,
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_MPLS_CMD_TABLE_ENTRY_SIZE];
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);
 

  res = soc_mem_read(
          unit,
          EPNI_MPLS_CMD_PROFILEm,
          MEM_BLOCK_ANY,
          profile_ndx,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  soc_mem_field32_set(unit, EPNI_MPLS_CMD_PROFILEm, data, CWf, profile_info->has_cw);
  soc_mem_field32_set(unit, EPNI_MPLS_CMD_PROFILEm, data, EXPf, profile_info->exp);
  soc_mem_field32_set(unit, EPNI_MPLS_CMD_PROFILEm, data, TTLf, profile_info->ttl);
  soc_mem_field32_set(unit, EPNI_MPLS_CMD_PROFILEm, data, REMARK_PROFILEf, profile_info->remark_profile);

  mpls_is_pipe = profile_info->model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE ? 0x1 : 0x0;

  if (!SOC_IS_JERICHO_PLUS(unit)) {
      
      soc_mem_field32_set(unit, EPNI_MPLS_CMD_PROFILEm, data, MODELf, mpls_is_pipe);
      soc_mem_field32_set(unit, EPNI_MPLS_CMD_PROFILEm, data, ADD_ENTROPY_LABELf, profile_info->add_entropy_label);
  } else { 
      
      uint32 is_exp_set, is_ttl_set;
      uint32 reserved_label_profile_1=0, reserved_label_profile_2=0,reserved_label_profile_3=0;

      is_ttl_set = (profile_info->ttl_model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET) ? 1 : 0;
      is_exp_set = (profile_info->exp_model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET) ? 1 : 0;

      soc_mem_field32_set(unit, EPNI_MPLS_CMD_PROFILEm, data, TTL_MODELf, is_ttl_set);
      soc_mem_field32_set(unit, EPNI_MPLS_CMD_PROFILEm, data, EXP_MODELf, is_exp_set);

      
      if (profile_info->add_entropy_label) {
          reserved_label_profile_1 = (profile_info->add_entropy_label) ? 1 : 0;
          reserved_label_profile_2 = (profile_info->add_entropy_label_indicator) ? 2 : 0;
      } else if (profile_info->rsv_mpls_label_bmp) {
          reserved_label_profile_1 = (profile_info->rsv_mpls_label_bmp  == bcmMplsSpecialLabelPushMeaningful1)?3:
                                                 (profile_info->rsv_mpls_label_bmp  == bcmMplsSpecialLabelPushMeaningful12)?3:
                                                 (profile_info->rsv_mpls_label_bmp  == bcmMplsSpecialLabelPushMeaningful13)?3:
                                                 (profile_info->rsv_mpls_label_bmp  == bcmMplsSpecialLabelPushMeaningful123)?3:0;
         reserved_label_profile_2 = (profile_info->rsv_mpls_label_bmp  == bcmMplsSpecialLabelPushMeaningful2)?3:
                                                (profile_info->rsv_mpls_label_bmp  == bcmMplsSpecialLabelPushMeaningful12)?3:
                                                (profile_info->rsv_mpls_label_bmp  == bcmMplsSpecialLabelPushMeaningful23)?3:
                                                (profile_info->rsv_mpls_label_bmp  == bcmMplsSpecialLabelPushMeaningful123)?3:0;
        reserved_label_profile_3 = (profile_info->rsv_mpls_label_bmp  == bcmMplsSpecialLabelPushMeaningful3)?3:
                                               (profile_info->rsv_mpls_label_bmp  == bcmMplsSpecialLabelPushMeaningful13)?3:
                                               (profile_info->rsv_mpls_label_bmp  == bcmMplsSpecialLabelPushMeaningful23)?3:
                                               (profile_info->rsv_mpls_label_bmp  == bcmMplsSpecialLabelPushMeaningful123)?3:0;
      }
      soc_mem_field32_set(unit, EPNI_MPLS_CMD_PROFILEm, data, RESERVED_MPLS_PROFILE_1f, reserved_label_profile_1);
      soc_mem_field32_set(unit, EPNI_MPLS_CMD_PROFILEm, data, RESERVED_MPLS_PROFILE_2f, reserved_label_profile_2);
      soc_mem_field32_set(unit, EPNI_MPLS_CMD_PROFILEm, data, RESERVED_MPLS_PROFILE_3f, reserved_label_profile_3);
  }
 
  res = soc_mem_write(
          unit,
          EPNI_MPLS_CMD_PROFILEm,
          MEM_BLOCK_ANY,
          profile_ndx,
          data
        );
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_push_profile_info_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_eg_encap_push_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, ARAD_PP_EG_ENCAP_PROFILE_NDX_MAX, ARAD_PP_EG_ENCAP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  if (SOC_IS_JERICHO_PLUS(unit) && profile_info->add_entropy_label_indicator && !(profile_info->add_entropy_label)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Error: ELI cannot be set without setting EL\n")));
  }
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO,unit, profile_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_push_profile_info_set_verify()", profile_ndx, 0);
}

uint32
  arad_pp_eg_encap_push_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, ARAD_PP_EG_ENCAP_PROFILE_NDX_MAX, ARAD_PP_EG_ENCAP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_push_profile_info_get_verify()", profile_ndx, 0);
}


uint32
  arad_pp_eg_encap_push_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  )
{
   uint32
    mpls_is_pipe,
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_MPLS_CMD_TABLE_ENTRY_SIZE];
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = soc_mem_read(
          unit,
          EPNI_MPLS_CMD_PROFILEm,
          MEM_BLOCK_ANY,
          profile_ndx,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  profile_info->has_cw            = soc_mem_field32_get(unit, EPNI_MPLS_CMD_PROFILEm, data, CWf);
  profile_info->exp               = soc_mem_field32_get(unit, EPNI_MPLS_CMD_PROFILEm, data, EXPf);
  profile_info->ttl               = soc_mem_field32_get(unit, EPNI_MPLS_CMD_PROFILEm, data, TTLf);
  profile_info->remark_profile    = soc_mem_field32_get(unit, EPNI_MPLS_CMD_PROFILEm, data, REMARK_PROFILEf);

  if (!SOC_IS_JERICHO_PLUS(unit)){
      mpls_is_pipe                    = soc_mem_field32_get(unit, EPNI_MPLS_CMD_PROFILEm, data, MODELf);
      profile_info->add_entropy_label = soc_mem_field32_get(unit, EPNI_MPLS_CMD_PROFILEm, data, ADD_ENTROPY_LABELf);  
      profile_info->model  = mpls_is_pipe ? SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE : SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
  } else {
                 
      uint32 is_exp_set, is_ttl_set;
      uint32 reserved_label_profile_1, reserved_label_profile_2;

      is_ttl_set = soc_mem_field32_get(unit, EPNI_MPLS_CMD_PROFILEm, data, TTL_MODELf);
      is_exp_set = soc_mem_field32_get(unit, EPNI_MPLS_CMD_PROFILEm, data, EXP_MODELf);

      profile_info->ttl_model = is_ttl_set ? SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET : SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY;
      profile_info->exp_model = is_exp_set ? SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET : SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY;

      reserved_label_profile_1 = soc_mem_field32_get(unit, EPNI_MPLS_CMD_PROFILEm, data, RESERVED_MPLS_PROFILE_1f);
      reserved_label_profile_2 = soc_mem_field32_get(unit, EPNI_MPLS_CMD_PROFILEm, data, RESERVED_MPLS_PROFILE_2f);

      profile_info->add_entropy_label = (reserved_label_profile_1 == 1) ? 1 : 0;
      profile_info->add_entropy_label_indicator = (reserved_label_profile_2 == 2) ? 1 : 0;


  }

  profile_info->exp_mark_mode = SOC_PPC_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_push_profile_info_get_unsafe()", profile_ndx, 0);
}



uint32
  arad_pp_eg_encap_push_exp_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                          exp
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(exp_key);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_push_exp_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_encap_push_exp_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                          exp
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_push_exp_info_set_verify()", 0, 0);
}

uint32
  arad_pp_eg_encap_push_exp_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY               *exp_key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_push_exp_info_get_verify()", 0, 0);
}


uint32
  arad_pp_eg_encap_push_exp_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_EXP                          *exp
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(exp_key);
  SOC_SAND_CHECK_NULL_INPUT(exp);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_push_exp_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_encap_pwe_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  )
{
  uint32 res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_MPLS_CONTROL_WORDr, SOC_CORE_ALL, 0, MPLS_CONTROL_WORDf,  glbl_info->cw));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_pwe_glbl_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_encap_pwe_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO, glbl_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_pwe_glbl_info_set_verify()", 0, 0);
}

uint32
  arad_pp_eg_encap_pwe_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_pwe_glbl_info_get_verify()", 0, 0);
}


uint32
  arad_pp_eg_encap_pwe_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  )
{
  uint32 res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_clear(glbl_info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_MPLS_CONTROL_WORDr, SOC_CORE_ALL, 0, MPLS_CONTROL_WORDf, &glbl_info->cw));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_pwe_glbl_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_encap_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_GLBL_INFO              *glbl_info
  )
{

  uint32 res;
  
  #ifdef BCM_88660_A0 
    uint64 field64;
  #endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_GRE_ETHERNET_TYPE_ETHERNETr, SOC_CORE_ALL, 0, GRE_ETHERNET_TYPE_ETHERNETf,  (uint32)glbl_info->l2_gre_prtcl_type));
  
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_IPV4_UNKNOWN_HEADER_CODE_CFGr, SOC_CORE_ALL, 0, IPV4_UNKNOWN_HEADER_CODE_ETHERNET_TYPEf,  (uint32)glbl_info->unkown_gre_prtcl_type));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_IPV4_UNKNOWN_HEADER_CODE_CFGr, SOC_CORE_ALL, 0, IPV4_UNKNOWN_HEADER_CODE_PROTOCOLf,  (uint32)glbl_info->unkown_ip_next_prtcl_type));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_ETHERNET_IPr, SOC_CORE_ALL, 0, ETHERNET_IPf,  (uint32)glbl_info->eth_ip_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_ETHERTYPE_DEFAULTr, SOC_CORE_ALL, 0, ETHERTYPE_DEFAULTf,  (uint32)glbl_info->unkown_ll_ether_type));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_ENTROPY_LABEL_MSBSr, SOC_CORE_ALL, 0, ENTROPY_LABEL_MSBSf,  (uint32)glbl_info->mpls_entry_label_msbs));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_ETHERTYPE_CUSTOMr, SOC_CORE_ALL, 0, ETHERTYPE_CUSTOM_1f,  (uint32)glbl_info->custom1_ether_type));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_ETHERTYPE_CUSTOMr, SOC_CORE_ALL, 0, ETHERTYPE_CUSTOM_2f,  (uint32)glbl_info->custom2_ether_type));

  
  if (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN) {
      
      if (SOC_IS_ARAD_B1_AND_BELOW(unit)) { 
          
          res = arad_egr_prog_vxlan_program_info_set(
                  unit,
                  glbl_info->vxlan_udp_dest_port
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      }
      #ifdef BCM_88660_A0
      
      else {
          COMPILER_64_SET(field64, 0, (uint32) glbl_info->vxlan_udp_dest_port);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, EPNI_CFG_VXLAN_UDP_FIELDSr, SOC_CORE_ALL, 0, CFG_UDP_DEST_PORT_VXLANf,  field64));
      }
      #endif

      
      res = arad_parser_vxlan_program_info_set(
              unit,
              glbl_info->vxlan_udp_dest_port
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_glbl_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_encap_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_GLBL_INFO              *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_GLBL_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_ENCAP_GLBL_INFO, glbl_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_glbl_info_set_verify()", 0, 0);
}

uint32
  arad_pp_eg_encap_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_GLBL_INFO_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_glbl_info_get_verify()", 0, 0);
}


uint32
  arad_pp_eg_encap_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_GLBL_INFO              *glbl_info
  )
{
  uint32 res,
      field32;

  
  #ifdef BCM_88660_A0 
    uint64 field64;
  #endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPC_EG_ENCAP_GLBL_INFO_clear(glbl_info);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_GRE_ETHERNET_TYPE_ETHERNETr, SOC_CORE_ALL, 0, GRE_ETHERNET_TYPE_ETHERNETf, &field32));
  glbl_info->l2_gre_prtcl_type = field32;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_IPV4_UNKNOWN_HEADER_CODE_CFGr, SOC_CORE_ALL, 0, IPV4_UNKNOWN_HEADER_CODE_ETHERNET_TYPEf, &field32));
  glbl_info->unkown_gre_prtcl_type = field32;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_IPV4_UNKNOWN_HEADER_CODE_CFGr, SOC_CORE_ALL, 0, IPV4_UNKNOWN_HEADER_CODE_PROTOCOLf, &field32));
  glbl_info->unkown_ip_next_prtcl_type = field32;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_ETHERNET_IPr, SOC_CORE_ALL, 0, ETHERNET_IPf, &field32));
  glbl_info->eth_ip_val = field32;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_ETHERTYPE_DEFAULTr, SOC_CORE_ALL, 0, ETHERTYPE_DEFAULTf, &field32));
  glbl_info->unkown_ll_ether_type = field32;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_ENTROPY_LABEL_MSBSr, SOC_CORE_ALL, 0, ENTROPY_LABEL_MSBSf, &field32));
  glbl_info->mpls_entry_label_msbs = field32;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_ETHERTYPE_CUSTOMr, SOC_CORE_ALL, 0, ETHERTYPE_CUSTOM_1f, &field32));
  glbl_info->custom1_ether_type = field32;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_ETHERTYPE_CUSTOMr, SOC_CORE_ALL, 0, ETHERTYPE_CUSTOM_2f, &field32));
  glbl_info->custom2_ether_type = field32;

  
  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      res = arad_parser_vxlan_program_info_get(
              unit,
              &glbl_info->vxlan_udp_dest_port
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
  #ifdef BCM_88660_A0
  
  else {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, EPNI_CFG_VXLAN_UDP_FIELDSr, SOC_CORE_ALL, 0, CFG_UDP_DEST_PORT_VXLANf, &field64));
      COMPILER_64_TO_32_LO(glbl_info->vxlan_udp_dest_port, field64);
  }
  #endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_glbl_info_get_unsafe()", 0, 0);
}



uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  uint32                                  src_ip
  )
{
  uint32
    entry_offset,
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    reg_val;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_UNSAFE);
  
	SOC_REG_ABOVE_64_CLEAR(&reg_val);

  res = READ_EPNI_IPV4_SIPr(unit, REG_PORT_ANY, reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  entry_offset = entry_ndx * ARAD_PP_EG_ENCAP_SIP_NOF_BITS;

  res = soc_sand_bitstream_set_any_field(
          &src_ip,
          entry_offset,
          ARAD_PP_EG_ENCAP_SIP_NOF_BITS,
          reg_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_EPNI_IPV4_SIPr(unit, REG_PORT_ANY, reg_val));
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_EPNI_IPV4_SIP_ROUTINGr(unit, REG_PORT_ANY, reg_val));
  }
#endif 


  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  uint32                                  src_ip
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, ARAD_PP_EG_ENCAP_ENTRY_NDX_MAX, ARAD_PP_EG_ENCAP_ENTRY_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_verify()", entry_ndx, 0);
}

uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, ARAD_PP_EG_ENCAP_ENTRY_NDX_MAX, ARAD_PP_EG_ENCAP_ENTRY_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_verify()", entry_ndx, 0);
}


uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT uint32                                  *src_ip
  )
{
  uint32
    entry_offset,
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    fld_val,
    reg_val;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(fld_val);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  res = READ_EPNI_IPV4_SIPr(unit, REG_PORT_ANY, reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  ARAD_FLD_FROM_REG_ABOVE_64(EPNI_IPV4_SIPr, IPV4_SIPf, fld_val, reg_val, 15, exit);
  
  
  entry_offset = entry_ndx * ARAD_PP_EG_ENCAP_SIP_NOF_BITS;

  res = soc_sand_bitstream_get_any_field(
          fld_val,
          entry_offset,
          ARAD_PP_EG_ENCAP_SIP_NOF_BITS,
          src_ip
        );

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl
  )
{
  uint32
    res = SOC_SAND_OK,
    tmp,
    fld_offset,
    fld_val;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_IPV4_TTLr, SOC_CORE_ALL, 0, IPV4_TTLf, &fld_val));
  
  fld_offset = entry_ndx * ARAD_PP_EG_ENCAP_TTL_NOF_BITS;

  tmp = SOC_SAND_PP_TTL_VAL_GET(ttl);
  res = soc_sand_bitstream_set_any_field(
          &tmp,
          fld_offset,
          ARAD_PP_EG_ENCAP_TTL_NOF_BITS,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_IPV4_TTLr, SOC_CORE_ALL, 0, IPV4_TTLf,  fld_val));

  
  res = READ_EPNI_IPV4_TTL_MODELr(unit, REG_PORT_ANY, &fld_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  tmp = SOC_SAND_PP_TOS_IS_UNIFORM_GET(ttl);

  
  tmp = !tmp;
  res = soc_sand_bitstream_set_any_field(
          &tmp,
          entry_ndx,
          1,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = WRITE_EPNI_IPV4_TTL_MODELr(unit, REG_PORT_ANY, fld_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe()", entry_ndx, 0);
}

uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, ARAD_PP_EG_ENCAP_TTL_INDEX_MAX, ARAD_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_verify()", entry_ndx, 0);
}

uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, ARAD_PP_EG_ENCAP_TTL_INDEX_MAX, ARAD_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  
  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_verify()", entry_ndx, 0);
}



uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl
  )
{
  uint32
    res = SOC_SAND_OK,
    tmp,
    ttl_val,
    fld_offset,
    fld_val;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ttl);
  
  fld_offset = entry_ndx * ARAD_PP_EG_ENCAP_TTL_NOF_BITS;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_IPV4_TTLr, SOC_CORE_ALL, 0, IPV4_TTLf, &fld_val));
  
  fld_offset = entry_ndx * ARAD_PP_EG_ENCAP_TTL_NOF_BITS;

  res = soc_sand_bitstream_get_any_field(
          &fld_val,
          fld_offset,
          ARAD_PP_EG_ENCAP_TTL_NOF_BITS,
          &tmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  ttl_val = tmp & 255;

  
  res = READ_EPNI_IPV4_TTL_MODELr(unit, REG_PORT_ANY, &fld_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = (fld_val & (1 << entry_ndx));
  tmp = !tmp;

  SOC_SAND_PP_TTL_SET(*ttl,ttl_val,tmp);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe()", entry_ndx, 0);
}


uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          tos
  )
{
  uint32
    res = SOC_SAND_OK,
    tmp,
    fld_offset;
  soc_reg_above_64_val_t
    reg_val;
  uint32
    reg_32_val;  
  uint64
      reg_64_val;
  int
      is_uniform;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  is_uniform = SOC_SAND_PP_TOS_IS_UNIFORM_GET(tos);

  if (!is_uniform){
      res = READ_EPNI_IPV4_TOSr(unit, REG_PORT_ANY, reg_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


      fld_offset = entry_ndx * ARAD_PP_EG_ENCAP_TOS_NOF_BITS;

      tmp = SOC_SAND_PP_TOS_VAL_GET(tos);

      res = soc_sand_bitstream_set_any_field(
              &tmp,
              fld_offset,
              ARAD_PP_EG_ENCAP_TTL_NOF_BITS,
              reg_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = WRITE_EPNI_IPV4_TOSr(unit, REG_PORT_ANY, reg_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  } else {

      res = READ_EPNI_IPV4_TUNNEL_REMARKr(unit, REG_PORT_ANY, &reg_64_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

      fld_offset = entry_ndx * ARAD_PP_EG_ENCAP_REMARK_NOF_BITS;

      tmp = SOC_SAND_PP_TOS_VAL_GET(tos);

      res = soc_sand_bitstream_set_any_field(
          &tmp,
          fld_offset,
          ARAD_PP_EG_ENCAP_REMARK_NOF_BITS,
          reg_val
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      COMPILER_64_SET(reg_64_val, reg_val[1], reg_val[0]);

      res = WRITE_EPNI_IPV4_TUNNEL_REMARKr(unit, REG_PORT_ANY, reg_64_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
  }

  
  res = READ_EPNI_IPV4_TOS_MODELr(unit, REG_PORT_ANY, &reg_32_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  tmp = SOC_SAND_PP_TOS_IS_UNIFORM_GET(tos);

  
  tmp = !tmp;
  res = soc_sand_bitstream_set_any_field(
      &tmp,
      entry_ndx,
      1,
      &reg_32_val
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = WRITE_EPNI_IPV4_TOS_MODELr(unit, REG_PORT_ANY, reg_32_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe()", entry_ndx, 0);
}


uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_tos_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          tos
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, ARAD_PP_EG_ENCAP_TOS_INDEX_MAX, ARAD_PP_EG_ENCAP_TOS_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_tunnel_glbl_tos_set_verify()", entry_ndx, 0);
}

uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_tos_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, ARAD_PP_EG_ENCAP_TOS_INDEX_MAX, ARAD_PP_EG_ENCAP_TOS_INDEX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_tunnel_glbl_tos_get_verify()", entry_ndx, 0);
}


uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_tos_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_TOS                          *tos
  )
{
  uint32
    res = SOC_SAND_OK,
    tmp=0,
    tos_val,
    fld_offset;
  soc_reg_above_64_val_t
    reg_val;
  uint32
    reg_32_val;  
  uint64
      reg_64_val;
  int
      is_uniform;

    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tos);
  
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  
  res = READ_EPNI_IPV4_TOS_MODELr(unit, REG_PORT_ANY, &reg_32_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  tmp = (reg_32_val & (1 << entry_ndx));
  tmp = !tmp;

  is_uniform = tmp;
 
  if (!is_uniform)
  {
      res = READ_EPNI_IPV4_TOSr(unit, REG_PORT_ANY, reg_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

      fld_offset = entry_ndx * ARAD_PP_EG_ENCAP_TOS_NOF_BITS;
  } else {
      res = READ_EPNI_IPV4_TUNNEL_REMARKr(unit, REG_PORT_ANY, &reg_64_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      COMPILER_64_TO_32_LO(reg_val[0], reg_64_val);
      COMPILER_64_TO_32_LO(reg_val[1], reg_64_val);

      fld_offset = entry_ndx * ARAD_PP_EG_ENCAP_REMARK_NOF_BITS;
  }

  res = soc_sand_bitstream_get_any_field(
          reg_val,
          fld_offset,
          ARAD_PP_EG_ENCAP_TTL_NOF_BITS,
          &tmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tos_val = tmp & 255;

  SOC_SAND_PP_TOS_SET(*tos,tos_val,is_uniform);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe()", entry_ndx, 0);
}


uint32
  arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_set_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint8                                is_exp_copy
  )
{
    uint32
      res = SOC_SAND_OK;
    uint32
      reg_val;  

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 88, exit, READ_EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r(unit, REG_PORT_ANY, &reg_val));
    soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, &reg_val, CFG_MPLS_PIPE_FIX_ENABLEf, is_exp_copy);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 88, exit, WRITE_EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r(unit, REG_PORT_ANY, reg_val));

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_set_unsafe()", is_exp_copy, 0);
}

uint32
  arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_set_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint8                                is_exp_copy
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" Error: Unsupported device\n")));
    }

    if ((is_exp_copy!=0) && (is_exp_copy!=1)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" Error: is_exp_copy must be 0 or 1 (set or unset)\n")));
    }

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_set_verify()", 0, 0);
}

uint32
  arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_OUT uint8                                *is_exp_copy
  )
{
    uint32
      res = SOC_SAND_OK;
    uint32
      reg_val;  

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 88, exit, READ_EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r(unit, REG_PORT_ANY, &reg_val));
    *is_exp_copy = soc_reg_field_get(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, reg_val, CFG_MPLS_PIPE_FIX_ENABLEf);

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_get_unsafe()", 0, 0);
}

uint32
  arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_get_verify(
    SOC_SAND_IN  int                                  unit
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" Error: Unsupported device\n")));
    }

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_get_verify()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_encap_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_eg_encap;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_encap_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_eg_encap;
}

uint32
  SOC_PPC_EG_ENCAP_RANGE_INFO_verify(
    int unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_RANGE_INFO *info
  )
{
  uint32
      ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ll_limit, (16*1024-1), ARAD_PP_EG_ENCAP_LL_LIMIT_OUT_OF_RANGE_ERR, 10, exit);
  
  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->ip_tnl_limit, ARAD_PP_EG_ENCAP_IP_TNL_LIMIT_MIN, (16*1024-1), ARAD_PP_EG_ENCAP_IP_TNL_LIMIT_OUT_OF_RANGE_ERR, 20, exit);

  for (ndx=0; ndx<SOC_DPP_DEFS_MAX(EG_ENCAP_NOF_BANKS); ndx++) {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->bank_access_phase[ndx], SOC_PPC_EG_ENCAP_ACCESS_PHASE_MAX(unit), ARAD_PP_NOF_EG_ENCAP_ACCESS_PHASE_OUT_OF_RANGE_ERROR,
                            30+ndx, exit);
  }
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_RANGE_INFO_verify()",0,0);
}

uint32 SOC_PPC_EG_ENCAP_PROTECTION_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PROTECTION_INFO *info)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    if (info->is_protection_valid) {
        SOC_SAND_ERR_IF_ABOVE_MAX(info->protection_pointer, SOC_DPP_DEFS_GET(unit, nof_failover_egress_ids),
                                  ARAD_PP_EG_ENCAP_PROTECTION_POINTER_OUT_OF_RANGE_ERR, 10, exit);
        SOC_SAND_ERR_IF_ABOVE_MAX(info->protection_pass_value, ARAD_PP_EG_ENCAP_PROTECTION_PASS_VAL_MAX,
                                  ARAD_PP_EG_ENCAP_PROTECTION_PASS_VAL_OUT_OF_RANGE_ERR, 20, exit);
    }

    SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_PROTECTION_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_SWAP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->swap_label, ARAD_PP_EG_ENCAP_SWAP_LABEL_MAX, ARAD_PP_EG_ENCAP_SWAP_LABEL_OUT_OF_RANGE_ERR, 10, exit);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_SWAP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_DATA_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_DATA_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_PWE_INFO_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_INFO *info
  )
{
  uint32 res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->label, ARAD_PP_EG_ENCAP_LABEL_MAX, ARAD_PP_EG_ENCAP_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->push_profile, ARAD_PP_EG_ENCAP_PUSH_PROFILE_MAX, ARAD_PP_EG_ENCAP_PUSH_PROFILE_OUT_OF_RANGE_ERR, 11, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_PROTECTION_INFO, unit, &(info->protection_info), 20, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_PWE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile, ARAD_PP_EG_ENCAP_TPID_PROFILE_MAX, ARAD_PP_EG_ENCAP_TPID_PROFILE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_POP_INFO_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->pop_type != SOC_PPC_MPLS_COMMAND_TYPE_POP) {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_POP_TYPE_OUT_OF_RANGE_ERR, 10, exit); 
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->model, ARAD_PP_EG_ENCAP_MODEL_MAX, ARAD_PP_EG_ENCAP_MODEL_OUT_OF_RANGE_ERR, 11,exit );
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO, &(info->ethernet_info), 12, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->pop_next_header, SOC_PPC_PKT_FRWRD_TYPE_BRIDGE, SOC_PPC_PKT_FRWRD_TYPE_MPLS, SOC_PPC_EG_ENCAP_POP_INFO_PKT_FRWRD_TYPE_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->out_vsi, SOC_DPP_DEFS_GET(unit, nof_out_vsi) - 1, ARAD_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR, 10, exit);
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_POP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tunnel_label, ARAD_PP_EG_ENCAP_TUNNEL_LABEL_MAX, ARAD_PP_EG_ENCAP_TUNNEL_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->push_profile, ARAD_PP_EG_ENCAP_PUSH_PROFILE_MAX, ARAD_PP_EG_ENCAP_PUSH_PROFILE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PPC_EG_ENCAP_MPLS_MAX_NOF_TUNNELS; ++ind)
  {
    ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO, unit, &(info->tunnels[ind]), 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_tunnels, ARAD_PP_EG_ENCAP_NOF_TUNNELS_MAX, ARAD_PP_EG_ENCAP_NOF_TUNNELS_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(
          info->orientation, SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS,
          ARAD_PP_EG_ENCAP_ORIENTATION_OUT_OF_RANGE_ERR, 12, exit
        );  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->out_vsi, SOC_DPP_DEFS_GET(unit, nof_out_vsi) - 1, ARAD_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR, 20, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_PROTECTION_INFO, unit, &(info->protection_info), 30, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_index, ARAD_PP_EG_ENCAP_SRC_INDEX_MAX, ARAD_PP_EG_ENCAP_SRC_INDEX_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl_index, ARAD_PP_EG_ENCAP_TTL_INDEX_MAX, ARAD_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tos_index, ARAD_PP_EG_ENCAP_TOS_INDEX_MAX, ARAD_PP_EG_ENCAP_TOS_INDEX_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO, &(info->dest), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->out_vsi, SOC_DPP_DEFS_GET(unit, nof_out_vsi) - 1, ARAD_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO, &(info->tunnel), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid, SOC_SAND_PP_VLAN_ID_MAX, ARAD_PP_EG_ENCAP_MIRROR_VLAN_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->encap_id, ARAD_PP_EG_ENCAP_NOF_MIRRORS, SOC_SAND_VALUE_ABOVE_MAX_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO, &(info->tunnel), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_LL_INFO_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_LL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->out_vid, SOC_DPP_DEFS_GET(unit, nof_out_vsi) - 1, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_index, ARAD_PP_EG_ENCAP_TPID_INDEX_MAX, ARAD_PP_EG_ENCAP_TPID_INDEX_OUT_OF_RANGE_ERR, 15, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ll_remark_profile, ARAD_PP_EG_LL_ENCAP_REMARK_PROFILE_INDEX_MAX, ARAD_PP_EG_LL_ENCAP_REMARK_PROFILE_INDEX_OUT_OF_RANGE_ERR, 20, exit);
  if (info->out_ac_valid) {
      ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, info->out_ac_lsb, SOC_PPC_AC_ID_OUT_OF_RANGE_ERR, 30, exit); 
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_LL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_verify(
    SOC_SAND_IN    int unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_NOF(info->out_vsi, SOC_DPP_DEFS_GET(unit, nof_out_vsi), ARAD_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR, 10, exit);

  if (SOC_IS_JERICHO_PLUS(unit)) {
      SOC_SAND_ERR_IF_ABOVE_NOF(info->outrif_profile_index, SOC_DPP_CONFIG(unit)->l3.nof_rif_profiles, ARAD_PP_EG_ENCAP_OUT_RIF_PROFILE_OUT_OF_RANGE_ERR, 15, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(info->remark_profile, ARAD_PP_EG_VSI_ENCAP_REMARK_PROFILE_INDEX_MAX, ARAD_PP_EG_VSI_ENCAP_REMARK_PROFILE_INDEX_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_verify()",0,0);
}



uint32
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_verify(
     SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

  
  if (SOC_IS_JERICHO_PLUS(unit)) {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->exp_model, ARAD_PP_EG_ENCAP_MODEL_MAX, ARAD_PP_EG_ENCAP_MODEL_OUT_OF_RANGE_ERR, 10, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl_model, ARAD_PP_EG_ENCAP_MODEL_MAX, ARAD_PP_EG_ENCAP_MODEL_OUT_OF_RANGE_ERR, 10, exit);
  } else {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->model, ARAD_PP_EG_ENCAP_MODEL_MAX, ARAD_PP_EG_ENCAP_MODEL_OUT_OF_RANGE_ERR, 10, exit);
  }
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->exp_mark_mode, SOC_PPC_EG_ENCAP_EXP_MARK_MODE_MAX, SOC_PPC_EG_ENCAP_EXP_MARK_MODE_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->exp, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->add_entropy_label, ARAD_PP_EG_ENCAP_ADD_ENTROPY_LABEL_MAX, ARAD_PP_EG_ENCAP_ADD_ENTROPY_LABEL_OUT_OF_RANGE_ERR, 15, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->remark_profile, ARAD_PP_EG_VSI_ENCAP_REMARK_PROFILE_INDEX_MAX, ARAD_PP_EG_VSI_ENCAP_REMARK_PROFILE_INDEX_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->push_profile, ARAD_PP_EG_ENCAP_PUSH_PROFILE_MAX, ARAD_PP_EG_ENCAP_PUSH_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cw, ARAD_PP_EG_ENCAP_CW_MAX, ARAD_PP_EG_ENCAP_CW_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_ENTRY_VALUE_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ENTRY_VALUE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_AC_INFO, unit, &(info->out_ac),  11, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_ENCAP_SWAP_INFO, &(info->swap_info), 12, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_PWE_INFO, unit, &(info->pwe_info), 13, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_POP_INFO, unit, &(info->pop_info), 14, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO, unit, &(info->mpls_encap_info), 15, exit);
  ARAD_PP_STRUCT_VERIFY_UNIT(SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO, unit, &(info->ipv4_encap_info), 16, exit);
  res = SOC_PPC_EG_ENCAP_LL_INFO_verify(unit, &(info->ll_info));
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_ENTRY_VALUE_verify()",0,0);
}

uint32
  SOC_PPC_EG_ENCAP_ENTRY_INFO_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ENTRY_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->entry_type, SOC_PPC_EG_ENCAP_ENTRY_TYPE_MAX, SOC_PPC_EG_ENCAP_ENTRY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_EG_ENCAP_ENTRY_VALUE_verify(unit, &(info->entry_val));
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_ENTRY_INFO_verify()",0,0);
}



#define ARAD_PP_EG_ENCAP_NOF_MY_NICK_NAMES 4

uint32
   SOC_PPC_EG_ENCAP_TRILL_INFO_verify(
      SOC_SAND_IN int                          unit, 
      SOC_SAND_IN SOC_PPC_EG_ENCAP_TRILL_INFO *info) {
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    SOC_SAND_ERR_IF_ABOVE_NOF(info->my_nickname_index, 4, SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);
    
    SOC_SAND_ERR_IF_ABOVE_MAX(info->m, (1 << SOC_SAND_PP_TRILL_M_NOF_BITS) -1, SOC_SAND_VALUE_ABOVE_MAX_ERR, 12, exit);
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_TRILL_INFO_verify()", 0, 0); 
}




uint32
arad_pp_eg_encap_overlay_arp_data_entry_add_unsafe(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint32                                 overlay_ll_eep_ndx, 
     SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ll_encap_info
   ) 
{   
  uint32 
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(ll_encap_info);

  
  if (SOC_IS_ARADPLUS(unit)) {

      SOC_PPC_EG_ENCAP_DATA_INFO data_info;  

      ARAD_PP_EPNI_PRGE_DATA_TBL_DATA prge_data;
      uint32 prge_data_relative_index; 
      uint32 prge_data_base_index;     
      uint32 prge_data_absolute_index; 
      uint8  prge_data_first_appear;
      SOC_SAND_SUCCESS_FAILURE  prge_data_success;

      SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);

      
      res = arad_pp_eg_encap_overlay_arp_data_to_prge_buffer(
         unit, 
         ll_encap_info,   
         prge_data.prge_data_entry);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      
      res  = arad_sw_db_multiset_add(
         unit, 
		 ARAD_SW_DB_CORE_ANY,
         ARAD_PP_SW_DB_MULTI_SET_ENTRY_OVERLAY_ARP_PROG_DATA_ENTRY, 
         prge_data.prge_data_entry,
         &prge_data_relative_index,    
         &prge_data_first_appear,
         &prge_data_success
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

      
      res  = arad_sw_db_eg_encap_prge_tbl_overlay_arp_entries_base_index_get(unit, &prge_data_base_index);
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      
      prge_data_absolute_index = prge_data_base_index + prge_data_relative_index;

      
      res = arad_pp_epni_prge_data_tbl_set_unsafe(unit, prge_data_absolute_index, &prge_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

      
      ll_encap_info->ll_vsi_pointer = prge_data_relative_index;

      
      res = arad_pp_eg_encap_overlay_arp_data_to_data_entry_buffer(unit, ll_encap_info, &data_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

      
      res = arad_pp_eg_encap_data_lif_entry_add_unsafe(unit, overlay_ll_eep_ndx, &data_info, FALSE, 0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  } 

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_overlay_arp_data_entry_add_unsafe()",0,0);

}



uint32 
SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO_verify(SOC_SAND_IN SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *info) { 
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0); 
    SOC_SAND_CHECK_NULL_INPUT(info); 
    
    SOC_SAND_MAGIC_NUM_VERIFY(info); 
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO_verify()", 0, 0); 
}

uint32
  arad_pp_eg_encap_overlay_arp_data_entry_add_verify(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint32                                 overlay_ll_eep_ndx, 
     SOC_SAND_IN  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ll_encap_info
   ) 
{
  uint32
    res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, overlay_ll_eep_ndx, ARAD_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR, 10, exit);
    ARAD_PP_STRUCT_VERIFY(SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO, ll_encap_info, 20, exit);


    ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_overlay_arp_data_entry_add_verify()",0,0);
}

uint32
  arad_pp_eg_encap_mpls_default_ttl_set(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint8                                  ttl_val
   ) 
{
  uint32
      res = SOC_SAND_OK,
      data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = READ_EPNI_PP_CONFIGr(unit, REG_PORT_ANY, &data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    soc_reg_field_set(unit, EPNI_PP_CONFIGr, &data, DEFAULT_TTLf, ttl_val);

    res = WRITE_EPNI_PP_CONFIGr(unit, SOC_CORE_ALL, data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mpls_default_ttl_set()",0,0);
}

uint32
  arad_pp_eg_encap_mpls_default_ttl_get(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_OUT uint8                                  *ttl_val
   ) 
{
  uint32
      res = SOC_SAND_OK,
      data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = READ_EPNI_PP_CONFIGr(unit, REG_PORT_ANY, &data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    *ttl_val = (uint8) soc_reg_field_get(unit, EPNI_PP_CONFIGr, data, DEFAULT_TTLf);

    ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_mpls_default_ttl_get()",0,0);
}

STATIC soc_error_t 
    arad_pp_eg_encap_trill_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  eep_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                 *encap_info,
    SOC_SAND_OUT uint32                                  *next_eep){

    uint32 res = SOC_SAND_OK; 


    ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT tbl_data; 

    SOC_PPC_EG_ENCAP_EEP_TYPE                  
        eep_type_ndx = SOC_PPC_EG_ENCAP_EEP_TYPE_TRILL;
    ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
        cur_eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    SOC_SAND_CHECK_NULL_INPUT(encap_info);
    SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(encap_info); 
    soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

    

    res = arad_pp_eg_encap_access_key_prefix_type_get_unsafe(       
       unit, eep_ndx, &cur_eep_type);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    ARAD_PP_EG_ENCAP_VERIFY_EEP_TYPE_COMPATIBLE_TO_ACCESS_TYPE(
       eep_ndx, eep_type_ndx, cur_eep_type); 

    
    res = arad_pp_eg_encap_access_trill_entry_format_tbl_get_unsafe(
       unit, eep_ndx, &tbl_data); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    encap_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_TRILL_ENCAP; 

    
    encap_info->entry_val.trill_encap_info.m = tbl_data.m; 
    encap_info->entry_val.trill_encap_info.my_nickname_index = tbl_data.my_nickname_index; 
    encap_info->entry_val.trill_encap_info.nick = tbl_data.nick; 
    encap_info->entry_val.trill_encap_info.outlif_profile = tbl_data.outlif_profile; 
    encap_info->entry_val.trill_encap_info.ll_eep_ndx = tbl_data.next_outlif;
    encap_info->entry_val.trill_encap_info.ll_eep_is_valid = tbl_data.next_outlif_valid;
    encap_info->entry_val.trill_encap_info.remark_profile = tbl_data.remark_profile;

    
    if (tbl_data.next_outlif_valid) {
        *next_eep = tbl_data.next_outlif; 
    }  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_trill_entry_get()",0,0);
}

soc_error_t
    arad_pp_eg_trill_entry_set(
       SOC_SAND_IN int                          unit, 
       SOC_SAND_IN uint32                       trill_eep_ndx, 
       SOC_SAND_IN SOC_PPC_EG_ENCAP_TRILL_INFO* trill_encap_info
       ) {

    uint32 res = SOC_SAND_OK; 
    ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT tbl_data; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

    
    ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, trill_eep_ndx, ARAD_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR, 10, exit);
    res = SOC_PPC_EG_ENCAP_TRILL_INFO_verify(unit, trill_encap_info); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

    
    tbl_data.my_nickname_index = trill_encap_info->my_nickname_index; 
    tbl_data.m = trill_encap_info->m; 
    tbl_data.nick = trill_encap_info->nick; 
    tbl_data.outlif_profile = trill_encap_info->outlif_profile; 
    tbl_data.next_outlif = trill_encap_info->ll_eep_ndx;
    tbl_data.next_outlif_valid = trill_encap_info->ll_eep_is_valid;
    tbl_data.remark_profile = trill_encap_info->remark_profile;

    
    res = arad_pp_eg_encap_access_trill_entry_format_tbl_set_unsafe(unit, trill_eep_ndx, &tbl_data); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_trill_entry_set()", 0, 0); 
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


