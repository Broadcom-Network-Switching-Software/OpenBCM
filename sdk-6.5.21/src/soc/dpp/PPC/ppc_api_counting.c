/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPC

#include <shared/bsl.h>




#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PPC/ppc_api_counting.h>





















void
  SOC_PPC_CNT_COUNTER_ID_clear(
    SOC_SAND_OUT SOC_PPC_CNT_COUNTER_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_CNT_COUNTER_ID));
  info->stage = SOC_PPC_NOF_CNT_COUNTER_STAGES;
  info->group = 0;
  info->id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PPC_CNT_KEY_clear(
    SOC_SAND_OUT SOC_PPC_CNT_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_CNT_KEY));
  info->type = SOC_PPC_NOF_CNT_TYPES;
  info->id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_CNT_RESULT_clear(
    SOC_SAND_OUT SOC_PPC_CNT_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_CNT_RESULT));
  soc_sand_64cnt_clear(&(info->pkt_cnt));
  soc_sand_64cnt_clear(&(info->byte_cnt));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_CNT_COUNTER_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_CNT_COUNTER_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_CNT_COUNTER_TYPE_BYTES:
    str = "bytes";
  break;
  case SOC_PPC_CNT_COUNTER_TYPE_PACKETS:
    str = "packets";
  break;
  case SOC_PPC_CNT_COUNTER_TYPE_BYTES_AND_PACKETS:
    str = "bytes_and_packets";
  break;
  case SOC_PPC_CNT_COUNTER_TYPE_PER_COLOR:
    str = "per_color";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_CNT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_CNT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_CNT_TYPE_INLIF:
    str = "inlif";
  break;
  case SOC_PPC_CNT_TYPE_OUTLIF:
    str = "outlif";
  break;
  case SOC_PPC_CNT_TYPE_IN_AC:
    str = "in_ac";
  break;
  case SOC_PPC_CNT_TYPE_OUT_AC:
    str = "out_ac";
  break;
  case SOC_PPC_CNT_TYPE_VSID:
    str = "vsid";
  break;
  case SOC_PPC_CNT_TYPE_IN_PWE:
    str = "in_pwe";
  break;
  case SOC_PPC_CNT_TYPE_OUT_PWE:
    str = "out_pwe";
  break;
  case SOC_PPC_CNT_TYPE_IN_LABEL:
    str = "in_label";
  break;
  case SOC_PPC_CNT_TYPE_OUT_LABEL:
    str = "out_label";
  break;
  case SOC_PPC_CNT_TYPE_FEC_ID:
    str = "fec_id";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_CNT_COUNTER_STAGE_to_string(
    SOC_SAND_IN  SOC_PPC_CNT_COUNTER_STAGE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_CNT_COUNTER_STAGE_INGRESS:
    str = "ingress";
  break;
  case SOC_PPC_CNT_COUNTER_STAGE_EGRESS:
    str = "egress";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_CNT_COUNTER_ID_print(
    SOC_SAND_IN  SOC_PPC_CNT_COUNTER_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

    LOG_CLI((BSL_META_U(unit,
                        "stage %s "), SOC_PPC_CNT_COUNTER_STAGE_to_string(info->stage)));
    LOG_CLI((BSL_META_U(unit,
                        "group: %u\n\r"),info->group));
    LOG_CLI((BSL_META_U(unit,
                        "id: %u\n\r"),info->id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PPC_CNT_KEY_print(
    SOC_SAND_IN  SOC_PPC_CNT_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_CNT_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "id: %u\n\r"),info->id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_CNT_RESULT_print(
    SOC_SAND_IN  SOC_PPC_CNT_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "pkt_cnt:")));
  soc_sand_64cnt_print(&(info->pkt_cnt), TRUE);
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "byte_cnt:")));
  soc_sand_64cnt_print(&(info->byte_cnt), TRUE);
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

