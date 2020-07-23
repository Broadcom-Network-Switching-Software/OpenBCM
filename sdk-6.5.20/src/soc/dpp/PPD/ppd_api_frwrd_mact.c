/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif 




















uint32
  soc_ppd_frwrd_mact_app_to_prefix_get(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint32                                      app_id,
    SOC_SAND_OUT uint32                                      *prefix
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prefix);

  res = arad_pp_frwrd_mact_app_to_prefix_get_unsafe(
          unit,
          app_id,
          prefix);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_app_to_prefix_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_entry_pack(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint8                                       insert,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint32                                      *data,
    SOC_SAND_OUT uint8                                       *data_len
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_PACK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_SAND_CHECK_NULL_INPUT(data_len);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
  res = arad_pp_frwrd_mact_entry_pack_verify(
            unit,
            mac_entry_key,
            mac_entry_value
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_entry_pack_unsafe(
          unit,
          insert,
          add_type,
          mac_entry_key,
          mac_entry_value,
          data,
          data_len);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_entry_add()", 0, 0);
}



uint32
  soc_ppd_frwrd_mact_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_mact_entry_add_verify(
          unit,
          add_type,
          mac_entry_key,
          mac_entry_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_entry_add_unsafe(
          unit,
          add_type,
          mac_entry_key,
          mac_entry_value,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_entry_add()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  int                                sw_only
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);

  res = arad_pp_frwrd_mact_entry_remove_verify(
          unit,
          mac_entry_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_entry_remove_unsafe(
          unit,
          mac_entry_key,
          sw_only
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_entry_remove()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_ipmc_table_clear(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_ipmc_table_clear_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_ipmc_table_clear()", 0, 0);
}



uint32
  soc_ppd_frwrd_mact_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_frwrd_mact_entry_get_verify(
          unit,
          mac_entry_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_entry_get_unsafe(
          unit,
          mac_entry_key,
          mac_entry_value,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_entry_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_traverse_by_mac_limit_per_tunnel(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_TABLE_BLOCK_RANGE              
      block_range;
  ARAD_PP_LEM_ACCESS_KEY                  
      rule_key;
  ARAD_PP_LEM_ACCESS_KEY                  
      rule_key_mask;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_BY_MAC_LIMIT_PER_TUNNEL_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);

  res = arad_pp_frwrd_mact_traverse_verify(
          unit,
          rule,
          action,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
  block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

  ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key_mask);
  
  res = arad_pp_frwrd_mact_rule_to_key_convert(
          unit,
          rule,
          &rule_key,
          &rule_key_mask,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit_semaphore);

  res = arad_pp_frwrd_mact_traverse_by_mac_limit_per_tunnel_internal_unsafe(
          unit,
          &rule_key,
          &rule_key_mask,
          &rule->value_rule,
          &block_range,
          action,
          nof_matched_entries
        );

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_traverse_by_mac_limit_per_tunnel()", 0, 0);
}



uint32
  soc_ppd_frwrd_mact_traverse(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                                 wait_till_finish,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_TABLE_BLOCK_RANGE              
      block_range;
  ARAD_PP_LEM_ACCESS_KEY                  
      rule_key;
  ARAD_PP_LEM_ACCESS_KEY                  
      rule_key_mask;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);

  res = arad_pp_frwrd_mact_traverse_verify(
          unit,
          rule,
          action,
          wait_till_finish
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
  block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

  ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key_mask);
  
  res = arad_pp_frwrd_mact_rule_to_key_convert(
          unit,
          rule,
          &rule_key,
          &rule_key_mask,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit_semaphore);

  res = arad_pp_frwrd_mact_traverse_internal_unsafe(
          unit,
          &rule_key,
          &rule_key_mask,
          &rule->value_rule,
          &block_range,
          action,
          wait_till_finish,
          nof_matched_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_traverse()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_traverse_status_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO     *status
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(status);

  

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_traverse_status_get_unsafe(
          unit,
          status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_traverse_status_get()", 0, 0);
}



uint32
  soc_ppd_frwrd_mact_traverse_mode_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET);

#ifdef CRASH_RECOVERY_SUPPORT
  if (SOC_IS_DONE_INIT(unit))
  {
      if (BCM_UNIT_DO_HW_READ_WRITE(unit))
      {
          SOC_SAND_EXIT_AND_SEND_ERROR("WARNING: in soc_ppd_frwrd_mact_traverse_mode_info_set() - HA HW Log mode is not implemented for this function", 0, 0);
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 8050, exit);
      }
  }
#endif 


  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_traverse_mode_info_set_unsafe(
          unit,
          mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_traverse_mode_info_set()", 0, 0);
}

uint32
  soc_ppd_frwrd_mact_traverse_mode_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_traverse_mode_info_get_unsafe(
          unit,
          mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_traverse_mode_info_get()", 0, 0);
}



uint32
  soc_ppd_frwrd_mact_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TABLE_TYPE               mact_type,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(mact_keys);
  SOC_SAND_CHECK_NULL_INPUT(mact_vals);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_frwrd_mact_get_block_verify(
          unit,
          rule,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  if ((mact_type == SOC_PPC_FRWRD_MACT_TABLE_SW_ONLY) || (SAL_BOOT_PLISIM)) {
      res = arad_pp_frwrd_mact_get_sw_block_unsafe(
              unit,
              rule,
              block_range,
              mact_keys,
              mact_vals,
              nof_entries
            );
  }
  else
  {
      res = arad_pp_frwrd_mact_get_block_unsafe(
              unit,
              rule,
              block_range,
              mact_keys,
              mact_vals,
              nof_entries
            );
  }

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_get_block()", 0, 0);
}
#if SOC_PPC_DEBUG_IS_LVL1

uint32
  soc_ppd_frwrd_mact_print_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_IN  uint32                                nof_entries,
    SOC_SAND_IN  uint32                                flavors
  )
{
  uint32
    indx;
  SOC_SAND_PP_IPV4_ADDRESS
    ipv4_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(mact_keys);
  SOC_SAND_CHECK_NULL_INPUT(mact_vals);
  soc_sand_SAND_PP_IPV4_ADDRESS_clear(&ipv4_addr);

  LOG_CLI((BSL_META_U(unit,
                      " ----------------------------------------------------------------------------------\n\r"
                      "|                                      MAC Table                                   |\n\r"
                      "|----------------------------------------------------------------------------------|\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "|      |                   |   |SA |grp| Destination  |     Additional Info        |\n\r"
                      "| FID  |      MAC/IP       |Dyn|Drp|   | Type  | Val  |Type |                      |\n\r"
                      " ---------------------------------------------------------------------------------- \n\r")));

  for (indx = 0; indx < nof_entries; ++indx)
  {
    if (mact_keys[indx].key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR)
    {
      LOG_CLI((BSL_META_U(unit,
                          "| %-5d| "), mact_keys[indx].key_val.mac.fid));
      soc_sand_SAND_PP_MAC_ADDRESS_print((&mact_keys[indx].key_val.mac.mac));
    }
    else if (mact_keys[indx].key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC)
    {
      LOG_CLI((BSL_META_U(unit,
                          "| %-5d| "), mact_keys[indx].key_val.ipv4_mc.fid));
      
      ipv4_addr.address[0] = mact_keys[indx].key_val.ipv4_mc.dip;
      soc_sand_SAND_PP_IPV4_ADDRESS_print(&ipv4_addr);
      LOG_CLI((BSL_META_U(unit,
                          "         ")));
    }
    LOG_CLI((BSL_META_U(unit,
                        " |")));

    LOG_CLI((BSL_META_U(unit,
                        " %s |"), (mact_vals[indx].aging_info.is_dynamic)?"V":"X"));
    LOG_CLI((BSL_META_U(unit,
                        " %s |"), (mact_vals[indx].frwrd_info.drop_when_sa_is_known)?"V":"X"));
    if (mact_vals[indx].group == 0) {
        LOG_CLI((BSL_META_U(unit,
                            " - |")));
    }
    else {
        LOG_CLI((BSL_META_U(unit,
                            " %d |"), mact_vals[indx].group));
    }

    SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
      "",
      &(mact_vals[indx].frwrd_info.forward_decision)
    );

    LOG_CLI((BSL_META_U(unit,
                        "|\n\r")));
  }

  LOG_CLI((BSL_META_U(unit,
                      " ----------------------------------------------------------------------------- \n\r"
               )));
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_frwrd_mact_print_block()",0,0);
}
#endif



#include <soc/dpp/SAND/Utils/sand_footer.h>


