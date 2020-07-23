/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_PPD_API_GENERAL_INCLUDED__

#define __SOC_PPD_API_GENERAL_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_general.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>






#define  SOC_PPD_IGNORE_VAL 0xFFFFFFFF

#define SOC_PPD_NOF_BITS_MPLS_LABEL                (20)




#define SOC_PPD_DO_NOTHING_AND_EXIT                    \
  do                                              \
  {                                               \
    SOC_SAND_IGNORE_UNUSED_VAR(res);                  \
    goto exit;                                    \
  } while(0)


#define ARAD_MACRO(macro,params, res) res = SOC_SAND_OK;\
    ARAD_PP_##macro params

#define ARAD_MACRO_FUNC(func,params) ARAD_PP_##func params


#define SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, macro, params, res)    \
  switch (SOC_SAND_DEVICE_TYPE_GET(unit))        \
  {                                                   \
  case SOC_SAND_DEV_ARAD:                             \
      ARAD_MACRO(macro, params, res);                 \
	  break;                                          \
    default:                                          \
      res =  SOC_PPD_INVALID_DEVICE_TYPE_ERR;         \
  }




#define SOC_PPD_ARAD_TMP_DEVICE_MACRO_VAL_CALL(unit, func, params)    \
	((SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_ARAD) ? ARAD_MACRO_FUNC(func, params) : -1 )







#define SOC_PPD_FRWRD_DECISION_DROP_SET(unit, fwd_decision, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_DROP_SET, (unit, fwd_decision), res);

#define SOC_PPD_FRWRD_DECISION_LOCAL_CPU_SET(unit, fwd_decision, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_LOCAL_CPU_SET, (unit, fwd_decision), res);

#define SOC_PPD_FRWRD_DECISION_PHY_SYS_PORT_SET(unit, fwd_decision, phy_port, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_PHY_SYS_PORT_SET, (unit, fwd_decision, phy_port), res);

#define SOC_PPD_FRWRD_DECISION_LAG_SET(unit, fwd_decision, lag_id, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_LAG_SET, (unit, fwd_decision, lag_id), res);

#define SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, fwd_decision, mc_id, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_MC_GROUP_SET, (unit, fwd_decision, mc_id), res);

#define SOC_PPD_FRWRD_DECISION_FEC_SET(unit, fwd_decision, fec_id, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_FEC_SET, (unit, fwd_decision, fec_id), res);

#define SOC_PPD_FRWRD_DECISION_EXPL_FLOW_SET(unit, fwd_decision, flow_id, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_EXPL_FLOW_SET, (unit, fwd_decision, flow_id), res);

#define SOC_PPD_FRWRD_DECISION_TRAP_SET(unit, fwd_decision, _trap_code, frwrd_strength, snp_strength, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_TRAP_SET, (unit, fwd_decision, _trap_code, frwrd_strength,snp_strength), res);

#define SOC_PPD_FRWRD_DECISION_AC_SET(unit, fwd_decision, ac_id, is_lag, sys_port_id, res)  \
  SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_AC_SET, (unit, fwd_decision, ac_id, is_lag, sys_port_id), res);

#define SOC_PPD_FRWRD_DECISION_AC_WITH_COSQ_SET(unit, fwd_decision, ac_id,flow_id, res)  \
  SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_AC_WITH_COSQ_SET, (unit, fwd_decision, ac_id, flow_id), res);

#define SOC_PPD_FRWRD_DECISION_PROTECTED_AC_SET(unit, fwd_decision, ac_id, fec_index, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_PROTECTED_AC_SET, (unit, fwd_decision, ac_id, fec_index), res);

#define SOC_PPD_FRWRD_DECISION_PWE_SET(unit, fwd_decision, pwe_id, is_lag, sys_port_id, res)  \
  SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_PWE_SET, (unit, fwd_decision, pwe_id, is_lag, sys_port_id), res);

#define SOC_PPD_FRWRD_DECISION_TRILL_SET(unit, fwd_decision, nick, is_multi, fec_or_mc_id, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_TRILL_SET, (unit, fwd_decision, nick, is_multi, fec_or_mc_id), res);

#define SOC_PPD_FRWRD_DECISION_PWE_WITH_COSQ_SET(unit, fwd_decision, pwe_id, flow_id, res)  \
  SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_PWE_WITH_COSQ_SET, (unit, fwd_decision, pwe_id, flow_id), res);


#define SOC_PPD_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_SET(unit, fwd_decision, vc_label, push_profile, fec_index, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_PWE_PROTECTED_TUNNEL_SET, (unit, fwd_decision, vc_label, push_profile, fec_index), res);


#define SOC_PPD_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_WITH_OUTLIF_SET(unit, fwd_decision, fec_index, pwe_outlif, res) \
    ARAD_MACRO(FRWRD_DECISION_PWE_PROTECTED_TUNNEL_WITH_OUTLIF_SET, (unit, fwd_decision, fec_index, pwe_outlif), res); 


#define SOC_PPD_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_WITH_EEI_SET(unit, fwd_decision, fec_index, pwe_outlif, res) \
    ARAD_MACRO(FRWRD_DECISION_PWE_PROTECTED_TUNNEL_WITH_EEI_SET, (unit, fwd_decision, fec_index, pwe_outlif), res); 


#define SOC_PPD_FRWRD_DECISION_PROTECTED_PWE_SET(unit, fwd_decision, fec_index, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_PROTECTED_PWE_SET, (unit, fwd_decision, fec_index), res);

#define SOC_PPD_FRWRD_DECISION_ILM_SWAP_SET(unit, fwd_decision, swap_label, fec_index, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_ILM_SWAP_SET, (unit, fwd_decision, swap_label, fec_index), res);

#define SOC_PPD_FRWRD_DECISION_ILM_PUSH_SET(unit, fwd_decision,label, push_profile, fec_index, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_ILM_PUSH_SET, (unit, fwd_decision,label, push_profile, fec_index), res);

#define SOC_PPD_FRWRD_DECISION_MAC_IN_MAC_SET(unit, fwd_decision, isid_id, fec_id, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_MAC_IN_MAC_SET, (unit, fwd_decision, isid_id, fec_id), res);








#define SOC_PPD_CUD_EEP_GET(unit, eep_ndx)   \
    SOC_PPD_ARAD_TMP_DEVICE_MACRO_VAL_CALL(unit, CUD_EEP_GET, (unit, eep_ndx));



#define SOC_PPD_CUD_VSI_GET(unit, vsi_ndx)   \
    SOC_PPD_ARAD_TMP_DEVICE_MACRO_VAL_CALL(unit, CUD_VSI_GET, (unit, vsi_ndx));

#define SOC_PPD_CUD_AC_GET(unit, ac_ndx)   \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_VAL_CALL(unit, CUD_AC_GET, (unit, ac_ndx));


#define SOC_PPD_DEST_TRAP_VAL_SET(__dest_val, __trap_code,__fwd_strenght, __snoop_strength)   \
            __dest_val = (((__snoop_strength)<<19) | (__fwd_strenght)<<16)|((__trap_code))

#define SOC_PPD_DEST_TRAP_VAL_GET_TRAP_CODE(__dest_val)   \
            ((__dest_val) & 0xFFFF)

#define SOC_PPD_DEST_TRAP_VAL_GET_FWD_STRENGTH(__dest_val)   \
            (((__dest_val) >> 16) & 0x7)

#define SOC_PPD_DEST_TRAP_VAL_GET_SNP_STRENGTH(__dest_val)   \
            (((__dest_val) >> 19) & 0x3)


#define SOC_PPC_EEI_ENCODING_MPLS_COMMAND(identifier,mpls_label)           ((identifier << SOC_PPD_NOF_BITS_MPLS_LABEL) | mpls_label)
#define SOC_PPD_MPLS_LABEL_FROM_EEI_COMMAND_ENCODING(eei)                  (eei & 0xfffff)
#define SOC_PPD_MPLS_IDENTIFIER_FROM_EEI_COMMAND_ENCODING(eei)           (eei >> SOC_PPD_NOF_BITS_MPLS_LABEL)





typedef enum
{
  
  SOC_PPD_GENERAL_GET_PROCS_PTR = SOC_PPD_PROC_DESC_BASE_GENERAL_FIRST,
  

   SOC_PPD_FWD_DECISION_TO_SAND_DEST,
   SOC_PPD_SAND_DEST_TO_FWD_DECISION,



  
  SOC_PPD_GENERAL_PROCEDURE_DESC_LAST
} SOC_PPD_GENERAL_PROCEDURE_DESC;








uint32
  soc_ppd_fwd_decision_to_sand_dest(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN SOC_PPC_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID *dest_id
  );

uint32
  soc_ppd_sand_dest_to_fwd_decision(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN SOC_SAND_PP_DESTINATION_ID *dest_id,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO *fwd_decision
  );


uint32
  soc_ppd_l2_next_prtcl_type_allocate_test(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               l2_next_prtcl_type,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE            *success
  );

int
  soc_ppd_mpls_speculative_map_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                              nibble_value,
    SOC_SAND_IN  bcm_switch_mpls_next_protocol_t    next_protocol
  );

int
  soc_ppd_mpls_speculative_map_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                              nibble_value,
    SOC_SAND_OUT  bcm_switch_mpls_next_protocol_t   *next_protocol
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
