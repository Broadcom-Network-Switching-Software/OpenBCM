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

#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON



#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/SAND/Utils/sand_rand.h>

#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>





#define ARAD_PP_GENERAL_VAL_MAX                                  (SOC_SAND_U32_MAX)
#define ARAD_PP_GENERAL_COMMAND_VAL_MAX                          16
#define ARAD_PP_GENERAL_PUSH_PROFILE_MAX                         (7)
#define ARAD_PP_GENERAL_FRWRD_ACTION_STRENGTH_MAX                (SOC_SAND_U32_MAX)
#define ARAD_PP_GENERAL_SNOOP_ACTION_STRENGTH_MAX                (SOC_SAND_U32_MAX)
#define ARAD_PP_GENERAL_TPID1_INDEX_MAX                          (3)
#define ARAD_PP_GENERAL_TPID2_INDEX_MAX                          (3)
#define ARAD_PP_GENERAL_ENTRIES_TO_SCAN_MAX                      (SOC_SAND_U32_MAX)
#define ARAD_PP_GENERAL_ENTRIES_TO_ACT_MAX                       (SOC_SAND_U32_MAX)

#define SOC_PPC_FRWRD_DECISION_INFO_TYPE_MAX                     (SOC_PPC_NOF_FRWRD_DECISION_TYPES-1)

#define ARAD_PP_ENCODE_NOF_BITS_19_FLOW_ID                       (17)
#define ARAD_PP_ENCODE_NOF_BITS_19_FEC_PTR(_unit)                (SOC_IS_JERICHO(_unit) ? 17 : 15)
#define ARAD_PP_ENCODE_NOF_BITS_19_MC_ID(_unit)                  (SOC_IS_JERICHO(_unit) ? 17 : 16)
#define ARAD_PP_ENCODE_NOF_BITS_19_TRAP_CODE                     (8)
#define ARAD_PP_ENCODE_NOF_BITS_19_TRAP_FWD                      (3)
#define ARAD_PP_ENCODE_NOF_BITS_19_TRAP_SNOOP                    (2)
#define ARAD_PP_ENCODE_NOF_BITS_19_SYS_PORT                      (16)


#define ARAD_PP_ENCODE_DEST_PREFIX_NOF_BITS_19_FLOW               (2)
#define ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_FLOW                  (3)
#define ARAD_PP_ENCODE_DEST_PREFIX_NOF_BITS_19_FEC_PTR(unit)      (SOC_DPP_DEFS_GET(unit, encode_dest_prefix_nof_bits_19_fec_ptr))
#define ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_FEC_PTR(unit)         (SOC_DPP_DEFS_GET(unit, encode_dest_prefix_value_19_fec_ptr))
#define ARAD_PP_ENCODE_DEST_PREFIX_NOF_BITS_19_MC_ID(unit)        (SOC_DPP_DEFS_GET(unit, encode_dest_prefix_nof_bits_19_mc_id))
#define ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_MC_ID(unit)           (SOC_DPP_DEFS_GET(unit, encode_dest_prefix_value_19_mc_id))
#define ARAD_PP_ENCODE_DEST_PREFIX_NOF_BITS_19_TRAP               (6)
#define ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_TRAP                  (0)
#define ARAD_PP_ENCODE_DEST_PREFIX_NOF_BITS_19_SYS_PORT           (3)
#define ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_SYS_PORT(unit)        (SOC_DPP_DEFS_GET(unit, encode_dest_prefix_value_19_sys_port))
#define ARAD_PP_ENCODE_DEST_PREFIX_NOF_BITS_19_DROP               (19)
#define ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_DROP                  (0x7FFFF)
#define ARAD_PP_ENCODE_DEST_PREFIX_VALUE_18_DROP                  (0x3FFFF)






#define ARAD_PP_ASD_NOF_BITS_MPLS_LABEL                          (20)
#define ARAD_PP_ASD_NOF_BITS_MPLS_PUSH_PROFILE                   (3)

#define ARAD_PP_ASD_BIT_POSITION_TRILL_MC                        (16)
#define ARAD_PP_ASD_BIT_POSITION_TRILL_RESERV                    (17)



#define SOC_PPC_EEI_POP_TPID_PROFILE_NOF_BITS                (2)
#define SOC_PPC_EEI_POP_MODEL_NOF_BITS                       (1)
#define SOC_PPC_EEI_POP_CW_NOF_BITS                          (1)
#define SOC_PPC_EEI_POP_UPPER_LAYER_NOF_BITS                 (4)

#define SOC_PPC_EEI_PUSH_CMD_NOF_BITS                        (4)


#define SOC_PPC_EEI_ENCAP_PTR_NOF_BITS_MSB                   (SOC_IS_JERICHO(unit)?2:0)
#define SOC_PPC_EEI_ENCAP_PTR_NOF_BITS_LSB                   (16)
#define SOC_PPC_EEI_ENCAP_PTR_NOF_BITS                       (SOC_PPC_EEI_ENCAP_PTR_NOF_BITS_LSB+SOC_PPC_EEI_ENCAP_PTR_NOF_BITS_MSB)




#define ARAD_PP_EEI_NOF_BITS                       (24)







#define ARAD_PP_L2_NEXT_PRTCL_UD_NDX_TO_TBL_NDX(ndx) (ndx-1)
#define ARAD_PP_L2_NEXT_PRTCL_TBL_NDX_TO_UD_NDX(ndx) (ndx+1)





typedef enum
{
  
  ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC = 9,
  
  ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_TRILL = 8,
  
  ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_IPV4 = 13,
  
  ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_IPV6 = 14,
  
  ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_ARP = 10,
  
  ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_CFM = 11,
  
  ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_MPLS = 15,
  
  ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_FC = 12,
  
  ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_OTHER = 0x0,
  
  ARAD_PP_INTERNAL_NOF_L2_NEXT_PRTCL_TYPES = 9
}ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE;






CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_general[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_GENERAL_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_GENERAL_GET_ERRS_PTR),
  

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FWD_DECISION_DEST_PREFIX_FIND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FWD_DECISION_DEST_TYPE_FIND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FWD_DECISION_EEI_TYPE_FIND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FWD_DECISION_IN_BUFFER_BUILD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FWD_DECISION_IN_BUFFER_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FWD_DECISION_BUILD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FWD_DECISION_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PKT_HDR_TYPE_TO_INTERANL_VAL_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_PKT_HDR_INTERANL_VAL_TO_TYPE_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_L2_NEXT_PRTCL_TYPE_TO_INTERANL_VAL_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_NEXT_PRTCL_INTERANL_VAL_TO_TYPE_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_L2_NEXT_PRTCL_TYPE_ALLOCATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_L2_NEXT_PRTCL_TYPE_DEALLOCATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_L2_NEXT_PRTCL_TYPE_FIND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_L2_NEXT_PRTCL_TYPE_FROM_INTERNAL_FIND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L3_NEXT_PROTOCOL_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L3_NEXT_PROTOCOL_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L3_NEXT_PROTOCOL_FIND),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_general[] =
{
  
  {
    SOC_PPC_TM_PORT_OUT_OF_RANGE_ERR,
    "SOC_PPC_TM_PORT_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PPC_TM_PORT' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PPC_PORT' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FID_OUT_OF_RANGE_ERR,
    "SOC_PPC_FID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PPC_FID' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR,
    "SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PPC_VSI_ID' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_RIF_VSI_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_RIF_VSI_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PPC_VSI_ID' is out of range. \n\r "
    "this feature supported only to 4K VSIs. \n\r "
    "The range is: 1 - 4*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_SYS_VSI_ID_OUT_OF_RANGE_ERR,
    "SOC_PPC_SYS_VSI_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PPC_SYS_VSI_ID' is out of range. \n\r "
    "The range is: 0 - 64*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR,
    "SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PPC_FEC_ID' is out of range. \n\r "
    "The range is: 0 - 32*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR,
    "SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PPC_VRF_ID' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_AC_ID_OUT_OF_RANGE_ERR,
    "SOC_PPC_AC_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PPC_AC_ID' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_RIF_ID_OUT_OF_RANGE_ERR,
    "SOC_PPC_RIF_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PPC_RIF_ID' is out of range. \n\r "
    "The range is: 0 - 4*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR,
    "SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PPC_LIF_ID' is out of range. \n\r "
    "The range is: 0 - 64*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_MP_LEVEL_OUT_OF_RANGE_ERR,
    "SOC_PPC_MP_LEVEL_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_PPC_MP_LEVEL' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_OUTLIF_ENCODE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR",
    "The parameter 'val' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_COMMAND_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_COMMAND_OUT_OF_RANGE_ERR",
    "The parameter 'command' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_MPLS_COMMAND_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_PUSH_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_PUSH_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'push_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_DEST_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_DEST_ID_OUT_OF_RANGE_ERR",
    "The parameter 'dest_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_TRAP_CODE_LSB_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_action_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'snoop_action_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_TPID1_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_TPID1_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tpid1_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_TPID2_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_TPID2_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tpid2_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR",
    "The parameter 'entries_to_scan' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR",
    "The parameter 'entries_to_act' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

    {
    ARAD_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'encode_type' of the destination is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_DEST_ENCODE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR,
      "ARAD_PP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR",
    "The parameter EEI.type of the destination is out of range. \n\r "
    "Allowed values are empty, trill amd mpls.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR,
      "ARAD_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR",
    "The parameter OutLIF.type of the destination is out of range. \n\r "
    "Allowed values are none, eep, out-vsi, ac, trap.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR",
    "The physical system port of the destination is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR",
    "The lag id of the destination is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_MC_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_MC_ID_OUT_OF_RANGE_ERR",
    "The multicast id of the destination is out of range. \n\r "
    "The range is: 0 - 16383.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_FEC_PTR_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_FEC_PTR_OUT_OF_RANGE_ERR",
    "The FEC pointer of the destination is out of range. \n\r "
    "The range is: 0 - 16383.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_TRAP_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_TRAP_ID_OUT_OF_RANGE_ERR",
    "The id of the destination is out of range. \n\r "
    "When the action is 'trap', the id must be 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_DROP_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_DROP_ID_OUT_OF_RANGE_ERR",
    "The id of the destination is out of range. \n\r "
    "When the action is 'drop', the id must be 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_FEC_PTR_INVALID_ERR,
    "ARAD_PP_FRWRD_DEST_FEC_PTR_INVALID_ERR",
    "The FEC pointer of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for this destination encoding.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_EEI_TYPE_INVALID_ERR,
    "ARAD_PP_FRWRD_DEST_EEI_TYPE_INVALID_ERR",
    "The EEI type of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non Multicast or FEC types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_TRAP_CODE_INVALID_ERR,
      "ARAD_PP_FRWRD_DEST_TRAP_CODE_INVALID_ERR",
    "The trap code of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non Trap types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_TRAP_CODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The Trap code of the destination is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_TRAP_FWD_INVALID_ERR,
    "ARAD_PP_FRWRD_DEST_TRAP_FWD_INVALID_ERR",
    "The forward strength of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non Trap types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR",
    "The Trap forward strength of the destination is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_TRAP_SNOOP_INVALID_ERR,
    "ARAD_PP_FRWRD_DEST_TRAP_SNOOP_INVALID_ERR",
    "The snoop strength of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non Trap types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR",
    "The Trap snoop strength of the destination is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_EEI_VAL_INVALID_ERR,
    "ARAD_PP_FRWRD_DEST_EEI_VAL_INVALID_ERR",
    "The EEI value of the destination is invalid (non zero). \n\r "
    "This parameter is invalid for non trill or mpls types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR,
    "ARAD_PP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR",
    "The OutLIF value of the destination is invalid (non zero). \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_NICK_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_NICK_OUT_OF_RANGE_ERR",
    "The nick value of the destination is out of range. \n\r "
    "The range is: 0 - 65535.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR",
    "The OutLIF value of the destination is out of range. \n\r "
    "The range is: 0 - 16383.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_MPLS_PUSH_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_MPLS_PUSH_PROFILE_OUT_OF_RANGE_ERR",
    "The push profile value of the destination is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_MPLS_TPID_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_MPLS_TPID_PROFILE_OUT_OF_RANGE_ERR",
    "The tpid profile value is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_MPLS_POP_MODEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_MPLS_POP_MODEL_OUT_OF_RANGE_ERR",
    "The model value is out of range. \n\r "
    "The range is: 0 - 1, pipe or uniform\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_MPLS_POP_NEXT_HEADER_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_MPLS_POP_NEXT_HEADER_OUT_OF_RANGE_ERR",
    "The next header value is out of range. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR,
      "ARAD_PP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR",
    "The MPLS label value of the destination is out of range. \n\r "
    "The range is: 0 - 1048575 (1M).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR,
      "ARAD_PP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR",
    "The MPLS command value of the destination is out of range. \n\r "
    "The range is: 0 - 0 (only push is accepted).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_OUTLIF_TYPE_INVALID_ERR,
    "ARAD_PP_FRWRD_DEST_OUTLIF_TYPE_INVALID_ERR",
    "The OutLIF type of the destination is invalid. \n\r "
    "This parameter is invalid for non Unicast types.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR,
      "ARAD_PP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR",
    "The flow id of the destination is out of range. \n\r "
    "The range is (according to the encoding): 0 - 16383 (or 32767).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_DROP_ILLEGAL_ERR,
    "ARAD_PP_FRWRD_DEST_DROP_ILLEGAL_ERR",
    "the destination cannot be drop in this place use trap. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR,
    "ARAD_PP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR",
    "The retrieved key has an unknown key prefix. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FEATURE_NOT_SUPPORTED_ERR,
    "ARAD_PP_FEATURE_NOT_SUPPORTED_ERR",
    "Feature is not supported in Arad. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GEN_NUM_CLEAR_ERR,
    "ARAD_PP_GEN_NUM_CLEAR_ERR",
    ". \n\r "
    ".\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MAX_BELOW_MIN_ERR,
    "ARAD_PP_MAX_BELOW_MIN_ERR",
    "Max value must be >= than Min value. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_EEI_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_EEI_TYPE_OUT_OF_RANGE_ERR",
    "'eei_type' is out of range. \n\r "
    "The range is: 0 to SOC_PPC_NOF_EEI_TYPES_ARAD-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to SOC_PPC_NOF_FRWRD_DECISION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
    "ARAD_PP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEFAULT_ACTIION_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEFAULT_ACTIION_TYPE_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to SOC_PPC_NOF_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_ASD_FORMAT_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_DEST_ASD_FORMAT_TYPE_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to ARAD_PP_FRWRD_DECISION_ASD_FORMAT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PKT_HDR_TYPE_NOT_SUPPORTED_ERR,
    "SOC_PPC_PKT_HDR_TYPE_NOT_SUPPORTED_ERR",
    "packet header stack not supported/expected\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_L2_NEXT_PRTCL_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PPC_L2_NEXT_PRTCL_TYPE_OUT_OF_RANGE_ERR",
    "Layer 2 next header is out of range \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_SYS_PORT_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_SYS_PORT_TYPE_OUT_OF_RANGE_ERR",
    "system port type can be physical or LAG \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DECISION_EEI_AND_OUTLIF_VALID_ERR,
    "ARAD_PP_FRWRD_DECISION_EEI_AND_OUTLIF_VALID_ERR",
    "Both outlif and EEI  are valid\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_IP_ITER_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_IP_ITER_TYPE_OUT_OF_RANGE_ERR",
    "ipv4 iterator type out of range \n\r"
    "for ARAD iter cannot of type fast \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_ISID_VAL_INVALID_ERR,
    "ARAD_PP_FRWRD_DEST_ISID_VAL_INVALID_ERR",
    "The isid value of the destination is invalid. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_DEST_RAW_VAL_INVALID_ERR,
    "ARAD_PP_FRWRD_DEST_RAW_VAL_INVALID_ERR",
    "the eei val is out of range. there is 24 bits. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_FILTER_PORT_ACC_FRAMES_PROFILE_OUT_OF_RANGE_ERR",
    "the range is 0 - 3. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_PARSE_TPID_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "the range is 0 - 7. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};




STATIC uint32
  arad_pp_fwd_decision_additional_info_type_get(
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_TYPE             fwd_decision_type,
    SOC_SAND_OUT uint32                                *is_eei,
    SOC_SAND_OUT uint32                                *is_outlif
    )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  *is_eei = 0;
  *is_outlif = 0;

  if(fwd_decision_type == SOC_PPC_FRWRD_DECISION_TYPE_MC || fwd_decision_type == SOC_PPC_FRWRD_DECISION_TYPE_FEC)
  {
    *is_eei = TRUE;
  }
  else
  {
    *is_outlif = TRUE;
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fwd_decision_additional_info_type_get()", 0, 0);
}


STATIC uint32
  arad_pp_fwd_decision_dest_prefix_find(
     SOC_SAND_IN  int                                 unit,    
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_TYPE fwd_decision_type,
    SOC_SAND_OUT uint32                  *prefix_nof_bits,
    SOC_SAND_OUT uint32                  *prefix_value
    )
{
  uint32
    nof_bits,
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FWD_DECISION_DEST_PREFIX_FIND);

  SOC_SAND_CHECK_NULL_INPUT(prefix_nof_bits);
  SOC_SAND_CHECK_NULL_INPUT(prefix_value);

  switch(fwd_decision_type)
  {
    
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG: 
      nof_bits = ARAD_PP_ENCODE_DEST_PREFIX_NOF_BITS_19_SYS_PORT;
      value = ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_SYS_PORT(unit);
    break;
    
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
      nof_bits = ARAD_PP_ENCODE_DEST_PREFIX_NOF_BITS_19_FLOW;
      value = ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_FLOW;
    break;
    
  case SOC_PPC_FRWRD_DECISION_TYPE_MC:
      nof_bits = ARAD_PP_ENCODE_DEST_PREFIX_NOF_BITS_19_MC_ID(unit);
      value = ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_MC_ID(unit);
    break;

    
  case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
       nof_bits = ARAD_PP_ENCODE_DEST_PREFIX_NOF_BITS_19_FEC_PTR(unit);
       value = ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_FEC_PTR(unit);
    break;
   
  case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
      nof_bits = ARAD_PP_ENCODE_DEST_PREFIX_NOF_BITS_19_TRAP;
      value = ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_TRAP;
    break;

    
  case SOC_PPC_FRWRD_DECISION_TYPE_DROP:
    nof_bits = ARAD_PP_ENCODE_DEST_PREFIX_NOF_BITS_19_DROP;
    value = ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_DROP;
  break;

  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 17, exit);
  }

  *prefix_value = value;
  *prefix_nof_bits = nof_bits;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fwd_decision_dest_prefix_find()", 0, 0);
}


STATIC uint32
  arad_pp_fwd_decision_eei_type_find(
      SOC_SAND_IN  int                        unit,      
      SOC_SAND_IN  uint32                     eei_buffer,
      SOC_SAND_OUT SOC_PPC_EEI_TYPE             *eei_type
  )
{
  uint32
    buffer_size,
    buffer_4_msb = 0,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FWD_DECISION_EEI_TYPE_FIND);

  SOC_SAND_CHECK_NULL_INPUT(eei_type);

  if(eei_buffer == 0)
  {
    *eei_type = SOC_PPC_EEI_TYPE_EMPTY;
    goto exit;
  }

  buffer_size = ARAD_PP_EEI_NOF_BITS;

  
  res = soc_sand_bitstream_get_any_field(
          &(eei_buffer),
          buffer_size - 4,
          4,
          &(buffer_4_msb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  if(buffer_4_msb <= SOC_PPC_EEI_IDENTIFIER_SWAP_VAL)
  {
    *eei_type = SOC_PPC_EEI_TYPE_MPLS;
  }
  else if(buffer_4_msb == SOC_PPC_EEI_IDENTIFIER_TRILL_VAL)
  {
    *eei_type = SOC_PPC_EEI_TYPE_TRILL;
  }
  else if(buffer_4_msb == SOC_PPC_EEI_IDENTIFIER_ENCAP_VAL)
  {
    *eei_type = SOC_PPC_EEI_TYPE_OUTLIF;
  }
  else
  {
      
      if (SOC_IS_JERICHO(unit)) 
      {
          uint32 buffer_2_msb = 0; 
          
          res = soc_sand_bitstream_get_any_field(
                  &(eei_buffer),
                  buffer_size - 2,
                  2,
                  &(buffer_2_msb)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);       

          if (buffer_2_msb == SOC_PPC_EEI_IDENTIFIER_ENCAP_VAL_2_MSBS) 
          {
              *eei_type = SOC_PPC_EEI_TYPE_OUTLIF;
          }
          else 
          {
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR, 12, exit);
          }
      } 
      else 
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR, 20, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fwd_decision_eei_type_find()", eei_buffer, 0);
}



STATIC uint32
  arad_pp_fwd_decision_dest_type_find(
      SOC_SAND_IN  int                                 unit,      
      SOC_SAND_IN  uint32                     dest_buffer,
      SOC_SAND_IN  uint8                     is_learn,
      SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_TYPE  *decision_type
  )
{
  uint32
    buffer_size,
    buffer_4_msb = 0,
    res = SOC_SAND_OK;
  SOC_PPC_FRWRD_DECISION_TYPE
    decision_type_lcl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FWD_DECISION_DEST_TYPE_FIND);

  SOC_SAND_CHECK_NULL_INPUT(decision_type);

  
  if (dest_buffer == ARAD_PP_ENCODE_DEST_PREFIX_VALUE_19_DROP || dest_buffer == ARAD_PP_ENCODE_DEST_PREFIX_VALUE_18_DROP)
  {
    *decision_type = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
    goto exit;
  }
  
  

  buffer_size = ARAD_PP_DEST_ENCODE_TOTAL_IN_BITS_19;

  res = soc_sand_bitstream_get_any_field(
          &(dest_buffer),
          buffer_size - 4,
          4,
          &(buffer_4_msb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch(buffer_4_msb)
  {
  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_1100:
  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_1101:
  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_1110:
  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_1111:
    decision_type_lcl = SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW;
  break;

  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_1010:
  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_1011:
    decision_type_lcl = SOC_PPC_FRWRD_DECISION_TYPE_MC;
  break;

  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_1000:
  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_1001:
      if (SOC_IS_JERICHO(unit) && (is_learn==0)) {
          decision_type_lcl = SOC_PPC_FRWRD_DECISION_TYPE_MC;
      } else {
          decision_type_lcl = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT; 
      }
    
  break;

  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_0100:
    decision_type_lcl = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
    break;
  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_0101:
  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_0110:
  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_0111:
      if (SOC_IS_JERICHO(unit) && (is_learn==0)) {
          decision_type_lcl = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
      } else {
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR, 18, exit);
      }
      break;
  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_0010:
  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_0011:
      if (SOC_IS_JERICHO(unit) && (is_learn==0)) {
          decision_type_lcl = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT; 
      } else {
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR, 18, exit);
      }
      break;
  case  ARAD_PP_DEST_ENCODED_PREFIX_TYPE_0000:
      decision_type_lcl = SOC_PPC_FRWRD_DECISION_TYPE_TRAP;
  break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_GENERAL_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR, 20, exit);
  }

  *decision_type = decision_type_lcl;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fwd_decision_dest_type_find()", 0, 0);
}



uint32
  arad_pp_fwd_decision_in_buffer_build(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO             *fwd_decision,
    SOC_SAND_OUT uint32                                *dest_buffer,
    SOC_SAND_OUT uint32                                *asd_buffer
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    temp = 0,
    temp_lsb =0,
    dest_buffer_lcl = 0,
    asd_buffer_lcl = 0,
    sys_port_id = 0,
    param_id_length,
    prefix_value,
    prefix_nof_bits,
    is_multicast = 0,
    eei_type = 0,
    val;  
  ARAD_SOC_REG_FIELD
    strength_fld_fwd,
    strength_fld_snp;
  SOC_PPC_TRAP_CODE_INTERNAL
    trap_code_internal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FWD_DECISION_IN_BUFFER_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(fwd_decision);
  SOC_SAND_CHECK_NULL_INPUT(dest_buffer);
  SOC_SAND_CHECK_NULL_INPUT(asd_buffer);

  
  SOC_PPC_FRWRD_DECISION_INFO_verify(fwd_decision);  

  res = SOC_PPC_FRWRD_DECISION_INFO_with_encode_type_verify(
          unit,
          appl_type,
          fwd_decision
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  prefix_value = 0;
  prefix_nof_bits = 0;
  res = arad_pp_fwd_decision_dest_prefix_find(
          unit,          
          fwd_decision->type,
          &(prefix_nof_bits),
          &(prefix_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  
  param_id_length = ARAD_PP_DEST_ENCODE_TOTAL_IN_BITS_19 - prefix_nof_bits;

  
   res = soc_sand_bitstream_set_any_field(
            &(prefix_value),
            param_id_length,
            prefix_nof_bits,
            &(dest_buffer_lcl)
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  switch(fwd_decision->type)
  {
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:

      res = arad_ports_logical_sys_id_build(
                unit,
                fwd_decision->type == SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG ,
                fwd_decision->dest_id, 
                0,   
                fwd_decision->dest_id, 
                &sys_port_id
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

      res = soc_sand_bitstream_set_any_field(
               &(sys_port_id),
               0,
               param_id_length,
               &(dest_buffer_lcl)
             );
     SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  break;
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
  case SOC_PPC_FRWRD_DECISION_TYPE_MC:
  case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
  case SOC_PPC_FRWRD_DECISION_TYPE_DROP:
     res = soc_sand_bitstream_set_any_field(
              &(fwd_decision->dest_id),
              0,
              param_id_length,
              &(dest_buffer_lcl)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    break;

    
  case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:

    res = arad_pp_trap_mgmt_trap_code_to_internal(
          unit,
          fwd_decision->additional_info.trap_info.action_profile.trap_code,
          &trap_code_internal,
          &strength_fld_fwd,
          &strength_fld_snp
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    temp = (uint32)trap_code_internal;

    res = soc_sand_bitstream_set_any_field(
              &(temp),
              0,
              ARAD_PP_ENCODE_NOF_BITS_19_TRAP_CODE,
              &(dest_buffer_lcl)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    temp = fwd_decision->additional_info.trap_info.action_profile.frwrd_action_strength;
    res = soc_sand_bitstream_set_any_field(
              &(temp),
              ARAD_PP_ENCODE_NOF_BITS_19_TRAP_CODE,
              ARAD_PP_ENCODE_NOF_BITS_19_TRAP_FWD,
              &(dest_buffer_lcl)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 61, exit);

    res = soc_sand_bitstream_set_any_field(
              &(fwd_decision->additional_info.trap_info.action_profile.snoop_action_strength),
              ARAD_PP_ENCODE_NOF_BITS_19_TRAP_CODE + ARAD_PP_ENCODE_NOF_BITS_19_TRAP_FWD,
              ARAD_PP_ENCODE_NOF_BITS_19_TRAP_SNOOP,
              &(dest_buffer_lcl)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }


  
  switch(fwd_decision->additional_info.outlif.type)
  {
    case SOC_PPC_OUTLIF_ENCODE_TYPE_RAW:
    case SOC_PPC_OUTLIF_ENCODE_TYPE_RAW_INVALID:
    case SOC_PPC_OUTLIF_ENCODE_TYPE_EEP:
      asd_buffer_lcl = fwd_decision->additional_info.outlif.val;
      break;
    case SOC_PPC_OUTLIF_ENCODE_TYPE_NONE:
      break;
    default:
      
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR, 30, exit);
  }

  
  if (fwd_decision->additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE 
      && fwd_decision->additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) 
  {
     SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_DECISION_EEI_AND_OUTLIF_VALID_ERR, 31, exit);
  }

  
  if (fwd_decision->additional_info.eei.type == SOC_PPC_EEI_TYPE_RAW)
  {
      
      asd_buffer_lcl = fwd_decision->additional_info.eei.val.raw;
  }
  else if (fwd_decision->additional_info.eei.type == SOC_PPC_EEI_TYPE_TRILL)
  {
    eei_type = SOC_PPC_EEI_IDENTIFIER_TRILL_VAL;

    val = 0;
    res = soc_sand_bitstream_set_any_field(
            &val,
            ARAD_PP_ASD_BIT_POSITION_TRILL_RESERV,
            1,
            &(asd_buffer_lcl)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    is_multicast = fwd_decision->additional_info.eei.val.trill_dest.is_multicast;
    res = soc_sand_bitstream_set_any_field(
            &(is_multicast),
            ARAD_PP_ASD_BIT_POSITION_TRILL_MC,
            1,
            &(asd_buffer_lcl)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

     res = soc_sand_bitstream_set_any_field(
            &(fwd_decision->additional_info.eei.val.trill_dest.dest_nick),
            0,
            ARAD_PP_ASD_NOF_BITS_TRILL_NICK,
            &(asd_buffer_lcl)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 81, exit);
  }
  else if(fwd_decision->additional_info.eei.type == SOC_PPC_EEI_TYPE_MPLS) 
  {
    if(fwd_decision->additional_info.eei.val.mpls_command.command == SOC_PPC_MPLS_COMMAND_TYPE_PUSH)
    {
      

      
      eei_type = fwd_decision->additional_info.eei.val.mpls_command.push_profile;  

      
      res = soc_sand_bitstream_set_any_field(
              &(fwd_decision->additional_info.eei.val.mpls_command.label),
              0,
              ARAD_PP_ASD_NOF_BITS_MPLS_LABEL,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
    }
    else if(fwd_decision->additional_info.eei.val.mpls_command.command == SOC_PPC_MPLS_COMMAND_TYPE_SWAP)
    {
      
      eei_type = SOC_PPC_EEI_IDENTIFIER_SWAP_VAL;
      
      res = soc_sand_bitstream_set_any_field(
              &(fwd_decision->additional_info.eei.val.mpls_command.label),
              0,
              ARAD_PP_ASD_NOF_BITS_MPLS_LABEL,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
    }
    else if(fwd_decision->additional_info.eei.val.mpls_command.command == SOC_PPC_MPLS_COMMAND_TYPE_POP)
    {
      eei_type = SOC_PPC_EEI_IDENTIFIER_POP_VAL;  

      
      temp = fwd_decision->additional_info.eei.val.mpls_command.pop_next_header;
      res = soc_sand_bitstream_set_any_field(
              &(temp),
              temp_lsb,
              SOC_PPC_EEI_POP_UPPER_LAYER_NOF_BITS,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
      temp_lsb += SOC_PPC_EEI_POP_UPPER_LAYER_NOF_BITS;

      
      temp = fwd_decision->additional_info.eei.val.mpls_command.has_cw;
      res = soc_sand_bitstream_set_any_field(
              &(temp),
              temp_lsb,
              SOC_PPC_EEI_POP_CW_NOF_BITS,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
      temp_lsb += SOC_PPC_EEI_POP_CW_NOF_BITS;

      
      temp = (fwd_decision->additional_info.eei.val.mpls_command.model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE)?1:0;
      res = soc_sand_bitstream_set_any_field(
              &(temp),
              temp_lsb,
              SOC_PPC_EEI_POP_MODEL_NOF_BITS,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
      temp_lsb += SOC_PPC_EEI_POP_MODEL_NOF_BITS;

      
      temp = fwd_decision->additional_info.eei.val.mpls_command.tpid_profile;
      res = soc_sand_bitstream_set_any_field(
              &(temp),
              temp_lsb,
              SOC_PPC_EEI_POP_TPID_PROFILE_NOF_BITS,
              &(asd_buffer_lcl)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
      temp_lsb += SOC_PPC_EEI_POP_MODEL_NOF_BITS;


      temp = fwd_decision->additional_info.eei.val.mpls_command.tpid_profile;
    }

  }
  else if(fwd_decision->additional_info.eei.type == SOC_PPC_EEI_TYPE_OUTLIF) 
  {
    
    eei_type = SOC_IS_JERICHO(unit)?SOC_PPC_EEI_IDENTIFIER_ENCAP_VAL_2_MSBS:SOC_PPC_EEI_IDENTIFIER_ENCAP_VAL;

    
    res = soc_sand_bitstream_set_any_field(
            &(fwd_decision->additional_info.eei.val.outlif),
            0,
            SOC_PPC_EEI_ENCAP_PTR_NOF_BITS_LSB,
            &(asd_buffer_lcl)
          );    
    SOC_SAND_CHECK_FUNC_RESULT(res, 73, exit);

    if (SOC_IS_JERICHO(unit)) {
        uint8 system_headers_mode;
        SOC_SAND_CHECK_FUNC_RESULT(sw_state_access[unit].dpp.soc.arad.pp.header_data.system_headers_mode.get(unit, &system_headers_mode), 73, exit);

        
        temp = (system_headers_mode == ARAD_PP_SYSTEM_HEADERS_MODE_ARAD) ? 0x3
                : fwd_decision->additional_info.eei.val.outlif >> SOC_PPC_EEI_ENCAP_PTR_NOF_BITS_LSB;
        res = soc_sand_bitstream_set_any_field(
                &(temp),
                ARAD_PP_EEI_BIT_POSITION_OUTLIF_MSB,
                SOC_PPC_EEI_ENCAP_PTR_NOF_BITS_MSB,
                &(asd_buffer_lcl)
              );    
        SOC_SAND_CHECK_FUNC_RESULT(res, 73, exit);
    }
  }
  else if(fwd_decision->additional_info.eei.type == SOC_PPC_EEI_TYPE_MIM) 
  {
    asd_buffer_lcl =fwd_decision->additional_info.eei.val.isid;
  }

  if(fwd_decision->additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY && fwd_decision->additional_info.eei.type != SOC_PPC_EEI_TYPE_MIM
     && fwd_decision->additional_info.eei.type != SOC_PPC_NOF_EEI_TYPES_ARAD && fwd_decision->additional_info.eei.type != SOC_PPC_EEI_TYPE_RAW)
  {
    uint32 start_bit, length;

    start_bit = (SOC_IS_JERICHO(unit)&&(fwd_decision->additional_info.eei.type == SOC_PPC_EEI_TYPE_OUTLIF))?
        ARAD_PP_ASD_BIT_POSITION_IDENTIFIER_OUTLIF:ARAD_PP_ASD_BIT_POSITION_IDENTIFIER;
    length = (SOC_IS_JERICHO(unit)&&(fwd_decision->additional_info.eei.type == SOC_PPC_EEI_TYPE_OUTLIF))?
        ARAD_PP_EEI_IDENTIFIER_NOF_BITDS_OUTLIF : ARAD_PP_EEI_IDENTIFIER_NOF_BITDS;

    
    res = soc_sand_bitstream_set_any_field(
            &(eei_type),
            start_bit,
			length,
            &(asd_buffer_lcl)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);
  }

 


  *dest_buffer = dest_buffer_lcl;
  *asd_buffer = asd_buffer_lcl;
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fwd_decision_in_buffer_build()", 0, 0);
}



uint32
  arad_pp_fwd_decision_in_buffer_parse(
    SOC_SAND_IN  int                  unit,    
    SOC_SAND_IN  uint32                  dest_buffer,
    SOC_SAND_IN  uint32                  asd_buffer,
    SOC_SAND_IN  uint32                  flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO *fwd_decision
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_FRWRD_DECISION_TYPE
    decision_type=0;
  uint32
    temp = 0,
    temp_lsb = 0,
    dest_buffer_lcl = dest_buffer,
    asd_buffer_lcl = asd_buffer,
    param_id_length,
    prefix_value,
    mpls_cmd_type=0,
    prefix_nof_bits,
    buffer_size,
    is_eei=0,
    is_outlif=0,
    lag_id,
    lag_member_id,
    sys_phys_port_id,
    is_multicast = 0;
  uint8
    is_lag_not_phys,
    is_learn;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FWD_DECISION_IN_BUFFER_PARSE);

  SOC_SAND_CHECK_NULL_INPUT(fwd_decision);

  
  if(flags & ARAD_PP_FWD_DECISION_APP_TYPE_IS_TM) {
      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          dest_buffer_lcl |= 1 << 18;
      }
  }

  is_learn = ((flags & ARAD_PP_FWD_DECISION_PARSE_IS_LEARN) ? 1 : 0);

  if(flags & (ARAD_PP_FWD_DECISION_PARSE_LEGACY | ARAD_PP_FWD_DECISION_PARSE_DEST)) {
      
      fwd_decision->dest_id = 0;
      res = arad_pp_fwd_decision_dest_type_find(
              unit,              
              dest_buffer_lcl,
              is_learn,
              &(decision_type)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      
      fwd_decision->type = decision_type;

      
      prefix_value = 0;
      prefix_nof_bits = 0;
      res = arad_pp_fwd_decision_dest_prefix_find(
              unit,              
              decision_type,
              &(prefix_nof_bits),
              &(prefix_value)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      
      param_id_length = ARAD_PP_DEST_ENCODE_TOTAL_IN_BITS_19 - prefix_nof_bits;
      
      switch(decision_type)
      {
      case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
      case SOC_PPC_FRWRD_DECISION_TYPE_MC:
      case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
      case SOC_PPC_FRWRD_DECISION_TYPE_DROP:
         res = soc_sand_bitstream_get_any_field(
                  &(dest_buffer_lcl),
                  0,
                  param_id_length,
                  &(fwd_decision->dest_id)
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        break;
      case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
      case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
         res = soc_sand_bitstream_get_any_field(
                  &(dest_buffer_lcl),
                  0,
                  param_id_length,
                  &(fwd_decision->dest_id) 
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

        res = arad_ports_logical_sys_id_parse(
                unit,
                fwd_decision->dest_id,
                &is_lag_not_phys,
                &lag_id,
                &lag_member_id,
                &sys_phys_port_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

        if(is_lag_not_phys) 
        {
          fwd_decision->dest_id = lag_id;
          fwd_decision->type = SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG;
        }
        else
        {
          fwd_decision->dest_id = sys_phys_port_id;
          fwd_decision->type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
        }

        break;
        
      case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
         res = soc_sand_bitstream_get_any_field(
                  &(dest_buffer_lcl),
                  0,
                  ARAD_PP_ENCODE_NOF_BITS_19_TRAP_CODE,
                  &temp
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        
        res = arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit,temp,&fwd_decision->additional_info.trap_info.action_profile.trap_code);
        SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);    

        res = soc_sand_bitstream_get_any_field(
                  &(dest_buffer_lcl),
                  ARAD_PP_ENCODE_NOF_BITS_19_TRAP_CODE,
                  ARAD_PP_ENCODE_NOF_BITS_19_TRAP_FWD,
                  &(fwd_decision->additional_info.trap_info.action_profile.frwrd_action_strength)
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

        res = soc_sand_bitstream_get_any_field(
                  &(dest_buffer_lcl),
                  ARAD_PP_ENCODE_NOF_BITS_19_TRAP_CODE + ARAD_PP_ENCODE_NOF_BITS_19_TRAP_FWD,
                  ARAD_PP_ENCODE_NOF_BITS_19_TRAP_SNOOP,
                  &(fwd_decision->additional_info.trap_info.action_profile.snoop_action_strength)
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
        break;

      default:
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 70, exit);
      }
  }

  if(flags & ARAD_PP_FWD_DECISION_PARSE_LEGACY) {
      
      
      res = arad_pp_fwd_decision_additional_info_type_get(decision_type, &is_eei, &is_outlif);
      SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);
  }

  if(is_outlif == 1 || flags & ARAD_PP_FWD_DECISION_PARSE_OUTLIF)
  {
    fwd_decision->additional_info.outlif.val = asd_buffer;
    fwd_decision->additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
    goto exit;
  }
  else if (flags & ARAD_PP_FWD_DECISION_PARSE_OUTLIF_INVALID) {
      fwd_decision->additional_info.outlif.val = asd_buffer;
      fwd_decision->additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW_INVALID;
  }
  else {
      fwd_decision->additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;
  }

  if(is_eei == 1 || flags & ARAD_PP_FWD_DECISION_PARSE_EEI)
  {
    if (flags & ARAD_PP_FWD_DECISION_PARSE_EEI_MIM) {
      fwd_decision->additional_info.eei.type = SOC_PPC_EEI_TYPE_MIM;
    }
    else {
      res = arad_pp_fwd_decision_eei_type_find(
              unit,
              asd_buffer,
              &(fwd_decision->additional_info.eei.type)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }

    

    
    if (fwd_decision->additional_info.eei.type == SOC_PPC_EEI_TYPE_TRILL)
    {
      res = soc_sand_bitstream_get_any_field(
              &(asd_buffer),
              ARAD_PP_ASD_BIT_POSITION_TRILL_MC,
              1,
              &(is_multicast)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

      fwd_decision->additional_info.eei.val.trill_dest.is_multicast = (uint8)is_multicast;

      res = soc_sand_bitstream_get_any_field(
              &(asd_buffer_lcl),
              0,
              ARAD_PP_ASD_NOF_BITS_TRILL_NICK,
              &(fwd_decision->additional_info.eei.val.trill_dest.dest_nick)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }
    else if(fwd_decision->additional_info.eei.type == SOC_PPC_EEI_TYPE_MPLS) 
    {

      buffer_size = ARAD_PP_EEI_NOF_BITS;

      res = soc_sand_bitstream_get_any_field(
              &(asd_buffer_lcl),
              buffer_size - SOC_PPC_EEI_PUSH_CMD_NOF_BITS,
              SOC_PPC_EEI_PUSH_CMD_NOF_BITS,
              &(mpls_cmd_type)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      
      if(mpls_cmd_type < SOC_PPC_EEI_IDENTIFIER_POP_VAL)
      {
        
        fwd_decision->additional_info.eei.val.mpls_command.command = SOC_PPC_MPLS_COMMAND_TYPE_PUSH;

        
        fwd_decision->additional_info.eei.val.mpls_command.push_profile = mpls_cmd_type;

        
        res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                0,
                ARAD_PP_ASD_NOF_BITS_MPLS_LABEL,
                &(fwd_decision->additional_info.eei.val.mpls_command.label)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
      }
      else if(mpls_cmd_type == SOC_PPC_EEI_IDENTIFIER_SWAP_VAL)
      {
        
        fwd_decision->additional_info.eei.val.mpls_command.command = SOC_PPC_MPLS_COMMAND_TYPE_SWAP;
        
        res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                0,
                ARAD_PP_ASD_NOF_BITS_MPLS_LABEL,
                &(fwd_decision->additional_info.eei.val.mpls_command.label)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
      }
      
      else if(mpls_cmd_type == SOC_PPC_EEI_IDENTIFIER_POP_VAL)
      {
        fwd_decision->additional_info.eei.val.mpls_command.command = SOC_PPC_MPLS_COMMAND_TYPE_POP;
        
        res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                temp_lsb,
                SOC_PPC_EEI_POP_UPPER_LAYER_NOF_BITS,
                &(temp)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
        
         fwd_decision->additional_info.eei.val.mpls_command.pop_next_header = temp;

        temp_lsb += SOC_PPC_EEI_POP_UPPER_LAYER_NOF_BITS;

        
        res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                temp_lsb,
                SOC_PPC_EEI_POP_CW_NOF_BITS,
                &(temp)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
        fwd_decision->additional_info.eei.val.mpls_command.has_cw = (uint8)temp;
        temp_lsb += SOC_PPC_EEI_POP_CW_NOF_BITS;

        
        res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                temp_lsb,
                SOC_PPC_EEI_POP_MODEL_NOF_BITS,
                &(temp)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
        fwd_decision->additional_info.eei.val.mpls_command.model = (temp == 1)?SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE:SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
        temp_lsb += SOC_PPC_EEI_POP_MODEL_NOF_BITS;

        
        
        res = soc_sand_bitstream_get_any_field(
                &(asd_buffer_lcl),
                temp_lsb,
                SOC_PPC_EEI_POP_TPID_PROFILE_NOF_BITS,
                &(temp)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
        fwd_decision->additional_info.eei.val.mpls_command.tpid_profile = temp;
        temp_lsb += SOC_PPC_EEI_POP_MODEL_NOF_BITS;

        temp = fwd_decision->additional_info.eei.val.mpls_command.tpid_profile;
      }

    }
    else if(fwd_decision->additional_info.eei.type == SOC_PPC_EEI_TYPE_OUTLIF) 
    {
      uint8 system_headers_mode;
      
      
      res = soc_sand_bitstream_get_any_field(
              &(asd_buffer_lcl),
              0,
              SOC_PPC_EEI_ENCAP_PTR_NOF_BITS_LSB,
              &(fwd_decision->additional_info.eei.val.outlif)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

      temp = 0;
      res = soc_sand_bitstream_get_any_field(
              &(asd_buffer_lcl),
              ARAD_PP_EEI_BIT_POSITION_OUTLIF_MSB,
              SOC_PPC_EEI_ENCAP_PTR_NOF_BITS_MSB,
              &temp
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

      
      SOC_SAND_CHECK_FUNC_RESULT(sw_state_access[unit].dpp.soc.arad.pp.header_data.system_headers_mode.get(unit, &system_headers_mode), 73, exit);

      if (system_headers_mode) {
          temp = 0;
      }

      fwd_decision->additional_info.eei.val.outlif |= temp << SOC_PPC_EEI_ENCAP_PTR_NOF_BITS_LSB;

    }
    else if(fwd_decision->additional_info.eei.type == SOC_PPC_EEI_TYPE_MIM) 
    {
      fwd_decision->additional_info.eei.val.isid = asd_buffer_lcl;
    }
   
  }
  else {
      fwd_decision->additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fwd_decision_parse()", 0, 0);
}

uint32
  arad_pp_l2_next_prtcl_type_to_interanl_val_map(
    SOC_SAND_IN  SOC_PPC_L2_NEXT_PRTCL_TYPE    l2_next_prtcl_type,
    SOC_SAND_OUT uint32                    *internal_val,
    SOC_SAND_OUT uint8                   *found
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_PP_PKT_HDR_INTERANL_VAL_TO_TYPE_MAP);

  SOC_SAND_CHECK_NULL_INPUT(internal_val);

  *found = TRUE;
  switch(l2_next_prtcl_type)
  {
    case SOC_PPC_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC:
      *internal_val = ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC;
    break;
    case SOC_PPC_L2_NEXT_PRTCL_TYPE_TRILL:
      *internal_val = ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_TRILL;
    break;
    case SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV4:
      *internal_val = ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_IPV4;
    break;
    case SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV6:
      *internal_val = ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_IPV6;
    break;
    case SOC_PPC_L2_NEXT_PRTCL_TYPE_ARP:
      *internal_val = ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_ARP;
    break;
    case SOC_PPC_L2_NEXT_PRTCL_TYPE_CFM:
      *internal_val = ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_CFM;
    break;
    case SOC_PPC_L2_NEXT_PRTCL_TYPE_MPLS:
      *internal_val = ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_MPLS;
    break;
    case SOC_PPC_L2_NEXT_PRTCL_TYPE_FC_ETH:
      *internal_val = ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_FC;
    break;
    case SOC_PPC_L2_NEXT_PRTCL_TYPE_OTHER:
      *internal_val = ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_OTHER;
    break;
    default:
      *found = FALSE;
    break;;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_next_prtcl_type_to_interanl_val_map()", 0, 0);
}

uint32
  arad_pp_l2_next_prtcl_interanl_val_to_type_map(
    SOC_SAND_IN  uint32                  internal_val,
    SOC_SAND_OUT SOC_PPC_L2_NEXT_PRTCL_TYPE    *l2_next_prtcl_type,
    SOC_SAND_OUT uint8                   *found
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_PP_L2_NEXT_PRTCL_INTERANL_VAL_TO_TYPE_MAP);

  SOC_SAND_CHECK_NULL_INPUT(l2_next_prtcl_type);

  *found = TRUE;

  switch(internal_val)
  {
    case ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC:
      *l2_next_prtcl_type = SOC_PPC_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC;
    break;
    case ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_TRILL:
      *l2_next_prtcl_type = SOC_PPC_L2_NEXT_PRTCL_TYPE_TRILL;
    break;
    case ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_IPV4:
      *l2_next_prtcl_type = SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV4;
    break;
    case ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_IPV6:
      *l2_next_prtcl_type = SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV6;
    break;
    case ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_ARP:
      *l2_next_prtcl_type = SOC_PPC_L2_NEXT_PRTCL_TYPE_ARP;
    break;
    case ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_CFM:
      *l2_next_prtcl_type = SOC_PPC_L2_NEXT_PRTCL_TYPE_CFM;
    break;
    case ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_MPLS:
      *l2_next_prtcl_type = SOC_PPC_L2_NEXT_PRTCL_TYPE_MPLS;
    break;
    case ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_FC:
      *l2_next_prtcl_type = SOC_PPC_L2_NEXT_PRTCL_TYPE_FC_ETH;
    break;
    case ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_OTHER:
      *l2_next_prtcl_type = SOC_PPC_L2_NEXT_PRTCL_TYPE_OTHER;
    break;
    default:
      *found = FALSE;
    break;;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_pkt_hdr_interanl_val_to_type_map()", 0, 0);
}

uint32
  arad_pp_l2_next_prtcl_type_find(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  l2_next_prtcl_type,
    SOC_SAND_OUT uint32                  *internal_ndx,
    SOC_SAND_OUT uint8                 *found
  )
{
  uint32
    res = SOC_SAND_OK,
    internal_val,
    prtcl_tbl_ndx,
    ref_count;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_L2_NEXT_PRTCL_TYPE_FIND);

  SOC_SAND_CHECK_NULL_INPUT(internal_ndx);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_l2_next_prtcl_type_to_interanl_val_map(
          l2_next_prtcl_type,
          &internal_val,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*found)
  {
    *internal_ndx = internal_val;
  }
  else
  {
    
    res = arad_sw_db_multiset_lookup(
            unit,
            ARAD_SW_DB_CORE_ANY,
			ARAD_PP_SW_DB_MULTI_SET_ETHER_TYPE,
            &l2_next_prtcl_type,
            &prtcl_tbl_ndx,
            &ref_count
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (ref_count == 0)
    {
      *found = FALSE;
    }
    else
    {
      *internal_ndx = ARAD_PP_L2_NEXT_PRTCL_TBL_NDX_TO_UD_NDX(prtcl_tbl_ndx);
      *found = TRUE;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_next_prtcl_type_find()", 0, 0);
}

uint32
  arad_pp_l2_next_prtcl_type_from_internal_find(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  internal_ndx,
    SOC_SAND_OUT uint32                  *l2_next_prtcl_type,
    SOC_SAND_OUT uint8                 *found
  )
{
  uint32
    res = SOC_SAND_OK,
    prtcl_tbl_ndx;
  ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA
    tbl_data;
  SOC_PPC_L2_NEXT_PRTCL_TYPE
    l2_next_prtcl_type_enum;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_L2_NEXT_PRTCL_TYPE_FROM_INTERNAL_FIND);

  SOC_SAND_CHECK_NULL_INPUT(l2_next_prtcl_type);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_l2_next_prtcl_interanl_val_to_type_map(
          internal_ndx,
          &l2_next_prtcl_type_enum,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*found)
  {
    *l2_next_prtcl_type = (uint32)l2_next_prtcl_type_enum;
  }
  else
  {
    prtcl_tbl_ndx = ARAD_PP_L2_NEXT_PRTCL_UD_NDX_TO_TBL_NDX(internal_ndx);

    
    res = arad_pp_ihp_parser_eth_protocols_tbl_get_unsafe(
            unit,
            prtcl_tbl_ndx,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    *l2_next_prtcl_type = tbl_data.eth_type_protocol;
    *found = TRUE;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_next_prtcl_type_from_internal_find()", 0, 0);
}

uint32
  arad_pp_l2_next_prtcl_type_allocate(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  l2_next_prtcl_type,
    SOC_SAND_OUT uint32                  *internal_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE      *success
  )
{
  uint32
    res = SOC_SAND_OK,
    internal_val,
    prtcl_tbl_ndx = 0;
  uint8
    first_appear = FALSE,
    found;
  ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_L2_NEXT_PRTCL_TYPE_ALLOCATE);

  SOC_SAND_CHECK_NULL_INPUT(internal_ndx);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_l2_next_prtcl_type_to_interanl_val_map(
          l2_next_prtcl_type,
          &internal_val,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  if (found)
  {
    *internal_ndx = internal_val;
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    
    res = arad_sw_db_multiset_add(
            unit,
			ARAD_SW_DB_CORE_ANY,
            ARAD_PP_SW_DB_MULTI_SET_ETHER_TYPE,
            &l2_next_prtcl_type,
            &prtcl_tbl_ndx,
            &first_appear,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if ((*success == SOC_SAND_SUCCESS) && first_appear)
    {
      
      res = arad_pp_ihp_parser_eth_protocols_tbl_get_unsafe(
        unit,
        prtcl_tbl_ndx,
        &tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      tbl_data.eth_type_protocol = l2_next_prtcl_type;
      tbl_data.eth_sap_protocol = l2_next_prtcl_type; 

      res = arad_pp_ihp_parser_eth_protocols_tbl_set_unsafe(
        unit,
        prtcl_tbl_ndx,
        &tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

    *internal_ndx = ARAD_PP_L2_NEXT_PRTCL_TBL_NDX_TO_UD_NDX(prtcl_tbl_ndx);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_next_prtcl_type_allocate()", 0, 0);
}

uint32
  arad_pp_l2_next_prtcl_type_deallocate(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  l2_next_prtcl_type
  )
{
  uint32
    res = SOC_SAND_OK,
    internal_val;
  uint8
    found;
  uint32
    prtcl_tbl_ndx = 0;
  uint8
    last_appear = FALSE;

  ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_L2_NEXT_PRTCL_TYPE_DEALLOCATE);

  res = arad_pp_l2_next_prtcl_type_to_interanl_val_map(
          l2_next_prtcl_type,
          &internal_val,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!found)
  {
    
    res = arad_sw_db_multiset_remove(
            unit,
			ARAD_SW_DB_CORE_ANY,
            ARAD_PP_SW_DB_MULTI_SET_ETHER_TYPE,
            &l2_next_prtcl_type,
            &prtcl_tbl_ndx,
            &last_appear
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (last_appear)
    {
      
      res = arad_pp_ihp_parser_eth_protocols_tbl_get_unsafe(
        unit,
        prtcl_tbl_ndx,
        &tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      tbl_data.eth_type_protocol = ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_OTHER;
      tbl_data.eth_sap_protocol = ARAD_PP_INTERNAL_L2_NEXT_PRTCL_TYPE_OTHER;

      res = arad_pp_ihp_parser_eth_protocols_tbl_set_unsafe(
        unit,
        prtcl_tbl_ndx,
        &tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }
  else
  {
    
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_next_prtcl_type_deallocate()", 0, 0);
}


uint32
  arad_pp_l2_next_prtcl_type_allocate_test_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               l2_next_prtcl_type,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE            *success
  )
{ 
  uint32
    res = SOC_SAND_OK,
    ether_type_internal;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_l2_next_prtcl_type_find(
          unit,
          l2_next_prtcl_type,
          &ether_type_internal,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (found)
  {
    *success = SOC_SAND_SUCCESS;
    goto exit;
  }

  
  res = arad_pp_l2_next_prtcl_type_allocate(
          unit,
          l2_next_prtcl_type,
          &ether_type_internal,
          success
        );  
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (*success == SOC_SAND_SUCCESS)
  { 
    
    res = arad_pp_l2_next_prtcl_type_deallocate(
            unit,
            l2_next_prtcl_type
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_next_prtcl_type_allocate_test_unsafe()", l2_next_prtcl_type, 0);
}

uint32
  arad_pp_l2_next_prtcl_type_allocate_test_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               l2_next_prtcl_type,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE            *success
  )
{ 
    uint32
    res = SOC_SAND_OK;
	
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
	
    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_next_prtcl_type_allocate_test_verify()", 0, 0);

}


#define ARAD_PP_L3_PRTCL_TO_NDX_CASE(prtcl_suffix) \
  case (SOC_PPC_L3_NEXT_PRTCL_TYPE_##prtcl_suffix): \
    *ndx = ARAD_PP_L3_NEXT_PRTCL_NDX_##prtcl_suffix; \
    break;

uint32
  arad_pp_l3_prtcl_to_ndx(
    SOC_SAND_IN  SOC_PPC_L3_NEXT_PRTCL_TYPE prtcl_type,
    SOC_SAND_OUT uint8 *ndx
  )
{
  switch (prtcl_type)
  {
  ARAD_PP_L3_PRTCL_TO_NDX_CASE(NONE)
  ARAD_PP_L3_PRTCL_TO_NDX_CASE(TCP)
  ARAD_PP_L3_PRTCL_TO_NDX_CASE(UDP)
  ARAD_PP_L3_PRTCL_TO_NDX_CASE(IGMP)
  ARAD_PP_L3_PRTCL_TO_NDX_CASE(ICMP)
  ARAD_PP_L3_PRTCL_TO_NDX_CASE(ICMPV6)
  ARAD_PP_L3_PRTCL_TO_NDX_CASE(IPV4)
  ARAD_PP_L3_PRTCL_TO_NDX_CASE(IPV6)
  ARAD_PP_L3_PRTCL_TO_NDX_CASE(MPLS)
  default:
    *ndx = ARAD_PP_L3_NEXT_PRTCL_NDX_USER_DEFINED;
    break;
  }

  return SOC_SAND_OK;
}

#define ARAD_PP_L3_NDX_TO_PRTCL_CASE(prtcl_suffix) \
  case (ARAD_PP_L3_NEXT_PRTCL_NDX_##prtcl_suffix): \
  *prtcl_type = SOC_PPC_L3_NEXT_PRTCL_TYPE_##prtcl_suffix; \
  break;

uint32
  arad_pp_ndx_to_l3_prtcl(
    SOC_SAND_IN  uint8 ndx,
    SOC_SAND_OUT SOC_PPC_L3_NEXT_PRTCL_TYPE *prtcl_type,
    SOC_SAND_OUT uint8 *is_found
  )
{
  *is_found = TRUE;

  switch (ndx)
  {
  ARAD_PP_L3_NDX_TO_PRTCL_CASE(NONE)
  ARAD_PP_L3_NDX_TO_PRTCL_CASE(TCP)
  ARAD_PP_L3_NDX_TO_PRTCL_CASE(UDP)
  ARAD_PP_L3_NDX_TO_PRTCL_CASE(IGMP)
  ARAD_PP_L3_NDX_TO_PRTCL_CASE(ICMP)
  ARAD_PP_L3_NDX_TO_PRTCL_CASE(ICMPV6)
  ARAD_PP_L3_NDX_TO_PRTCL_CASE(IPV4)
  ARAD_PP_L3_NDX_TO_PRTCL_CASE(IPV6)
  ARAD_PP_L3_NDX_TO_PRTCL_CASE(MPLS)
  default:
    *is_found = FALSE;
    break;
  }

  return SOC_SAND_OK;
}

uint32
  arad_pp_l3_next_protocol_add(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint8                next_protocol_ndx,
    SOC_SAND_OUT uint8               *internal_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  )
{
  uint32
    res = SOC_SAND_OK,
    next_protocol_data = next_protocol_ndx,
    data = 0;
  uint8
    ip_protocol_ndx;
  uint8
    ip_protocol_add_to_hw = FALSE;
  ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA
    ihp_parser_ip_protocols_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L3_NEXT_PROTOCOL_ADD);

  *success = SOC_SAND_SUCCESS;

  

  res = arad_pp_l3_prtcl_to_ndx(
          next_protocol_ndx,
          internal_ndx
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (*internal_ndx == ARAD_PP_L3_NEXT_PRTCL_NDX_USER_DEFINED)
  {
    
    res = arad_sw_db_multiset_add(
            unit,
			ARAD_SW_DB_CORE_ANY,
            ARAD_PP_SW_DB_MULTI_SET_L3_PROTOCOL,
            &next_protocol_data,
            &data,
            &ip_protocol_add_to_hw,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    ip_protocol_ndx = (uint8)(ARAD_PP_IP_PROTOCOL_UD_NDX_TO_FIELD_VAL(data));

    if (ip_protocol_add_to_hw)
    {
      
      ihp_parser_ip_protocols_tbl_data.ip_protocol = next_protocol_ndx;

      res = arad_pp_ihp_parser_ip_protocols_tbl_set_unsafe(
              unit,
              data,
              &ihp_parser_ip_protocols_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }

    *internal_ndx = ip_protocol_ndx;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l3_next_protocol_add()", 0, 0);
}


uint32
  arad_pp_l3_next_protocol_remove(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint8                next_protocol_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    internal_ndx;
  uint8
    is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L3_NEXT_PROTOCOL_REMOVE);

  

  res = arad_pp_l3_next_protocol_find(
          unit,
          next_protocol_ndx,
          &internal_ndx,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l3_next_protocol_remove()", 0, 0);
}


uint32
  arad_pp_l3_next_protocol_find(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint8                next_protocol_ndx,
    SOC_SAND_OUT uint8               *internal_ndx,
    SOC_SAND_OUT uint8             *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L3_NEXT_PROTOCOL_FIND);

  

  res = arad_pp_l3_prtcl_to_ndx(
          next_protocol_ndx,
          internal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  *is_found = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l3_next_protocol_find()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_general_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_general;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_general_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_general;
}

uint32  
   arad_pp_fwd_decision_lag_verify(
     SOC_SAND_IN int unit,
     SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO *fwd_decision
  )
{
  uint32
    max_nof_groups;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  max_nof_groups = 
    arad_ports_lag_nof_lag_groups_get_unsafe(
      unit
    );

  if(fwd_decision->type == SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(fwd_decision->dest_id, max_nof_groups, ARAD_PP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR, 20, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fwd_decision_lag_verify()",0,0);
}

uint32
  SOC_PPC_TRAP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_TRAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_ACTION_PROFILE, &(info->action_profile), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_TRAP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_OUTLIF_verify(
    SOC_SAND_IN  SOC_PPC_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PPC_NOF_OUTLIF_ENCODE_TYPES, ARAD_PP_GENERAL_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->val, ARAD_PP_GENERAL_VAL_MAX, ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_OUTLIF_verify()",0,0);
}

uint32
  SOC_PPC_MPLS_COMMAND_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_COMMAND *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->command, ARAD_PP_GENERAL_COMMAND_VAL_MAX, ARAD_PP_GENERAL_COMMAND_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->push_profile, ARAD_PP_GENERAL_PUSH_PROFILE_MAX, ARAD_PP_GENERAL_PUSH_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_COMMAND_verify()",0,0);
}

uint32
  SOC_PPC_EEI_verify(
    SOC_SAND_IN  SOC_PPC_EEI *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PPC_NOF_EEI_TYPES_ARAD, ARAD_PP_GENERAL_EEI_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  if (info->type == SOC_PPC_EEI_TYPE_TRILL)
  {
    ARAD_PP_STRUCT_VERIFY(SOC_SAND_PP_TRILL_DEST, &(info->val.trill_dest), 20, exit);
  }
  else if (info->type == SOC_PPC_EEI_TYPE_MPLS)
  {
    ARAD_PP_STRUCT_VERIFY(SOC_PPC_MPLS_COMMAND, &(info->val.mpls_command), 21, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EEI_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_DECISION_TYPE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_TYPE       type,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_TYPE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(type)
  {
  case SOC_PPC_FRWRD_DECISION_TYPE_DROP:
    break;
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
    ARAD_PP_STRUCT_VERIFY(SOC_PPC_OUTLIF, &(info->outlif), 11, exit);
    break;

  case SOC_PPC_FRWRD_DECISION_TYPE_MC:
  case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
    ARAD_PP_STRUCT_VERIFY(SOC_PPC_EEI, &(info->eei), 10, exit);
    break;

  case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
    ARAD_PP_STRUCT_VERIFY(SOC_PPC_TRAP_INFO, &(info->trap_info), 12, exit);
    break;
  
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR, 18, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_DECISION_TYPE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_DECISION_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PPC_FRWRD_DECISION_INFO_TYPE_MAX, ARAD_PP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  res = SOC_PPC_FRWRD_DECISION_TYPE_INFO_verify(
          info->type,
          &(info->additional_info)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_DECISION_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_DECISION_INFO_with_encode_type_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE appl_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO *info
  )
{
  uint32 res = SOC_SAND_OK;  

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(appl_type, ARAD_PP_FRWRD_DECISION_APPLICATION_TYPES, ARAD_PP_FRWRD_DEST_ASD_FORMAT_TYPE_OUT_OF_RANGE_ERR, 4, exit);  

  
  switch(info->type)
  {
    
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<ARAD_PP_ENCODE_NOF_BITS_19_FLOW_ID)-1, ARAD_PP_FRWRD_DEST_FLOW_ID_OUT_OF_RANGE_ERR, 10, exit);    
      break;
    
  case SOC_PPC_FRWRD_DECISION_TYPE_MC:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<ARAD_PP_ENCODE_NOF_BITS_19_MC_ID(unit))-1, ARAD_PP_FRWRD_DEST_MC_ID_OUT_OF_RANGE_ERR, 30, exit);
      break;

    
  case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, SOC_DPP_DEFS_GET(unit, nof_fecs), ARAD_PP_FRWRD_DEST_FEC_PTR_OUT_OF_RANGE_ERR, 40, exit);    
      break;
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, (1<<ARAD_PP_ENCODE_NOF_BITS_19_SYS_PORT)-1, ARAD_PP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR, 60, exit);    
      break;
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
      res = arad_pp_fwd_decision_lag_verify(unit, info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);    
    break;
    
  case SOC_PPC_FRWRD_DECISION_TYPE_DROP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, 0, ARAD_PP_FRWRD_DEST_DROP_ID_OUT_OF_RANGE_ERR, 100, exit);
    break;

  
  case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id, 0, ARAD_PP_FRWRD_DEST_TRAP_ID_OUT_OF_RANGE_ERR, 110, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.trap_info.action_profile.trap_code, SOC_PPC_NOF_TRAP_CODES-1, ARAD_PP_FRWRD_DEST_TRAP_CODE_OUT_OF_RANGE_ERR, 120, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.trap_info.action_profile.frwrd_action_strength, (1<<ARAD_PP_ENCODE_NOF_BITS_19_TRAP_FWD)-1, ARAD_PP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR, 130, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.trap_info.action_profile.snoop_action_strength, (1<<ARAD_PP_ENCODE_NOF_BITS_19_TRAP_SNOOP)-1, ARAD_PP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR, 140, exit);
  break;

  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_DEST_ENCODE_TYPE_OUT_OF_RANGE_ERR, 34, exit);
  }

  

    
    switch(info->additional_info.outlif.type)
    {
    case SOC_PPC_OUTLIF_ENCODE_TYPE_NONE:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.outlif.val, 0, ARAD_PP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR, 44, exit);
      break;
    case SOC_PPC_OUTLIF_ENCODE_TYPE_EEP:
      ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, info->additional_info.outlif.val, ARAD_PP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR, 46, exit);
      break;

    case SOC_PPC_OUTLIF_ENCODE_TYPE_RAW:
    case SOC_PPC_OUTLIF_ENCODE_TYPE_RAW_INVALID:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.outlif.val, SOC_DPP_CONFIG(unit)->pp.nof_global_out_lifs-1, ARAD_PP_FRWRD_DEST_OUTLIF_VAL_OUT_OF_RANGE_ERR, 46, exit);
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_DEST_OUTLIF_TYPE_OUT_OF_RANGE_ERR, 48, exit);
    }

  switch(info->additional_info.eei.type)
  {
  case SOC_PPC_EEI_TYPE_EMPTY:
    break;

  case SOC_PPC_EEI_TYPE_TRILL:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.trill_dest.dest_nick, (1<<ARAD_PP_ASD_NOF_BITS_TRILL_NICK)-1, ARAD_PP_FRWRD_DEST_NICK_OUT_OF_RANGE_ERR, 68, exit);
    break;

  case SOC_PPC_EEI_TYPE_MPLS:
      switch(info->additional_info.eei.val.mpls_command.command)
      {
        case SOC_PPC_MPLS_COMMAND_TYPE_PUSH:
        case SOC_PPC_MPLS_COMMAND_TYPE_POP:
        case SOC_PPC_MPLS_COMMAND_TYPE_SWAP:
          break;
        default:
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_DEST_MPLS_COMMAND_OUT_OF_RANGE_ERR, 74, exit);
      }
    SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.mpls_command.label, (1<<ARAD_PP_ASD_NOF_BITS_MPLS_LABEL)-1, ARAD_PP_FRWRD_DEST_MPLS_LABEL_OUT_OF_RANGE_ERR, 76, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.mpls_command.push_profile, (1<<ARAD_PP_ASD_NOF_BITS_MPLS_PUSH_PROFILE)-1, ARAD_PP_FRWRD_DEST_MPLS_PUSH_PROFILE_OUT_OF_RANGE_ERR, 78, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.mpls_command.tpid_profile, (1<<SOC_PPC_EEI_POP_TPID_PROFILE_NOF_BITS)-1, ARAD_PP_FRWRD_DEST_MPLS_TPID_PROFILE_OUT_OF_RANGE_ERR, 79, exit);
    if(info->additional_info.eei.val.mpls_command.command == SOC_PPC_MPLS_COMMAND_TYPE_POP) {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.mpls_command.pop_next_header, SOC_PPC_NOF_PKT_FRWRD_TYPES-1, ARAD_PP_FRWRD_DEST_MPLS_POP_NEXT_HEADER_OUT_OF_RANGE_ERR, 80, exit);
    }
  break;

  case SOC_PPC_EEI_TYPE_OUTLIF:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.outlif , (1<<SOC_DPP_DEFS_GET(unit,out_lif_nof_bits))-1, ARAD_PP_FRWRD_DEST_OUTLIF_VAL_INVALID_ERR, 85, exit);
  break;

  case SOC_PPC_EEI_TYPE_MIM:
        SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.isid, (1<<ARAD_PP_EEI_ISID_NOF_BITS)-1, ARAD_PP_FRWRD_DEST_ISID_VAL_INVALID_ERR, 95, exit);
        break;
  case SOC_PPC_EEI_TYPE_RAW: 
      SOC_SAND_ERR_IF_ABOVE_MAX(info->additional_info.eei.val.raw, (1<<ARAD_PP_EEI_NOF_BITS)-1, ARAD_PP_FRWRD_DEST_RAW_VAL_INVALID_ERR, 96, exit);
      break;

  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_DEST_EEI_TYPE_OUT_OF_RANGE_ERR, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_DECISION_INFO_with_encode_type_verify()",0,0);
}
uint32
  SOC_PPC_ACTION_PROFILE_verify(
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->trap_code, SOC_PPC_NOF_TRAP_CODES-1, ARAD_PP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->frwrd_action_strength, ARAD_PP_GENERAL_FRWRD_ACTION_STRENGTH_MAX, ARAD_PP_GENERAL_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->snoop_action_strength, ARAD_PP_GENERAL_SNOOP_ACTION_STRENGTH_MAX, ARAD_PP_GENERAL_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR, 12, exit);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_ACTION_PROFILE_verify()",0,0);
}

uint32
  SOC_PPC_TPID_PROFILE_verify(
    SOC_SAND_IN  SOC_PPC_TPID_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid1_index, ARAD_PP_GENERAL_TPID1_INDEX_MAX, ARAD_PP_GENERAL_TPID1_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid2_index, ARAD_PP_GENERAL_TPID2_INDEX_MAX, ARAD_PP_GENERAL_TPID2_INDEX_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_TPID_PROFILE_verify()",0,0);
}

uint32
  SOC_PPC_PEP_KEY_verify(
    SOC_SAND_IN  SOC_PPC_PEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->port, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_PEP_KEY_verify()",0,0);
}

uint32
  SOC_PPC_IP_ROUTING_TABLE_ITER_verify(
    SOC_SAND_IN  SOC_PPC_IP_ROUTING_TABLE_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->type, SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED, SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_PREFIX_IP_ORDERED, ARAD_PP_IP_ITER_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_IP_ROUTING_TABLE_ITER_verify()",0,0);
}

uint32
  SOC_PPC_IP_ROUTING_TABLE_RANGE_verify(
    SOC_SAND_IN  SOC_PPC_IP_ROUTING_TABLE_RANGE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_IP_ROUTING_TABLE_ITER, &(info->start), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entries_to_scan, ARAD_PP_GENERAL_ENTRIES_TO_SCAN_MAX, ARAD_PP_GENERAL_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entries_to_act, ARAD_PP_GENERAL_ENTRIES_TO_ACT_MAX, ARAD_PP_GENERAL_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_IP_ROUTING_TABLE_RANGE_verify()",0,0);
}

uint32
  arad_pp_SAND_PP_SYS_PORT_ID_verify(
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->sys_port_type, SOC_SAND_PP_SYS_PORT_TYPE_LAG, ARAD_PP_SYS_PORT_TYPE_OUT_OF_RANGE_ERR, 11, exit);

  if (info->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_LAG)
  {
     SOC_SAND_ERR_IF_ABOVE_MAX(info->sys_id, ARAD_NOF_LAG_GROUPS_MAX - 1, ARAD_PP_FRWRD_DEST_LAG_ID_OUT_OF_RANGE_ERR, 12, exit);
  }

  if (info->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->sys_id, ARAD_PP_SYS_PORT_MAX, ARAD_PP_FRWRD_DEST_PHY_PORT_ID_OUT_OF_RANGE_ERR, 20, exit);
  }
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_IP_ROUTING_TABLE_RANGE_verify()",0,0);
}

const char*
  ARAD_PP_DEST_ENCODE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_DEST_ENCODE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_PP_DEST_ENCODE_TYPE_19_BITS:
    str = "19_bits";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}



#define ARAD_PP_IP_TUNNEL_LIF_LEARN_BUFFER_MSB(unit) (SOC_DPP_DEFS_GET(unit, ip_tunnel_lif_learn_data_nof_bits) -1)
#define ARAD_PP_IP_TUNNEL_LIF_LEARN_BUFFER_MSB_ARAD (SOC_DPP_IP_TUNNEL_LIF_LEARN_DATA_NOF_BITS_ARAD -1)

#define ARAD_PP_AC_MP_LIF_LEARN_BUFFER_MSB(unit)     (SOC_DPP_DEFS_GET(unit, ac_mp_lif_learn_data_nof_bits)     -1)
#define ARAD_PP_AC_MP_LIF_LEARN_BUFFER_MSB_ARAD     (SOC_DPP_AC_MP_LIF_LEARN_DATA_NOF_BITS_ARAD     -1)

uint32
    arad_pp_fwd_decision_to_learn_buffer(
        SOC_SAND_IN  int                         unit,
        SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_TYPE    fwd_decision_type,
        SOC_SAND_IN  uint32                         dest_id,
        SOC_SAND_IN  SOC_PPC_LIF_ENTRY_TYPE         lif_entry_type,
        SOC_SAND_OUT uint32                         *learn_buffer,
        SOC_SAND_OUT uint32                         *additional_info
        )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(learn_buffer);
    SOC_SAND_CHECK_NULL_INPUT(additional_info);

    *additional_info = 0;

    
    if (dest_id == 0) {
        *learn_buffer = 0; 
    }
    else {
        
        if (fwd_decision_type == SOC_PPC_FRWRD_DECISION_TYPE_FEC) {
            

            
            if (lif_entry_type == SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {  
                *learn_buffer = (1<<ARAD_PP_IP_TUNNEL_LIF_LEARN_BUFFER_MSB(unit)) | dest_id; 
            }
            
            else if ((lif_entry_type == SOC_PPC_LIF_ENTRY_TYPE_AC) || (lif_entry_type == SOC_PPC_LIF_ENTRY_TYPE_EXTENDER)) { 
                *learn_buffer = (1<<ARAD_PP_AC_MP_LIF_LEARN_BUFFER_MSB(unit)) | dest_id; 
            }
            
            else {
                *learn_buffer = 0; 
            }
        } 
        
        else if (fwd_decision_type == SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW) {
            
            if (lif_entry_type == SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {  
                *learn_buffer = (dest_id >> SOC_DPP_DEFS_GET(unit, ip_tunnel_lif_uc_flow_learn_data_static_lsbs)); 
            }
            else if (lif_entry_type == SOC_PPC_LIF_ENTRY_TYPE_AC) { 
                *learn_buffer = (dest_id >> SOC_DPP_DEFS_GET(unit, ac_mp_lif_uc_flow_learn_data_static_lsbs)); 
            } 
            
            else {
                *learn_buffer = 0; 
            }
        }
        
        else {
            *learn_buffer = 0; 
        }
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fwd_decision_to_learn_buffer()",0,0);

}



#define ARAD_PP_AC_MP_LIF_LEARN_BUFFER_MASK(unit)  ((1 << (SOC_DPP_DEFS_GET(unit, ac_mp_lif_learn_data_nof_bits) -1)) -1)

#define ARAD_PP_AC_MP_LIF_LEARN_BUFFER_MASK_ARAD  ((1 << (SOC_DPP_AC_MP_LIF_LEARN_DATA_NOF_BITS_ARAD -1)) -1)

#define ARAD_PP_IP_TUNNEL_LIF_LEARN_BUFFER_MASK(unit)  ((1 << (SOC_DPP_DEFS_GET(unit, ip_tunnel_lif_learn_data_nof_bits) -1)) -1)

#define ARAD_PP_IP_TUNNEL_LIF_LEARN_BUFFER_MASK_ARAD  ((1 << (SOC_DPP_IP_TUNNEL_LIF_LEARN_DATA_NOF_BITS_ARAD -1)) -1)

uint32
    arad_pp_fwd_decision_from_learn_buffer(
        SOC_SAND_IN  int                       unit,
        SOC_SAND_IN  uint32                       learn_buffer,
        SOC_SAND_IN  SOC_PPC_LIF_ENTRY_TYPE         lif_entry_type,
        SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO *fwd_decision
        )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(fwd_decision);

    
    if (learn_buffer == 0) {
        fwd_decision->dest_id = 0; 
    }
    
    else {
        
        if ((lif_entry_type == SOC_PPC_LIF_ENTRY_TYPE_AC) || (lif_entry_type == SOC_PPC_LIF_ENTRY_TYPE_EXTENDER)) {
            
            if (learn_buffer >> ARAD_PP_AC_MP_LIF_LEARN_BUFFER_MSB(unit) == 1) {
                fwd_decision->dest_id = (learn_buffer & ARAD_PP_AC_MP_LIF_LEARN_BUFFER_MASK(unit));
                fwd_decision->type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
            }
            
            else {
                fwd_decision->dest_id = ((learn_buffer & ARAD_PP_AC_MP_LIF_LEARN_BUFFER_MASK(unit)) << SOC_DPP_DEFS_GET(unit, ac_mp_lif_uc_flow_learn_data_static_lsbs));
                fwd_decision->type = SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW;
            }
        }
        
        else if (lif_entry_type == SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {
            
            if (learn_buffer >> ARAD_PP_IP_TUNNEL_LIF_LEARN_BUFFER_MSB(unit) == 1) {
                fwd_decision->dest_id = (learn_buffer & ARAD_PP_IP_TUNNEL_LIF_LEARN_BUFFER_MASK(unit));
                fwd_decision->type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
            } 
            
            else {
                fwd_decision->dest_id = ((learn_buffer & ARAD_PP_IP_TUNNEL_LIF_LEARN_BUFFER_MASK(unit)) << SOC_DPP_DEFS_GET(unit, ip_tunnel_lif_uc_flow_learn_data_static_lsbs));
                fwd_decision->type = SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW;
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fwd_decision_from_learn_buffer()",0,0);
}

int
arad_pp_init_orientation_maps(int unit){ 
    int res;
    uint32
        sh_mask = 0, 
        sh_mask_lo = 0, sh_mask_hi = 0,
        entry,
        i;
    uint64
        orientation_map_val;
    soc_reg_above_64_val_t 
        profile_to_orientation_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) == 3) {
        if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && !SOC_IS_JERICHO_PLUS(unit) && soc_property_get(unit, "logical_port_mim_out", 0) && soc_property_get(unit, "bcm886xx_roo_enable", 0)) {
            SOC_SAND_CHECK_FUNC_RESULT(_SHR_E_CONFIG, 10, exit);
        }
    }

    
    
    
    res = arad_pp_occ_mgmt_get_app_mask(unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_ORIENTATION, &sh_mask);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
    COMPILER_64_ZERO(orientation_map_val);
    for(entry = 0; entry < 32; entry++){
        
        uint32 profile_bit = ((entry >> 1) & sh_mask) ? 1 : 0; 
        uint32 is_hub = entry & 1;
        
        uint64 on_mask;
        if (entry < 16) {
            COMPILER_64_SET(on_mask, 0, ((profile_bit << 1) | is_hub) << (entry * 2));
        } else {
            COMPILER_64_SET(on_mask, ((profile_bit << 1) | is_hub) << ((entry - 16) * 2), 0);
        }
        COMPILER_64_OR(orientation_map_val, on_mask);
    }
    res = WRITE_IHB_IN_LIF_ORIENTATION_MAPr(unit, SOC_CORE_ALL, orientation_map_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
    
     
    
    res = arad_pp_occ_mgmt_get_app_mask(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION, &sh_mask);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    
    
    for(i = 0; (1 << i) <= sh_mask; i++){
        if (sh_mask & (1 << i)){
            sh_mask_lo |= (1 << i);
            sh_mask_hi = sh_mask & (~sh_mask_lo);
            break;
        }
    }
    
    SOC_REG_ABOVE_64_CLEAR(profile_to_orientation_val);
    for(entry = 0; entry < 64; entry++){
        
        uint32
            profile_bit_lo = (entry & sh_mask_lo) ? 1 : 0, 
            profile_bit_hi = (entry & sh_mask_hi) ? 1 : 0;
                
        
        if ((entry > 31) && SOC_IS_JERICHO_B0_AND_ABOVE(unit) && !SOC_IS_JERICHO_PLUS(unit) && soc_property_get(unit, "logical_port_mim_out", 0) && soc_property_get(unit, "bcm886xx_roo_enable", 0)) {
            profile_to_orientation_val[entry * 2 / 32] |= 3 << ((entry * 2) % 32);
        } else {
            profile_to_orientation_val[entry * 2 / 32] |= ((profile_bit_hi << 1) | profile_bit_lo) << ((entry * 2) % 32);
        }
    }
    res = WRITE_EPNI_CFG_MAP_OUTLIF_PROFILE_TO_ORIENTATIONr(unit, SOC_CORE_ALL, profile_to_orientation_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_init_orientation_maps()",0,0);
}


int arad_pp_fwd_decision_learn_buffer_bist(int unit) {

    int32 res;

    SOC_PPC_FRWRD_DECISION_TYPE fwd_decision_types[2] = 
        {SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW, SOC_PPC_FRWRD_DECISION_TYPE_FEC};
    SOC_PPC_LIF_ENTRY_TYPE      lif_entry_types[2] = 
        {SOC_PPC_LIF_ENTRY_TYPE_AC, SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF};

    int iteration = 0; 
    int fwd_decision_types_index = 0; 
    int lif_entry_types_index = 0; 
    uint32 learn_buffer, learn_buffer_expected; 
    uint32 additional_info = 0; 

    uint32 seed=0x65464;
    SOC_SAND_RAND soc_sand_rand;
    SOC_PPC_FRWRD_DECISION_INFO fwd_decision; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    soc_sand_rand_seed_set(&soc_sand_rand, seed);

    
    SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision); 

    for (iteration = 0; iteration < 1000; iteration++) {
        
        for (lif_entry_types_index = 0; lif_entry_types_index < 2; lif_entry_types_index++) {
            for (fwd_decision_types_index = 0; fwd_decision_types_index < 2; fwd_decision_types_index++) {

                
                learn_buffer = 0; 
                if (lif_entry_types[lif_entry_types_index] == SOC_PPC_LIF_ENTRY_TYPE_AC) {
                    
                    learn_buffer = soc_sand_rand_int(&soc_sand_rand) & ARAD_PP_AC_MP_LIF_LEARN_BUFFER_MASK(unit); 
                    
                    if (fwd_decision_types[fwd_decision_types_index] == SOC_PPC_FRWRD_DECISION_TYPE_FEC) {
                        
                        learn_buffer |= (1 << ARAD_PP_AC_MP_LIF_LEARN_BUFFER_MSB(unit)); 
                    } else {
                        
                        learn_buffer = learn_buffer & ~(1 << ARAD_PP_AC_MP_LIF_LEARN_BUFFER_MSB(unit)); 
                    }
                } else if (lif_entry_types[lif_entry_types_index] == SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {
                    learn_buffer = soc_sand_rand_int(&soc_sand_rand) & ARAD_PP_IP_TUNNEL_LIF_LEARN_BUFFER_MASK(unit); 
                    
                    if (fwd_decision_types[fwd_decision_types_index] == SOC_PPC_FRWRD_DECISION_TYPE_FEC) {
                        
                        learn_buffer |= (1 << ARAD_PP_IP_TUNNEL_LIF_LEARN_BUFFER_MSB(unit)); 
                    } else {
                        
                        learn_buffer = learn_buffer & ~(1 << ARAD_PP_IP_TUNNEL_LIF_LEARN_BUFFER_MSB(unit)); 
                    }
                }
                learn_buffer_expected = learn_buffer; 

                
                res = arad_pp_fwd_decision_from_learn_buffer(unit, 
                                                             learn_buffer, 
                                                             lif_entry_types[lif_entry_types_index], 
                                                             &fwd_decision
                                                             ); 
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

                
                learn_buffer = 0; 
                res = arad_pp_fwd_decision_to_learn_buffer(unit, 
                                                           fwd_decision_types[fwd_decision_types_index], 
                                                           fwd_decision.dest_id, 
                                                           lif_entry_types[lif_entry_types_index], 
                                                           &learn_buffer, 
                                                           &additional_info); 
                SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

                
                if (lif_entry_types[lif_entry_types_index] == SOC_PPC_LIF_ENTRY_TYPE_AC) {
                    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(learn_buffer, learn_buffer_expected, SOC_E_INTERNAL, 30, exit); 
                } else if (lif_entry_types[lif_entry_types_index] == SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {
                    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(learn_buffer, learn_buffer_expected, SOC_E_INTERNAL, 40, exit); 
                }
            }
        }
    }


exit: 
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fwd_decision_learn_buffer_bist()", 0, 0);

}




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

