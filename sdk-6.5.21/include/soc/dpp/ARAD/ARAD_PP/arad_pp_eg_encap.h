
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_EG_ENCAP_INCLUDED__

#define __ARAD_PP_EG_ENCAP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_eg_encap.h>
#include <soc/dpp/PPC/ppc_api_general.h>





#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_IN_CUD_NOF_BITS 17
#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_OUT_CUD_NOF_BITS 21

#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_EEDB 19
#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_NOF_BITS_IN_DSCP 2

#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_DATA_INDEX_START 0
#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_DATA_INDEX_NOF_BITS 2

#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_MEM_INDEX_START \
        (ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_DATA_INDEX_START + ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_DATA_INDEX_NOF_BITS)
#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_MEM_INDEX_NOF_BITS 11

#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_ARRAY_INDEX_START \
        (ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_MEM_INDEX_START + ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_MEM_INDEX_NOF_BITS)
#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_EEDB_ARRAY_INDEX_NOF_BITS 4

#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_DATA_INDEX_START 0
#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_DATA_INDEX_NOF_BITS 4

#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_MEM_INDEX_START \
        (ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_DATA_INDEX_START + ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_DATA_INDEX_NOF_BITS)
#define ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_DSCP_MEM_INDEX_NOF_BITS 13

#define ARAD_PP_EG_ENCAP_NOF_ENTRIES_PER_EEDB_BANK(_unit)       (2 * SOC_DPP_DEFS_GET(_unit, nof_eedb_lines_per_bank))
#define ARAD_PP_EG_ENCAP_NOF_ENTRIES_PER_HALF_EEDB_BANK(_unit)  (ARAD_PP_EG_ENCAP_NOF_ENTRIES_PER_EEDB_BANK(_unit)/2)
#define ARAD_PP_EG_ENCAP_NOF_TOP_BANKS_PER_EEDB_BANK            (4)
#define ARAD_PP_EG_ENCAP_NOF_REGULAR_EEDB_BANKS(_unit)          (SOC_DPP_DEFS_GET(_unit, eg_encap_nof_banks)    \
                                                                    - (SOC_DPP_DEFS_GET(unit, eg_encap_nof_top_banks)  \
                                                                       / ARAD_PP_EG_ENCAP_NOF_TOP_BANKS_PER_EEDB_BANK))
#define ARAD_PP_EG_ENCAP_NOF_ENTRIES_PER_TOP_BANK(unit)               (SOC_IS_QUX(unit) ? (256) : (1024))

#define ARAD_PP_EG_ENCAP_PROG_DATA_ENTRY_SIZE    (4)

#define ARAD_PP_EG_ENCAP_EEDB_SIZE(unit)          (((SOC_PPC_EG_ENCAP_BANK_NDX_MAX(unit)+1)*SOC_PPC_EG_ENCAP_NOF_BANKS(unit)))


#define ARAD_PP_EG_ENCAP_VERIFY_EEP_TYPE_COMPATIBLE_TO_ACCESS_TYPE(outlif,eep_type, access_type)   \
    if (!(access_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE) && (outlif != 0))                   \
    {                                                                               \
    switch (eep_type) {                                                             \
    case SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP:                                         \
      if ((access_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_POP) &&           \
          (access_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_SWAP) &&          \
          (access_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_TUNNEL))            \
      {                                                                            \
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_INCOMPATIBLE_TYPE_ERR, 560, exit);    \
      }                                                                            \
      break;                                                                       \
    case SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP:                                     \
      if ((access_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_TUNNEL) &&        \
          (access_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_IP))                   \
      {                                                                            \
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_INCOMPATIBLE_TYPE_ERR, 570, exit);    \
      }                                                                            \
      break;                                                                        \
    case SOC_PPC_EG_ENCAP_EEP_TYPE_VSI:                                             \
      if (access_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF)                 \
      {                                                                             \
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_INCOMPATIBLE_TYPE_ERR, 580, exit);     \
      }                                                                             \
      break;                                                                        \
    case SOC_PPC_EG_ENCAP_EEP_TYPE_LL:                                              \
      if (access_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER )               \
      {           \
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_INCOMPATIBLE_TYPE_ERR, 590, exit);     \
      }                                                                             \
      break;                                                                        \
    case SOC_PPC_EG_ENCAP_EEP_TYPE_DATA:                                              \
      if (access_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA && access_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_IPV6 && access_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_ARP_OVERLAY) {           \
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_INCOMPATIBLE_TYPE_ERR, 595, exit);     \
      }                                                                             \
      break;                                                                        \
    case SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL:                                            \
      if (access_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ROO_LINK_LAYER)           \
      {                                                                              \
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_INCOMPATIBLE_TYPE_ERR, 596, exit);  \
      }                                                                              \
      break;                                                                         \
    case SOC_PPC_EG_ENCAP_EEP_TYPE_TRILL:                                            \
      if (access_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_TRILL)                    \
      {                                                                              \
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_INCOMPATIBLE_TYPE_ERR, 597, exit);  \
      }                                                                              \
      break;                                                                         \
    default:                                                                        \
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_INCOMPATIBLE_TYPE_ERR, 600, exit);       \
    }                                                                               \
    }                                                                               \
    else                                                                            \
    {                                                                               \
      ARAD_PP_DO_NOTHING_AND_EXIT;                                                  \
    }

#define ARAD_PP_EG_ENCAP_MPLS2_MAX_PUSH_PROFILE_VALUE (7)



#define ARAD_PP_EG_ENCAP_NOF_RIF_BANKS_TYPE(rif_max_id)  ( rif_max_id / _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK(unit) )
#define ARAD_PP_EG_ENCAP_NOF_RIF_BANKS_PHASE(rif_max_id) ((rif_max_id / _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_BANK(unit))*2  \
                                                        + (rif_max_id % _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_BANK(unit)) ? 2 : 0)





typedef enum
{
  
  SOC_PPC_EG_ENCAP_RANGE_INFO_SET = ARAD_PP_PROC_DESC_BASE_EG_ENCAP_FIRST,
  SOC_PPC_EG_ENCAP_RANGE_INFO_SET_PRINT,
  SOC_PPC_EG_ENCAP_RANGE_INFO_SET_UNSAFE,
  SOC_PPC_EG_ENCAP_RANGE_INFO_SET_VERIFY,
  SOC_PPC_EG_ENCAP_RANGE_INFO_GET,
  SOC_PPC_EG_ENCAP_RANGE_INFO_GET_PRINT,
  SOC_PPC_EG_ENCAP_RANGE_INFO_GET_VERIFY,
  SOC_PPC_EG_ENCAP_RANGE_INFO_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD,
  ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_PRINT,
  ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_UNSAFE,
  ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_VERIFY,
  ARAD_PP_EG_ENCAP_DATA_LIF_ENTRY_ADD,
  ARAD_PP_EG_ENCAP_DATA_LIF_ENTRY_ADD_PRINT,
  ARAD_PP_EG_ENCAP_DATA_LIF_ENTRY_ADD_UNSAFE,
  ARAD_PP_EG_ENCAP_DATA_LIF_ENTRY_ADD_VERIFY,
  ARAD_PP_EG_ENCAP_AC_ENTRY_ADD,
  ARAD_PP_EG_ENCAP_AC_ENTRY_ADD_PRINT,
  ARAD_PP_EG_ENCAP_AC_ENTRY_ADD_UNSAFE,
  ARAD_PP_EG_ENCAP_AC_ENTRY_ADD_VERIFY,
  ARAD_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD,
  ARAD_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_PRINT,
  ARAD_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_UNSAFE,
  ARAD_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_VERIFY,
  ARAD_PP_EG_ENCAP_PWE_ENTRY_ADD,
  ARAD_PP_EG_ENCAP_PWE_ENTRY_ADD_PRINT,
  ARAD_PP_EG_ENCAP_PWE_ENTRY_ADD_UNSAFE,
  ARAD_PP_EG_ENCAP_PWE_ENTRY_ADD_VERIFY,
  ARAD_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD,
  ARAD_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_PRINT,
  ARAD_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_UNSAFE,
  ARAD_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_VERIFY,
  ARAD_PP_EG_ENCAP_VSI_ENTRY_ADD,
  ARAD_PP_EG_ENCAP_VSI_ENTRY_ADD_PRINT,
  ARAD_PP_EG_ENCAP_VSI_ENTRY_ADD_UNSAFE,
  ARAD_PP_EG_ENCAP_VSI_ENTRY_ADD_VERIFY,
  ARAD_PP_EG_ENCAP_MPLS_ENTRY_ADD,
  ARAD_PP_EG_ENCAP_MPLS_ENTRY_ADD_UNSAFE,
  ARAD_PP_EG_ENCAP_MPLS_ENTRY_ADD_VERIFY,
  ARAD_PP_EG_ENCAP_IPV4_ENTRY_ADD,
  ARAD_PP_EG_ENCAP_IPV4_ENTRY_ADD_UNSAFE,
  ARAD_PP_EG_ENCAP_IPV4_ENTRY_ADD_VERIFY,
  ARAD_PP_EG_ENCAP_IPV6_ENTRY_ADD,
  ARAD_PP_EG_ENCAP_IPV6_ENTRY_ADD_UNSAFE,
  ARAD_PP_EG_ENCAP_IPV6_ENTRY_ADD_VERIFY,
  ARAD_PP_EG_ENCAP_MIRROR_ENTRY_SET,
  ARAD_PP_EG_ENCAP_MIRROR_ENTRY_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_MIRROR_ENTRY_SET_VERIFY,
  ARAD_PP_EG_ENCAP_MIRROR_ENTRY_GET,
  ARAD_PP_EG_ENCAP_MIRROR_ENTRY_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_PORT_ERSPAN_SET,
  ARAD_PP_EG_ENCAP_PORT_ERSPAN_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_PORT_ERSPAN_GET,
  ARAD_PP_EG_ENCAP_PORT_ERSPAN_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_LL_ENTRY_ADD,
  ARAD_PP_EG_ENCAP_LL_ENTRY_ADD_PRINT,
  ARAD_PP_EG_ENCAP_LL_ENTRY_ADD_UNSAFE,
  ARAD_PP_EG_ENCAP_LL_ENTRY_ADD_VERIFY,
  ARAD_PP_EG_ENCAP_ENTRY_REMOVE,
  ARAD_PP_EG_ENCAP_ENTRY_REMOVE_PRINT,
  ARAD_PP_EG_ENCAP_ENTRY_REMOVE_UNSAFE,
  ARAD_PP_EG_ENCAP_ENTRY_REMOVE_VERIFY,
  ARAD_PP_EG_ENCAP_ENTRY_GET,
  ARAD_PP_EG_ENCAP_ENTRY_GET_PRINT,
  ARAD_PP_EG_ENCAP_ENTRY_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_ENTRY_GET_VERIFY,
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_GET,
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_GET_UNSAFE,
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_GET_VERIFY,
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_SET,
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_SET_UNSAFE,
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_SET_VERIFY,
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_GET,
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_GET_VERIFY,
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_SET,
  ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_SET_VERIFY,
  ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_GET,
  ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_GET_VERIFY,
  ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_GET_UNSAFE,
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_SET,
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_SET_UNSAFE,
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_SET_VERIFY,
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_GET,
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_GET_VERIFY,
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_GET_UNSAFE,
  SOC_PPC_EG_ENCAP_GLBL_INFO_SET,
  SOC_PPC_EG_ENCAP_GLBL_INFO_SET_UNSAFE,
  SOC_PPC_EG_ENCAP_GLBL_INFO_SET_VERIFY,
  SOC_PPC_EG_ENCAP_GLBL_INFO_GET,
  SOC_PPC_EG_ENCAP_GLBL_INFO_GET_VERIFY,
  SOC_PPC_EG_ENCAP_GLBL_INFO_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_VERIFY,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_PRINT,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_VERIFY,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_PRINT,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_VERIFY,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_PRINT,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_VERIFY,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_PRINT,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_VERIFY,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_PRINT,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_VERIFY,
  ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_GET_PROCS_PTR,
  ARAD_PP_EG_ENCAP_GET_ERRS_PTR,
  
  ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_INIT,
  ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_INIT_UNSAFE,
  ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_INIT_VERIFY,
  ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET,
  ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET_VERIFY,
  ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET,
  ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET_UNSAFE,
  ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET_VERIFY,
  ARAD_PP_EG_ENCAP_LL_ENTRY_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_TUNNEL_ENTRY_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_LIF_ENTRY_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_VSI_ENTRY_GET_UNSAFE,
  ARAD_PP_EG_ENCAP_DATA_ENTRY_GET_UNSAFE,

  
  ARAD_PP_EG_ENCAP_PROCEDURE_DESC_LAST
} ARAD_PP_EG_ENCAP_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_EG_ENCAP_FIRST,
  ARAD_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_LL_EEP_NDX_OUT_OF_RANGE_ERR,
  SOC_PPC_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_EEP_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_DEPTH_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_ENTRY_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_SRC_IP_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_LL_LIMIT_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_IP_TNL_LIMIT_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_MPLS_TNL_LIMIT_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_SWAP_LABEL_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_LABEL_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_PUSH_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_TPID_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_POP_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_TUNNEL_LABEL_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_NOF_TUNNELS_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_ORIENTATION_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_DEST_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_SRC_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_TOS_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_PCP_DEI_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_TPID_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_MODEL_OUT_OF_RANGE_ERR,
  SOC_PPC_EG_ENCAP_EXP_MARK_MODE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_CW_OUT_OF_RANGE_ERR,
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_OUT_OF_RANGE_ERR,
  
  ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_UNSUPPORTED,
  ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_LIF_EEP_NDX_AC_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_LIF_EEP_AND_AC_ID_NOT_EQUAL_ERR,
  ARAD_PP_EG_ENCAP_LIF_EEP_NDX_MPLS_TUNNEL_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_LIF_EEP_NDX_PWE_MCAST_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_IP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_NOF_EG_ENCAP_ACCESS_PHASE_OUT_OF_RANGE_ERROR,
  ARAD_PP_EG_LL_ENCAP_REMARK_PROFILE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_VSI_ENCAP_REMARK_PROFILE_INDEX_OUT_OF_RANGE_ERR,
  SOC_PPC_EG_ENCAP_POP_INFO_PKT_FRWRD_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_ADD_ENTROPY_LABEL_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_INCOMPATIBLE_TYPE_ERR,
  ARAD_PP_EG_ENCAP_MIRROR_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_MIRROR_VLAN_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_IS_ERSPAN_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_PROTECTION_POINTER_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_PROTECTION_PASS_VAL_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_OUT_RIF_PROFILE_OUT_OF_RANGE_ERR,
  
  ARAD_PP_EG_ENCAP_ERR_LAST
} ARAD_PP_EG_ENCAP_ERR;


typedef struct arad_pp_eg_encap_eedb_bank_info_s
{
    uint8   valid;         
    uint8   phase;         
    uint16  app_bmp;         
    uint8   type;            
    uint8   is_extended;     
    uint8   ext_type;        
    uint32  force_flags;
    int     extension_bank;  
    int     nof_free_entries;
} arad_pp_eg_encap_eedb_bank_info_t;

typedef struct arad_pp_eg_encap_eedb_top_bank_info_s
{
    uint8 extension_bank_owner;    
} arad_pp_eg_encap_eedb_top_bank_info_t;











uint32
  arad_pp_eg_encap_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_eg_encap_entry_init_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx
  );



uint32
  arad_pp_eg_encap_range_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_RANGE_INFO                 *range_info
  );

uint32
  arad_pp_eg_encap_range_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_RANGE_INFO                 *range_info
  );

uint32
  arad_pp_eg_encap_range_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_eg_encap_range_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_RANGE_INFO                 *range_info
  );




uint32 arad_pp_eg_encap_protection_info_set_unsafe(
    SOC_SAND_IN int                                 unit,
    SOC_SAND_IN int                                 out_lif_id,
    SOC_SAND_IN uint8                               is_full_entry_extension,
    SOC_SAND_IN SOC_PPC_EG_ENCAP_PROTECTION_INFO    *protection_info);


uint32 arad_pp_eg_encap_protection_info_get_unsafe(
    SOC_SAND_IN int                                 unit,
    SOC_SAND_IN int                                 out_lif_id,
    SOC_SAND_IN uint8                               is_full_entry_extension,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PROTECTION_INFO   *protection_info);


uint32 arad_pp_eg_encap_protection_info_set_verify(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PROTECTION_INFO       *protection_info);



uint32
  arad_pp_eg_encap_null_lif_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  uint32                                  next_eep
  );

uint32
  arad_pp_eg_encap_null_lif_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  uint32                                  next_eep
  );

uint32
  arad_pp_eg_encap_lif_field_set_unsafe(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                                lif_eep_ndx,
      SOC_SAND_IN  uint32                                flags,
      SOC_SAND_IN  uint32                                val
  );

uint32
  arad_pp_eg_encap_lif_field_set_verify(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                                lif_eep_ndx,
      SOC_SAND_IN  uint32                                flags,
      SOC_SAND_IN  uint32                                val
  );


uint32
  arad_pp_eg_encap_lif_field_get_unsafe(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                                lif_eep_ndx,
      SOC_SAND_IN  uint32                                flags,
      SOC_SAND_OUT  uint32                               *val
  );


uint32
  arad_pp_eg_encap_data_local_to_global_cud_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_eg_encap_data_local_to_global_cud_init_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_eg_encap_data_local_to_global_cud_set_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  in_cud,
    SOC_SAND_IN  uint32                                  out_cud
  );

uint32
  arad_pp_eg_encap_data_local_to_global_cud_set_verify(
     SOC_SAND_IN  int                                  unit,
     SOC_SAND_IN  uint32                                  in_cud,
     SOC_SAND_OUT uint32                                  out_cud
   );


uint32
  arad_pp_eg_encap_data_local_to_global_cud_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  in_cud,
    SOC_SAND_OUT uint32*                                 out_cud
  );

uint32
  arad_pp_eg_encap_data_local_to_global_cud_get_verify(
     SOC_SAND_IN  int                                  unit,
     SOC_SAND_IN  uint32                                  in_cud,
     SOC_SAND_OUT uint32*                                 out_cud
   );

uint32
  arad_pp_eg_encap_data_lif_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO              *data_info,
    SOC_SAND_IN  uint8                                   next_eep_valid,
    SOC_SAND_IN  uint32                                  next_eep
  );

uint32
  arad_pp_eg_encap_data_lif_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO              *data_info,
    SOC_SAND_IN  uint8                                   next_eep_valid,
    SOC_SAND_IN  uint32                                  next_eep
  );


uint32
  arad_pp_eg_encap_ac_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_AC_ID                               ac_id
  );

uint32
  arad_pp_eg_encap_ac_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_AC_ID                               ac_id
  );


uint32
  arad_pp_eg_encap_swap_command_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                  next_eep
  );

uint32
  arad_pp_eg_encap_swap_command_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                  next_eep
  );


uint32
  arad_pp_eg_encap_pwe_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                  next_eep
  );

uint32
  arad_pp_eg_encap_pwe_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                  next_eep
  );


uint32
  arad_pp_eg_encap_pop_command_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                  next_eep
  );

uint32
  arad_pp_eg_encap_pop_command_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                  next_eep
  );


uint32
  arad_pp_eg_encap_vsi_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO             *vsi_info, 
    SOC_SAND_IN uint8                                  next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep
  );

uint32
  arad_pp_eg_encap_vsi_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO            *vsi_info, 
    SOC_SAND_IN  uint8                                 next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep
  );


uint32
  arad_pp_eg_encap_mpls_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  );

uint32
  arad_pp_eg_encap_mpls_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  );


uint32
  arad_pp_eg_encap_ipv4_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  );

uint32
  arad_pp_eg_encap_ipv4_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  );


uint32
  arad_pp_eg_encap_ipv6_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO     *ipv6_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  );

uint32
  arad_pp_eg_encap_ipv6_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO            *ipv6_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  );



uint32
arad_pp_eg_encap_overlay_arp_data_entry_add_unsafe(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint32                                 overlay_ll_eep_ndx, 
     SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ll_encap_info
   );

uint32
arad_pp_eg_encap_overlay_arp_data_entry_add_verify(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint32                                 overlay_ll_eep_ndx, 
     SOC_SAND_IN  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ll_encap_info
   );


uint32
  arad_pp_eg_encap_mpls_default_ttl_set(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint8                                  ttl_val
   );


uint32
  arad_pp_eg_encap_mpls_default_ttl_get(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_OUT uint8                                  *ttl_val
   );


uint32
arad_pp_eg_encap_overlay_arp_data_entry_get_unsafe(
   SOC_SAND_IN  int                                    unit,
   SOC_SAND_IN  uint32                                 overlay_ll_eep_ndx, 
   SOC_SAND_OUT SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *overlay_ll_encap_info

   );




uint32
  arad_pp_eg_encap_mirror_entry_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  );


uint32
  arad_pp_eg_encap_mirror_entry_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  );

uint32
  arad_pp_eg_encap_mirror_entry_set_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  );



uint32
  arad_pp_eg_encap_port_erspan_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  is_erspan
  );


uint32
  arad_pp_eg_encap_port_erspan_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *is_erspan
  );



uint32
  arad_pp_eg_encap_ll_entry_add_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                ll_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_LL_INFO                 *ll_encap_info
  );

uint32
  arad_pp_eg_encap_ll_entry_add_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                ll_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_LL_INFO                 *ll_encap_info
  );


uint32
  arad_pp_eg_encap_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx
  );

uint32
  arad_pp_eg_encap_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx
  );


uint32
  arad_pp_eg_encap_entry_type_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 eep_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_TYPE            *entry_type
  );

uint32
  arad_pp_eg_encap_entry_type_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  eep_ndx
  );

uint32
  arad_pp_eg_encap_entry_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE                       eep_type_ndx,
    SOC_SAND_IN  uint32                                      eep_ndx,
    SOC_SAND_IN  uint32                                      depth,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                     encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD],
    SOC_SAND_OUT uint32                                      next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD],
    SOC_SAND_OUT uint32                                      *nof_entries
  );

uint32
  arad_pp_eg_encap_entry_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE                       eep_type_ndx,
    SOC_SAND_IN  uint32                                      eep_ndx,
    SOC_SAND_IN  uint32                                      depth
  );





uint32
arad_pp_eg_encap_entry_data_info_to_overlay_arp_encap_info_unsafe(
 SOC_SAND_IN  int                                    unit, 
   SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO          *exact_data_info
   );


uint32
  arad_pp_eg_encap_push_profile_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO              *profile_info
  );

uint32
  arad_pp_eg_encap_push_profile_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO              *profile_info
  );

uint32
  arad_pp_eg_encap_push_profile_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx
  );


uint32
  arad_pp_eg_encap_push_profile_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO              *profile_info
  );


uint32
  arad_pp_eg_encap_push_exp_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                          exp
  );

uint32
  arad_pp_eg_encap_push_exp_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                          exp
  );

uint32
  arad_pp_eg_encap_push_exp_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY               *exp_key
  );


uint32
  arad_pp_eg_encap_push_exp_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_EXP                          *exp
  );


uint32
  arad_pp_eg_encap_pwe_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );

uint32
  arad_pp_eg_encap_pwe_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );

uint32
  arad_pp_eg_encap_pwe_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_eg_encap_pwe_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );

uint32
  arad_pp_eg_encap_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_GLBL_INFO              *glbl_info
  );
uint32
  arad_pp_eg_encap_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_GLBL_INFO              *glbl_info
  );
uint32
  arad_pp_eg_encap_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );
uint32
  arad_pp_eg_encap_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_GLBL_INFO              *glbl_info
  );

uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  uint32                                  src_ip
  );

uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  uint32                                  src_ip
  );

uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  );


uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT uint32                                  *src_ip
  );


uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl
  );

uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl
  );

uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  );


uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl
  );


uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          tos
  );

uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_tos_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          tos
  );

uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_tos_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  );


uint32
  arad_pp_eg_encap_ipv4_tunnel_glbl_tos_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_TOS                          *tos
  );


uint32
  arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_set_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint8                                is_exp_copy
  );

uint32
  arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_set_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint8                                is_exp_copy
  );


uint32
  arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_OUT uint8                                *is_exp_copy
  );

uint32
  arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_get_verify(
    SOC_SAND_IN  int                                  unit
  );




soc_error_t
    arad_pp_eg_trill_entry_set(
       SOC_SAND_IN int                          unit, 
       SOC_SAND_IN uint32                       trill_eep_ndx, 
       SOC_SAND_IN SOC_PPC_EG_ENCAP_TRILL_INFO* trill_encap_info
  ); 




CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_encap_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_encap_get_errs_ptr(void);

uint32
  SOC_PPC_EG_ENCAP_RANGE_INFO_verify(
     int unit, 
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_RANGE_INFO *info
  );

uint32 SOC_PPC_EG_ENCAP_PROTECTION_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PROTECTION_INFO *info);

uint32
  SOC_PPC_EG_ENCAP_SWAP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_SWAP_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_PWE_INFO_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_INFO *info
  );

uint32 
  SOC_PPC_EG_ENCAP_DATA_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_POP_INFO_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_LL_INFO_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_LL_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY *info
  );

uint32
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_GLBL_INFO *info
  );
uint32
  SOC_PPC_EG_ENCAP_ENTRY_VALUE_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ENTRY_VALUE *info
  );

uint32
  SOC_PPC_EG_ENCAP_ENTRY_INFO_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ENTRY_INFO *info
  );

uint32
  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *info
  );






#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


