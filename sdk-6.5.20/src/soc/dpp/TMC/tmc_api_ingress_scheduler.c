/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_ingress_scheduler.h>


























void
  SOC_TMC_ING_SCH_SHAPER_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_SHAPER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ING_SCH_SHAPER));
  info->max_rate = 0;
  info->max_burst = 0;

  info->slow_start_enable = 0;
  info->slow_start_rate_phase_0 = 0;
  info->slow_start_rate_phase_1 = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_MESH_CONTEXT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_MESH_CONTEXT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ING_SCH_MESH_CONTEXT_INFO));
  SOC_TMC_ING_SCH_SHAPER_clear(&(info->shaper));
  info->weight = 0;
  info->id = SOC_TMC_ING_NOF_SCH_MESH_CONTEXTSS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_MESH_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_MESH_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ING_SCH_MESH_INFO));
  for (ind = 0; ind < SOC_TMC_ING_SCH_MESH_LAST; ++ind)
  {
    SOC_TMC_ING_SCH_MESH_CONTEXT_INFO_clear(&(info->contexts[ind]));
  }
  info->nof_entries = 0;
  SOC_TMC_ING_SCH_SHAPER_clear(&(info->total_rate_shaper));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
  
void
  SOC_TMC_ING_SCH_MESH_INFO_SHAPERS_dont_touch(
    SOC_SAND_OUT SOC_TMC_ING_SCH_MESH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  info->contexts[SOC_TMC_ING_SCH_MESH_CON1].shaper.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->contexts[SOC_TMC_ING_SCH_MESH_CON2].shaper.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->contexts[SOC_TMC_ING_SCH_MESH_CON3].shaper.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->contexts[SOC_TMC_ING_SCH_MESH_CON4].shaper.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->contexts[SOC_TMC_ING_SCH_MESH_CON5].shaper.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->contexts[SOC_TMC_ING_SCH_MESH_CON6].shaper.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->contexts[SOC_TMC_ING_SCH_MESH_CON7].shaper.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->total_rate_shaper.max_rate=SOC_TMC_ING_SCH_DONT_TOUCH;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}  
  

void
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT));
  info->weight1 = 0;
  info->weight2 = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_CLOS_WFQS_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_WFQS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ING_SCH_CLOS_WFQS));
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_clear(&(info->fabric_hp));
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_clear(&(info->fabric_lp));
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_clear(&(info->global_hp));
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_clear(&(info->global_lp));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_HP_SHAPERS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ING_SCH_CLOS_HP_SHAPERS));
  SOC_TMC_ING_SCH_SHAPER_clear(&(info->local));
  SOC_TMC_ING_SCH_SHAPER_clear(&(info->fabric_unicast));
  SOC_TMC_ING_SCH_SHAPER_clear(&(info->fabric_multicast));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_CLOS_SHAPERS_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_SHAPERS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ING_SCH_CLOS_SHAPERS));
  SOC_TMC_ING_SCH_SHAPER_clear(&(info->local));
  SOC_TMC_ING_SCH_SHAPER_clear(&(info->fabric));
  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS_clear(&(info->hp));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_CLOS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ING_SCH_CLOS_INFO));
  SOC_TMC_ING_SCH_CLOS_SHAPERS_clear(&(info->shapers));
  SOC_TMC_ING_SCH_CLOS_WFQS_clear(&(info->weights));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_TMC_ING_SCH_CLOS_INFO_SHAPERS_dont_touch(
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->shapers.local.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->shapers.hp.local.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->shapers.fabric.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->shapers.hp.fabric_unicast.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->shapers.hp.fabric_multicast.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->shapers.hp.fabric_multicast.slow_start_enable = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->shapers.hp.fabric_multicast.slow_start_rate_phase_0 = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->shapers.hp.fabric_multicast.slow_start_rate_phase_1 = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->shapers.lp.fabric_unicast.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->shapers.lp.fabric_multicast.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->shapers.lp.fabric_multicast.slow_start_enable = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->shapers.lp.fabric_multicast.slow_start_rate_phase_0 = SOC_TMC_ING_SCH_DONT_TOUCH;
  info->shapers.lp.fabric_multicast.slow_start_rate_phase_1 = SOC_TMC_ING_SCH_DONT_TOUCH;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
	

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_ING_SCH_MESH_CONTEXTS_to_string(
    SOC_SAND_IN  SOC_TMC_ING_SCH_MESH_CONTEXTS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_ING_SCH_MESH_LOCAL:
    str = "LOCAL";
  break;

  case SOC_TMC_ING_SCH_MESH_CON1:
    str = "CONTEXT1";
  break;

  case SOC_TMC_ING_SCH_MESH_CON2:
    str = "CONTEXT2";
  break;

  case SOC_TMC_ING_SCH_MESH_CON3:
    str = "CONTEXT3";
  break;

  case SOC_TMC_ING_SCH_MESH_CON4:
    str = "CONTEXT4";
  break;

  case SOC_TMC_ING_SCH_MESH_CON5:
    str = "CONTEXT5";
  break;

  case SOC_TMC_ING_SCH_MESH_CON6:
    str = "CONTEXT6";
  break;

  case SOC_TMC_ING_SCH_MESH_CON7:
    str = "CONTEXT7";
  break;

  case SOC_TMC_ING_SCH_MESH_LAST:
    str = " Not initialized";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_ING_SCH_SHAPER_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_SHAPER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Max_rate: %u[Kbps], "),info->max_rate));
  LOG_CLI((BSL_META_U(unit,
                      "Max_burst: %u[Bytes].\n\r"),info->max_burst));

  if (info->slow_start_enable)
  {
      LOG_CLI((BSL_META_U(unit,
                          "Slow rate first phase precent: %u%% "),info->slow_start_rate_phase_0));
      LOG_CLI((BSL_META_U(unit,
                          "Slow rate second phase precent: %u%% "),info->slow_start_rate_phase_1));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_MESH_CONTEXT_INFO_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_MESH_CONTEXT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  Shaper:")));
  SOC_TMC_ING_SCH_SHAPER_print(&(info->shaper));
  LOG_CLI((BSL_META_U(unit,
                      "  Weight: %u\n\r"),info->weight));
  LOG_CLI((BSL_META_U(unit,
                      " Id %s \n\r"),
           SOC_TMC_ING_SCH_MESH_CONTEXTS_to_string(info->id)
           ));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_MESH_INFO_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_MESH_INFO *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_TMC_ING_SCH_MESH_LAST; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Contexts[%u]:\n\r"),ind));
    SOC_TMC_ING_SCH_MESH_CONTEXT_INFO_print(&(info->contexts[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "Nof_entries: %u[Entries]\n\r"),info->nof_entries));
  LOG_CLI((BSL_META_U(unit,
                      "Total_rate_shaper: ")));
  SOC_TMC_ING_SCH_SHAPER_print(&(info->total_rate_shaper));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "    Weight1: %u, "),info->weight1));
  LOG_CLI((BSL_META_U(unit,
                      "Weight2: %u\n\r"),info->weight2));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_CLOS_WFQS_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_WFQS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  Fabric_hp:\n\r")));
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_print(&(info->fabric_hp));
  LOG_CLI((BSL_META_U(unit,
                      "  Fabric_lp:\n\r")));
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_print(&(info->fabric_lp));
  LOG_CLI((BSL_META_U(unit,
                      "  Global_hp:\n\r")));
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_print(&(info->global_hp));
  LOG_CLI((BSL_META_U(unit,
                      "  Global_lp:\n\r")));
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_print(&(info->global_lp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS_print(
    SOC_SAND_IN  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " Local:")));
  SOC_TMC_ING_SCH_SHAPER_print(&(info->local));
  LOG_CLI((BSL_META_U(unit,
                      " Fabric_unicast:")));
  SOC_TMC_ING_SCH_SHAPER_print(&(info->fabric_unicast));
  LOG_CLI((BSL_META_U(unit,
                      " Fabric_multicast:")));
  SOC_TMC_ING_SCH_SHAPER_print(&(info->fabric_multicast));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_CLOS_SHAPERS_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_SHAPERS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  Local: ")));
  SOC_TMC_ING_SCH_SHAPER_print(&(info->local));
  LOG_CLI((BSL_META_U(unit,
                      "  Fabric: ")));
  SOC_TMC_ING_SCH_SHAPER_print(&(info->fabric));
  LOG_CLI((BSL_META_U(unit,
                      "hp:")));
  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS_print(&(info->hp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ING_SCH_CLOS_INFO_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Shapers:\n\r")));
  SOC_TMC_ING_SCH_CLOS_SHAPERS_print(&(info->shapers));
  LOG_CLI((BSL_META_U(unit,
                      "Weights:\n\r")));
  SOC_TMC_ING_SCH_CLOS_WFQS_print(&(info->weights));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

