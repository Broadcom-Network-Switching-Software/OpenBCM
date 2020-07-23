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

#define _ERR_MSG_MODULE_NAME BSL_SOC_MANAGEMENT


#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_general.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/drv.h>


























uint32
  arad_register_device(
             uint32                  *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr,
    SOC_SAND_INOUT int                 *unit_ptr
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_REGISTER_DEVICE);

  SOC_SAND_CHECK_NULL_INPUT(unit_ptr);

  res = arad_register_device_unsafe(
          base_address,
          reset_device_ptr,
          unit_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_register_device()",0,0);
}


uint32
  arad_unregister_device(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_UNREGISTER_DEVICE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_unregister_device_unsafe(
    unit
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_unregister_device()",0,0);
}


uint32
  arad_plus_mgmt_module_to_credit_worth_map_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_IN  uint32    credit_value_type 
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_FUNC_RESULT(arad_mgmt_system_fap_id_verify(unit, fap_id), 10, exit);
    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }
    if (credit_value_type > SOC_TMC_FAP_CREDIT_VALUE_MAX) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
    }
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    SOC_SAND_CHECK_FUNC_RESULT(arad_plus_mgmt_module_to_credit_worth_map_set_unsafe(unit, fap_id, credit_value_type), 100, exit_semaphore);
exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_plus_mgmt_module_to_credit_worth_map_set()", unit, fap_id);
}

uint32
  arad_plus_mgmt_module_to_credit_worth_map_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_OUT uint32    *credit_value_type 
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(credit_value_type);
    SOC_SAND_CHECK_FUNC_RESULT(arad_mgmt_system_fap_id_verify(unit, fap_id), 10, exit);
    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    SOC_SAND_CHECK_FUNC_RESULT(arad_plus_mgmt_module_to_credit_worth_map_get_unsafe(unit, fap_id, credit_value_type), 100, exit_semaphore);
exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_plus_mgmt_module_to_credit_worth_map_get()", unit, fap_id);
}

uint32
  arad_mgmt_init_sequence_phase1(
    SOC_SAND_IN     int                 unit,
    SOC_SAND_IN     ARAD_MGMT_INIT           *init,
    SOC_SAND_IN     uint8                 silent
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_INIT_SEQUENCE_PHASE1);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(init);

  res = arad_mgmt_init_sequence_phase1_verify(unit, init);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_mgmt_init_sequence_phase1_unsafe(
    unit,
    init,
    silent
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_init_sequence_phase1()",0,0);
}


uint32
  arad_mgmt_init_sequence_phase2(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_INIT_SEQUENCE_PHASE2);

  if (!SOC_DPP_CONTROL(unit)->is_modid_set_called) {

      SOC_SAND_CHECK_DRIVER_AND_DEVICE;

      SOC_SAND_TAKE_DEVICE_SEMAPHORE;

      res = arad_mgmt_init_sequence_phase2_unsafe(
        unit,
        silent
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);
      SOC_DPP_CONTROL(unit)->is_modid_set_called = 1;

    exit_semaphore:
      SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_init_sequence_phase2()",0,0);
}


uint32
  arad_mgmt_system_fap_id_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_fap_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_SYSTEM_FAP_ID_SET);
 SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_mgmt_system_fap_id_verify(
    unit,
    sys_fap_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_mgmt_system_fap_id_set_unsafe(
    unit,
    sys_fap_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
 SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_system_fap_id_set()",0,0);
}



uint32
  arad_mgmt_tm_domain_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tm_domain
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_mgmt_tm_domain_verify(
    unit,
    tm_domain
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_mgmt_tm_domain_set_unsafe(
    unit,
    tm_domain
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
 SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_tm_domain_set()",0,0);
}


uint32
  arad_mgmt_tm_domain_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                 *tm_domain
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tm_domain);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_mgmt_tm_domain_get_unsafe(
    unit,
    tm_domain
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
 SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_tm_domain_get()",0,0);
}  


uint32
  arad_mgmt_all_ctrl_cells_enable_set(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_ALL_CTRL_CELLS_ENABLE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_PCID_LITE_SKIP(unit);

  res = arad_mgmt_all_ctrl_cells_enable_verify(
    unit,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  if (!soc_feature(unit, soc_feature_no_fabric)) {
      res = arad_mgmt_all_ctrl_cells_enable_set_unsafe(
        unit,
        enable,
        ARAD_MGMT_ALL_CTRL_CELLS_FLAGS_NONE
      );
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_all_ctrl_cells_enable_set()",0,0);
}


uint32
  arad_force_tdm_bypass_traffic_to_fabric_set(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  int     enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  SOC_SAND_CHECK_FUNC_RESULT(arad_force_tdm_bypass_traffic_to_fabric_set_unsafe(unit, enable) , 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_force_tdm_bypass_traffic_to_fabric_set()", enable, 0);
}


uint32
  arad_force_tdm_bypass_traffic_to_fabric_get(
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT int     *enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  SOC_SAND_CHECK_FUNC_RESULT(arad_force_tdm_bypass_traffic_to_fabric_get_unsafe(unit, enable) , 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_force_tdm_bypass_traffic_to_fabric_get()", 0, 0);
}


uint32
  arad_mgmt_all_ctrl_cells_enable_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint8  *enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_ALL_CTRL_CELLS_ENABLE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_mgmt_all_ctrl_cells_enable_get_unsafe(
    unit,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_all_ctrl_cells_enable_get()",0,0);
}



uint32
  arad_mgmt_enable_traffic_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_ENABLE_TRAFFIC_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_mgmt_enable_traffic_get_unsafe(
    unit,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_enable_traffic_get()",0,0);
}



uint32
  arad_mgmt_max_pckt_size_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  uint32                       max_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_MAX_PCKT_SIZE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_mgmt_max_pckt_size_set_verify(
          unit,
          port_ndx,
          conf_mode_ndx,
          max_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_mgmt_max_pckt_size_set_unsafe(
          unit,
          port_ndx,
          conf_mode_ndx,
          max_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_mgmt_max_pckt_size_set()", port_ndx, 0);
}


uint32
  arad_mgmt_max_pckt_size_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT uint32                       *max_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_MAX_PCKT_SIZE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(max_size);

  res = arad_mgmt_max_pckt_size_get_verify(
          unit,
          port_ndx,
          conf_mode_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_mgmt_max_pckt_size_get_unsafe(
          unit,
          port_ndx,
          conf_mode_ndx,
          max_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_mgmt_max_pckt_size_get()", port_ndx, 0);
}


uint32
  arad_mgmt_ocb_mc_range_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      range_ndx,
    SOC_SAND_IN  ARAD_MGMT_OCB_MC_RANGE         *range
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_OCB_MC_RANGE_SET);
  SOC_SAND_ERR_IF_ABOVE_MAX(range_ndx, 1, ARAD_MGMT_OCB_MC_RANGE_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_CHECK_NULL_INPUT(range);
  if (range->min != ARAD_MGMT_OCB_MC_RANGE_DISABLE) {
      SOC_SAND_ERR_IF_ABOVE_NOF(range->min, SOC_DPP_CONFIG(unit)->tm.nof_mc_ids, ARAD_MGMT_OCB_MC_RANGE_MIN_OUT_OF_RANGE_ERR, 11, exit);
  }
  if (range->max != ARAD_MGMT_OCB_MC_RANGE_DISABLE) {
      SOC_SAND_ERR_IF_ABOVE_NOF(range->max, SOC_DPP_CONFIG(unit)->tm.nof_mc_ids, ARAD_MGMT_OCB_MC_RANGE_MAX_OUT_OF_RANGE_ERR, 12, exit);
  }

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_mgmt_ocb_mc_range_set_unsafe(
          unit,
          range_ndx,
          range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
   
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_mgmt_ocb_mc_range_set()", 0, 0);
}

uint32
  arad_mgmt_ocb_mc_range_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      range_ndx,
    SOC_SAND_OUT ARAD_MGMT_OCB_MC_RANGE         *range
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_OCB_MC_RANGE_GET);

  SOC_SAND_CHECK_NULL_INPUT(range);
  SOC_SAND_ERR_IF_ABOVE_MAX(range_ndx, 1, ARAD_MGMT_OCB_MC_RANGE_INDEX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  
  res = arad_mgmt_ocb_mc_range_get_unsafe(
          unit,
          range_ndx,
          range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_mgmt_ocb_mc_range_get()", 0, 0);
}


uint32
  arad_mgmt_ocb_voq_eligible_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    q_category_ndx,
    SOC_SAND_IN  uint32                    rate_class_ndx,
    SOC_SAND_IN  uint32                    tc_ndx,
    SOC_SAND_IN  ARAD_MGMT_OCB_VOQ_INFO       *info,
    SOC_SAND_OUT ARAD_MGMT_OCB_VOQ_INFO    *exact_info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_OCB_VOQ_ELIGIBLE_SET);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);
  SOC_SAND_ERR_IF_ABOVE_MAX(q_category_ndx, 3, ARAD_MGMT_OCB_VOQ_ELIGIBLE_QUEUE_CATEGORY_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(rate_class_ndx, 63, ARAD_MGMT_OCB_VOQ_ELIGIBLE_RATE_CLASS_INDEX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, 7, ARAD_MGMT_OCB_VOQ_ELIGIBLE_TRAFFIC_CLASS_INDEX_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  
  res = arad_mgmt_ocb_voq_eligible_set_unsafe(
          unit,
          q_category_ndx,
          rate_class_ndx,
          tc_ndx,
          info,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_mgmt_ocb_voq_eligible_set()", 0, 0);
}

uint32
  arad_mgmt_ocb_voq_eligible_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    q_category_ndx,
    SOC_SAND_IN  uint32                    rate_class_ndx,
    SOC_SAND_IN  uint32                    tc_ndx,
    SOC_SAND_OUT ARAD_MGMT_OCB_VOQ_INFO       *info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_OCB_VOQ_ELIGIBLE_GET);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_ERR_IF_ABOVE_MAX(q_category_ndx, 3, ARAD_MGMT_OCB_VOQ_ELIGIBLE_QUEUE_CATEGORY_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(rate_class_ndx, 63, ARAD_MGMT_OCB_VOQ_ELIGIBLE_RATE_CLASS_INDEX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, 7, ARAD_MGMT_OCB_VOQ_ELIGIBLE_TRAFFIC_CLASS_INDEX_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  
  res = arad_mgmt_ocb_voq_eligible_get_unsafe(
          unit,
          q_category_ndx,
          rate_class_ndx,
          tc_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_mgmt_ocb_voq_eligible_get()", 0, 0);
}


uint32
  arad_mgmt_ocb_voq_eligible_dynamic_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    qid,
    SOC_SAND_IN  uint32                    enable
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  
  res = arad_mgmt_ocb_voq_eligible_dynamic_set_unsafe(
          unit,
          qid,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_mgmt_ocb_voq_eligible_dynamic_set()", 0, 0);
}




uint32 soc_arad_mgmt_rev_b0_set(
    SOC_SAND_IN  int       unit)
{
    uint32
        res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = soc_arad_mgmt_rev_b0_set_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_arad_mgmt_rev_b0_set()", 0, 0);
}

#ifdef BCM_88660_A0

uint32 soc_arad_mgmt_rev_arad_plus_set(
    SOC_SAND_IN  int       unit)
{
    uint32
        res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = soc_arad_mgmt_rev_arad_plus_set_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_arad_mgmt_rev_arad_plus_set()", 0, 0);
}

#endif

void
  arad_ARAD_HW_PLL_PARAMS_clear(
    SOC_SAND_OUT ARAD_HW_PLL_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_HW_PLL_PARAMS));
  info->m = 0;
  info->n = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_INIT_DDR_clear(
    SOC_SAND_OUT ARAD_INIT_DDR *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_DDR));
  info->enable = 0;
  for (ind=0; ind<SOC_DPP_HW_DRAM_INTERFACES_MAX_ARAD; ++ind)
  {
    info->is_valid[ind] = 0;
    info->dram_clam_shell_mode[ind] = ARAD_DDR_CLAM_SHELL_MODE_DISABLED;
  }
  info->dram_type = ARAD_DRAM_NOF_TYPES;
  info->pdm_mode = ARAD_INIT_PDM_MODE_SIMPLE;
  arad_ARAD_HW_PLL_PARAMS_clear(&(info->pll_conf));
  info->nof_banks = ARAD_NOF_DRAM_NUM_BANKS;
  info->nof_columns = ARAD_NOF_DRAM_NUMS_COLUMNS;
  info->dram_size_total_mbyte = 0;
  info->dbuff_size = ARAD_ITM_DBUFF_SIZE_BYTES_1024;
  info->conf_mode = ARAD_HW_DRAM_CONF_MODE_BUFFER;
  for (ind=0; ind<ARAD_HW_DRAM_CONF_SIZE_MAX; ++ind)
  {
    arad_ARAD_REG_INFO_clear(&info->dram_conf.buffer_mode.buff_seq[ind]);
  }
  info->dram_conf.buffer_mode.buff_len = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_DRAM_PLL_clear(
    SOC_SAND_OUT ARAD_INIT_DRAM_PLL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_DRAM_PLL));
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_PLL_clear(
    SOC_SAND_OUT ARAD_INIT_PLL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_INIT_DRAM_PLL_clear(&info->dram_pll);
  info->nif_clk_freq = ARAD_INIT_SERDES_REF_CLOCK_125;
  info->fabric_clk_freq = ARAD_INIT_SERDES_REF_CLOCK_125;
  info->synthesizer_clock_freq = ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY_25_MHZ;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
  

void
  arad_ARAD_INIT_FABRIC_clear(
    SOC_SAND_OUT ARAD_INIT_FABRIC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_FABRIC));
  info->enable = 0;
  info->connect_mode = ARAD_FABRIC_NOF_CONNECT_MODES;
  info->ftmh_extension = ARAD_PORTS_FTMH_EXT_OUTLIF_NEVER;
  info->ftmh_lb_ext_mode = ARAD_MGMT_FTMH_LB_EXT_MODE_DISABLED;
  info->ftmh_stacking_ext_mode = 0x0;
  info->is_fe600 = FALSE;
  info->is_128_in_system = FALSE;
  info->dual_pipe_tdm_packet = FALSE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_STAT_IF_clear(
    SOC_SAND_OUT ARAD_INIT_STAT_IF *info
  )
{
  int i;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_STAT_IF));
  info->stat_if_enable = 0;
  for (i=0 ; i < ARAD_MGMT_STAT_IF_REPORT_INFO_ARRAY_SIZE_MAX; i++)
  {
      SOC_TMC_STAT_IF_REPORT_INFO_clear(&(info->stat_if_info[i]));
  }
  
  
  SOC_SAND_MAGIC_NUM_SET;
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_INIT_CORE_FREQ_clear(
    SOC_SAND_OUT ARAD_INIT_CORE_FREQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_CORE_FREQ));
  info->enable = 0;
  info->frequency = 0;
  info->system_ref_clock = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_CREDIT_clear(
    SOC_SAND_OUT ARAD_INIT_CREDIT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_CREDIT));
  info->credit_worth_enable = 0;
  info->credit_worth = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_OCB_clear(
    SOC_SAND_OUT ARAD_INIT_OCB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_OCB));

  info->ocb_enable = OCB_DISABLED;
  info->databuffer_size = 0;
  info->repartition_mode = ARAD_OCB_REPARTITION_MODE_80_PRESENTS_UNICAST;

  SOC_SAND_MAGIC_NUM_SET;
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  arad_ARAD_MGMT_INIT_clear(
    SOC_SAND_OUT ARAD_MGMT_INIT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_MGMT_INIT));
  arad_ARAD_INIT_DDR_clear(&(info->dram));
  arad_ARAD_INIT_FABRIC_clear(&(info->fabric));
  info->eg_cgm_scheme = ARAD_NOF_EGR_QUEUING_PARTITION_SCHEMES;
  arad_ARAD_INIT_CORE_FREQ_clear(&(info->core_freq));
  ARAD_INIT_PORTS_clear(&(info->ports));
  info->tdm_mode = ARAD_MGMT_NOF_TDM_MODES;
  ARAD_INIT_STAT_IF_clear(&(info->stat_if));
  ARAD_INIT_CREDIT_clear(&(info->credit));
  ARAD_INIT_PLL_clear(&(info->pll));
  ARAD_INIT_OCB_clear(&(info->ocb));
  arad_ARAD_INIT_FC_clear(&(info->fc));

  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_MGMT_OCB_MC_RANGE_clear(
    SOC_SAND_OUT ARAD_MGMT_OCB_MC_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_MGMT_OCB_MC_RANGE));
  info->min  = 0;
  info->max = 0;

  SOC_SAND_MAGIC_NUM_SET;
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_MGMT_OCB_VOQ_INFO_clear(
    SOC_SAND_OUT ARAD_MGMT_OCB_VOQ_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MGMT_OCB_VOQ_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_MGMT_PCKT_SIZE_clear(
    SOC_SAND_OUT ARAD_MGMT_PCKT_SIZE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MGMT_PCKT_SIZE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  arad_ARAD_INIT_PORT_HDR_TYPE_clear(
    SOC_SAND_OUT ARAD_INIT_PORT_HDR_TYPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_PORT_HDR_TYPE));
  info->port_ndx = 0;
  info->header_type_in = ARAD_PORT_NOF_HEADER_TYPES;
  info->header_type_out = ARAD_PORT_NOF_HEADER_TYPES;
  info->first_header_size = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_INIT_PORT_TO_IF_MAP_clear(
    SOC_SAND_OUT ARAD_INIT_PORT_TO_IF_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_PORT_TO_IF_MAP));
  info->port_ndx = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_PP_PORT_clear(
    SOC_SAND_OUT ARAD_INIT_PP_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_PP_PORT));
  info->profile_ndx = 0;
  ARAD_PORT_PP_PORT_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_PP_PORT_MAP_clear(
    SOC_SAND_OUT ARAD_INIT_PP_PORT_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_PP_PORT_MAP));
  info->port_ndx = 0;
  info->pp_port = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_EGR_Q_PROFILE_MAP_clear(
    SOC_SAND_OUT ARAD_INIT_EGR_Q_PROFILE_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_EGR_Q_PROFILE_MAP));
  info->port_ndx = 0;
  info->conf = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_PORTS_clear(
    SOC_SAND_OUT ARAD_INIT_PORTS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->lag_mode = ARAD_PORT_LAG_MODE_1K_16;
  
  info->tm_domain = 0x0;
  
  arad_ARAD_SWAP_INFO_clear(&(info->swap_info));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_INIT_FC_clear(
    SOC_SAND_OUT ARAD_INIT_FC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_INIT_FC));


  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1


const char*
  arad_ARAD_HW_DRAM_CONF_MODE_to_string(
    SOC_SAND_IN ARAD_HW_DRAM_CONF_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_HW_DRAM_CONF_MODE_BUFFER:
    str = "ARAD_HW_DRAM_CONF_MODE_BUFFER";
  break;

  case ARAD_HW_DRAM_CONF_MODE_PARAMS:
    str = "ARAD_HW_DRAM_CONF_MODE_PARAMS";
  break;

  case ARAD_HW_NOF_DRAM_CONF_MODES:
    str = "ARAD_HW_NOF_DRAM_CONF_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  arad_ARAD_HW_PLL_PARAMS_print(
    SOC_SAND_IN ARAD_HW_PLL_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " m:                     %u\n\r"),info->m));
  LOG_CLI((BSL_META_U(unit,
                      " n:                     %u\n\r"),info->n));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_INIT_DDR_print(
    SOC_SAND_IN ARAD_INIT_DDR *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_INIT_CORE_FREQ_print(
    SOC_SAND_IN  ARAD_INIT_CORE_FREQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "frequency: %u[kHz]\n\r"),info->frequency));
  LOG_CLI((BSL_META_U(unit,
                      "system_ref_clock: %u[kHz]\n\r"),info->system_ref_clock));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_INIT_FABRIC_print(
    SOC_SAND_IN ARAD_INIT_FABRIC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " Enable:                      %u\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      " Connect_mode:                %s \n\r"),
           arad_ARAD_FABRIC_CONNECT_MODE_to_string(info->connect_mode)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "Ftmh_extension:               %s \n\r"),
           arad_ARAD_PORTS_FTMH_EXT_OUTLIF_to_string(info->ftmh_extension)
           ));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_MGMT_INIT_print(
    SOC_SAND_IN ARAD_MGMT_INIT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Dram:\n\r")));
  arad_ARAD_INIT_DDR_print(&(info->dram));
  LOG_CLI((BSL_META_U(unit,
                      "Fabric:\n\r")));
  arad_ARAD_INIT_FABRIC_print(&(info->fabric));
  LOG_CLI((BSL_META_U(unit,
                      "Core_freq:")));
  arad_ARAD_INIT_CORE_FREQ_print(&(info->core_freq));
  LOG_CLI((BSL_META_U(unit,
                      "Ports:")));
  ARAD_INIT_PORTS_print(&(info->ports));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_INIT_PORT_HDR_TYPE_print(
    SOC_SAND_IN ARAD_INIT_PORT_HDR_TYPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " Port_ndx: %u\n\r"),info->port_ndx));
  LOG_CLI((BSL_META_U(unit,
                      " header_type_in: %s \n\r"),
           arad_ARAD_PORT_HEADER_TYPE_to_string(info->header_type_in)
           ));
  LOG_CLI((BSL_META_U(unit,
                      " header_type_out: %s \n\r"),
           arad_ARAD_PORT_HEADER_TYPE_to_string(info->header_type_out)
           ));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_INIT_PORT_TO_IF_MAP_print(
    SOC_SAND_IN ARAD_INIT_PORT_TO_IF_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " Port_ndx: %u\n\r"),info->port_ndx));
  LOG_CLI((BSL_META_U(unit,
                      " Conf:\n\r")));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_PP_PORT_print(
    SOC_SAND_IN  ARAD_INIT_PP_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "profile_ndx: %u\n\r"),info->profile_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "conf:")));
  ARAD_PORT_PP_PORT_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_PP_PORT_MAP_print(
    SOC_SAND_IN  ARAD_INIT_PP_PORT_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "port_ndx: %u\n\r"),info->port_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "pp_port: %u\n\r"),info->pp_port));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_EGR_Q_PROFILE_MAP_print(
    SOC_SAND_IN  ARAD_INIT_EGR_Q_PROFILE_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "port_ndx: %u\n\r"),info->port_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "conf: %u\n\r"),info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_INIT_PORTS_print(
    SOC_SAND_IN  ARAD_INIT_PORTS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  arad_ARAD_SWAP_INFO_print(&(info->swap_info));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 
