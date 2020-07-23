/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_multicast_fabric.h>
#include <soc/dpp/TMC/tmc_api_general.h>


























void
  SOC_TMC_MULT_FABRIC_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_FABRIC_PORT_INFO));
  info->multicast_class_valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_SHAPER_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_SHAPER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_FABRIC_SHAPER_INFO));
  info->rate = 0;
  info->max_burst = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_BE_CLASS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_BE_CLASS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_FABRIC_BE_CLASS_INFO));
  SOC_TMC_MULT_FABRIC_PORT_INFO_clear(&(info->be_sch_port));
  info->weight = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_BE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_BE_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_FABRIC_BE_INFO));
  SOC_TMC_MULT_FABRIC_SHAPER_INFO_clear(&(info->be_shaper));
  info->wfq_enable = 0;
  for (ind=0; ind<SOC_TMC_MULT_FABRIC_NOF_BE_CLASSES; ++ind)
  {
    SOC_TMC_MULT_FABRIC_BE_CLASS_INFO_clear(&(info->be_sch_port[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_GR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_GR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_FABRIC_GR_INFO));
  SOC_TMC_MULT_FABRIC_SHAPER_INFO_clear(&(info->gr_shaper));
  SOC_TMC_MULT_FABRIC_PORT_INFO_clear(&(info->gr_sch_port));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_FABRIC_INFO));
  SOC_TMC_MULT_FABRIC_GR_INFO_clear(&(info->guaranteed));
  SOC_TMC_MULT_FABRIC_BE_INFO_clear(&(info->best_effort));
  info->max_rate = 0;
  info->max_burst = 1;
  info->credits_via_sch = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_ACTIVE_LINKS_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_ACTIVE_LINKS *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_FABRIC_ACTIVE_LINKS));
  for (ind=0; ind<SOC_TMC_MULT_FABRIC_NOF_UINT32S_FOR_ACTIVE_MC_LINKS(unit); ++ind)
  {
    info->bitmap[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_FLOW_CONTROL_MAP_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_FLOW_CONTROL_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_FABRIC_FLOW_CONTROL_MAP));
  
  info->bfmc0_lb_fc_map = SOC_TMC_MULT_FABRIC_FLOW_CONTROL_DONT_MAP;
  info->bfmc1_lb_fc_map = SOC_TMC_MULT_FABRIC_FLOW_CONTROL_DONT_MAP;
  info->bfmc2_lb_fc_map = SOC_TMC_MULT_FABRIC_FLOW_CONTROL_DONT_MAP;
  info->gfmc_lb_fc_map = SOC_TMC_MULT_FABRIC_FLOW_CONTROL_DONT_MAP;

  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_MULT_FABRIC_CLS_RNG_to_string(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_CLS_RNG enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_MULT_FABRIC_CLS_MIN:
    str = "MIN";
  break;

  case SOC_TMC_MULT_FABRIC_CLS_MAX:
    str = "MAX";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_MULT_FABRIC_PORT_INFO_print(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "   Class_valid: %d, "),info->multicast_class_valid));
  LOG_CLI((BSL_META_U(unit,
                      "Port_id: %d\n\r"),info->mcast_class_port_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_SHAPER_INFO_print(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_SHAPER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Rate: %u, "),info->rate));
  LOG_CLI((BSL_META_U(unit,
                      "Max_burst: %u.\n\r"),info->max_burst));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_BE_CLASS_INFO_print(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_BE_CLASS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  Be_sch_port:\n\r")));
  SOC_TMC_MULT_FABRIC_PORT_INFO_print(&(info->be_sch_port));
  LOG_CLI((BSL_META_U(unit,
                      "  Weight: %u\n\r"),info->weight));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_BE_INFO_print(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_BE_INFO *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " Be_shaper: ")));
  SOC_TMC_MULT_FABRIC_SHAPER_INFO_print(&(info->be_shaper));
  LOG_CLI((BSL_META_U(unit,
                      " Wfq_enable: %d\n\r"),info->wfq_enable));
  for (ind=0; ind<SOC_TMC_MULT_FABRIC_NOF_BE_CLASSES; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        " Be_sch_port[%u]:\n\r"),ind));
    SOC_TMC_MULT_FABRIC_BE_CLASS_INFO_print(&(info->be_sch_port[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_GR_INFO_print(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_GR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " Gr_shaper: ")));
  SOC_TMC_MULT_FABRIC_SHAPER_INFO_print(&(info->gr_shaper));
  LOG_CLI((BSL_META_U(unit,
                      " Gr_sch_port:\n\r")));
  SOC_TMC_MULT_FABRIC_PORT_INFO_print(&(info->gr_sch_port));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_INFO_print(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Guaranteed:\n\r")));
  SOC_TMC_MULT_FABRIC_GR_INFO_print(&(info->guaranteed));
  LOG_CLI((BSL_META_U(unit,
                      "Best_effort:\n\r")));
  SOC_TMC_MULT_FABRIC_BE_INFO_print(&(info->best_effort));
  LOG_CLI((BSL_META_U(unit,
                      "Max_rate:        %u[Kbps]\n\r"),info->max_rate));
  LOG_CLI((BSL_META_U(unit,
                      "Credits_via_sch: %d\n\r"),info->credits_via_sch));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_FABRIC_ACTIVE_LINKS_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_ACTIVE_LINKS *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_TMC_MULT_FABRIC_NOF_UINT32S_FOR_ACTIVE_MC_LINKS(unit); ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Bitmap[%u]: %u\n\r"),ind,info->bitmap[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

