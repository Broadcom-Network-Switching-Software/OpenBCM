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

#include <soc/dpp/PPC/ppc_api_lag.h>





















void
  SOC_PPC_LAG_HASH_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LAG_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LAG_HASH_GLOBAL_INFO));
  info->use_port_id = 0;
  info->seed = 0;
  info->hash_func_id = 0;
  info->key_shift = 0;
  info->eli_search = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LAG_HASH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LAG_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LAG_HASH_PORT_INFO));
  info->nof_headers = 0;
  info->first_header_to_parse = SOC_PPC_NOF_LAG_HASH_FRST_HDRS;
  info->include_bos = FALSE;
  info->start_from_bos = FALSE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_HASH_MASK_INFO_clear(
    SOC_SAND_OUT SOC_PPC_HASH_MASK_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_HASH_MASK_INFO));
  info->mask = 0;
  info->is_symmetric_key = 0;
  info->expect_cw = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LAG_MEMBER_clear(
    SOC_SAND_OUT SOC_PPC_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LAG_MEMBER));
  info->sys_port = 0;
  info->member_id = 0;
  info->flags = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LAG_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LAG_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LAG_INFO));
#ifdef SOC_SAND_MAGIC_NUM_ENABLE
  for (ind = 0; ind < SOC_PPC_LAG_MEMBERS_MAX; ++ind)
  {
    info->members[ind].soc_sand_magic_num = SOC_SAND_MAGIC_NUM_VAL;
  }
#endif
  info->lb_type = SOC_PPC_NOF_LAG_LB_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LAG_HASH_FRST_HDR_to_string(
    SOC_SAND_IN  SOC_PPC_LAG_HASH_FRST_HDR enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LAG_HASH_FRST_HDR_FARWARDING:
    str = "farwarding";
  break;
  case SOC_PPC_LAG_HASH_FRST_HDR_LAST_TERMINATED:
    str = "last_terminated";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_LAG_LB_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LAG_LB_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LAG_LB_TYPE_HASH:
    str = "hash";
    break;
  case SOC_PPC_LAG_LB_TYPE_ROUND_ROBIN:
    str = "round_robin";
    break;
  case SOC_PPC_LAG_LB_TYPE_SMOOTH_DIVISION:
    str = "smooth_division";
    break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_LAG_HASH_GLOBAL_INFO_print(
    SOC_SAND_IN  SOC_PPC_LAG_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "use_port_id: %u\n\r"),info->use_port_id));
  LOG_CLI((BSL_META_U(unit,
                      "seed: %u\n\r"), info->seed));
  LOG_CLI((BSL_META_U(unit,
                      "hash_func_id: %u\n\r"), info->hash_func_id));
  LOG_CLI((BSL_META_U(unit,
                      "key_shift: %u\n\r"), info->key_shift));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LAG_HASH_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LAG_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "nof_headers: %u\n\r"), info->nof_headers));
  LOG_CLI((BSL_META_U(unit,
                      "first_header_to_parse %s "), SOC_PPC_LAG_HASH_FRST_HDR_to_string(info->first_header_to_parse)));

  LOG_CLI((BSL_META_U(unit,
                      "start_from_bos: %u\n\r"), info->start_from_bos));
  LOG_CLI((BSL_META_U(unit,
                      "include_bos: %u\n\r"), info->include_bos));
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_HASH_MASK_INFO_print(
    SOC_SAND_IN  SOC_PPC_HASH_MASK_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "key includes:\n\r")));
  for (ind = 0 ; ind < SOC_PPC_NOF_HASH_MASKS; ++ind)
  {
    if ((SOC_SAND_BIT(ind) & info->mask) == 0)
    {
      LOG_CLI((BSL_META_U(unit,
                          " %s \n\r"), SOC_PPC_HASH_MASKS_to_string(SOC_SAND_BIT(ind))));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "key excludes:\n\r")));
    for (ind = 0 ; ind < SOC_PPC_NOF_HASH_MASKS; ++ind)
    {
      if ((SOC_SAND_BIT(ind) & info->mask) != 0)
      {
        LOG_CLI((BSL_META_U(unit,
                            " %s \n\r"), SOC_PPC_HASH_MASKS_to_string(SOC_SAND_BIT(ind))));
      }
    }
  LOG_CLI((BSL_META_U(unit,
                      "is_symmetric_key: %u\n\r"),info->is_symmetric_key));
  LOG_CLI((BSL_META_U(unit,
                      "expect_cw: %u\n\r"),info->expect_cw));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LAG_MEMBER_print(
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "sys_port: %u\n\r"),info->sys_port));
  LOG_CLI((BSL_META_U(unit,
                      "member_id: %u\n\r"),info->member_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LAG_INFO_print(
    SOC_SAND_IN  SOC_PPC_LAG_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "nof_entries: %u\n\r"),info->nof_entries));
  for (ind = 0; ind < info->nof_entries; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "members[%u]:"),ind));
    SOC_PPC_LAG_MEMBER_print(&(info->members[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "lb_type %s "), SOC_PPC_LAG_LB_TYPE_to_string(info->lb_type)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

