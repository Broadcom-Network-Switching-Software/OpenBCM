/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h>


























void
  SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO));
  info->base_queue_id = 0;
  
  info->queue_id_add_not_decrement = FALSE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_IPQ_BASEQ_MAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_IPQ_BASEQ_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_IPQ_BASEQ_MAP_INFO));
  info->valid = 0;
  info->base_queue = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_IPQ_QUARTET_MAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_IPQ_QUARTET_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_IPQ_QUARTET_MAP_INFO));
  info->flow_quartet_index = SOC_TMC_IPQ_INVALID_FLOW_QUARTET;
  info->is_composite = 0;
  info->system_physical_port = SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_IPQ_TR_CLS_RNG_to_string(
    SOC_SAND_IN SOC_TMC_IPQ_TR_CLS_RNG enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_IPQ_TR_CLS_MIN:
    str = "TR_CLS_MIN";
  break;

  case SOC_TMC_IPQ_TR_CLS_MAX:
    str = "TR_CLS_MAX";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO_print(
    SOC_SAND_IN SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Base_queue_id: %u\n\r"),info->base_queue_id));
  LOG_CLI((BSL_META_U(unit,
                      "Queue_id_add_not_decrement: %d\n\r"),info->queue_id_add_not_decrement));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_IPQ_BASEQ_MAP_INFO_print(
    SOC_SAND_IN  SOC_TMC_IPQ_BASEQ_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "valid: %u\n\r"),info->valid));
  LOG_CLI((BSL_META_U(unit,
                      "base_queue: %u\n\r"),info->base_queue));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_IPQ_QUARTET_MAP_INFO_print(
    SOC_SAND_IN SOC_TMC_IPQ_QUARTET_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (
      (info->flow_quartet_index == SOC_TMC_IPQ_INVALID_FLOW_QUARTET) &&
      (info->is_composite == 0) &&
      (info->system_physical_port == SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID)
     )
  {
    LOG_CLI((BSL_META_U(unit,
                        "The queue quartet is unmapped.\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "Flow_quartet_index: %u\n\r"),info->flow_quartet_index));
    LOG_CLI((BSL_META_U(unit,
                        "Is_composite: %d\n\r"),info->is_composite));
    LOG_CLI((BSL_META_U(unit,
                        "System_physical_port: %u\n\r"),info->system_physical_port));
    LOG_CLI((BSL_META_U(unit,
                        "Fap_id: %u\n\r"),((unsigned)info->fap_id)));
    LOG_CLI((BSL_META_U(unit,
                        "Fap_port_id: %u\n\r"),((unsigned)info->fap_port_id)));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

