/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_DIAG_INCLUDED__

#define __SOC_PPD_API_DIAG_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_diag.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#include <soc/dpp/PPD/ppd_api_frwrd_bmact.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv4.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_rif.h>
#include <soc/dpp/PPD/ppd_api_llp_sa_auth.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_mpls_term.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv6.h>
#include <soc/dpp/PPD/ppd_api_frwrd_trill.h>
#include <soc/dpp/PPD/ppd_api_lif_ing_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_eg_encap.h>













typedef enum
{
  
  SOC_PPD_DIAG_SAMPLE_ENABLE_SET = SOC_PPD_PROC_DESC_BASE_DIAG_FIRST,
  SOC_PPD_DIAG_SAMPLE_ENABLE_SET_PRINT,
  SOC_PPD_DIAG_SAMPLE_ENABLE_GET,
  SOC_PPD_DIAG_SAMPLE_ENABLE_GET_PRINT,
  SOC_PPD_DIAG_MODE_INFO_SET,
  SOC_PPD_DIAG_MODE_INFO_SET_PRINT,
  SOC_PPD_DIAG_MODE_INFO_GET,
  SOC_PPD_DIAG_MODE_INFO_GET_PRINT,
  SOC_PPD_DIAG_PKT_TRACE_CLEAR,
  SOC_PPD_DIAG_PKT_TRACE_CLEAR_PRINT,
  SOC_PPD_DIAG_RECEIVED_PACKET_INFO_GET,
  SOC_PPD_DIAG_RECEIVED_PACKET_INFO_GET_PRINT,
  SOC_PPD_DIAG_PARSING_INFO_GET,
  SOC_PPD_DIAG_PARSING_INFO_GET_PRINT,
  SOC_PPD_DIAG_TERMINATION_INFO_GET,
  SOC_PPD_DIAG_TERMINATION_INFO_GET_PRINT,
  SOC_PPD_DIAG_FRWRD_LKUP_INFO_GET,
  SOC_PPD_DIAG_FRWRD_LKUP_INFO_GET_PRINT,
  SOC_PPD_DIAG_FRWRD_LPM_LKUP_GET,
  SOC_PPD_DIAG_FRWRD_LPM_LKUP_GET_PRINT,
  SOC_PPD_DIAG_TRAPS_INFO_GET,
  SOC_PPD_DIAG_TRAPS_INFO_GET_PRINT,
  SOC_PPD_DIAG_FRWRD_DECISION_TRACE_GET,
  SOC_PPD_DIAG_FRWRD_DECISION_TRACE_GET_PRINT,
  SOC_PPD_DIAG_LEARNING_INFO_GET,
  SOC_PPD_DIAG_LEARNING_INFO_GET_PRINT,
  SOC_PPD_DIAG_ING_VLAN_EDIT_INFO_GET,
  SOC_PPD_DIAG_ING_VLAN_EDIT_INFO_GET_PRINT,
  SOC_PPD_DIAG_PKT_ASSOCIATED_TM_INFO_GET,
  SOC_PPD_DIAG_PKT_ASSOCIATED_TM_INFO_GET_PRINT,
  SOC_PPD_DIAG_ENCAP_INFO_GET,
  SOC_PPD_DIAG_ENCAP_INFO_GET_PRINT,
  SOC_PPD_DIAG_EG_DROP_LOG_GET,
  SOC_PPD_DIAG_EG_DROP_LOG_GET_PRINT,
  SOC_PPD_DIAG_DB_LIF_LKUP_INFO_GET,
  SOC_PPD_DIAG_DB_LIF_LKUP_INFO_GET_PRINT,
  SOC_PPD_DIAG_DB_LEM_LKUP_INFO_GET,
  SOC_PPD_DIAG_DB_LEM_LKUP_INFO_GET_PRINT,
  SOC_PPD_DIAG_PKT_SEND,
  SOC_PPD_DIAG_PKT_SEND_PRINT,
  SOC_PPD_DIAG_GET_PROCS_PTR,
  

  SOC_PPD_DIAG_VSI_INFO_GET,

  
  SOC_PPD_DIAG_PROCEDURE_DESC_LAST
} SOC_PPD_DIAG_PROCEDURE_DESC;










uint32
  soc_ppd_diag_sample_enable_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                               enable
  );


uint32
  soc_ppd_diag_sample_enable_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT uint8                               *enable
  );


uint32
  soc_ppd_diag_mode_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_DIAG_MODE_INFO                      *mode_info
  );


uint32
  soc_ppd_diag_mode_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_DIAG_MODE_INFO                      *mode_info
  );


uint32
  soc_ppd_diag_vsi_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_VSI_INFO                      *vsi_info
  );


uint32
  soc_ppd_diag_pkt_trace_clear(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  uint32                                pkt_trace
  );


uint32
  soc_ppd_diag_received_packet_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_RECEIVED_PACKET_INFO           *rcvd_pkt_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  );


uint32
  soc_ppd_diag_parsing_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_INFO                   *pars_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  );


uint32
  soc_ppd_diag_termination_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_TERM_INFO                      *term_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  );


uint32
  soc_ppd_diag_frwrd_lkup_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_LKUP_INFO                *frwrd_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  );


uint32
  soc_ppd_diag_frwrd_lpm_lkup_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_DIAG_IPV4_VPN_KEY              *lpm_key,
    SOC_SAND_OUT uint32                           *fec_ptr,
    SOC_SAND_OUT uint8                          *found,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT            *ret_val
  );


uint32
  soc_ppd_diag_traps_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_TRAPS_INFO                     *traps_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  );

uint32
  soc_ppd_diag_frwrd_decision_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                core_id,
    SOC_SAND_OUT uint32                             *frwrd_trace_info
  );

uint32
  soc_ppd_diag_frwrd_decision_trace_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO      *frwrd_trace_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  );


uint32
  soc_ppd_diag_learning_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_LEARN_INFO                     *learn_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    
  );


uint32
  soc_ppd_diag_ing_vlan_edit_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_VLAN_EDIT_RES                  *vec_res,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    
  );


uint32
  soc_ppd_diag_pkt_associated_tm_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_PKT_TM_INFO                    *pkt_tm_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val

  );


uint32
  soc_ppd_diag_encap_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_ENCAP_INFO                     *encap_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  );


uint32
  soc_ppd_diag_eg_drop_log_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_DIAG_EG_DROP_LOG_INFO               *eg_drop_log
  );


uint32
  soc_ppd_diag_db_lif_lkup_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT SOC_PPC_DIAG_LIF_LKUP_INFO                  *lkup_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  );


uint32
  soc_ppd_diag_db_lem_lkup_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_LKUP_TYPE                  *type,
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_KEY                        *key,
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_VALUE                      *val,
    SOC_SAND_OUT uint8                               *valid
  );



uint32 
  soc_ppd_diag_egress_vlan_edit_info_get( 
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO           *prm_vec_res,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

