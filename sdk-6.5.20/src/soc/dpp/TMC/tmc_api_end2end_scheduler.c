/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/





#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/TMC/tmc_api_end2end_scheduler.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>


























void
  SOC_TMC_SCH_DEVICE_RATE_ENTRY_clear(
    SOC_SAND_OUT SOC_TMC_SCH_DEVICE_RATE_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_DEVICE_RATE_ENTRY));
  info->rci_level = 0;
  info->num_active_links = 0;
  info->rate = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_DEVICE_RATE_TABLE_clear(
    SOC_SAND_OUT SOC_TMC_SCH_DEVICE_RATE_TABLE *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_DEVICE_RATE_TABLE));
  for (ind=0; ind<SOC_TMC_SCH_DRT_SIZE; ++ind)
  {
    SOC_TMC_SCH_DEVICE_RATE_ENTRY_clear(&(info->rates[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_IF_WEIGHT_ENTRY_clear(
    SOC_SAND_OUT SOC_TMC_SCH_IF_WEIGHT_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_IF_WEIGHT_ENTRY));
  info->id = 0;
  info->val = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_IF_WEIGHTS_clear(
    SOC_SAND_OUT SOC_TMC_SCH_IF_WEIGHTS *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_IF_WEIGHTS));
  for (ind=0; ind<SOC_TMC_SCH_NOF_IF_WEIGHTS; ++ind)
  {
    SOC_TMC_SCH_IF_WEIGHT_ENTRY_clear(&(info->weight[ind]));
  }
  info->nof_enties = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_PORT_HP_CLASS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_PORT_HP_CLASS_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_PORT_HP_CLASS_INFO));
  for (ind=0; ind<SOC_TMC_SCH_LOW_FC_NOF_AVAIL_CONFS; ++ind)
  {
    info->lowest_hp_class[ind] = SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_LAST;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_PORT_INFO *info
  )
{
  uint32
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_PORT_INFO));
  info->enable = 0;
  info->max_expected_rate = SOC_TMC_SCH_PORT_MAX_EXPECTED_RATE_AUTO;
  info->hr_mode = SOC_TMC_SCH_SE_HR_MODE_LAST;
  info->lowest_hp_class = SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_LAST;
  info->group = SOC_TMC_SCH_GROUP_AUTO;

  for (index = 0; index < SOC_TMC_NOF_TRAFFIC_CLASSES; index++)
  {
    info->hr_modes[index] = SOC_TMC_SCH_SE_HR_MODE_LAST;
  }

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_TCG_WEIGHT_clear(
    SOC_SAND_OUT SOC_TMC_SCH_TCG_WEIGHT *info
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_TCG_WEIGHT));
  info->tcg_weight_valid = FALSE;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_HR_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_HR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SE_HR));
  info->mode = SOC_TMC_SCH_SE_HR_MODE_LAST;
  info->tcg_ndx = SOC_TMC_NOF_TCGS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_CL_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_CL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SE_CL));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_FQ_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_FQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SE_FQ));
  info->no_info = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_CL_CLASS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_CL_CLASS_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SE_CL_CLASS_INFO));
  info->mode = SOC_TMC_SCH_CL_CLASS_MODE_LAST;
  for (ind=0; ind<SOC_TMC_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS; ++ind)
  {
    info->weight[ind] = 0;
  }
  info->weight_mode = SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE_LAST;
  info->enhanced_mode = SOC_TMC_SCH_CL_ENHANCED_MODE_LAST;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_CL_CLASS_TABLE_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_CL_CLASS_TABLE *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SE_CL_CLASS_TABLE));
  for (ind=0; ind<SOC_TMC_SCH_NOF_CLASS_TYPES; ++ind)
  {
    SOC_TMC_SCH_SE_CL_CLASS_INFO_clear(&(info->class_types[ind]));
  }
  info->nof_class_types = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_PER_TYPE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_PER_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SE_PER_TYPE_INFO));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SE_INFO));
  info->state = SOC_TMC_SCH_SE_STATE_LAST;
  info->type = SOC_TMC_SCH_SE_TYPE_LAST;
  SOC_TMC_SCH_SE_PER_TYPE_INFO_clear(&(info->type_info));
  info->is_dual = 0;
  info->group = SOC_TMC_SCH_GROUP_AUTO;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SUB_FLOW_SHAPER_clear(
    int unit,
    SOC_SAND_OUT SOC_TMC_SCH_SUB_FLOW_SHAPER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SUB_FLOW_SHAPER));
  info->max_rate = SOC_TMC_SCH_SUB_FLOW_SHAPE_NO_LIMIT;
  info->max_burst = SOC_TMC_SCH_SUB_FLOW_SHAPER_BURST_NO_LIMIT;
#ifdef BCM_88675_A0
  if (SOC_IS_JERICHO(unit))
  {
      info->max_burst = SOC_DPP_CONFIG(unit)->arad->init.max_burst_default_value_bucket_width;
  }
#endif
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SUB_FLOW_HR_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SUB_FLOW_HR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SUB_FLOW_HR));
  info->sp_class = SOC_TMC_SCH_SUB_FLOW_HR_CLASS_LAST;
  info->weight = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SUB_FLOW_CL_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SUB_FLOW_CL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SUB_FLOW_CL));
  info->sp_class = SOC_TMC_SCH_SUB_FLOW_CL_CLASS_LAST;
  info->weight = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SUB_FLOW_SE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SUB_FLOW_SE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SUB_FLOW_SE_INFO));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE));
  SOC_TMC_SCH_SUB_FLOW_SE_INFO_clear(&(info->se_info));
  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SUBFLOW_clear(
    int unit,
    SOC_SAND_OUT SOC_TMC_SCH_SUBFLOW *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SUBFLOW));
  info->is_valid = 0;
  SOC_TMC_SCH_SUB_FLOW_SHAPER_clear(unit, &(info->shaper));
  info->slow_rate_ndx = SOC_TMC_SCH_SLOW_RATE_NDX_1;
  SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE_clear(&(info->credit_source));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_FLOW_clear(
    int unit,
    SOC_SAND_OUT SOC_TMC_SCH_FLOW *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_FLOW));
  for (ind=0; ind<SOC_TMC_SCH_NOF_SUB_FLOWS; ++ind)
  {
    SOC_TMC_SCH_SUBFLOW_clear(unit, &(info->sub_flow[ind]));
  }
  info->flow_type = SOC_TMC_SCH_FLOW_TYPE_LAST;
  info->is_slow_enabled = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_GLOBAL_PER1K_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_GLOBAL_PER1K_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_GLOBAL_PER1K_INFO));
  info->is_interdigitated = FALSE;
  info->is_odd_even = TRUE;
  info->is_cl_cir = FALSE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_QUARTET_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_QUARTET_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_QUARTET_MAPPING_INFO));
  info->is_composite = 0;
  info->fip_id = SOC_TMC_DEFAULT_DESTINATION_FAP;
  info->base_q_qrtt_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SLOW_RATE_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SLOW_RATE *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SCH_SLOW_RATE));
  for (ind=0; ind<SOC_TMC_SCH_NOF_SLOW_RATES; ++ind)
  {
    info->rates[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_FLOW_AND_UP_PORT_INFO_clear(
     SOC_SAND_OUT SOC_TMC_SCH_FLOW_AND_UP_PORT_INFO *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    SOC_TMC_SCH_PORT_INFO_clear(&(info->port_info));
    SOC_TMC_OFP_RATE_INFO_clear(&(info->ofp_rate_info));

    info->credit_rate = 0;
    info->fc_cnt = 0;
    info->fc_percent = 0;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_FLOW_AND_UP_SE_INFO_clear(
     int unit,
     SOC_SAND_OUT SOC_TMC_SCH_FLOW_AND_UP_SE_INFO *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    SOC_TMC_SCH_SE_INFO_clear(&(info->se_info));
    SOC_TMC_SCH_FLOW_clear(unit, &(info->sch_consumer));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_TMC_SCH_FLOW_AND_UP_INFO_clear(
     int unit,
     SOC_SAND_OUT SOC_TMC_SCH_FLOW_AND_UP_INFO *info,
     SOC_SAND_IN uint32                         is_full 
     )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (is_full)
  {
      info->base_queue = 0;
      SOC_TMC_IPQ_QUARTET_MAP_INFO_clear(&(info->qrtt_map_info));
      SOC_TMC_SCH_FLOW_clear(unit, &(info->sch_consumer));
      info->credit_rate = 0;
      SOC_TMC_OFP_RATES_TBL_INFO_clear(&(info->ofp_rates_table));
      info->ofp_rate_valid = 0;
  }

  info->credit_sources_nof = 0;

  

  info->next_level_credit_sources_nof = 0;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_to_string(
    SOC_SAND_IN SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_NONE:
    str = "NONE";
  break;

  case SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_EF1:
    str = "EF1";
  break;

  case SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_EF2:
    str = "EF2";
  break;

  case SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_EF3:
    str = "EF3";
  break;

  case SOC_TMC_SCH_PORT_LOWEST_HP_HR_SINGLE_CLASS_AF1_WFQ:
    str = "SINGLE_AF1_WFQ";
  break;

  case SOC_TMC_SCH_PORT_LOWEST_HP_HR_DUAL_OR_ENHANCED:
    str = "DUAL_OR_ENHANCED";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_SCH_CL_CLASS_MODE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_CL_CLASS_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SCH_CL_MODE_NONE:
    str = "MODE_NONE";
  break;

  case SOC_TMC_SCH_CL_MODE_1:
    str = "MODE_1";
  break;

  case SOC_TMC_SCH_CL_MODE_2:
    str = "MODE_2";
  break;

  case SOC_TMC_SCH_CL_MODE_3:
    str = "MODE_3";
  break;

  case SOC_TMC_SCH_CL_MODE_4:
    str = "MODE_4";
  break;

  case SOC_TMC_SCH_CL_MODE_5:
    str = "MODE_5";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SCH_CL_WEIGHTS_MODE_INDEPENDENT_PER_FLOW:
    str = "INDEPENDENT_PER_FLOW";
  break;

  case SOC_TMC_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_FLOW:
    str = "DISCRETE_PER_FLOW";
  break;

  case SOC_TMC_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_CLASS:
    str = "DISCRETE_PER_CLASS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_SCH_CL_ENHANCED_MODE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_CL_ENHANCED_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_CL_ENHANCED_MODE_DISABLED:
    str = "DISABLED";
  break;

  case SOC_TMC_CL_ENHANCED_MODE_ENABLED_HP:
    str = "ENABLED_HP";
  break;

  case SOC_TMC_CL_ENHANCED_MODE_ENABLED_LP:
    str = "ENABLED_LP";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_SCH_SE_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_SE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SCH_SE_TYPE_NONE:
    str = "NONE";
  break;

  case SOC_TMC_SCH_SE_TYPE_HR:
    str = "HR";
  break;

  case SOC_TMC_SCH_SE_TYPE_CL:
    str = "CL";
  break;

  case SOC_TMC_SCH_SE_TYPE_FQ:
    str = "FQ";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_SCH_SE_STATE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_SE_STATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SCH_SE_STATE_DISABLE:
    str = "disabled";
  break;

  case SOC_TMC_SCH_SE_STATE_ENABLE:
    str = "enabled";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_SCH_GROUP_to_string(
    SOC_SAND_IN SOC_TMC_SCH_GROUP enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SCH_GROUP_A:
    str = "A";
  break;

  case SOC_TMC_SCH_GROUP_B:
    str = "B";
  break;

  case SOC_TMC_SCH_GROUP_C:
    str = "C";
  break;

  case SOC_TMC_SCH_GROUP_AUTO:
    str = "AUTO";
  break;

  case SOC_TMC_SCH_GROUP_NONE:
    str = "GROUP_NONE";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_SCH_SE_HR_MODE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_SE_HR_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SCH_HR_MODE_NONE:
    str = "NONE";
  break;

  case SOC_TMC_SCH_HR_MODE_SINGLE_WFQ:
    str = "SINGLE_WFQ";
  break;

  case SOC_TMC_SCH_HR_MODE_DUAL_WFQ:
    str = "DUAL_WFQ";
  break;

  case SOC_TMC_SCH_HR_MODE_ENHANCED_PRIO_WFQ:
    str = "ENHANCED_PRIO_WFQ";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_SCH_SUB_FLOW_HR_CLASS_to_string(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_HR_CLASS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SCH_FLOW_HR_CLASS_NONE:
    str = "NONE (Undef)  ";
  break;

  case SOC_TMC_SCH_FLOW_HR_CLASS_EF1:
    str = "EF1           ";
  break;

  case SOC_TMC_SCH_FLOW_HR_CLASS_EF2:
    str = "EF2           ";
  break;

  case SOC_TMC_SCH_FLOW_HR_CLASS_EF3:
    str = "EF3           ";
  break;

  case SOC_TMC_SCH_FLOW_HR_SINGLE_CLASS_AF1_WFQ:
    str = "SINGLE_AF1_WFQ";
  break;

  case SOC_TMC_SCH_FLOW_HR_SINGLE_CLASS_BE1:
    str = "SINGLE_BE1    ";
  break;

  case SOC_TMC_SCH_FLOW_HR_DUAL_CLASS_AF1_WFQ:
    str = "DUAL_AF1_WFQ  ";
  break;

  case SOC_TMC_SCH_FLOW_HR_DUAL_CLASS_BE1_WFQ:
    str = "DUAL_BE1_WFQ  ";
  break;

  case SOC_TMC_SCH_FLOW_HR_DUAL_CLASS_BE2:
    str = "DUAL_BE2      ";
  break;

  case SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF1:
    str = "ENHANCED_AF1  ";
  break;

  case SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF2:
    str = "ENHANCED_AF2  ";
  break;

  case SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF3:
    str = "ENHANCED_AF3  ";
  break;

  case SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF4:
    str = "ENHANCED_AF4  ";
  break;

  case SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF5:
    str = "ENHANCED_AF5  ";
  break;

  case SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF6:
    str = "ENHANCED_AF6  ";
  break;

  case SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_BE1_WFQ:
    str = "ENHNCD_BE1_WFQ";
  break;

  case SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_BE2:
    str = "ENHANCED_BE2  ";
  break;

  default:
    str = " Undefined    ";
  }
  return str;
}

const char*
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_to_string(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_CL_CLASS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_NONE:
    str = "NONE (Undef) ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1:
    str = "SP1          ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2:
    str = "SP2          ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3:
    str = "SP3          ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP4:
    str = "SP4          ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ:
    str = "SP1_WFQ      ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ1:
    str = "SP1_WFQ1     ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ2:
    str = "SP1_WFQ2     ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ3:
    str = "SP1_WFQ3     ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ4:
    str = "SP1_WFQ4     ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ:
    str = "SP2_WFQ      ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ1:
    str = "SP2_WFQ1     ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ2:
    str = "SP2_WFQ2     ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ3:
    str = "SP2_WFQ3     ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ1:
    str = "SP3_WFQ1     ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ2:
    str = "SP3_WFQ2     ";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP_0_ENHANCED:
    str = "SP_0_ENHANCED";
  break;

  case SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP_5_ENHANCED:
    str = "SP_5_ENHANCED";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_SCH_SLOW_RATE_NDX_to_string(
    SOC_SAND_IN SOC_TMC_SCH_SLOW_RATE_NDX enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SCH_SLOW_RATE_NDX_1:
    str = "1";
  break;

  case SOC_TMC_SCH_SLOW_RATE_NDX_2:
    str = "2";
  break;

  case SOC_TMC_SCH_NOF_SLOW_RATE_NDXS:
    str = " Not initialized";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_SCH_FLOW_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_FLOW_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_FLOW_NONE:
    str = "Type-undefined";
  break;

  case SOC_TMC_FLOW_SIMPLE:
    str = "simple";
  break;

  case SOC_TMC_FLOW_AGGREGATE:
    str = "aggregate";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_SCH_FLOW_STATUS_to_string(
    SOC_SAND_IN SOC_TMC_SCH_FLOW_STATUS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SCH_FLOW_OFF:
    str = "FLOW_OFF";
  break;

  case SOC_TMC_SCH_FLOW_ON:
    str = "FLOW_ON";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE_to_string(
  SOC_SAND_IN  SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE_INVERSE:
    str = "inverse";
    break;
  case SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE_PROPORTIONAL:
    str = "proportional";
    break;
  default:
    str = " Unknown";
  }
  return str;
}


void
  SOC_TMC_SCH_DEVICE_RATE_ENTRY_print(
    SOC_SAND_IN SOC_TMC_SCH_DEVICE_RATE_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Rci_level: %u\n\r"),info->rci_level));
  LOG_CLI((BSL_META_U(unit,
                      "Num_active_links: %u[links]\n\r"),info->num_active_links));
  LOG_CLI((BSL_META_U(unit,
                      "Rate: %u[Mbps]\n\r"),info->rate));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  SOC_TMC_SCH_DEVICE_RATE_TABLE_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_TMC_SCH_DEVICE_RATE_TABLE *info
  )
{
  uint32
    ind_j = 0,
    ind_i = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind_i = 0; ind_i < SOC_DPP_DEFS_GET(unit, nof_rci_levels) + 1; ++ind_i)
  {
    LOG_CLI((BSL_META_U(unit,
                        "|--------")));
  }
  LOG_CLI((BSL_META_U(unit,
                      "|\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "|LNK\\RCI ")));
  for (ind_i = 0; ind_i < SOC_DPP_DEFS_GET(unit, nof_rci_levels); ++ind_i)
  {
    LOG_CLI((BSL_META_U(unit,
                        "| %6d "), ind_i));
  }
  LOG_CLI((BSL_META_U(unit,
                      "|\n\r")));

  for (ind_i = 0; ind_i < SOC_DPP_DEFS_GET(unit, nof_rci_levels) + 1; ++ind_i)
  {
    LOG_CLI((BSL_META_U(unit,
                        "|--------")));
  }
  LOG_CLI((BSL_META_U(unit,
                      "|\n\r")));

  for (ind_j = 0; ind_j < SOC_DPP_DEFS_GET(unit, nof_fabric_links) + 1; ++ind_j)
  {
    LOG_CLI((BSL_META_U(unit,
                        "| %6d "), ind_j));
    for (ind_i = 0; ind_i < SOC_DPP_DEFS_GET(unit, nof_rci_levels) ; ++ind_i)
    {
      LOG_CLI((BSL_META_U(unit,
                          "| %6u "), info->rates[ind_j * SOC_DPP_DEFS_GET(unit, nof_rci_levels) + ind_i].rate));
    }
    LOG_CLI((BSL_META_U(unit,
                        "|\n\r")));
  }

  for (ind_i = 0; ind_i < SOC_DPP_DEFS_GET(unit, nof_rci_levels) + 1; ++ind_i)
  {
    LOG_CLI((BSL_META_U(unit,
                        "|--------")));
  }
  LOG_CLI((BSL_META_U(unit,
                      "|\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_IF_WEIGHT_ENTRY_print(
    SOC_SAND_IN SOC_TMC_SCH_IF_WEIGHT_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Id: %u, "),info->id));
  LOG_CLI((BSL_META_U(unit,
                      "Val: %u\n\r"),info->val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_IF_WEIGHTS_print(
    SOC_SAND_IN SOC_TMC_SCH_IF_WEIGHTS *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_TMC_SCH_NOF_IF_WEIGHTS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Weight[%u]: "),ind));
    SOC_TMC_SCH_IF_WEIGHT_ENTRY_print(&(info->weight[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "nof_enties: %u\n\r"),info->nof_enties));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_PORT_HP_CLASS_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_PORT_HP_CLASS_INFO *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_TMC_SCH_LOW_FC_NOF_AVAIL_CONFS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Lowest_hp_class[%u] %s \n\r"), ind,
             SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_to_string(info->lowest_hp_class[ind])
             ));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_TMC_SCH_PORT_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_PORT_INFO *info,
    SOC_SAND_IN uint32           port_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Port[%-2u] - %s, "),
           port_id,
           info->enable?"enabled":"disabled"
           ));

  LOG_CLI((BSL_META_U(unit,
                      "HR mode: %s, "),
           SOC_TMC_SCH_SE_HR_MODE_to_string(info->hr_mode)
           ));

  LOG_CLI((BSL_META_U(unit,
                      "Flow Control: HP starts with %s"),
           SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_to_string(info->lowest_hp_class)
           ));

  if (info->max_expected_rate != SOC_TMC_SCH_PORT_MAX_EXPECTED_RATE_AUTO)
  {
    LOG_CLI((BSL_META_U(unit,
                        ", Max_expected_rate: %u[Mbps]\n\r"),info->max_expected_rate));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_TCG_WEIGHT_print(
    SOC_SAND_IN SOC_TMC_SCH_TCG_WEIGHT *tcg_weight
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(tcg_weight);

  LOG_CLI((BSL_META_U(unit,
                      "tcg_weight: %s, "),
           tcg_weight->tcg_weight_valid?"enabled":"disabled"
           ));

  if (tcg_weight->tcg_weight_valid)
  {
    LOG_CLI((BSL_META_U(unit,
                        "tcg_weight: %d, "),
             tcg_weight->tcg_weight
             ));
  }

  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_CL_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_CL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Class-id: %d"), info->id));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_FQ_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_FQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "None")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_HR_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_HR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "HR-mode: %s"), SOC_TMC_SCH_SE_HR_MODE_to_string(info->mode)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_PER_TYPE_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_PER_TYPE_INFO *info,
    SOC_SAND_IN SOC_TMC_SCH_SE_TYPE type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(type)
  {
  case SOC_TMC_SCH_SE_TYPE_CL:
    LOG_CLI((BSL_META_U(unit,
                        "CL, ")));
    SOC_TMC_SCH_SE_CL_print(&(info->cl));
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));
    break;
  case SOC_TMC_SCH_SE_TYPE_FQ:
    LOG_CLI((BSL_META_U(unit,
                        "FQ, ")));
    SOC_TMC_SCH_SE_FQ_print(&(info->fq));
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));
    break;
  case SOC_TMC_SCH_SE_TYPE_HR:
    LOG_CLI((BSL_META_U(unit,
                        "HR, ")));
    SOC_TMC_SCH_SE_HR_print(&(info->hr));
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));
    break;
  default:
    break;
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_CL_CLASS_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_CL_CLASS_INFO *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  Id: %u, "),info->id));
  LOG_CLI((BSL_META_U(unit,
                      "Mode %s \n\r"),
           SOC_TMC_SCH_CL_CLASS_MODE_to_string(info->mode)
           ));
  for (ind=0; ind<SOC_TMC_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "    Weight[%u]: %u\n\r"),ind,info->weight[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      " Weight_mode %s \n\r"),
           SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE_to_string(info->weight_mode)
           ));
  LOG_CLI((BSL_META_U(unit,
                      " Enhanced_mode %s \n\r"),
           SOC_TMC_SCH_CL_ENHANCED_MODE_to_string(info->enhanced_mode)
           ));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_CL_CLASS_TABLE_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_CL_CLASS_TABLE *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<(info->nof_class_types); ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Class_types[%u]:\n\r"),ind));
    SOC_TMC_SCH_SE_CL_CLASS_INFO_print(&(info->class_types[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "Nof_class_types: %u[class types]\n\r"),info->nof_class_types));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SE_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "%s[%u] - %s, "),
           SOC_TMC_SCH_SE_TYPE_to_string(info->type),
           info->id,
           SOC_TMC_SCH_SE_STATE_to_string(info->state)
           ));

  if (info->state == SOC_TMC_SCH_SE_STATE_ENABLE)
  {
    switch(info->type)
    {
    case SOC_TMC_SCH_SE_TYPE_CL:
      SOC_TMC_SCH_SE_CL_print(&(info->type_info.cl));
      LOG_CLI((BSL_META_U(unit,
                          ", ")));
      break;
    case SOC_TMC_SCH_SE_TYPE_FQ:
      break;
    case SOC_TMC_SCH_SE_TYPE_HR:
      SOC_TMC_SCH_SE_HR_print(&(info->type_info.hr));
      LOG_CLI((BSL_META_U(unit,
                          ", ")));
      break;
    default:
      break;
    }

    LOG_CLI((BSL_META_U(unit,
                        "%s"),(info->is_dual)?"Dual Bucket":"Not Dual Bucket"));

    if (info->group != SOC_TMC_SCH_GROUP_AUTO)
    {
      LOG_CLI((BSL_META_U(unit,
                          ", group: %s \n\r"),
               SOC_TMC_SCH_GROUP_to_string(info->group)
               ));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "\n\r")));
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SUB_FLOW_SHAPER_print(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_SHAPER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->max_rate == SOC_TMC_SCH_SUB_FLOW_SHAPE_NO_LIMIT)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Max_rate: not limited, ")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "Max_rate:  %u[Kbps], "),info->max_rate));
  }

  if (info->max_burst >= SOC_TMC_SCH_SUB_FLOW_SHAPER_BURST_NO_LIMIT)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Max_burst: not limited. ")));
  }
  else if (info->max_burst == 0)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Max_burst: 0 (sub-flow disabled by shaping). ")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "Max_burst: %u[Bytes]. "),info->max_burst));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SUB_FLOW_HR_print(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_HR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "SP class %s \n\r"),
           SOC_TMC_SCH_SUB_FLOW_HR_CLASS_to_string(info->sp_class)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "Weight: %u\n\r"),info->weight));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SUB_FLOW_CL_print(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_CL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Class %s \n\r"),
           SOC_TMC_SCH_SUB_FLOW_CL_CLASS_to_string(info->sp_class)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "Weight: %u\n\r"),info->weight));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SUB_FLOW_FQ_print(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_FQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "No info\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SUB_FLOW_SE_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_SE_INFO *info,
    SOC_SAND_IN SOC_TMC_SCH_SE_TYPE se_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(se_type)
  {
  case SOC_TMC_SCH_SE_TYPE_HR:
    LOG_CLI((BSL_META_U(unit,
                        "HR, ")));
    SOC_TMC_SCH_SUB_FLOW_HR_print(&(info->hr));
    break;
  case SOC_TMC_SCH_SE_TYPE_CL:
    LOG_CLI((BSL_META_U(unit,
                        "CL, ")));
    SOC_TMC_SCH_SUB_FLOW_CL_print(&(info->cl));
    break;
  case SOC_TMC_SCH_SE_TYPE_FQ:
    LOG_CLI((BSL_META_U(unit,
                        "FQ ")));
      break;
  default:
    LOG_CLI((BSL_META_U(unit,
                        "Undefined SE Type!\n\r")));
      break;
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE_print(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "    ID: %u, SE-TYPE: "),
           info->id
           ));
  SOC_TMC_SCH_SUB_FLOW_SE_INFO_print(&(info->se_info), info->se_type);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_TMC_SCH_SUBFLOW_print(
    SOC_SAND_IN SOC_TMC_SCH_SUBFLOW *info,
    SOC_SAND_IN uint8 is_table_flow,
    SOC_SAND_IN uint32 subflow_id
  )
{
  char
    rate_tbl_style[15],
    burst_tbl_style[15],
    crdt_src_tbl_style[45],
    crdt_src_weight_tbl_style[15];

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (!is_table_flow)
  {
    if (info->is_valid)
    {
      LOG_CLI((BSL_META_U(unit,
                          "__________________________\n\r")));
      LOG_CLI((BSL_META_U(unit,
                          "Shaper: ")));
      SOC_TMC_SCH_SUB_FLOW_SHAPER_print(&(info->shaper));
      LOG_CLI((BSL_META_U(unit,
                          "Slow rate id %s, "),
               SOC_TMC_SCH_SLOW_RATE_NDX_to_string(info->slow_rate_ndx)
               ));
      LOG_CLI((BSL_META_U(unit,
                          "Credit_source:\n\r")));
      SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE_print(&(info->credit_source));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "Disabled:\n\r")));
    }
  }
  else
  {
    
    if (info->shaper.max_rate == SOC_TMC_SCH_SUB_FLOW_SHAPE_NO_LIMIT)
    {
      sal_sprintf(rate_tbl_style, " No Limit  ");
    }
    else if (info->shaper.max_rate == 0)
    {
      sal_sprintf(rate_tbl_style, "    *0*    ");
    }
    else
    {
      sal_sprintf(rate_tbl_style, " %-8u  ",info->shaper.max_rate);
    }

    if (info->shaper.max_burst >= SOC_TMC_SCH_SUB_FLOW_SHAPER_BURST_NO_LIMIT)
    {
      sal_sprintf(burst_tbl_style, " No Limit  ");
    }
    else if (info->shaper.max_burst == 0)
    {
      sal_sprintf(burst_tbl_style, "   *0*     ");
    }
    else
    {
      sal_sprintf(burst_tbl_style, "  %-6u   ",info->shaper.max_burst);
    }

    

    
    switch(info->credit_source.se_type) {
    case SOC_TMC_SCH_SE_TYPE_HR:
      sal_sprintf(
        crdt_src_tbl_style,
        " HR[%-5u], SP-Cls: %s",
        info->credit_source.id,
        SOC_TMC_SCH_SUB_FLOW_HR_CLASS_to_string(info->credit_source.se_info.hr.sp_class)
      );
      break;
    case SOC_TMC_SCH_SE_TYPE_CL:
      sal_sprintf(
        crdt_src_tbl_style,
        " CL[%-5u], Class: %s  ",
        info->credit_source.id,
        SOC_TMC_SCH_SUB_FLOW_CL_CLASS_to_string(info->credit_source.se_info.cl.sp_class)
      );
      break;
    case SOC_TMC_SCH_SE_TYPE_FQ:
      sal_sprintf(
        crdt_src_tbl_style,
        " FQ[%-5u] %s",
        info->credit_source.id,
        "                       "
       );
      break;
    default:
      sal_sprintf(
        crdt_src_tbl_style,
        "%-34s",
        "  ???  "
       );
    }

    
    switch(info->credit_source.se_type) {
    case SOC_TMC_SCH_SE_TYPE_HR:
      sal_sprintf(
        crdt_src_weight_tbl_style,
        "  %-4u ",
        info->credit_source.se_info.hr.weight
      );
      break;
    case SOC_TMC_SCH_SE_TYPE_CL:
      sal_sprintf(
        crdt_src_weight_tbl_style,
        "  %-4u ",
        info->credit_source.se_info.cl.weight
      );
      break;
    case SOC_TMC_SCH_SE_TYPE_FQ:
      sal_sprintf(
        crdt_src_weight_tbl_style,
        "  ---  "
       );
      break;
    default:
      sal_sprintf(
        crdt_src_weight_tbl_style,
        "  ???  "
       );
    }

    
    LOG_CLI((BSL_META_U(unit,
                        "| %1u |%s|%s|%s|%s|\n\r"),
             subflow_id,
             rate_tbl_style,
             burst_tbl_style,
             crdt_src_tbl_style,
             crdt_src_weight_tbl_style
             ));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_FLOW_print(
    SOC_SAND_IN SOC_TMC_SCH_FLOW *info,
    SOC_SAND_IN uint8 is_table
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (!is_table)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Flow id %u, enabled sub-flows: "), info->sub_flow[0].id));
    if (info->sub_flow[0].is_valid && info->sub_flow[1].is_valid)
    {
      LOG_CLI((BSL_META_U(unit,
                          "0, 1")));
    }else if (info->sub_flow[0].is_valid)
    {
      LOG_CLI((BSL_META_U(unit,
                          "0")));
    } else if (info->sub_flow[1].is_valid)
    {
      LOG_CLI((BSL_META_U(unit,
                          "1")));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "None (flow is disabled)")));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n\r===============================\n\r")));
    LOG_CLI((BSL_META_U(unit,
                        "flow_type %s \n\r"),
             SOC_TMC_SCH_FLOW_TYPE_to_string(info->flow_type)
             ));
    LOG_CLI((BSL_META_U(unit,
                        "Is_slow_enabled: %u\n\r"),info->is_slow_enabled));

    for (ind=0; ind<SOC_TMC_SCH_NOF_SUB_FLOWS; ++ind)
    {
      if (info->sub_flow[ind].is_valid)
      {
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
        LOG_CLI((BSL_META_U(unit,
                            "Sub_flow[%u]:\n\r"),ind));
        SOC_TMC_SCH_SUBFLOW_print(&(info->sub_flow[ind]), FALSE, ind);
      }
    }
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        " Flow[%u] - %s"),
             info->sub_flow[0].id,
             info->sub_flow[0].is_valid?"Enabled, ":"Disabled. "
             ));

    if (info->sub_flow[0].is_valid)
    {
      LOG_CLI((BSL_META_U(unit,
                          "%s, %s, "),
               SOC_TMC_SCH_FLOW_TYPE_to_string(info->flow_type),
               info->is_slow_enabled?"slow-enabled": "not slow-enabled"
               ));
      if (info->sub_flow[1].is_valid)
      {
        LOG_CLI((BSL_META_U(unit,
                            "2 subflows: %u, %u: \n\r"), info->sub_flow[0].id, info->sub_flow[1].id));
      }
      else
      {
        LOG_CLI((BSL_META_U(unit,
                            "1 subflow: %u: \n\r"), info->sub_flow[0].id));
      }

      LOG_CLI((BSL_META_U(unit,
                          "+----------------------------------------------------------------------+\n\r")));
      LOG_CLI((BSL_META_U(unit,
                          "|Id |Rate[Kbps] |Burst[Byte]|Credit Src[ID],                   |Weight |\n\r")));
      LOG_CLI((BSL_META_U(unit,
                          "+----------------------------------------------------------------------+\n\r")));

      for (ind = 0; ind < SOC_TMC_SCH_NOF_SUB_FLOWS; ind++)
      {
        if (info->sub_flow[ind].is_valid)
        {
          SOC_TMC_SCH_SUBFLOW_print(
            &(info->sub_flow[ind]),
            TRUE,
            ind
          );
        }
      }

      LOG_CLI((BSL_META_U(unit,
                          "+----------------------------------------------------------------------+\n\r")));

      for (ind = 0; ind < SOC_TMC_SCH_NOF_SUB_FLOWS; ind++)
      {
        if (info->sub_flow[ind].is_valid)
        {
          if (info->sub_flow[ind].shaper.max_rate == 0)
          {
            LOG_CLI((BSL_META_U(unit,
                                "Note: sub-flow[%u] disabled by shaper rate\n\r"), ind));
          }
          if (info->sub_flow[ind].shaper.max_burst == 0)
          {
            LOG_CLI((BSL_META_U(unit,
                                "Note: sub-flow[%u] disabled by shaper burst\n\r"), ind));
          }
        }
      }
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_GLOBAL_PER1K_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_GLOBAL_PER1K_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Is_interdigitated: %u\n\r"),info->is_interdigitated));
  LOG_CLI((BSL_META_U(unit,
                      "Is_odd_even: %u\n\r"),info->is_odd_even));
  LOG_CLI((BSL_META_U(unit,
                      "Is_cl_cir: %u\n\r"),info->is_cl_cir));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_QUARTET_MAPPING_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_QUARTET_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Base_q_qrtt_id: %u\n\r"),info->base_q_qrtt_id));
  LOG_CLI((BSL_META_U(unit,
                      "Is_composite: %u\n\r"),info->is_composite));
  LOG_CLI((BSL_META_U(unit,
                      "Fip_id: %u\n\r"),info->fip_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SCH_SLOW_RATE_print(
    SOC_SAND_IN SOC_TMC_SCH_SLOW_RATE *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_TMC_SCH_NOF_SLOW_RATES; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Rates[%u]: %u[Kbps]\n\r"),ind,info->rates[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>
