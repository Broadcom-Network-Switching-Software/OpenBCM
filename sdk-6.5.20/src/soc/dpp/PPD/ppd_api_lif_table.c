/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LIF



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_table.h>






















uint32
  soc_ppd_lif_table_get_block(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE             *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                        *block_range,
    SOC_SAND_OUT SOC_PPC_LIF_ENTRY_INFO                          *entries_array,
    SOC_SAND_OUT uint32                                      *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_TABLE_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_lif_table_get_block_verify(
          unit,
          rule,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

      res = arad_pp_lif_table_get_block_unsafe(
              unit,
              rule,
              block_range,
              entries_array,
              nof_entries
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_table_get_block()", 0, 0);
}


uint32
  soc_ppd_lif_table_entry_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_ENTRY_INFO                          *lif_entry_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_TABLE_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_entry_info);

  res = arad_pp_lif_table_entry_get_verify(
          unit,
          lif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

      res = arad_pp_lif_table_entry_get_unsafe(
              unit,
              lif_ndx,
              lif_entry_info
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_table_entry_get()", lif_ndx, 0);
}


uint32
  soc_ppd_lif_table_entry_update(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                                   lif_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_INFO                          *lif_entry_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_TABLE_ENTRY_UPDATE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_entry_info);

  res = arad_pp_lif_table_entry_update_verify(
          unit,
          lif_ndx,
          lif_entry_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_table_entry_update_unsafe(
              unit,
              lif_ndx,
              lif_entry_info
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_table_entry_update()", lif_ndx, 0);
}


uint32
  soc_ppd_lif_table_entry_accessed_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                                   lif_ndx,
    SOC_SAND_IN  uint8                                      clear_access_stat,
    SOC_SAND_OUT  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO           *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_table_entry_accessed_info_get_unsafe(
          unit,
          lif_ndx,
          clear_access_stat,
          accessed_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_table_entry_accessed_info_get()", lif_ndx, 0);
}

uint32
  soc_ppd_lif_table_print_block(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE            *rule,
    SOC_SAND_IN SOC_SAND_TABLE_BLOCK_RANGE                        *block_range,
    SOC_SAND_IN SOC_PPC_LIF_ENTRY_INFO                          *entries_array,
    SOC_SAND_IN uint32                                      nof_entries
  )
{
  uint32
    indx;
  uint8
    inc_l2=FALSE,
    inc_ac=FALSE,
    inc_pwe=FALSE,
    inc_l3=FALSE;
  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO
    accessed_info;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries_array);

  if ((rule->entries_type_bm & (SOC_PPC_LIF_ENTRY_TYPE_AC|SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP|SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP)) != 0)
  {
    inc_ac = TRUE;
  }
  if ((rule->entries_type_bm & (SOC_PPC_LIF_ENTRY_TYPE_PWE)) != 0)
  {
    inc_pwe = TRUE;
  }
  if (inc_ac || inc_pwe)
  {
    inc_l2 = TRUE;;
  }
  if ((rule->entries_type_bm & (SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF|SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF)) != 0)
  {
    inc_l3 = TRUE;
  }

  if (inc_l3 && inc_l2)
  {
    LOG_CLI((BSL_META_U(unit,
                        " ------------------------\n\r"
                        "|         LIF Table      |\n\r"
                        " ------------------------\n\r"
                        "|offset| type |      |ACC|\n\r"
                        " ------------------------\n\r"
                 )));
  }
  else if (inc_l2)
  {
#ifndef COMPILER_STRING_CONST_LIMIT
    LOG_CLI((BSL_META_U(unit,
                        " -------------------------------------------------------------------------------------\n\r"
                        "|                                L2  LIF Table                                        |\n\r"
                        " -------------------------------------------------------------------------------------\n\r"
                        "|      |             |A|         Default/Learn                     |     |VID Edit|C|O|\n\r"
                        "|      |             |C| Destination  |     Additional Info        |     |   |prof|O|r|\n\r"
                        "|offset| type |      |C| Type  | Val  | Type |                     |VSID |VID |E|P|S|i|\n\r"
                        " -------------------------------------------------------------------------------------\n\r"
                 )));
#else 
    LOG_CLI((BSL_META_U(unit,
                        "|                                L2  LIF Table                                        |\n\r"
                        " -------------------------------------------------------------------------------------\n\r"
                        "|      |             |A|         Default/Learn                     |     |VID Edit|C|O|\n\r"
                        "|      |             |C| Destination  |     Additional Info        |     |   |prof|O|r|\n\r"
                        "|offset| type |      |C| Type  | Val  | Type |                     |VSID |VID |E|P|S|i|\n\r"
                 )));
#endif 
  }
  else if (inc_l3)
  {
    LOG_CLI((BSL_META_U(unit,
                        " --------------------------------------------------\n\r"
                        "|                   L3 LIF (RIF) Table             |\n\r"
                        " --------------------------------------------------\n\r"
                        "|offset| type |      |ACC| VRF |COS|TTL|RPF|Routing|\n\r"
                        " --------------------------------------------------\n\r"
                 )));
  }

  for (indx = 0; indx < nof_entries; ++indx)
  {
    if (entries_array[indx].type == SOC_PPC_LIF_ENTRY_TYPE_EMPTY)
    {
      continue;
    }
    res = soc_ppd_lif_table_entry_accessed_info_get(
            unit,
            entries_array[indx].index,
            TRUE,
            &accessed_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    LOG_CLI((BSL_META_U(unit,
                        "|%-6u"), entries_array[indx].index));
    
    LOG_CLI((BSL_META_U(unit,
                        "|%-6s"), SOC_PPC_LIF_ENTRY_TYPE_to_string(entries_array[indx].type)));

    
    if (entries_array[indx].type & (SOC_PPC_LIF_ENTRY_TYPE_AC|SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP|SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP))
    {
      
      LOG_CLI((BSL_META_U(unit,
                          "|%-6s"),SOC_PPC_L2_LIF_AC_SERVICE_TYPE_to_string(entries_array[indx].value.ac.service_type)));
      if (accessed_info.accessed)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"V"));
      }
      else
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"X"));
      }

      if (inc_l3&inc_l2)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|\n\r")));
        continue;
      }
      
      if (entries_array[indx].value.ac.service_type != SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|")));
        SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
          "",
          &(entries_array[indx].value.ac.default_frwrd.default_forwarding)
        );
      }
      else
      {
        if (entries_array[indx].value.ac.learn_record.learn_type == SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT)
        {
          LOG_CLI((BSL_META_U(unit,
                              "%-7s "), "|sys-port"));
          LOG_CLI((BSL_META_U(unit,
                              "%-5s|"),"   -" ));
          LOG_CLI((BSL_META_U(unit,
                              "%-6s|"),"   -" ));
          LOG_CLI((BSL_META_U(unit,
                              "%10s%-10s ")," ","-"));
        }
        else if (entries_array[indx].value.ac.learn_record.learn_type == SOC_PPC_L2_LIF_AC_LEARN_DISABLE)
        {
          LOG_CLI((BSL_META_U(unit,
                              "%-7s "), "|disable"));
          LOG_CLI((BSL_META_U(unit,
                              "%-5s|"),"   -" ));
          LOG_CLI((BSL_META_U(unit,
                              "%-6s|"),"   -" ));
          LOG_CLI((BSL_META_U(unit,
                              "%10s%-10s ")," ","-"));

        }
        else if (entries_array[indx].value.ac.learn_record.learn_type == SOC_PPC_L2_LIF_AC_LEARN_INFO)
        {
          SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
            "",
            &(entries_array[indx].value.ac.learn_record.learn_info)
            );
        }
      }
      
      LOG_CLI((BSL_META_U(unit,
                          "|%-5u"), entries_array[indx].value.ac.vsid));
      LOG_CLI((BSL_META_U(unit,
                          "|%-4u"), entries_array[indx].value.ac.ing_edit_info.vid));
      LOG_CLI((BSL_META_U(unit,
                          "|%-1u"), entries_array[indx].value.ac.ing_edit_info.ing_vlan_edit_profile));
      LOG_CLI((BSL_META_U(unit,
                          "|%-1u"), entries_array[indx].value.ac.ing_edit_info.edit_pcp_profile));
      LOG_CLI((BSL_META_U(unit,
                          "|%-1u"), entries_array[indx].value.ac.cos_profile));
      if (entries_array[indx].value.ac.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"H"));
      }
      else if (entries_array[indx].value.ac.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"S"));
      }
    }
    
    else if (entries_array[indx].type & (SOC_PPC_LIF_ENTRY_TYPE_PWE))
    {
      
      LOG_CLI((BSL_META_U(unit,
                          "|%-6s"),SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_to_string(entries_array[indx].value.pwe.service_type)));
      if (accessed_info.accessed)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"V"));
      }
      else
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"X"));
      }

      if (inc_l3&inc_l2)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|\n\r")));
        continue;
      }
      
      if (entries_array[indx].value.pwe.service_type != SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|")));
        SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
          "",
          &(entries_array[indx].value.pwe.default_frwrd.default_forwarding)
          );
      }
      else
      {
        if (entries_array[indx].value.pwe.learn_record.enable_learning == FALSE)
        {
          LOG_CLI((BSL_META_U(unit,
                              "%-7s "), "|disable"));
          LOG_CLI((BSL_META_U(unit,
                              "%-5s|"),"   -" ));
          LOG_CLI((BSL_META_U(unit,
                              "%-6s|"),"   -" ));
          LOG_CLI((BSL_META_U(unit,
                              "%10s%-10s ")," ","-"));
        }
        else
        {
          SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
            "",
            &(entries_array[indx].value.pwe.learn_record.learn_info)
            );
        }
      }
      
      LOG_CLI((BSL_META_U(unit,
                          "|%-5u"), entries_array[indx].value.pwe.vsid));
      LOG_CLI((BSL_META_U(unit,
                          "|%-4s"), ""));
      LOG_CLI((BSL_META_U(unit,
                          "|%-1s"), ""));
      LOG_CLI((BSL_META_U(unit,
                          "|%-1s"), ""));
      LOG_CLI((BSL_META_U(unit,
                          "|%-1s"), ""));
      if (entries_array[indx].value.pwe.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"H"));
      }
      else if (entries_array[indx].value.pwe.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"S"));
      }
    }
    else if (entries_array[indx].type & (SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF|SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF))
    {
      
      LOG_CLI((BSL_META_U(unit,
                          "|%s"),"      "));
      if (accessed_info.accessed)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s")," V "));
      }
      else
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s")," X "));
      }
      if (inc_l3&inc_l2)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|\n\r")));
        continue;
      }
      LOG_CLI((BSL_META_U(unit,
                          "|%-5u"), entries_array[indx].value.rif.vrf_id));
      LOG_CLI((BSL_META_U(unit,
                          "|%-3u"), entries_array[indx].value.rif.cos_profile));
      LOG_CLI((BSL_META_U(unit,
                          "|%-3u"), entries_array[indx].value.rif.ttl_scope_index));
      LOG_CLI((BSL_META_U(unit,
                          "|%-3u"), entries_array[indx].value.rif.uc_rpf_enable));
      LOG_CLI((BSL_META_U(unit,
                          "|%-7u"), entries_array[indx].value.rif.routing_enablers_bm));
    }
                
    LOG_CLI((BSL_META_U(unit,
                        "|\n\r")));
  }

  if (inc_l3 && inc_l2)
  {
    LOG_CLI((BSL_META_U(unit,
                        " ------------------------\n\r"
                 )));
  }
  else if (inc_l2)
  {
    LOG_CLI((BSL_META_U(unit,
                        " -------------------------------------------------------------------------------------\n\r"
                 )));
  }
  else if (inc_l3)
  {
    LOG_CLI((BSL_META_U(unit,
                        " --------------------------------------------------\n\r"
                 )));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_print_block()", 0, 0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>



