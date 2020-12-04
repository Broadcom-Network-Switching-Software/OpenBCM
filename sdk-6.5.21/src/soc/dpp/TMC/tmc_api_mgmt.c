/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_mgmt.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>



























void
  SOC_TMC_MGMT_PCKT_SIZE_clear(
    SOC_SAND_OUT SOC_TMC_MGMT_PCKT_SIZE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MGMT_PCKT_SIZE));
  info->min = 0;
  info->max = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MGMT_OCB_VOQ_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MGMT_OCB_VOQ_INFO *info
  )
{
  int32
    index;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MGMT_OCB_VOQ_INFO));
  info->voq_eligible = TRUE;
  for(index = 0; index < SOC_TMC_MGMT_OCB_VOQ_NOF_THRESHOLDS; ++index)
  {
      info->th_words[index] = SOC_TMC_MGMT_OCB_PRM_EN_TH_DEFAULT;
      info->th_buffers[index] = SOC_TMC_MGMT_OCB_PRM_EN_TH_DEFAULT;
  }

  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_MGMT_FABRIC_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_MGMT_FABRIC_HDR_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_MGMT_FABRIC_HDR_TYPE_PETRA:
    str = "petra";
  break;
  case SOC_TMC_MGMT_FABRIC_HDR_TYPE_FAP20:
    str = "fap20";
  break;
  case SOC_TMC_MGMT_FABRIC_HDR_TYPE_FAP10M:
    str = "fap10m";
  break;
  case SOC_TMC_MGMT_NOF_FABRIC_HDR_TYPES:
    str = " Not initialized";
  }
  return str;
}


const char*
  SOC_TMC_MGMT_TDM_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_MGMT_TDM_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_MGMT_TDM_MODE_PACKET:
    str = "packet";
  break;
  case SOC_TMC_MGMT_TDM_MODE_TDM_OPT:
    str = "tdm_opt";
  break;
  case SOC_TMC_MGMT_TDM_MODE_TDM_STA:
    str = "tdm_sta";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE_EXTERN:
    str = "extern";
  break;
  case SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE_INTERN:
    str = "intern";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_MGMT_PCKT_SIZE_print(
    SOC_SAND_IN SOC_TMC_MGMT_PCKT_SIZE *info
  )
{
  char
    min_str[30],
    max_str[30];

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->min == SOC_TMC_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT)
  {
    sal_sprintf(min_str, "Not Limited by Original Size");
  }
  else
  {
    sal_sprintf(min_str, "%u[Bytes]", info->min);
  }

  if (info->max == SOC_TMC_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT)
  {
    sal_sprintf(max_str, "Not Limited by Original Size");
  }
  else
  {
    sal_sprintf(max_str, "%u[Bytes]", info->max);
  }

  LOG_CLI((BSL_META_U(unit,
                      "Min: %s, Max: %s\n\r"),min_str, max_str));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

