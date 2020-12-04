/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_DIAG
#include <shared/bsl.h>




#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>

#include <soc/sand/sand_signals.h>






















uint32
  soc_ppd_diag_sample_enable_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_SAMPLE_ENABLE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_diag_sample_enable_set_verify(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_sample_enable_set_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_sample_enable_set()", 0, 0);
}


uint32
  soc_ppd_diag_sample_enable_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_SAMPLE_ENABLE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  res = arad_pp_diag_sample_enable_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_sample_enable_get_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_sample_enable_get()", 0, 0);
}


uint32
  soc_ppd_diag_mode_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_DIAG_MODE_INFO                      *mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_MODE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mode_info);

  res = arad_pp_diag_mode_info_set_verify(
          unit,
          mode_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_mode_info_set_unsafe(
          unit,
          mode_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_mode_info_set()", 0, 0);
}


uint32
  soc_ppd_diag_mode_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_DIAG_MODE_INFO                      *mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_MODE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mode_info);

  res = arad_pp_diag_mode_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_mode_info_get_unsafe(
          unit,
          mode_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_mode_info_get()", 0, 0);
}


uint32
  soc_ppd_diag_vsi_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_VSI_INFO                      *vsi_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_VSI_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  res = arad_pp_diag_vsi_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_vsi_info_get_unsafe(
          unit,
          core_id,
          vsi_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_mode_info_get()", 0, 0);
}


uint32
  soc_ppd_diag_pkt_trace_clear(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  uint32                                  pkt_trace
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_PKT_TRACE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_diag_pkt_trace_clear_verify(
          unit,
          pkt_trace
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_pkt_trace_clear_unsafe(
          unit,
          core_id,
          pkt_trace
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_pkt_trace_clear()", 0, 0);
}


uint32
  soc_ppd_diag_received_packet_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_RECEIVED_PACKET_INFO           *rcvd_pkt_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_RECEIVED_PACKET_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rcvd_pkt_info);

  res = arad_pp_diag_received_packet_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_received_packet_info_get_unsafe(
          unit,
          core_id,
          rcvd_pkt_info,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_received_packet_info_get()", 0, 0);
}


uint32
  soc_ppd_diag_parsing_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_INFO                   *pars_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_PARSING_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pars_info);

  res = arad_pp_diag_parsing_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_parsing_info_get_unsafe(
          unit,
          core_id,
          pars_info,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_parsing_info_get()", 0, 0);
}


uint32
  soc_ppd_diag_termination_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_TERM_INFO                      *term_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_TERMINATION_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_info);

  res = arad_pp_diag_termination_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_termination_info_get_unsafe(
          unit,
          core_id,
          term_info,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_termination_info_get()", 0, 0);
}


uint32
  soc_ppd_diag_frwrd_lkup_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_LKUP_INFO                *frwrd_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_FRWRD_LKUP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(frwrd_info);

  res = arad_pp_diag_frwrd_lkup_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_frwrd_lkup_info_get_unsafe(
          unit,
          core_id,
          frwrd_info,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_frwrd_lkup_info_get()", 0, 0);
}


uint32
  soc_ppd_diag_frwrd_lpm_lkup_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core_id,
    SOC_SAND_IN  SOC_PPC_DIAG_IPV4_VPN_KEY              *lpm_key,
    SOC_SAND_OUT uint32                             *fec_ptr,
    SOC_SAND_OUT uint8                            *found,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT              *ret_val    
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lpm_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_ptr);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_diag_frwrd_lpm_lkup_get_verify(
          unit,
          lpm_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_frwrd_lpm_lkup_get_unsafe(
          unit,
          core_id,
          lpm_key,
          fec_ptr,
          found,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_frwrd_lpm_lkup_get()", 0, 0);
}


uint32
  soc_ppd_diag_traps_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_TRAPS_INFO                     *traps_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_TRAPS_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(traps_info);

  res = arad_pp_diag_traps_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_traps_info_get_unsafe(
          unit,
          core_id,
          traps_info,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_traps_info_get()", 0, 0);
}

uint32
  soc_ppd_diag_frwrd_decision_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                core_id,
    SOC_SAND_OUT uint32                             *frwrd_trace_info
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = dpp_dsig_read(unit, core_id, "IRPP", NULL, "PMF", "Fwd_Action_Dst", frwrd_trace_info, 1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_decision_get_unsafe()", 0, 0);
}




uint32
  soc_ppd_diag_frwrd_decision_trace_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO      *frwrd_trace_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(frwrd_trace_info);

  res = arad_pp_diag_frwrd_decision_trace_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_frwrd_decision_trace_get_unsafe(
          unit,
          core_id,
          frwrd_trace_info,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_frwrd_decision_trace_get()", 0, 0);
}


uint32
  soc_ppd_diag_learning_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_LEARN_INFO                     *learn_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_LEARNING_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(learn_info);

  res = arad_pp_diag_learning_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_learning_info_get_unsafe(
          unit,
          core_id,
          learn_info,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_learning_info_get()", 0, 0);
}


uint32
  soc_ppd_diag_ing_vlan_edit_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_VLAN_EDIT_RES                  *vec_res,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vec_res);

  res = arad_pp_diag_ing_vlan_edit_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_ing_vlan_edit_info_get_unsafe(
          unit,
          core_id,
          vec_res,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_ing_vlan_edit_info_get()", 0, 0);
}


uint32
  soc_ppd_diag_pkt_associated_tm_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_PKT_TM_INFO                    *pkt_tm_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    

  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pkt_tm_info);

  res = arad_pp_diag_pkt_associated_tm_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_pkt_associated_tm_info_get_unsafe(
          unit,
          core_id,
          pkt_tm_info,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_pkt_associated_tm_info_get()", 0, 0);
}


uint32
  soc_ppd_diag_encap_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_ENCAP_INFO                     *encap_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  )
{
  uint32
    res = SOC_SAND_OK;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_ENCAP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(encap_info);

  res = arad_pp_diag_encap_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_encap_info_get_unsafe(
          unit,
          core_id,
          encap_info,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  if (*ret_val != SOC_PPC_DIAG_OK) {
      goto exit_semaphore;
  }

  
  if (encap_info->encap_info[1].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP) {
      res = arad_pp_diag_out_rif_get_unsafe(
              unit,
              core_id,
              &encap_info->rif_is_valid,
              &encap_info->out_rif
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit_semaphore);
  }

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_encap_info_get()", 0, 0);
}

uint32
  soc_ppd_diag_eg_drop_log_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_DIAG_EG_DROP_LOG_INFO               *eg_drop_log
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_EG_DROP_LOG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(eg_drop_log);

  res = arad_pp_diag_eg_drop_log_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_eg_drop_log_get_unsafe(
          unit,
          eg_drop_log
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_eg_drop_log_get()", 0, 0);
}


uint32
  soc_ppd_diag_db_lif_lkup_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT SOC_PPC_DIAG_LIF_LKUP_INFO                  *lkup_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(db_info);
  SOC_SAND_CHECK_NULL_INPUT(lkup_info);

  res = arad_pp_diag_db_lif_lkup_info_get_verify(
          unit,
          db_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_db_lif_lkup_info_get_unsafe(
          unit,
          core_id,
          db_info,
          lkup_info,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_db_lif_lkup_info_get()", 0, 0);
}


uint32
  soc_ppd_diag_db_lem_lkup_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                core_id,
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_LKUP_TYPE                  *type,
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_KEY                        *key,
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_VALUE                      *val,
    SOC_SAND_OUT uint8                                 *valid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(db_info);
  SOC_SAND_CHECK_NULL_INPUT(type);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(val);
  SOC_SAND_CHECK_NULL_INPUT(valid);

  res = arad_pp_diag_db_lem_lkup_info_get_verify(
          unit,
          db_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_db_lem_lkup_info_get_unsafe(
          unit,
          core_id,
          db_info,
          type,
          key,
          val,
          valid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_db_lem_lkup_info_get()", 0, 0);
}


uint32
  soc_ppd_diag_egress_vlan_edit_info_get (
     SOC_SAND_IN  int                                 unit,
     SOC_SAND_IN  int                                 core_id,
     SOC_SAND_OUT SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO           *prm_vec_res,
     SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val    
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_EGRESS_VLAN_EDIT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prm_vec_res);

  res = arad_pp_diag_egress_vlan_edit_info_get_verify(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_diag_egress_vlan_edit_info_get_unsafe(
          unit,
          core_id,
          prm_vec_res,
          ret_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_egress_vlan_edit_info_get()", 0, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


